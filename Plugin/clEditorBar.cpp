#include "clEditorBar.h"

#include "ColoursAndFontsManager.h"
#include "IWorkspace.h"
#include "bitmap_loader.h"
#include "bookmark_manager.h"
#include "clSystemSettings.h"
#include "clTabRenderer.h"
#include "clWorkspaceManager.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "optionsconfig.h"

#include <algorithm>
#include <unordered_map>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/menu.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

#define X_SPACER 10
#define Y_SPACER 5

clEditorBar::clEditorBar(wxWindow* parent)
    : clEditorBarBase(parent)
{
    m_functionBmp = clGetManager()->GetStdIcons()->LoadBitmap("function_public", 16);
    CreateBookmarksBitmap();

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clEditorBar::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_MARKER_CHANGED, &clEditorBar::OnMarkerChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CC_UPDATE_NAVBAR, &clEditorBar::OnUpdate, this);
    m_buttonScope->SetBitmap(m_functionBmp);
    m_buttonScope->SetHasDropDownMenu(true);
    m_buttonFilePath->SetHasDropDownMenu(true);
    m_buttonBookmarks->SetHasDropDownMenu(true);
}

clEditorBar::~clEditorBar()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clEditorBar::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_MARKER_CHANGED, &clEditorBar::OnMarkerChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_UPDATE_NAVBAR, &clEditorBar::OnUpdate, this);
}

void clEditorBar::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
}

void clEditorBar::SetScopes(const wxString& filename, const clEditorBar::ScopeEntry::vec_t& entries)
{
    m_scopes = entries;
    m_scopesFile = filename;
    std::sort(m_scopes.begin(), m_scopes.end(),
              [](const ScopeEntry& a, const ScopeEntry& b) { return a.line_number < b.line_number; });
    CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
}

void clEditorBar::DoShow(bool s)
{
    m_shouldShow = s;
    if(Show(s)) {
        GetParent()->GetSizer()->Layout();
    }
    CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
}

void clEditorBar::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();
    CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
}

void clEditorBar::DoRefreshColoursAndFonts()
{
    // Set the defaults
    m_filename.clear();
    m_projectFile.clear();
    m_projectName.clear();
    m_filenameRelative.clear();
    m_bookmarks.clear();

    wxColour bgcolour = clSystemSettings::GetDefaultPanelColour();
    wxColour textColour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

    SetBackgroundColour(bgcolour);
    m_labelText->SetForegroundColour(textColour);
    m_labelText->SetBackgroundColour(bgcolour);

    if(!m_shouldShow) {
        return;
    }

    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        if(!IsShown()) {
            Show();
        }

        wxString current_file = editor->GetRemotePathOrLocal();

        // Update bookmarks button
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
        editor->GetFindMarkers(m_bookmarks);

        if(!m_bookmarks.empty()) {
            CreateBookmarksBitmap();
            if(!m_buttonBookmarks->IsShown()) {
                m_buttonBookmarks->Show();
            }
            m_buttonBookmarks->SetText(_("Bookmarks"));
            m_buttonBookmarks->SetBitmap(m_bookmarksBmp);
        } else {
            m_buttonBookmarks->Hide();
        }

        // Update file path button
        if(!m_buttonFilePath->IsShown()) {
            m_buttonFilePath->Show();
        }

        wxString filepath;

        wxFileName fn(current_file, editor->IsRemoteFile() ? wxPATH_UNIX : wxPATH_NATIVE);
        wxString PATH_SEPARATOR = editor->IsRemoteFile() ? wxString("/") : wxFileName::GetPathSeparator();

        if(fn.GetDirCount()) {
            filepath << fn.GetDirs().Last() << PATH_SEPARATOR;
        }
        filepath << fn.GetFullName();
        m_buttonFilePath->SetText(filepath);
        m_filename = current_file;

        // update the scope
        bool hide_scope_button = m_scopes.empty() || m_scopesFile != current_file;
        if(hide_scope_button) {
            m_buttonScope->Hide();
        } else {
            if(!m_buttonScope->IsShown()) {
                m_buttonScope->Show();
            }
            UpdateScope();
        }
    } else {
        m_scopes.clear();
        m_scopesFile.clear();
        m_buttonScope->SetText("");
        m_buttonFilePath->SetText("");
        m_buttonBookmarks->SetText("");
        m_buttonScope->Hide();
        m_buttonFilePath->Hide();
        m_buttonBookmarks->Hide();
        Hide();
    }
    GetParent()->GetSizer()->Layout();
}

void clEditorBar::DoRefresh() { Refresh(); }

void clEditorBar::OnMarkerChanged(clCommandEvent& event)
{
    event.Skip();
    if(!IsShown()) {
        return;
    }
    CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
}

void clEditorBar::CreateBookmarksBitmap()
{
    m_bookmarksBmp = wxBitmap(16, 16);
    wxMemoryDC memDC(m_bookmarksBmp);
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    wxColour bgColour = options->GetBookmarkBgColour(0);
    wxColour fgColour = options->GetBookmarkFgColour(0);
    memDC.SetPen(fgColour);
    memDC.SetBrush(bgColour);
    memDC.DrawRectangle(0, 0, 16, 16);
    memDC.SelectObject(wxNullBitmap);
}

void clEditorBar::OnButtonActions(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxMenu menu;
    wxString text;

    text << _("Copy '") << m_filename << _("' to the clipboard");
    wxMenuItem* idCopyFullPath = menu.Append(wxID_ANY, text);

    text.Clear();
    text << _("Copy '") << wxFileName(m_filename).GetFullName() << _("' to the clipboard");
    wxMenuItem* idCopyName = menu.Append(wxID_ANY, text);

    text.Clear();
    text << _("Copy '") << wxFileName(m_filename).GetPath() << _("' to the clipboard");
    wxMenuItem* idCopyPath = menu.Append(wxID_ANY, text);

    menu.AppendSeparator();
    wxMenuItem *idOpenExplorer, *idOpenShell;
    {
        idOpenShell = new wxMenuItem(NULL, wxID_ANY, _("Open Shell"), _("Open Shell"));
        idOpenShell->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("console"));
        menu.Append(idOpenShell);
    }

    {
        idOpenExplorer = new wxMenuItem(NULL, wxID_ANY, _("Open Containing Folder"), _("Open Containing Folder"));
        idOpenExplorer->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("folder-yellow"));
        menu.Append(idOpenExplorer);
    }

    // Capture all menu items in a single callback that simply keeps the selected menu item id
    int selection = wxID_NONE;
    menu.Bind(
        wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
    m_buttonFilePath->ShowMenu(menu);

    if(selection == wxID_NONE)
        return;

    text.Clear();
    if(selection == idCopyFullPath->GetId()) {
        text = m_filename;
    } else if(selection == idCopyName->GetId()) {
        text = wxFileName(m_filename).GetFullName();
    } else if(selection == idCopyPath->GetId()) {
        text = wxFileName(m_filename).GetPath();
    } else if(selection == idOpenExplorer->GetId()) {
        FileUtils::OpenFileExplorerAndSelect(wxFileName(m_filename).GetFullPath());
    } else if(selection == idOpenShell->GetId()) {
        FileUtils::OpenTerminal(wxFileName(m_filename).GetPath());
    }

    // Clipboard action?
    if(!text.IsEmpty()) {
        ::CopyToClipboard(text);
        clGetManager()->SetStatusMessage((wxString() << "'" << text << _("' copied!")), 2);
    }
}

void clEditorBar::OnButtonBookmarks(wxCommandEvent& event)
{
    wxUnusedVar(event);
    IEditor* editor = clGetManager()->GetActiveEditor();
    std::vector<std::pair<int, wxString>> V;
    if(editor && editor->GetFindMarkers(V)) {
        // Show bookmarks menu
        wxMenu menu;
        std::unordered_map<int, int> M;
        std::for_each(V.begin(), V.end(), [&](const std::pair<int, wxString>& p) {
            wxString text = wxString::Format("%5u: ", p.first);
            text << p.second;
            M[menu.Append(wxID_ANY, text)->GetId()] = p.first; // Make the menu item ID with the line number
        });

        // We got something to display
        int selection = wxID_NONE;
        menu.Bind(
            wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
        m_buttonBookmarks->ShowMenu(menu);

        if(selection == wxID_NONE)
            return;
        if(M.count(selection)) {
            int lineNumber = M[selection] - 1;
            editor->CenterLine(lineNumber);
            editor->GetCtrl()->EnsureVisible(lineNumber);
            editor->GetCtrl()->EnsureCaretVisible();
            editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
        }
    }
}

void clEditorBar::OnButtonScope(wxCommandEvent& event)
{
    wxUnusedVar(event);
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    // build the menu
    wxMenu menu;
    std::unordered_map<int, ScopeEntry> M;
    for(const ScopeEntry& entry : m_scopes) {
        int menu_id = wxXmlResource::GetXRCID(entry.display_string);
        M[menu_id] = entry;
        menu.Append(menu_id, entry.display_string);
    }
    CHECK_EXPECTED_RETURN(menu.GetMenuItemCount() > 0, true);

    // Popup the menu
    int selection = wxID_NONE;
    menu.Bind(
        wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
    m_buttonScope->ShowMenu(menu);

    if(selection == wxID_NONE)
        return;

    if(M.count(selection)) {
        // Display the matched entry from the menu
        auto stc = editor->GetCtrl();
        ScopeEntry entry = M[selection];
        editor->CenterLine(entry.line_number);
        stc->EnsureVisible(entry.line_number);
        stc->CallAfter(&wxStyledTextCtrl::SetFocus);
    }
}

void clEditorBar::SetLabel(const wxString& text) { m_labelText->SetLabel(text); }

wxString clEditorBar::GetLabel() const { return m_labelText->GetLabel(); }

void clEditorBar::OnUpdate(clCodeCompletionEvent& event)
{
    wxUnusedVar(event);
    UpdateScope();
}

thread_local clEditorBar::ScopeEntry InvalidScope;

const clEditorBar::ScopeEntry& clEditorBar::FindByLine(int lineNumber) const
{
    const ScopeEntry* match = nullptr;
    for(const clEditorBar::ScopeEntry& entry : m_scopes) {
        if((entry.line_number - 1) >= lineNumber) {
            break;
        } else {
            match = &entry;
        }
    }

    if(match) {
        return *match;
    } else {
        return InvalidScope;
    }
}

void clEditorBar::UpdateScope()
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    wxString fullpath = editor->GetRemotePathOrLocal();
    if(fullpath != m_scopesFile) {
        m_scopes.clear();
        m_buttonScope->SetText(wxEmptyString);
        return;
    }

    const auto& scope = FindByLine(editor->GetCurrentLine());
    if(scope.is_ok()) {
        m_buttonScope->SetText(scope.display_string);
    } else {
        m_buttonScope->SetText(wxEmptyString);
    }
}

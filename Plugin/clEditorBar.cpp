#include "ColoursAndFontsManager.h"
#include "IWorkspace.h"
#include "bitmap_loader.h"
#include "bookmark_manager.h"
#include "clEditorBar.h"
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
#include "clThemeUpdater.h"

#define X_SPACER 10
#define Y_SPACER 5

clEditorBar::clEditorBar(wxWindow* parent)
    : clEditorBarBase(parent)
{
    clThemeUpdater::Get().RegisterWindow(this);

    m_functionBmp = clGetManager()->GetStdIcons()->LoadBitmap("function_public", 16);
    CreateBookmarksBitmap();

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_MARKER_CHANGED, &clEditorBar::OnMarkerChanged, this);
    m_buttonScope->SetBitmap(m_functionBmp);
    m_buttonScope->SetHasDropDownMenu(true);
    m_buttonFilePath->SetHasDropDownMenu(true);
    m_buttonBookmarks->SetHasDropDownMenu(true);
}

clEditorBar::~clEditorBar()
{
    clThemeUpdater::Get().UnRegisterWindow(this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_MARKER_CHANGED, &clEditorBar::OnMarkerChanged, this);
}

void clEditorBar::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
}

void clEditorBar::SetMessage(const wxString& className, const wxString& function)
{
    if((className != m_classname) || (function != m_function)) {
        m_classname = className;
        m_function = function;
        CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
    }
}

void clEditorBar::DoShow(bool s)
{
    m_shouldShow = s;
    if(Show(s)) { GetParent()->GetSizer()->Layout(); }
    CallAfter(&clEditorBar::DoRefreshColoursAndFonts);
}

void clEditorBar::OnThemeChanged(wxCommandEvent& e)
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
    
    if(!m_shouldShow) { return; }
    
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        if(!IsShown()) { Show(); }
        // Update bookmarks button
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
        editor->GetFindMarkers(m_bookmarks);

        if(!m_bookmarks.empty()) {
            CreateBookmarksBitmap();
            if(!m_buttonBookmarks->IsShown()) { m_buttonBookmarks->Show(); }
            m_buttonBookmarks->SetText(_("Bookmarks"));
            m_buttonBookmarks->SetBitmap(m_bookmarksBmp);
        } else {
            m_buttonBookmarks->Hide();
        }

        // Update file path button
        if(!m_buttonFilePath->IsShown()) { m_buttonFilePath->Show(); }
        m_buttonFilePath->SetText(editor->GetFileName().GetFullPath());
        m_filename = editor->GetFileName().GetFullPath();

        wxString scope;
        if(!m_classname.IsEmpty()) { scope << m_classname << "::"; }
        if(!m_function.IsEmpty()) { scope << m_function; }
        if(scope.IsEmpty()) {
            m_buttonScope->Hide();
        } else {
            if(!m_buttonScope->IsShown()) { m_buttonScope->Show(); }
            m_buttonScope->SetText(scope);
        }
    } else {
        m_classname.clear();
        m_function.clear();
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
    if(!IsShown()) { return; }
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
    menu.Bind(wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
    m_buttonFilePath->ShowMenu(menu);

    if(selection == wxID_NONE) return;

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
    std::vector<std::pair<int, wxString> > V;
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
        menu.Bind(wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
        m_buttonBookmarks->ShowMenu(menu);

        if(selection == wxID_NONE) return;
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
    wxMenu menu;
    clContextMenuEvent menuEvent(wxEVT_NAVBAR_SCOPE_MENU_SHOWING);
    menuEvent.SetMenu(&menu);
    EventNotifier::Get()->ProcessEvent(menuEvent);

    if(menu.GetMenuItemCount()) {
        // We got something to display
        // Keep track of the menu items
        std::unordered_map<int, wxString> M;
        const wxMenuItemList& list = menu.GetMenuItems();
        wxMenuItemList::const_iterator iter = list.begin();
        for(; iter != list.end(); ++iter) {
            wxMenuItem* menuItem = *iter;
            M[menuItem->GetId()] = menuItem->GetItemLabel();
        }

        // Popup the menu
        int selection = wxID_NONE;
        menu.Bind(wxEVT_MENU, [&](wxCommandEvent& evt) { selection = evt.GetId(); }, wxID_ANY);
        m_buttonScope->ShowMenu(menu);

        if(selection == wxID_NONE) return;
        if(M.count(selection)) {
            // Fire selection made event
            clCommandEvent selectionEvent(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE);
            selectionEvent.SetString(M[selection]);
            EventNotifier::Get()->AddPendingEvent(selectionEvent);
        }
    }
}

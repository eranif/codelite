#include "ColoursAndFontsManager.h"
#include "IWorkspace.h"
#include "bitmap_loader.h"
#include "bookmark_manager.h"
#include "clEditorBar.h"
#include "clTabRenderer.h"
#include "clWorkspaceManager.h"
#include "cl_command_event.h"
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
#include <unordered_map>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/menu.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <algorithm>

#define X_SPACER 10
#define Y_SPACER 5

clEditorBar::clEditorBar(wxWindow* parent)
    : clEditorBarBase(parent)
    , m_scopeButtonState(eButtonState::kNormal)
    , m_state(eButtonState::kNormal)
    , m_bookmarksButtonState(eButtonState::kNormal)
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    m_defaultColour = DrawingUtils::GetPanelTextColour();
    m_functionColour = DrawingUtils::GetPanelTextColour();
    m_classColour = DrawingUtils::GetPanelTextColour();
    m_bgColour = DrawingUtils::GetPanelBgColour();
    m_textFont = m_textFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    LexerConf::Ptr_t defaultLexer = ColoursAndFontsManager::Get().GetLexer("default");
    if(defaultLexer) { m_textFont = defaultLexer->GetFontForSyle(0); }

    m_functionBmp = clGetManager()->GetStdIcons()->LoadBitmap("function_public", 16);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    memDC.SetFont(m_textFont);
    wxSize sz = memDC.GetTextExtent("Tp");
    //wxCoord baseY = wxMax(sz.y, m_functionBmp.GetScaledHeight());
    wxCoord baseY = sz.y;
    baseY += (2 * Y_SPACER); // 2*3 pixels
    SetSizeHints(wxSize(-1, baseY));

    CreateBookmarksBitmap();

    Bind(wxEVT_LEFT_UP, &clEditorBar::OnLeftUp, this);
    Bind(wxEVT_IDLE, &clEditorBar::OnIdle, this);

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_MARKER_CHANGED, &clEditorBar::OnMarkerChanged, this);
}

clEditorBar::~clEditorBar()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_MARKER_CHANGED, &clEditorBar::OnMarkerChanged, this);
    Unbind(wxEVT_LEFT_UP, &clEditorBar::OnLeftUp, this);
    Unbind(wxEVT_IDLE, &clEditorBar::OnIdle, this);
}

void clEditorBar::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }
void clEditorBar::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC bdc(this);
    PrepareDC(bdc);
#ifdef __WXGTK__
    wxDC &gcdc = bdc;
#else
    wxGCDC gcdc(bdc);
#endif

    PrepareDC(gcdc);

    // Clear the rects
    m_scopeRect = wxRect();
    m_filenameRect = wxRect();
    m_bookmarksRect = wxRect();

    wxFont guiFont = DrawingUtils::GetDefaultGuiFont();
    wxRect rect(GetClientRect());
    gcdc.SetPen(m_bgColour);
    gcdc.SetBrush(m_bgColour);
    gcdc.DrawRectangle(rect);

    // Draw the text
    wxCoord textX = 0;
    gcdc.SetFont(guiFont);

    wxString fulltext;
    if(!m_classname.IsEmpty()) { fulltext << m_classname << "::"; }
    if(!m_function.IsEmpty()) { fulltext << m_function; }

    if(!fulltext.IsEmpty()) {
        int scopeButtonWidth =
            /*m_functionBmp.GetScaledWidth() + */ gcdc.GetTextExtent("W" + fulltext + "W").GetWidth();
        m_scopeRect = wxRect(textX, 0, scopeButtonWidth + 20, rect.GetHeight() - 2);
        DrawingUtils::DrawButton(gcdc, this, m_scopeRect, fulltext, wxNullBitmap, eButtonKind::kDropDown,
                                 m_scopeButtonState);
        textX += m_scopeRect.GetWidth();
    }

    if(!m_breadcrumbs.IsEmpty()) {
        wxString breadcumbsText;
        for(size_t i = 0; i < m_breadcrumbs.size(); ++i) {
            breadcumbsText << m_breadcrumbs.Item(i) << " / ";
        }

        breadcumbsText.RemoveLast(3);
        gcdc.SetFont(guiFont);
        int filenameButtonWidth = gcdc.GetTextExtent("W" + breadcumbsText + "W").GetWidth();

        wxString bookmarksLabel = _("Bookmarks");
        wxSize bookmarksTextSize = gcdc.GetTextExtent(bookmarksLabel);

        int total_width = filenameButtonWidth + 20; // The file name button width
        if(!m_bookmarks.empty()) {
            total_width += X_SPACER +                     // Separator between the buttons
                           bookmarksTextSize.GetWidth() + // The bookmarks button size
                           60; // Add 60 (40 for the image and 20 needed for the drop down button)
        }

        textX = GetClientRect().GetWidth() - total_width;

        if(!m_bookmarks.empty()) {
            // Update the bookmarks bitmap according to the user settings
            CreateBookmarksBitmap();
            // Draw the bookmarks button
            // Add 60 (about 40 for the image and 20 needed for the drop down button
            m_bookmarksRect = wxRect(textX, 0, bookmarksTextSize.GetWidth() + 60, rect.GetHeight() - 2);
            DrawingUtils::DrawButton(gcdc, this, m_bookmarksRect, bookmarksLabel, m_bookmarksBmp,
                                     eButtonKind::kDropDown, m_bookmarksButtonState);

            textX += m_bookmarksRect.GetWidth();
            textX += X_SPACER;
        }

        // Draw the file name button
        // We add 20 since its the drop down button size
        m_filenameRect = wxRect(textX, 0, filenameButtonWidth + 20, rect.GetHeight() - 2);
        DrawingUtils::DrawButton(gcdc, this, m_filenameRect, breadcumbsText, wxNullBitmap, eButtonKind::kDropDown,
                                 m_state);
        textX += m_filenameRect.GetWidth();
        textX += X_SPACER;
    }
}

void clEditorBar::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    DoRefreshColoursAndFonts();
}

void clEditorBar::SetMessage(const wxString& className, const wxString& function)
{
    if((className != m_classname) || (function != m_function)) {
        m_classname = className;
        m_function = function;
        Refresh();
    }
}

void clEditorBar::DoShow(bool s)
{
    if(Show(s)) { GetParent()->GetSizer()->Layout(); }
}

void clEditorBar::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    DoRefreshColoursAndFonts();
}

void clEditorBar::DoRefreshColoursAndFonts()
{
    // Set the defaults
    m_filename.clear();
    m_projectFile.clear();
    m_projectName.clear();
    m_filenameRelative.clear();
    m_breadcrumbs.clear();
    m_bgColour = DrawingUtils::GetPanelBgColour();
    m_bookmarks.clear();

    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
        editor->GetFindMarkers(m_bookmarks);

        if(lexer) {
            wxColour m_bgColour = DrawingUtils::GetPanelBgColour();
            bool darkBG = DrawingUtils::IsDark(m_bgColour);
            m_defaultColour = DrawingUtils::GetPanelTextColour();
            m_classColour = darkBG ? "rgb(224, 108, 117)" : "rgb(0, 64, 128)";
            m_functionColour = m_defaultColour;
            m_textFont = lexer->GetFontForSyle(0); // Default font
                                                   // m_textFont.SetPointSize(m_textFont.GetPointSize() - 1);
        }

        m_filename = editor->GetFileName().GetFullPath();
        if(clWorkspaceManager::Get().IsWorkspaceOpened()) {
            IWorkspace* workspace = clWorkspaceManager::Get().GetWorkspace();
            wxFileName fn(m_filename);
            fn.MakeRelativeTo(workspace->GetFileName().GetPath());
            m_filenameRelative = fn.GetFullPath(wxPATH_UNIX);
        }

        // Build the Breadcrumbs
        if(!m_filenameRelative.IsEmpty()) {
            // m_breadcrumbs.push_back(m_projectName);
            // m_breadcrumbs.push_back(m_filenameRelative);
            m_breadcrumbs.push_back(m_filename);
        } else {
            m_breadcrumbs.push_back(m_filename);
        }
    } else {
        m_classname.clear();
        m_function.clear();
    }
    Refresh();
}

void clEditorBar::OnEditorSize(wxSizeEvent& event)
{
    event.Skip();
    DoRefreshColoursAndFonts();
}

void clEditorBar::OnLeftDown(wxMouseEvent& e) { e.Skip(); }

#ifdef __WXOSX__
#define MENU_POINT(rect, menuPoint)           \
    menuPoint = rect.GetTopLeft(); \
    menuPoint.y -= 5;
#else
#define MENU_POINT(rect, menuPoint) menuPoint = rect.GetTopLeft();
#endif

void clEditorBar::OnLeftUp(wxMouseEvent& e)
{
    e.Skip();
    if(m_filenameRect.Contains(e.GetPosition())) {
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

        wxPoint menuPoint;
        MENU_POINT(m_filenameRect, menuPoint);
        
        int selection = GetPopupMenuSelectionFromUser(menu, menuPoint);
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
    } else if(m_bookmarksRect.Contains(e.GetPosition())) {
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
            wxPoint menuPoint;
            MENU_POINT(m_bookmarksRect, menuPoint);
            
            int selection = GetPopupMenuSelectionFromUser(menu, menuPoint);
            if(selection == wxID_NONE) return;
            if(M.count(selection)) {
                int lineNumber = M[selection] - 1;
                editor->CenterLine(lineNumber);
                editor->GetCtrl()->EnsureVisible(lineNumber);
                editor->GetCtrl()->EnsureCaretVisible();
                editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
            }
        }

    } else if(m_scopeRect.Contains(e.GetPosition())) {
        wxMenu menu;
        clContextMenuEvent menuEvent(wxEVT_NAVBAR_SCOPE_MENU_SHOWING);
        menuEvent.SetMenu(&menu);
        EventNotifier::Get()->ProcessEvent(menuEvent);

        if(menu.GetMenuItemCount()) {
            // We got something to display
            wxPoint menuPoint;
            MENU_POINT(m_scopeRect, menuPoint);

            // Keep track of the menu items
            std::unordered_map<int, wxString> M;
            const wxMenuItemList& list = menu.GetMenuItems();
            wxMenuItemList::const_iterator iter = list.begin();
            for(; iter != list.end(); ++iter) {
                wxMenuItem* menuItem = *iter;
                M[menuItem->GetId()] = menuItem->GetItemLabel();
            }

            // Popup the menu
            int selection = GetPopupMenuSelectionFromUser(menu, menuPoint);
            if(selection == wxID_NONE) return;
            if(M.count(selection)) {
                // Fire selection made event
                clCommandEvent selectionEvent(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE);
                selectionEvent.SetString(M[selection]);
                EventNotifier::Get()->AddPendingEvent(selectionEvent);
            }
        }
    }
}

void clEditorBar::DoRefresh() { Refresh(); }

void clEditorBar::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    if(!IsShown()) { return; }
    wxPoint pos = ScreenToClient(::wxGetMousePosition());
    if(GetClientRect().Contains(pos)) {
        m_state = m_bookmarksButtonState = m_scopeButtonState = eButtonState::kNormal;
        if(m_filenameRect.Contains(pos)) {
            m_state = eButtonState::kHover;
        } else if(m_bookmarksRect.Contains(pos)) {
            m_bookmarksButtonState = eButtonState::kHover;
        } else if(m_scopeRect.Contains(pos)) {
            m_scopeButtonState = eButtonState::kHover;
        }
        Refresh();
    }
}

void clEditorBar::OnMarkerChanged(clCommandEvent& event)
{
    event.Skip();
    if(!IsShown()) { return; }
    clDEBUG1() << "Marker Changed, updating Navigation Bar (" << event.GetFileName() << ":" << event.GetLineNumber()
               << ")";
    IEditor* editor = clGetManager()->GetActiveEditor();

    // Update the markers
    m_bookmarks.clear();
    if(editor) { editor->GetFindMarkers(m_bookmarks); }
    Refresh();
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

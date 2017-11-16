#include "ColoursAndFontsManager.h"
#include "IWorkspace.h"
#include "clEditorBar.h"
#include "clTabRenderer.h"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "lexer_configuration.h"
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/menu.h>
#include <wx/renderer.h>
#include <wx/settings.h>

#define X_SPACER 10
#define Y_SPACER 5

clEditorBar::clEditorBar(wxWindow* parent)
    : clEditorBarBase(parent)
    , m_state(eButtonState::kNormal)
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

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    memDC.SetFont(m_textFont);
    wxSize sz = memDC.GetTextExtent("Tp");
    sz.y += 2 * Y_SPACER; // 2*3 pixels
    SetSizeHints(wxSize(-1, sz.y));

    Bind(wxEVT_ENTER_WINDOW, &clEditorBar::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &clEditorBar::OnLeaveWindow, this);
    Bind(wxEVT_LEFT_UP, &clEditorBar::OnLeftUp, this);
}

clEditorBar::~clEditorBar()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
    Unbind(wxEVT_ENTER_WINDOW, &clEditorBar::OnEnterWindow, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clEditorBar::OnLeaveWindow, this);
    Unbind(wxEVT_LEFT_UP, &clEditorBar::OnLeftUp, this);
}

void clEditorBar::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }
void clEditorBar::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC bdc(this);
    PrepareDC(bdc);
    wxGCDC gcdc(bdc);
    PrepareDC(gcdc);

    wxRect rect(GetClientRect());
    gcdc.SetPen(m_bgColour);
    gcdc.SetBrush(m_bgColour);
    gcdc.DrawRectangle(rect);

    wxString fulltext;
    if(!m_classname.IsEmpty()) { fulltext << m_classname << "::"; }
    if(!m_function.IsEmpty()) { fulltext << m_function; }

    gcdc.SetFont(m_textFont);
    fulltext << "wwww"; // spacer
    // wxSize fulltextSize = gcdc.GetTextExtent(fulltext);

    // Draw the text
    wxCoord textX = X_SPACER;
    gcdc.SetFont(m_textFont);
    wxSize textSize;
    wxCoord textY = ((rect.GetHeight() - gcdc.GetTextExtent("Tp").GetHeight()) / 2);

    const int filenameButtonWidth = 400 + (4 * X_SPACER);

    if(!m_classname.IsEmpty()) {
        textSize = gcdc.GetTextExtent(m_classname);
        gcdc.SetTextForeground(m_classColour);
        gcdc.DrawText(m_classname, textX, textY);
        textX += textSize.x;

        textSize = gcdc.GetTextExtent("::");
        gcdc.SetTextForeground(m_defaultColour);
        gcdc.DrawText("::", textX, textY);
        textX += textSize.x;
    }

    if(!m_function.IsEmpty()) {
        textSize = gcdc.GetTextExtent(m_function);
        gcdc.SetTextForeground(m_functionColour);
        gcdc.DrawText(m_function, textX, textY);
        textX += textSize.x;
    }

    if(!m_breadcrumbs.IsEmpty()) {
        wxString breadcumbsText;
        for(size_t i = 0; i < m_breadcrumbs.size(); ++i) {
            breadcumbsText << m_breadcrumbs.Item(i) << " / ";
        }
        breadcumbsText.RemoveLast(3);
        wxFont guiFont = clTabRenderer::GetTabFont();
        gcdc.SetFont(guiFont);
        // Geometry
        m_filenameRect =
            wxRect(GetClientRect().GetWidth() - filenameButtonWidth - X_SPACER, 0, filenameButtonWidth, rect.GetHeight());
        DrawingUtils::DrawButton(gcdc, this, m_filenameRect, breadcumbsText, eButtonKind::kDropDown, m_state);
        textX += filenameButtonWidth;
        textX += (2 * X_SPACER);
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

    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
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

void clEditorBar::OnLeftDown(wxMouseEvent& e)
{
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

        wxPoint menuPoint = m_filenameRect.GetBottomLeft();
#ifdef __WXOSX__
        menuPoint.y += 5;
#endif
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
    }
}

void clEditorBar::OnLeftUp(wxMouseEvent& e)
{
    m_state = eButtonState::kNormal;
    Refresh();
}

void clEditorBar::OnEnterWindow(wxMouseEvent& e)
{
    m_state = eButtonState::kHover;
    Refresh();
}

void clEditorBar::OnLeaveWindow(wxMouseEvent& e)
{
    m_state = eButtonState::kNormal;
    Refresh();
}

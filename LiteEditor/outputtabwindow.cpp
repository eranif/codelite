//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : outputtabwindow.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "clThemeUpdater.h"
#include "clToolBar.h"
#include "cl_config.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "frame.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "manager.h"
#include "output_pane.h"
#include "outputtabwindow.h"
#include "pluginmanager.h"
#include "quickfindbar.h"
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(OutputTabWindow, wxPanel)
EVT_MENU(XRCID("scroll_on_output"), OutputTabWindow::OnOutputScrolls)
EVT_MENU(XRCID("clear_all_output"), OutputTabWindow::OnClearAll)
EVT_MENU(XRCID("word_wrap_output"), OutputTabWindow::OnWordWrap)
EVT_MENU(XRCID("collapse_all"), OutputTabWindow::OnCollapseAll)
EVT_MENU(XRCID("repeat_output"), OutputTabWindow::OnRepeatOutput)
EVT_MENU(XRCID("hold_pane_open"), OutputTabWindow::OnToggleHoldOpen)
EVT_UPDATE_UI(XRCID("scroll_on_output"), OutputTabWindow::OnOutputScrollsUI)
EVT_UPDATE_UI(XRCID("clear_all_output"), OutputTabWindow::OnClearAllUI)
EVT_UPDATE_UI(XRCID("word_wrap_output"), OutputTabWindow::OnWordWrapUI)
EVT_UPDATE_UI(XRCID("collapse_all"), OutputTabWindow::OnCollapseAllUI)
EVT_UPDATE_UI(XRCID("repeat_output"), OutputTabWindow::OnRepeatOutputUI)
EVT_STC_DOUBLECLICK(wxID_ANY, OutputTabWindow::OnMouseDClick)
EVT_STC_HOTSPOT_CLICK(wxID_ANY, OutputTabWindow::OnHotspotClicked)
EVT_STC_MARGINCLICK(wxID_ANY, OutputTabWindow::OnMarginClick)
END_EVENT_TABLE()

OutputTabWindow::OutputTabWindow(wxWindow* parent, wxWindowID id, const wxString& name)
    : wxPanel(parent, id)
    , m_name(name)
    , m_tb(NULL)
    , m_sci(NULL)
    , m_outputScrolls(true)
    , m_autoAppear(true)
    , m_autoAppearErrors(false)
    , m_errorsFirstLine(false)
{
    clThemeUpdater::Get().RegisterWindow(this);
    m_styler.Reset(new clFindResultsStyler());
    CreateGUIControls();
    wxTheApp->Connect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit), NULL,
                      this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit), NULL,
                      this);
    wxTheApp->Connect(wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL, this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(OutputTabWindow::OnThemeChanged), NULL,
                                  this);
    m_themeHelper = new ThemeHandlerHelper(this);
}

OutputTabWindow::~OutputTabWindow()
{
    clThemeUpdater::Get().UnRegisterWindow(this);
    m_styler.Reset(NULL);
    wxDELETE(m_themeHelper);
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(OutputTabWindow::OnThemeChanged),
                                     NULL, this);
    wxTheApp->Disconnect(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit), NULL,
                         this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit),
                         NULL, this);
    wxTheApp->Disconnect(wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL,
                         this);
}

void OutputTabWindow::DefineMarker(wxStyledTextCtrl* sci, int marker, int markerType, wxColor fore, wxColor back)
{
    sci->MarkerDefine(marker, markerType);
    sci->MarkerSetForeground(marker, fore);
    sci->MarkerSetBackground(marker, back);
}

void OutputTabWindow::InitStyle(wxStyledTextCtrl* sci, int lexer, bool folding)
{
    sci->SetLexer(lexer);
    sci->StyleClearAll();

    for(int i = 0; i <= wxSTC_STYLE_DEFAULT; i++) {
        sci->StyleSetBackground(i, DrawingUtils::GetOutputPaneBgColour());
        sci->StyleSetForeground(i, DrawingUtils::GetOutputPaneFgColour());
    }

    wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxFont font = EditorConfigST::Get()->GetLexer("text")->GetFontForSyle(0, sci);
    sci->StyleSetFont(0, font);

    sci->SetIndicatorCurrent(1);
#ifdef __WXMSW__
    int facttor = 2;
#else
    int facttor = 5;
#endif
    sci->IndicatorSetForeground(1, MakeColourLighter(wxT("GOLD"), facttor));
    sci->IndicatorSetForeground(2, MakeColourLighter(wxT("RED"), 4));
    sci->IndicatorSetStyle(1, wxSTC_INDIC_ROUNDBOX);
    sci->IndicatorSetStyle(2, wxSTC_INDIC_ROUNDBOX);
    sci->IndicatorSetUnder(1, true);
    sci->IndicatorSetUnder(2, true);

    // sci->IndicatorSetAlpha(1, 70);
    // sci->IndicatorSetAlpha(2, 70);

    sci->SetHotspotActiveUnderline(true);
    sci->SetHotspotActiveForeground(true, wxT("BLUE"));
    sci->SetHotspotSingleLine(true);
    sci->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
    sci->SetMarginMask(4, wxSTC_MASK_FOLDERS);

    sci->SetMarginWidth(0, 0);
    sci->SetMarginWidth(1, 0);
    sci->SetMarginWidth(2, 0);

    if(folding) {
        sci->SetMarginWidth(4, 16);
        sci->SetProperty(wxT("fold"), wxT("1"));
        sci->SetMarginSensitive(4, true);
    }

    // current line marker
    DefineMarker(sci, 0x7, wxSTC_MARK_ARROW, wxColor(0x00, 0x00, 0x00), wxColor(0xff, 0xc8, 0xc8));

    wxColor fore(0xff, 0xff, 0xff);
    wxColor back(0x80, 0x80, 0x80);
    DefineMarker(sci, wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, fore, back);
    DefineMarker(sci, wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW, fore, back);
    DefineMarker(sci, wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_BACKGROUND, fore, back);
    DefineMarker(sci, wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_BACKGROUND, fore, back);
    DefineMarker(sci, wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_ARROW, fore, back);
    DefineMarker(sci, wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, fore, back);
    DefineMarker(sci, wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_BACKGROUND, fore, back);

    sci->SetWrapStartIndent(4);
    sci->SetWrapVisualFlags(2);

    sci->SetScrollWidthTracking(true);

    sci->SetReadOnly(true);
}

void OutputTabWindow::CreateGUIControls()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    m_vSizer = new wxBoxSizer(wxVERTICAL);

    // Create the default scintilla control
    m_sci = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

#if defined(__WXMSW__)
    bool useDirect2D = clConfig::Get().Read("Editor/UseDirect2D", true);
    m_sci->SetTechnology(useDirect2D ? wxSTC_TECHNOLOGY_DIRECTWRITE : wxSTC_TECHNOLOGY_DEFAULT);
#endif

    // We dont really want to collect undo in the output tabs...
    m_sci->SetUndoCollection(false);
    m_sci->EmptyUndoBuffer();

    InitStyle(m_sci, wxSTC_LEX_CONTAINER, false);

    // Add the find bar
    mainSizer->Add(m_vSizer, 1, wxEXPAND | wxALL, 0);
    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();

    // Create the toolbar
    m_tb = new clToolBar(this);
    m_tb->AddTool(XRCID("hold_pane_open"), _("Keep open"), bmpLoader->LoadBitmap("ToolPin"),
                  _("Don't close this pane when an editor gets focus"), wxITEM_CHECK);

    m_tb->AddTool(XRCID("scroll_on_output"), _("Scroll on Output"), bmpLoader->LoadBitmap("link_editor"),
                  _("Scroll on Output"), wxITEM_CHECK);
    m_tb->ToggleTool(XRCID("scroll_on_output"), m_outputScrolls);

    m_tb->AddTool(XRCID("word_wrap_output"), _("Word Wrap"), bmpLoader->LoadBitmap("word_wrap"), _("Word Wrap"),
                  wxITEM_CHECK);

    m_tb->AddTool(XRCID("clear_all_output"), _("Clear All"), bmpLoader->LoadBitmap("clear"), _("Clear All"));
    m_tb->AddTool(XRCID("collapse_all"), _("Fold All Results"), bmpLoader->LoadBitmap("fold"), _("Fold All Results"));
    m_tb->AddTool(XRCID("repeat_output"), _("Repeat"), bmpLoader->LoadBitmap("debugger_restart"), _("Repeat"));

    m_tb->Realize();

    m_vSizer->Add(m_tb, 0, wxEXPAND);
    m_vSizer->Add(m_sci, 1, wxEXPAND);

    // Hide the find bar by default
    m_vSizer->Layout();
}

void OutputTabWindow::Clear()
{
    if(m_sci) {
        m_sci->SetReadOnly(false);
        m_sci->ClearAll();
        m_sci->EmptyUndoBuffer();
        m_sci->SetReadOnly(true);
    }
}

void OutputTabWindow::AppendText(const wxString& text)
{
    if(m_sci) {
        if(m_autoAppear && m_sci->GetLength() == 0) {
            ManagerST::Get()->ShowOutputPane(m_name);
        }

        //----------------------------------------------
        // enable writing
        m_sci->SetReadOnly(false);

        if(m_outputScrolls) {
            // the next 4 lines make sure that the caret is at last line
            // and is visible
            m_sci->SetSelectionEnd(m_sci->GetLength());
            m_sci->SetSelectionStart(m_sci->GetLength());
            m_sci->SetCurrentPos(m_sci->GetLength());
            m_sci->EnsureCaretVisible();
        }

        // Strip any terminal escape chars from the buffer
        wxString modText;
        ::clStripTerminalColouring(text, modText);

        // add the text
        m_sci->InsertText(m_sci->GetLength(), modText);

        if(m_outputScrolls) {
            // the next 4 lines make sure that the caret is at last line
            // and is visible
            m_sci->SetSelectionEnd(m_sci->GetLength());
            m_sci->SetSelectionStart(m_sci->GetLength());
            m_sci->SetCurrentPos(m_sci->GetLength());
            m_sci->EnsureCaretVisible();
        }

        // enable readonly mode
        m_sci->SetReadOnly(true);
    }
}

void OutputTabWindow::OnOutputScrolls(wxCommandEvent& e) { m_outputScrolls = !m_outputScrolls; }

void OutputTabWindow::OnOutputScrollsUI(wxUpdateUIEvent& e)
{
    e.Check(m_outputScrolls);
    e.Enable(true);
}

void OutputTabWindow::OnClearAll(wxCommandEvent& e) { Clear(); }

void OutputTabWindow::OnClearAllUI(wxUpdateUIEvent& e) { e.Enable(m_sci && m_sci->GetLength() > 0); }

void OutputTabWindow::OnWordWrap(wxCommandEvent& e)
{
    if(m_sci) {
        m_sci->SetWrapMode(e.IsChecked() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
    }
}

void OutputTabWindow::OnWordWrapUI(wxUpdateUIEvent& e)
{
    if(m_sci) {
        e.Enable(true);
        e.Check(m_sci->GetWrapMode() == wxSTC_WRAP_WORD);
    } else {
        e.Enable(false);
    }
}

void OutputTabWindow::OnCollapseAll(wxCommandEvent& e)
{
    if(m_sci) {
        int maxLine = m_sci->GetLineCount();
        // do two passes: first see if any folds can be collapsed
        // if not, then expand instead
        bool done = false;
        for(int pass = 0; pass < 2 && !done; pass++) {
            for(int line = 0; line < maxLine; line++) {
                // Only test fold-header lines, otherwise we get false positives from "=== Searching for..."
                if(!(m_sci->GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG)) {
                    continue;
                }
                int foldLevel = (m_sci->GetFoldLevel(line) & wxSTC_FOLDLEVELNUMBERMASK);
                if(foldLevel == 2 && m_sci->GetFoldExpanded(line) == !pass) {
                    m_sci->ToggleFold(line);
                    done = true;
                }
            }
        }
    }
}

void OutputTabWindow::OnCollapseAllUI(wxUpdateUIEvent& e) { e.Enable(m_sci && m_sci->GetLength() > 0); }

void OutputTabWindow::OnRepeatOutput(wxCommandEvent& e) { e.Skip(); }

void OutputTabWindow::OnRepeatOutputUI(wxUpdateUIEvent& e) { e.Enable(false); }

void OutputTabWindow::OnMouseDClick(wxStyledTextEvent& e) { e.Skip(); }

void OutputTabWindow::OnHotspotClicked(wxStyledTextEvent& e) { OnMouseDClick(e); }

void OutputTabWindow::OnMarginClick(wxStyledTextEvent& e)
{
    if(m_sci && e.GetMargin() == 4) {
        m_sci->ToggleFold(m_sci->LineFromPosition(e.GetPosition()));
    }
}

bool OutputTabWindow::IsFocused()
{
    wxWindow* win = wxWindow::FindFocus();
    return (win && win == m_sci);
}

void OutputTabWindow::OnEditUI(wxUpdateUIEvent& e)
{
    if(!IsFocused() || !m_sci) {
        return;
    }

    switch(e.GetId()) {
    case wxID_COPY:
        e.Enable(m_sci->GetSelectedText().IsEmpty() == false);
        break;
    case wxID_SELECTALL:
        e.Enable(true);
        break;
    default:
        break;
    }
}

void OutputTabWindow::OnEdit(wxCommandEvent& e)
{
    if(!IsFocused() || !m_sci) {
        e.Skip();
        return;
    }

    switch(e.GetId()) {
    case wxID_COPY:
        m_sci->Copy();
        break;
    case wxID_SELECTALL:
        m_sci->SelectAll();
        break;
    default:
        break;
    }
}

void OutputTabWindow::OnToggleHoldOpen(wxCommandEvent& e)
{
    int sel = wxNOT_FOUND;
    Notebook* book = clMainFrame::Get()->GetOutputPane()->GetNotebook();
    if(book && (sel = book->GetSelection()) != wxNOT_FOUND) {
        EditorConfigST::Get()->SetPaneStickiness(book->GetPageText(sel), e.IsChecked());
    }
}

void OutputTabWindow::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    if(m_sci) {
        InitStyle(m_sci, m_sci->GetLexer(), m_sci->GetMarginWidth(4) == 6);
    }
}

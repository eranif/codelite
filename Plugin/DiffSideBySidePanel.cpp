//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : DiffSideBySidePanel.cpp
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

#include "DiffConfig.h"
#include "DiffSideBySidePanel.h"
#include "art_metro.h"
#include "bookmark_manager.h"
#include "clDTL.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "plugin.h"
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>

#define RED_MARKER 5
#define GREEN_MARKER 6
#define PLACE_HOLDER_MARKER 7

#define RED_MARKER_MASK (1 << RED_MARKER)
#define GREEN_MARKER_MASK (1 << GREEN_MARKER)
#define PLACE_HOLDER_MARKER_MASK (1 << PLACE_HOLDER_MARKER)

#define MARKER_SEQUENCE 8
#define MARKER_SEQUENCE_VERTICAL 9

#define NUMBER_MARGIN_ID 0

DiffSideBySidePanel::DiffSideBySidePanel(wxWindow* parent)
    : DiffSideBySidePanelBase(parent)
    , m_darkTheme(DrawingUtils::IsThemeDark())
    , m_flags(0)
    , m_ignoreWhitespaceDiffs(false)
    , m_showLinenos(false)
    , m_showOverviewBar(true)
    , m_storeFilepaths(true)
{
    m_config.Load();
    m_showLinenos = m_config.ShowLineNumbers();
    m_showOverviewBar = !m_config.HideOverviewBar();

    // Vertical is the default; DoLayout() unsplits; but nothing implements Horizontal. So:
    if(m_config.IsSplitHorizontal()) {
        m_splitter->Unsplit();
        m_splitter->SplitHorizontally(m_splitterPageLeft, m_splitterPageRight);
    }

    EventNotifier::Get()->Connect(wxEVT_NOTIFY_PAGE_CLOSING, wxNotifyEventHandler(DiffSideBySidePanel::OnPageClosing),
                                  NULL, this);

    Connect(ID_COPY_LEFT_TO_RIGHT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(DiffSideBySidePanel::OnMenuCopyLeft2Right));
    Connect(ID_COPY_LEFT_TO_RIGHT_AND_MOVE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(DiffSideBySidePanel::OnMenuCopyLeft2Right));
    Connect(ID_COPY_RIGHT_TO_LEFT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(DiffSideBySidePanel::OnMenuCopyRight2Left));
    Connect(ID_COPY_RIGHT_TO_LEFT_AND_MOVE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(DiffSideBySidePanel::OnMenuCopyRight2Left));
    CallAfter(&DiffSideBySidePanel::DoLayout);
}

DiffSideBySidePanel::~DiffSideBySidePanel()
{
    if((m_flags & kDeleteLeftOnExit)) { ::wxRemoveFile(m_textCtrlLeftFile->GetValue()); }
    if((m_flags & kDeleteRightOnExit)) { ::wxRemoveFile(m_textCtrlRightFile->GetValue()); }

    if((m_flags & kSavePaths)) {
        m_config.SetLeftFile(m_textCtrlLeftFile->GetValue());
        m_config.SetRightFile(m_textCtrlRightFile->GetValue());
    }

    // save the configuration
    m_config.Save();

    // Clean up any temporary files
    wxString tpath(wxFileName::GetTempDir());
    tpath << wxFileName::GetPathSeparator() << "CLdiff";
    wxFileName::Rmdir(tpath, wxPATH_RMDIR_RECURSIVE);

    EventNotifier::Get()->Disconnect(wxEVT_NOTIFY_PAGE_CLOSING,
                                     wxNotifyEventHandler(DiffSideBySidePanel::OnPageClosing), NULL, this);
}

void DiffSideBySidePanel::Diff()
{
    wxFileName fnLeft(m_textCtrlLeftFile->GetValue());
    wxFileName fnRight(m_textCtrlRightFile->GetValue());

    if(!fnLeft.Exists() && !m_textCtrlLeftFile->GetValue().IsEmpty()) {
        ::wxMessageBox(wxString() << _("Left Side File:\n") << fnLeft.GetFullPath() << _(" does not exist!"),
                       "CodeLite", wxICON_ERROR | wxCENTER | wxOK);
        return;
    }

    if(!fnRight.Exists() && !m_textCtrlRightFile->GetValue().IsEmpty()) {
        ::wxMessageBox(wxString() << _("Right Side File:\n") << fnRight.GetFullPath() << _(" does not exist!"),
                       "CodeLite", wxICON_ERROR | wxCENTER | wxOK);
        return;
    }

    // Cleanup
    DoClean();

    // Prepare the views
    PrepareViews();

    // Prepare the diff
    clDTL d;
    d.Diff(m_textCtrlLeftFile->GetValue(), m_textCtrlRightFile->GetValue(),
           m_config.IsSingleViewMode() ? clDTL::kOnePane : clDTL::kTwoPanes);
    clDTL::LineInfoVec_t& resultLeft = const_cast<clDTL::LineInfoVec_t&>(d.GetResultLeft());
    clDTL::LineInfoVec_t& resultRight = const_cast<clDTL::LineInfoVec_t&>(d.GetResultRight());
    m_sequences = d.GetSequences();

    if(m_sequences.empty()) {
        // Files are the same !
        m_stcLeft->SetReadOnly(false);
        m_stcRight->SetReadOnly(false);

        // Get the user converter
        wxString leftFileText, rightFileText;
        clMBConv(conv);
        if(!FileUtils::ReadFileContent(fnLeft, leftFileText, *conv)) {
            ::wxMessageBox(_("Failed to load file content: ") + fnLeft.GetFullPath(), "CodeLite",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }
        if(!FileUtils::ReadFileContent(fnRight, rightFileText, *conv)) {
            ::wxMessageBox(_("Failed to load file content: ") + fnRight.GetFullPath(), "CodeLite",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        m_stcLeft->LoadFile(fnLeft.GetFullPath());
        m_stcRight->LoadFile(fnRight.GetFullPath());

        m_stcLeft->SetSavePoint();
        m_stcRight->SetSavePoint();

        m_stcLeft->SetReadOnly(true);
        m_stcRight->SetReadOnly(true);
        return;
    }

    if(m_ignoreWhitespaceDiffs && !m_config.IsSingleViewMode()) {
        // If the user wants to ignore whitespace diffs, go through first to remove them
        // Note that this doesn't work in singleview mode where each change is shown on
        // 2 lines, before & after. Having those unmarked would be very confusing
        for(size_t l = 0, r = 0; (l < resultLeft.size()) && (r < resultRight.size()); ++l, ++r) {
            if(resultLeft.at(l).m_type == clDTL::LINE_REMOVED || resultLeft.at(l).m_type == clDTL::LINE_ADDED) {
                wxString left(resultLeft.at(l).m_line);
                left.Replace(" ", "");
                left.Replace("\t", "");
                left.Replace("\r", "");
                wxString right(resultRight.at(r).m_line);
                right.Replace(" ", "");
                right.Replace("\t", "");
                right.Replace("\r", "");
                if(left == right) {
                    resultLeft.at(l).m_type = clDTL::LINE_COMMON;
                    resultRight.at(r).m_type = clDTL::LINE_COMMON;
                }
            }
        }
    }

    m_cur_sequence = 0; // the first line of the sequence

    // Create 2 strings "left" and "right"
    wxString leftContent, rightContent;

    m_overviewPanelMarkers.SetCount(wxMax(resultLeft.size(), resultRight.size()) + 1, 0);

    // The left pane is always the one with the deletions "-"
    for(size_t i = 0; i < resultLeft.size(); ++i) {
        if(resultLeft.at(i).m_type == clDTL::LINE_ADDED) {
            leftContent << resultLeft.at(i).m_line;
            m_leftGreenMarkers.push_back(i);
            m_overviewPanelMarkers.Item(i) = 1;

        } else if(resultLeft.at(i).m_type == clDTL::LINE_REMOVED) {
            leftContent << resultLeft.at(i).m_line;
            m_leftRedMarkers.push_back(i);
            m_overviewPanelMarkers.Item(i) = 1;

        } else if(resultLeft.at(i).m_type == clDTL::LINE_PLACEHOLDER) {
            // PLACEHOLDER
            leftContent << resultLeft.at(i).m_line;
            m_leftPlaceholdersMarkers.push_back(i);

        } else {
            // COMMON
            leftContent << resultLeft.at(i).m_line;
        }
    }

    // The right pane is always with the new additions "+"
    for(size_t i = 0; i < resultRight.size(); ++i) {
        if(resultRight.at(i).m_type == clDTL::LINE_REMOVED) {
            rightContent << resultRight.at(i).m_line;
            m_rightRedMarkers.push_back(i);
            m_overviewPanelMarkers.Item(i) = 1;

        } else if(resultRight.at(i).m_type == clDTL::LINE_ADDED) {
            rightContent << resultRight.at(i).m_line;
            m_rightGreenMarkers.push_back(i);
            m_overviewPanelMarkers.Item(i) = 1;

        } else if(resultRight.at(i).m_type == clDTL::LINE_PLACEHOLDER) {
            rightContent << resultRight.at(i).m_line;
            m_rightPlaceholdersMarkers.push_back(i);

        } else {
            // COMMON
            rightContent << resultRight.at(i).m_line;
        }
    }
    UpdateViews(leftContent, rightContent);
    m_stcLeft->SetSavePoint();
    m_stcRight->SetSavePoint();

    // Select the first diff
    wxCommandEvent dummy;
    m_cur_sequence = -1;
    OnNextDiffSequence(dummy);
}

void DiffSideBySidePanel::PrepareViews()
{
    // Prepare the views by selecting the proper syntax highlight
    wxFileName fnLeft(m_textCtrlLeftFile->GetValue());
    wxFileName fnRight(m_textCtrlRightFile->GetValue());

    bool useRightSideLexer = false;
    if(fnLeft.GetExt() == "svn-base") {
        // doing svn diff, use the lexer for the right side file
        useRightSideLexer = true;
    }

    LexerConf::Ptr_t leftLexer =
        EditorConfigST::Get()->GetLexerForFile(useRightSideLexer ? fnRight.GetFullName() : fnLeft.GetFullName());
    wxASSERT(leftLexer);

    LexerConf::Ptr_t rightLexer = EditorConfigST::Get()->GetLexerForFile(fnRight.GetFullName());
    wxASSERT(rightLexer);

    leftLexer->Apply(m_stcLeft, true);
    rightLexer->Apply(m_stcRight, true);

    wxStyledTextCtrl* stc = m_stcLeft;
    for(int n = 0; n < 2; ++n, stc = m_stcRight) {
        // Create the markers we need
        DefineMarkers(stc);

        // Turn off PP highlighting
        stc->SetProperty("lexer.cpp.track.preprocessor", "0");
        stc->SetProperty("lexer.cpp.update.preprocessor", "0");

        // Show number margins if desired
        stc->SetMarginType(NUMBER_MARGIN_ID, wxSTC_MARGIN_NUMBER);
        stc->SetMarginMask(NUMBER_MARGIN_ID,
                           ~(mmt_folds | mmt_all_bookmarks | mmt_indicator | mmt_compiler | mmt_all_breakpoints));
        int pixelWidth = 4 + 5 * stc->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
        stc->SetMarginWidth(NUMBER_MARGIN_ID, m_showLinenos ? pixelWidth : 0);
    }
}

void DiffSideBySidePanel::DefineMarkers(wxStyledTextCtrl* ctrl)
{
    wxColour red, green, grey, sideMarker;
    if(m_darkTheme) {
        red = "RED";
        green = "GREEN";
        grey = "dark grey";
        sideMarker = "CYAN";

    } else {
        red = "RED";
        green = "GREEN";
        grey = "LIGHT GREY";
        sideMarker = "BLUE";
    }

    ctrl->MarkerDefine(GREEN_MARKER, wxSTC_MARK_BACKGROUND);
    ctrl->MarkerSetBackground(GREEN_MARKER, green);
    ctrl->MarkerSetAlpha(GREEN_MARKER, 50);

    ctrl->MarkerDefine(RED_MARKER, wxSTC_MARK_BACKGROUND);
    ctrl->MarkerSetBackground(RED_MARKER, red);
    ctrl->MarkerSetAlpha(RED_MARKER, 50);

    ctrl->MarkerDefine(PLACE_HOLDER_MARKER, wxSTC_MARK_BACKGROUND);
    ctrl->MarkerSetBackground(PLACE_HOLDER_MARKER, grey);
    ctrl->MarkerSetAlpha(PLACE_HOLDER_MARKER, 50);

    ctrl->MarkerDefine(MARKER_SEQUENCE, wxSTC_MARK_FULLRECT);
    ctrl->MarkerSetBackground(MARKER_SEQUENCE, sideMarker);

    ctrl->MarkerDefine(MARKER_SEQUENCE_VERTICAL, wxSTC_MARK_VLINE);
    ctrl->MarkerSetBackground(MARKER_SEQUENCE_VERTICAL, sideMarker);
}

void DiffSideBySidePanel::UpdateViews(const wxString& left, const wxString& right)
{
    m_stcLeft->SetEditable(true);
    m_stcRight->SetEditable(true);

    m_stcLeft->SetText(left);
    m_stcLeft->MarkerDeleteAll(RED_MARKER);

    m_stcRight->SetText(right);
    m_stcRight->MarkerDeleteAll(GREEN_MARKER);

    // Show whitespaces
    m_stcRight->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    m_stcLeft->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);

    // apply the markers
    for(size_t i = 0; i < m_leftRedMarkers.size(); ++i) {
        int line = m_leftRedMarkers.at(i);
        m_stcLeft->MarkerAdd(line, RED_MARKER);
    }
    for(size_t i = 0; i < m_leftGreenMarkers.size(); ++i) {
        int line = m_leftGreenMarkers.at(i);
        m_stcLeft->MarkerAdd(line, GREEN_MARKER);
    }
    for(size_t i = 0; i < m_leftPlaceholdersMarkers.size(); ++i) {
        int line = m_leftPlaceholdersMarkers.at(i);
        m_stcLeft->MarkerAdd(line, PLACE_HOLDER_MARKER);
    }
    for(size_t i = 0; i < m_rightGreenMarkers.size(); ++i) {
        int line = m_rightGreenMarkers.at(i);
        m_stcRight->MarkerAdd(line, GREEN_MARKER);
    }
    for(size_t i = 0; i < m_rightRedMarkers.size(); ++i) {
        int line = m_rightRedMarkers.at(i);
        m_stcRight->MarkerAdd(line, RED_MARKER);
    }
    for(size_t i = 0; i < m_rightPlaceholdersMarkers.size(); ++i) {
        int line = m_rightPlaceholdersMarkers.at(i);
        m_stcRight->MarkerAdd(line, PLACE_HOLDER_MARKER);
    }

    // Restore the 'read-only' state
    m_stcLeft->SetEditable(false);
    m_stcRight->SetEditable(false);
}

void DiffSideBySidePanel::OnLeftStcPainted(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    int rightFirstLine = m_stcRight->GetFirstVisibleLine();
    int leftFirsLine = m_stcLeft->GetFirstVisibleLine();
    if(rightFirstLine != leftFirsLine) { m_stcRight->SetFirstVisibleLine(leftFirsLine); }

    int rightScrollPos = m_stcRight->GetXOffset();
    int leftScrollPos = m_stcLeft->GetXOffset();
    if(leftScrollPos != rightScrollPos) { m_stcRight->SetXOffset(leftScrollPos); }
}

void DiffSideBySidePanel::OnRightStcPainted(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    int rightFirstLine = m_stcRight->GetFirstVisibleLine();
    int leftFirsLine = m_stcLeft->GetFirstVisibleLine();
    if(rightFirstLine != leftFirsLine) { m_stcLeft->SetFirstVisibleLine(rightFirstLine); }

    int rightScrollPos = m_stcRight->GetXOffset();
    int leftScrollPos = m_stcLeft->GetXOffset();
    if(leftScrollPos != rightScrollPos) { m_stcLeft->SetXOffset(rightScrollPos); }
}

void DiffSideBySidePanel::OnLeftStcUpdateUI(wxStyledTextEvent& event)
{
    event.Skip();
    if(m_showOverviewBar) {
        // This makes the Overview bar's 'Where are we?' marker react to scrolling
        if(m_config.IsSingleViewMode()) {
            m_panelOverviewL->Refresh();
        } else if(m_config.IsSplitHorizontal()) {
            m_panelOverviewFull->Refresh();
        } else {
            m_panelOverviewR->Refresh();
        }
    }
}

void DiffSideBySidePanel::SetFilesDetails(const DiffSideBySidePanel::FileInfo& leftFile,
                                          const DiffSideBySidePanel::FileInfo& rightFile)
{
    // left file
    m_textCtrlLeftFile->ChangeValue(leftFile.filename.GetFullPath());
    m_staticTextLeft->SetLabel(leftFile.title);

    m_textCtrlRightFile->ChangeValue(rightFile.filename.GetFullPath());
    m_staticTextRight->SetLabel(rightFile.title);

    m_flags = 0x0;
    if(leftFile.readOnly) m_flags |= kLeftReadOnly;
    if(leftFile.deleteOnExit) m_flags |= kDeleteLeftOnExit;
    if(rightFile.readOnly) m_flags |= kRightReadOnly;
    if(rightFile.deleteOnExit) m_flags |= kDeleteRightOnExit;
}

void DiffSideBySidePanel::OnNextDiffSequence(wxCommandEvent& event)
{
    m_cur_sequence++; // advance the sequence
    int firstLine = m_sequences.at(m_cur_sequence).first;
    int lastLine = m_sequences.at(m_cur_sequence).second;
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcLeft);
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcRight);
}

void DiffSideBySidePanel::OnPrevDiffSequence(wxCommandEvent& event)
{
    m_cur_sequence--;
    int firstLine = m_sequences.at(m_cur_sequence).first;
    int lastLine = m_sequences.at(m_cur_sequence).second;
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcLeft);
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcRight);
}

void DiffSideBySidePanel::OnRefreshDiff(wxCommandEvent& event)
{
    if(m_stcLeft->IsModified() || m_stcRight->IsModified()) {
        wxStandardID res = ::PromptForYesNoDialogWithCheckbox(
            _("Refreshing the view will lose all your changes\nDo you want to continue?"), "DiffRefreshViewDlg");
        if(res != wxID_YES) { return; }
    }
    Diff();
    Refresh();
}

void DiffSideBySidePanel::DoClean()
{
    // Cleanup
    m_leftRedMarkers.clear();
    m_leftPlaceholdersMarkers.clear();
    m_leftGreenMarkers.clear();

    m_rightGreenMarkers.clear();
    m_rightRedMarkers.clear();
    m_rightPlaceholdersMarkers.clear();

    m_overviewPanelMarkers.Clear();
    m_sequences.clear();

    m_stcLeft->SetReadOnly(false);
    m_stcRight->SetReadOnly(false);
    m_stcLeft->SetText("");
    m_stcRight->SetText("");
    m_stcLeft->SetSavePoint();
    m_stcRight->SetSavePoint();
    m_stcLeft->SetReadOnly(true);
    m_stcRight->SetReadOnly(true);
    m_cur_sequence = wxNOT_FOUND;
}

void DiffSideBySidePanel::DoDrawSequenceMarkers(int firstLine, int lastLine, wxStyledTextCtrl* ctrl)
{
    // delete old markers
    ctrl->MarkerDeleteAll(MARKER_SEQUENCE);
    ctrl->MarkerDeleteAll(MARKER_SEQUENCE_VERTICAL);

    int line1 = firstLine;
    int line2 = lastLine;

    for(int i = line1; i < line2; ++i) {
        ctrl->MarkerAdd(i, MARKER_SEQUENCE);
    }

    // Make sure that the seq lines are visible
    int visibleLine = firstLine - 5;
    if(visibleLine < 0) { visibleLine = 0; }

    ctrl->ScrollToLine(visibleLine);
}

void DiffSideBySidePanel::OnNextDiffUI(wxUpdateUIEvent& event) { event.Enable(CanNextDiff()); }

void DiffSideBySidePanel::OnPrevDiffUI(wxUpdateUIEvent& event) { event.Enable(CanPrevDiff()); }

void DiffSideBySidePanel::OnCopyLeftToRightUI(wxUpdateUIEvent& event)
{
    event.Enable(!IsRightReadOnly() && !m_config.IsSingleViewMode());
}

void DiffSideBySidePanel::OnCopyRightToLeftUI(wxUpdateUIEvent& event)
{
    event.Enable(!IsLeftReadOnly() && !m_config.IsSingleViewMode());
}

void DiffSideBySidePanel::OnCopyLeftToRight(wxCommandEvent& event)
{
    bool moveToNext = /*m_config.GetFlags() & DiffConfig::kCopyLeftToRightAndMove*/ true;
    DoCopyCurrentSequence(m_stcLeft, m_stcRight);
    if(moveToNext && CanNextDiff()) {
        wxCommandEvent dummy;
        OnNextDiffSequence(dummy);
    }
}

void DiffSideBySidePanel::OnCopyRightToLeft(wxCommandEvent& event)
{
    bool moveToNext = /*m_config.GetFlags() & DiffConfig::kCopyRightToLeftAndMove*/ true;
    DoCopyCurrentSequence(m_stcRight, m_stcLeft);
    if(moveToNext && CanNextDiff()) {
        wxCommandEvent dummy;
        OnNextDiffSequence(dummy);
    }
}

void DiffSideBySidePanel::DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to)
{
    if(m_cur_sequence == wxNOT_FOUND) return;
    to->SetReadOnly(false);
    int fromStartPos = wxNOT_FOUND;
    int fromEndPos = wxNOT_FOUND;

    int toStartPos = wxNOT_FOUND;
    int toEndPos = wxNOT_FOUND;
    int placeHolderMarkerFirstLine = wxNOT_FOUND;
    int placeHolderMarkerLastLine = wxNOT_FOUND;
    int dummy;
    DoGetPositionsToCopy(from, fromStartPos, fromEndPos, placeHolderMarkerFirstLine, placeHolderMarkerLastLine);
    if(fromStartPos == wxNOT_FOUND || fromEndPos == wxNOT_FOUND) return;

    DoGetPositionsToCopy(to, toStartPos, toEndPos, dummy, dummy);
    if(toStartPos == wxNOT_FOUND || toEndPos == wxNOT_FOUND) return;

    // remove the old markers from the range of lines
    int toLine1 = to->LineFromPosition(toStartPos);
    int toLine2 = to->LineFromPosition(toEndPos);

    for(int i = toLine1; i < toLine2; ++i) {
        to->MarkerDelete(i, RED_MARKER);
        to->MarkerDelete(i, GREEN_MARKER);
        to->MarkerDelete(i, PLACE_HOLDER_MARKER);
        to->MarkerDelete(i, MARKER_SEQUENCE);

        from->MarkerDelete(i, RED_MARKER);
        from->MarkerDelete(i, GREEN_MARKER);
        from->MarkerDelete(i, PLACE_HOLDER_MARKER);
    }

    wxString textToCopy = from->GetTextRange(fromStartPos, fromEndPos);
    to->SetSelection(toStartPos, toEndPos);
    to->ReplaceSelection(textToCopy);
    for(int i = placeHolderMarkerFirstLine; i < placeHolderMarkerLastLine; ++i) {
        to->MarkerAdd(i, PLACE_HOLDER_MARKER);
        from->MarkerAdd(i, PLACE_HOLDER_MARKER);
    }

    // Restore the MARKER_SEQUENCE
    for(int i = toLine1; i < toLine2; ++i) {
        to->MarkerAdd(i, MARKER_SEQUENCE);
    }
    to->SetReadOnly(true);
}

void DiffSideBySidePanel::DoGetPositionsToCopy(wxStyledTextCtrl* stc, int& startPos, int& endPos,
                                               int& placeHolderMarkerFirstLine, int& placeHolderMarkerLastLine)
{
    startPos = wxNOT_FOUND;
    endPos = wxNOT_FOUND;
    placeHolderMarkerFirstLine = wxNOT_FOUND;

    int from_line = m_sequences.at(m_cur_sequence).first;
    int to_line = m_sequences.at(m_cur_sequence).second;

    for(int i = from_line; i < to_line; ++i) {
        if((stc->MarkerGet(i) & PLACE_HOLDER_MARKER_MASK)) {
            placeHolderMarkerFirstLine = i;      // first line of the placholder marker
            placeHolderMarkerLastLine = to_line; // last line of the placholder marker
            break;
        }
    }

    startPos = stc->PositionFromLine(from_line);
    endPos = stc->PositionFromLine(to_line) + stc->LineLength(to_line);
}

void DiffSideBySidePanel::DoSave(wxStyledTextCtrl* stc, const wxFileName& fn)
{
    if(!stc->IsModified()) return;

    // remove all lines that have the 'placeholder' markers
    wxString newContent = DoGetContentNoPlaceholders(stc);

    clMBConv(conv);
    if(!FileUtils::WriteFileContent(fn, newContent, *conv)) {
        ::wxMessageBox(_("Failed to save content to file: ") + fn.GetFullPath(), "CodeLite",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    stc->SetReadOnly(false);
    stc->SetText(newContent);
    stc->SetReadOnly(true);
    stc->SetSavePoint();
    stc->SetModified(false);

    // Emit a file-saved event
    EventNotifier::Get()->PostFileSavedEvent(fn.GetFullPath());

    // Reload any file opened in codelite
    EventNotifier::Get()->PostReloadExternallyModifiedEvent(false);
}

void DiffSideBySidePanel::OnSaveChanges(wxCommandEvent& event)
{
    DoSave(m_stcLeft, m_textCtrlLeftFile->GetValue());
    DoSave(m_stcRight, m_textCtrlRightFile->GetValue());
    Diff();
}

void DiffSideBySidePanel::OnSaveChangesUI(wxUpdateUIEvent& event)
{
    event.Enable((m_stcLeft->IsModified() || m_stcRight->IsModified()) && !m_config.IsSingleViewMode());
}

bool DiffSideBySidePanel::CanNextDiff()
{
    bool canNext = ((m_cur_sequence + 1) < (int)m_sequences.size());
    return !m_sequences.empty() && canNext;
}

bool DiffSideBySidePanel::CanPrevDiff()
{
    bool canPrev = ((m_cur_sequence - 1) >= 0);
    return !m_sequences.empty() && canPrev;
}

void DiffSideBySidePanel::OnCopyFileFromRight(wxCommandEvent& event) { DoCopyFileContent(m_stcRight, m_stcLeft); }

void DiffSideBySidePanel::OnCopyFileLeftToRight(wxCommandEvent& event) { DoCopyFileContent(m_stcLeft, m_stcRight); }

void DiffSideBySidePanel::DoCopyFileContent(wxStyledTextCtrl* from, wxStyledTextCtrl* to)
{
    to->SetReadOnly(false);
    wxString newContent = DoGetContentNoPlaceholders(from);
    to->SetText(newContent);
    to->SetReadOnly(true);

    // Clear RED and GREEN markers
    to->MarkerDeleteAll(RED_MARKER);
    to->MarkerDeleteAll(GREEN_MARKER);

    from->MarkerDeleteAll(RED_MARKER);
    from->MarkerDeleteAll(GREEN_MARKER);
}

void DiffSideBySidePanel::OnPageClosing(wxNotifyEvent& event)
{
    if(m_stcLeft->IsModified() || m_stcRight->IsModified()) {
        wxStandardID res = ::PromptForYesNoDialogWithCheckbox(
            _("Closing the diff viewer, will lose all your changes.\nContinue?"), "PromptDiffViewClose");
        if(res != wxID_YES) {
            event.Veto();
        } else {
            event.Skip();
        }

    } else {
        event.Skip();
    }
}

void DiffSideBySidePanel::OnHorizontal(wxCommandEvent& event)
{
    m_splitter->Unsplit();
    m_config.SetViewMode(DiffConfig::kViewHorizontalSplit);

    Diff();
    CallAfter(&DiffSideBySidePanel::DoLayout);
}

void DiffSideBySidePanel::OnHorizontalUI(wxUpdateUIEvent& event) { event.Check(m_config.IsSplitHorizontal()); }

void DiffSideBySidePanel::OnVertical(wxCommandEvent& event)
{
    m_splitter->Unsplit();
    m_config.SetViewMode(DiffConfig::kViewVerticalSplit);

    Diff();
    CallAfter(&DiffSideBySidePanel::DoLayout);
}

void DiffSideBySidePanel::OnVerticalUI(wxUpdateUIEvent& event) { event.Check(m_config.IsSplitVertical()); }

void DiffSideBySidePanel::DiffNew()
{
    m_staticTextLeft->Hide();
    m_staticTextRight->Hide();
    if(m_storeFilepaths) {
        m_flags = kSavePaths; // store the paths on exit
    }
    m_config.SetViewMode(DiffConfig::kViewVerticalSplit);
    m_splitter->Unsplit();
    m_splitter->SplitVertically(m_splitterPageLeft, m_splitterPageRight);

    // Restore last used paths
    m_config.Load();
    m_textCtrlLeftFile->ChangeValue(m_config.GetLeftFile());
    m_textCtrlRightFile->ChangeValue(m_config.GetRightFile());
}

void DiffSideBySidePanel::DiffNew(const wxFileName& left, const wxFileName& right)
{
    if(!left.Exists()) {
        ::wxMessageBox(wxString() << _("Left Side File:\n") << left.GetFullPath() << _(" does not exist!"), "CodeLite",
                       wxICON_ERROR | wxCENTER | wxOK);
        return;
    }

    if(!right.Exists()) {
        ::wxMessageBox(wxString() << _("Right Side File:\n") << right.GetFullPath() << _(" does not exist!"),
                       "CodeLite", wxICON_ERROR | wxCENTER | wxOK);
        return;
    }

    m_staticTextLeft->Hide();
    m_staticTextRight->Hide();
    if(m_storeFilepaths) {
        m_flags = kSavePaths; // store the paths on exit
    }
    m_config.SetViewMode(DiffConfig::kViewVerticalSplit);
    m_splitter->Unsplit();
    m_splitter->SplitVertically(m_splitterPageLeft, m_splitterPageRight);

    // Restore last used paths
    m_config.Load();
    m_textCtrlLeftFile->ChangeValue(left.GetFullPath());
    m_textCtrlRightFile->ChangeValue(right.GetFullPath());

    CallAfter(&DiffSideBySidePanel::Diff); // trigger a diff
}

void DiffSideBySidePanel::OnRefreshDiffUI(wxUpdateUIEvent& event) { wxUnusedVar(event); }
void DiffSideBySidePanel::OnLeftPickerUI(wxUpdateUIEvent& event) { event.Enable(!IsOriginSourceControl()); }

void DiffSideBySidePanel::OnRightPickerUI(wxUpdateUIEvent& event) { event.Enable(!IsOriginSourceControl()); }

wxString DiffSideBySidePanel::DoGetContentNoPlaceholders(wxStyledTextCtrl* stc) const
{
    wxString newContent;
    for(int i = 0; i < stc->GetLineCount(); ++i) {
        if(!(stc->MarkerGet(i) & PLACE_HOLDER_MARKER_MASK)) { newContent << stc->GetLine(i); }
    }
    return newContent;
}

void DiffSideBySidePanel::OnMenuCopyLeft2Right(wxCommandEvent& event)
{
    size_t flags = m_config.GetFlags();

    flags &= ~DiffConfig::kCopyLeftToRightAndMove;

    if(event.IsChecked() && event.GetId() == ID_COPY_LEFT_TO_RIGHT_AND_MOVE) {
        // save the new settings
        flags |= DiffConfig::kCopyLeftToRightAndMove;
    }
    m_config.SetFlags(flags);
}

void DiffSideBySidePanel::OnMenuCopyRight2Left(wxCommandEvent& event)
{
    size_t flags = m_config.GetFlags();

    flags &= ~DiffConfig::kCopyRightToLeftAndMove;

    if(event.IsChecked() && event.GetId() == ID_COPY_RIGHT_TO_LEFT_AND_MOVE) {
        // save the new settings
        flags |= DiffConfig::kCopyRightToLeftAndMove;
    }
    m_config.SetFlags(flags);
}

void DiffSideBySidePanel::OnSingleUI(wxUpdateUIEvent& event) { event.Check(m_config.IsSingleViewMode()); }

void DiffSideBySidePanel::OnSingleView(wxCommandEvent& event)
{
    m_config.SetViewMode(DiffConfig::kViewSingle);
    m_splitter->Unsplit();

    Diff();
    CallAfter(&DiffSideBySidePanel::DoLayout);
}

void DiffSideBySidePanel::DoLayout()
{
    if(m_config.IsSingleViewMode()) {
        m_panelOverviewFull->Hide();
        m_panelOverviewL->Show(m_showOverviewBar);
        m_panelOverviewR->Hide();
        m_splitter->Unsplit();
    }
    if(m_config.IsSplitHorizontal()) {
        m_panelOverviewFull->Show(m_showOverviewBar);
        m_panelOverviewL->Hide();
        m_panelOverviewR->Hide();
        m_splitter->SplitHorizontally(m_splitterPageLeft, m_splitterPageRight);
    }
    if(m_config.IsSplitVertical()) {
        m_panelOverviewFull->Hide();
        m_panelOverviewL->Hide();
        m_panelOverviewR->Show(m_showOverviewBar);
        m_splitter->SplitVertically(m_splitterPageLeft, m_splitterPageRight);
    }
    m_panelOverviewFull->GetParent()->Layout();
    m_panelOverviewL->GetParent()->Layout();
    m_panelOverviewR->GetParent()->Layout();
    GetSizer()->Layout();
    Refresh();
}
void DiffSideBySidePanel::OnMouseWheel(wxMouseEvent& event)
{
    event.Skip();
    if(::wxGetKeyState(WXK_CONTROL) && !EditorConfigST::Get()->GetOptions()->IsMouseZoomEnabled()) {
        event.Skip(false);
        return;
    }
}
void DiffSideBySidePanel::OnBrowseLeftFile(wxCommandEvent& event)
{
    wxFileName path(m_textCtrlLeftFile->GetValue());
    wxString file = ::wxFileSelector(_("Choose a file"), path.GetPath());
    if(!file.IsEmpty()) { m_textCtrlLeftFile->ChangeValue(file); }
}

void DiffSideBySidePanel::OnBrowseRightFile(wxCommandEvent& event)
{
    wxFileName path(m_textCtrlRightFile->GetValue());
    wxString file = ::wxFileSelector(_("Choose a file"), path.GetPath());
    if(!file.IsEmpty()) { m_textCtrlRightFile->ChangeValue(file); }
}

void DiffSideBySidePanel::OnIgnoreWhitespaceClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_ignoreWhitespaceDiffs = !m_ignoreWhitespaceDiffs;
    Diff();
    Refresh();
}

void DiffSideBySidePanel::OnIgnoreWhitespaceUI(wxUpdateUIEvent& event)
{
    event.Check(m_ignoreWhitespaceDiffs);
    event.Enable(!m_config.IsSingleViewMode());
}

void DiffSideBySidePanel::OnShowLinenosClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_showLinenos = !m_showLinenos;
    m_config.SetShowLineNumbers(m_showLinenos);
    Diff();
}

void DiffSideBySidePanel::OnShowLinenosUI(wxUpdateUIEvent& event) { event.Check(m_showLinenos); }

void DiffSideBySidePanel::OnShowOverviewBarClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_showOverviewBar = !m_showOverviewBar;
    // m_config thinks 'Hide' not 'Show' so that the default state will be shown
    m_config.SetHideOverviewBar(!m_showOverviewBar);

    CallAfter(&DiffSideBySidePanel::DoLayout);
}

void DiffSideBySidePanel::OnShowOverviewBarUI(wxUpdateUIEvent& event) { event.Check(m_showOverviewBar); }

void DiffSideBySidePanel::OnPaneloverviewEraseBackground(wxEraseEvent& event)
{
    if(!m_showOverviewBar) { return; }

    wxWindow* win;
    if(m_config.IsSplitHorizontal()) {
        win = m_splitter;
    } else if(m_config.IsSingleViewMode()) {
        win = m_stcLeft;
    } else {
        win = m_stcRight;
    }
    wxWindow* panel = dynamic_cast<wxWindow*>(event.GetEventObject());

    int lines = m_stcLeft->GetLineCount();
    if(!lines || !win || !panel->IsShown()) { return; }

    int yOffset = 0, x1 = panel->GetClientSize().GetWidth() - 1;
    int ht = win->GetClientSize().GetHeight();

    if(m_config.IsSplitHorizontal()) {
        // It's harder if we have to span 2 wxSTCs...
        yOffset = (ht / 2) - m_stcLeft->GetSize().GetHeight(); // The height of the text bits above the stc
        ht -= yOffset;
    } else {
        // Without this, for short files the markers will be below the corresponding diffs
        ht = wxMin(ht, lines * m_stcLeft->TextHeight(0));
    }

    int pixelsPerLine = wxMax(ht / lines, 1);
    wxDC& dc = *event.GetDC();

    // Set a distinctive background colour, as the standard panel bg is the same as its container
    // NB When deciding whether to use dark-theme colours, don't use the CodeLite theme which
    // probably won't affect the bar; measure its actual colour
    wxColour bg = panel->GetBackgroundColour();
    bool isDarkTheme = (bg.Red() + bg.Blue() + bg.Green()) < 384;
    bg.ChangeLightness(isDarkTheme ? 105 : 95);
    dc.SetBrush(bg);
    dc.SetPen(bg);
    dc.DrawRectangle(0, yOffset, x1, ht - yOffset);

    if(!m_overviewPanelMarkers.GetCount()) { return; }

    if(m_stcLeft->LinesOnScreen() < lines) {
        // Make it clearer which markers correspond to any currently-displayed diff-lines
        int topVisibleLine = m_stcLeft->GetFirstVisibleLine();
        int depth = wxMin(m_stcLeft->LinesOnScreen(), lines);

        dc.SetBrush(bg.ChangeLightness(isDarkTheme ? 110 : 90));
        dc.SetPen(bg.ChangeLightness(isDarkTheme ? 120 : 80));
        if(m_config.IsSplitHorizontal()) {
            dc.DrawRectangle(0, yOffset + topVisibleLine * (ht - yOffset) / lines, x1,
                             wxMax(depth * (ht - yOffset) / lines, 1));
        } else {
            dc.DrawRectangle(0, yOffset + topVisibleLine * ht / lines, x1, wxMax(depth * ht / lines, 1));
        }
    }

    // Finally paint the markers
    dc.SetPen(isDarkTheme ? *wxCYAN_PEN : *wxBLUE_PEN);
    dc.SetBrush(isDarkTheme ? *wxCYAN_BRUSH : *wxBLUE_BRUSH);
    for(size_t n = 0; n < (size_t)lines; ++n) {
        if(m_overviewPanelMarkers.Item(n)) {
            if(pixelsPerLine > 1) {
                if(m_config.IsSplitHorizontal()) {
                    dc.DrawRectangle(0, yOffset + (n * pixelsPerLine), x1, pixelsPerLine);
                } else {
                    dc.DrawRectangle(0, yOffset + (n * ht) / lines, x1,
                                     pixelsPerLine); // Don't use pixelsPerLine for y0; it's wrong for short files
                }
            } else {
                if(m_config.IsSplitHorizontal()) {
                    int y = (n * (ht - yOffset)) / lines;
                    dc.DrawLine(0, yOffset + y, x1, yOffset + y);
                } else {
                    int y = n * ht / lines;
                    dc.DrawLine(0, yOffset + y, x1, yOffset + y);
                }
            }
        }
    }
}

void DiffSideBySidePanel::OnPaneloverviewLeftDown(wxMouseEvent& event)
{
    event.Skip();

    if(!m_showOverviewBar) { return; }

    wxWindow* panel = static_cast<wxWindow*>(event.GetEventObject());
    wxWindow* win;
    if(m_config.IsSplitHorizontal()) {
        win = m_splitter;
    } else if(m_config.IsSingleViewMode()) {
        win = m_stcLeft;
    } else {
        win = m_stcRight;
    }

    int yOffset = 0, ht = win->GetClientSize().GetHeight();
    int pos = event.GetPosition().y;
    int lines = m_stcLeft->GetLineCount();

    if(m_config.IsSplitHorizontal()) {
        // It's harder if we have to span 2 wxSTCs...
        yOffset = (ht / 2) - m_stcLeft->GetSize().GetHeight(); // The height of the text bits above the stc
        pos -= yOffset;
        ht -= (2 * yOffset);
    }

    const int extra = 10; // Allow clicks just above/below the 'bar' to succeed
    if(!lines || !win || !panel->IsShown() || pos > (ht + extra) || pos < -extra) { return; }

    // Make the wxSTCs scroll to the line matching the mouse-click
    m_stcLeft->ScrollToLine((lines * pos) / ht);
}

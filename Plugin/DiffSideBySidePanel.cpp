#include "DiffSideBySidePanel.h"
#include "clDTL.h"
#include "editor_config.h"
#include "lexer_configuration.h"
#include "drawingutils.h"
#include <wx/msgdlg.h>

#define RED_MARKER          5
#define GREEN_MARKER        6
#define PLACE_HOLDER_MARKER 7

#define RED_MARKER_MASK          (1 << RED_MARKER)
#define GREEN_MARKER_MASK        (1 << GREEN_MARKER)
#define PLACE_HOLDER_MARKER_MASK (1 << PLACE_HOLDER_MARKER)

#define MARKER_SEQUENCE          8
#define MARKER_SEQUENCE_VERTICAL 9

DiffSideBySidePanel::DiffSideBySidePanel(wxWindow* parent)
    : DiffSideBySidePanelBase(parent)
{
}

DiffSideBySidePanel::~DiffSideBySidePanel()
{
    m_leftFile.DeleteFileIfNeeded();
    m_rightFile.DeleteFileIfNeeded();
}

void DiffSideBySidePanel::Diff()
{
    wxFileName fnLeft (m_filePickerLeft->GetPath());
    wxFileName fnRIght(m_filePickerRight->GetPath());

    if ( !fnLeft.Exists( ) ) {
        ::wxMessageBox(wxString() << _("Left Side File:\n") << fnLeft.GetFullPath() << _(" does not exist!"), "CodeLite", wxICON_ERROR|wxCENTER|wxOK);
        return;
    }

    if ( !fnRIght.Exists( ) ) {
        ::wxMessageBox(wxString() << _("Right Side File:\n") << fnRIght.GetFullPath() << _(" does not exist!"), "CodeLite", wxICON_ERROR|wxCENTER|wxOK);
        return;
    }

    // Cleanup
    DoClean();

    // Prepare the views
    PrepareViews();

    // Prepare the diff
    clDTL d;
    d.Diff(m_filePickerLeft->GetPath(), m_filePickerRight->GetPath());
    const clDTL::LineInfoVec_t &resultLeft  = d.GetResultLeft();
    const clDTL::LineInfoVec_t &resultRight = d.GetResultRight();
    m_sequences = d.GetSequences();

    if ( m_sequences.empty() ) {
        // Files are the same !
        ::wxMessageBox(_("Files are the same!"));
        m_stcLeft->SetReadOnly(false);
        m_stcRight->SetReadOnly(false);
        
        m_stcLeft->LoadFile( fnLeft.GetFullPath() );
        m_stcRight->LoadFile( fnRIght.GetFullPath() );
        
        m_stcLeft->SetSavePoint();
        m_stcRight->SetSavePoint();
        
        m_stcLeft->SetReadOnly(true);
        m_stcRight->SetReadOnly(true);
        return;
    }

    m_cur_sequence = 0; // the first line of the sequence

    // Create 2 strings "left" and "right"
    wxString leftContent, rightContent;

    // The left pane is always the one with the deletions "-"
    for (size_t i=0; i<resultLeft.size(); ++i) {
        if ( resultLeft.at(i).m_type == clDTL::LINE_REMOVED ) {
            leftContent  << resultLeft.at(i).m_line;
            m_leftRedMarkers.push_back( i );

        } else if ( resultLeft.at(i).m_type == clDTL::LINE_PLACEHOLDER ) {
            // PLACEHOLDER
            leftContent  << resultLeft.at(i).m_line;
            m_leftPlaceholdersMarkers.push_back( i );

        } else {
            // COMMON
            leftContent  << resultLeft.at(i).m_line;
        }
    }

    // The right pane is always with the new additions "+"
    for (size_t i=0; i<resultRight.size(); ++i) {
        if ( resultRight.at(i).m_type == clDTL::LINE_ADDED ) {
            rightContent  << resultRight.at(i).m_line;
            m_rightGreenMarkers.push_back( i );

        } else if ( resultRight.at(i).m_type == clDTL::LINE_PLACEHOLDER ) {
            rightContent  << resultRight.at(i).m_line;
            m_rightPlaceholdersMarkers.push_back( i );

        } else {
            // COMMON
            rightContent  << resultRight.at(i).m_line;
        }
    }
    UpdateViews(leftContent, rightContent);
    m_stcLeft->SetSavePoint();
    m_stcRight->SetSavePoint();
    
    // Select the first diff
    wxRibbonButtonBarEvent dummy;
    m_cur_sequence = -1;
    OnNextDiffSequence( dummy );
}

void DiffSideBySidePanel::PrepareViews()
{
    // Prepare the views by selecting the proper syntax highlight
    wxFileName fnLeft(m_filePickerLeft->GetPath());
    wxFileName fnRight(m_filePickerRight->GetPath());

    bool useRightSideLexer = false;
    if ( fnLeft.GetExt() == "svn-base" ) {
        // doing svn diff, use the lexer for the right side file
        useRightSideLexer = true;
    }

    LexerConfPtr leftLexer  = EditorConfigST::Get()->GetLexerForFile(useRightSideLexer ? fnRight.GetFullName() : fnLeft.GetFullName());
    wxASSERT(leftLexer);

    LexerConfPtr rightLexer = EditorConfigST::Get()->GetLexerForFile(fnRight.GetFullName());
    wxASSERT(rightLexer);

    leftLexer->Apply( m_stcLeft, true );
    rightLexer->Apply( m_stcRight, true );

    // Create the markers we need
    wxColour red, green, grey, sideMarker;
    if ( DrawingUtils::IsThemeDark() ) {
        red   = "RED";
        green = "GREEN";
        grey  = "dark grey";
        sideMarker = "CYAN";

    } else {
        red   = "RED";
        green = "GREEN";
        grey  = "LIGHT GREY";
        sideMarker = "BLUE";
    }

    m_stcLeft->MarkerDefine(RED_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcLeft->MarkerSetBackground(RED_MARKER, red);
    m_stcLeft->MarkerSetAlpha(RED_MARKER, 50);

    m_stcLeft->MarkerDefine(PLACE_HOLDER_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcLeft->MarkerSetBackground(PLACE_HOLDER_MARKER, grey);
    m_stcLeft->MarkerSetAlpha(PLACE_HOLDER_MARKER, 50);

    m_stcLeft->MarkerDefine(MARKER_SEQUENCE, wxSTC_MARK_FULLRECT);
    m_stcLeft->MarkerSetBackground(MARKER_SEQUENCE, sideMarker);
    m_stcLeft->MarkerDefine(MARKER_SEQUENCE_VERTICAL, wxSTC_MARK_VLINE);
    m_stcLeft->MarkerSetBackground(MARKER_SEQUENCE_VERTICAL, sideMarker);

    m_stcRight->MarkerDefine(GREEN_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcRight->MarkerSetBackground(GREEN_MARKER, green);
    m_stcRight->MarkerSetAlpha(GREEN_MARKER, 50);

    m_stcRight->MarkerDefine(PLACE_HOLDER_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcRight->MarkerSetBackground(PLACE_HOLDER_MARKER, grey);
    m_stcRight->MarkerSetAlpha(PLACE_HOLDER_MARKER, 50);

    m_stcRight->MarkerDefine(MARKER_SEQUENCE, wxSTC_MARK_FULLRECT);
    m_stcRight->MarkerSetBackground(MARKER_SEQUENCE, sideMarker);
    m_stcRight->MarkerDefine(MARKER_SEQUENCE_VERTICAL, wxSTC_MARK_VLINE);
    m_stcRight->MarkerSetBackground(MARKER_SEQUENCE_VERTICAL, sideMarker);

}

void DiffSideBySidePanel::UpdateViews(const wxString& left, const wxString& right)
{
    m_stcLeft->SetEditable(true);
    m_stcRight->SetEditable(true);

    m_stcLeft->SetText( left );
    m_stcLeft->MarkerDeleteAll(RED_MARKER);

    m_stcRight->SetText( right );
    m_stcRight->MarkerDeleteAll(GREEN_MARKER);

    // Show whitespaces
    m_stcRight->SetViewWhiteSpace( wxSTC_WS_VISIBLEALWAYS );
    m_stcLeft->SetViewWhiteSpace( wxSTC_WS_VISIBLEALWAYS );

    // apply the markers
    for(size_t i=0; i<m_leftRedMarkers.size(); ++i) {
        int line = m_leftRedMarkers.at(i);
        m_stcLeft->MarkerAdd(line, RED_MARKER);
    }
    for(size_t i=0; i<m_leftPlaceholdersMarkers.size(); ++i) {
        int line = m_leftPlaceholdersMarkers.at(i);
        m_stcLeft->MarkerAdd(line, PLACE_HOLDER_MARKER);
    }
    for(size_t i=0; i<m_rightGreenMarkers.size(); ++i) {
        int line = m_rightGreenMarkers.at(i);
        m_stcRight->MarkerAdd(line, GREEN_MARKER);
    }
    for(size_t i=0; i<m_rightPlaceholdersMarkers.size(); ++i) {
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
    int leftFirsLine   = m_stcLeft->GetFirstVisibleLine();
    if ( rightFirstLine != leftFirsLine ) {
        m_stcRight->SetFirstVisibleLine( leftFirsLine );
    }

    int rightScrollPos = m_stcRight->GetXOffset();
    int leftScrollPos  = m_stcLeft->GetXOffset();
    if ( leftScrollPos != rightScrollPos ) {
        m_stcRight->SetXOffset( leftScrollPos );
    }
}

void DiffSideBySidePanel::OnRightStcPainted(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    int rightFirstLine = m_stcRight->GetFirstVisibleLine();
    int leftFirsLine   = m_stcLeft->GetFirstVisibleLine();
    if ( rightFirstLine != leftFirsLine ) {
        m_stcLeft->SetFirstVisibleLine( rightFirstLine );
    }

    int rightScrollPos = m_stcRight->GetXOffset();
    int leftScrollPos  = m_stcLeft->GetXOffset();
    if ( leftScrollPos != rightScrollPos ) {
        m_stcLeft->SetXOffset( rightScrollPos );
    }
}

void DiffSideBySidePanel::SetFilesDetails(const DiffSideBySidePanel::FileInfo& leftFile, const DiffSideBySidePanel::FileInfo& rightFile)
{
    m_leftFile.DeleteFileIfNeeded();  // will also call Clear()
    m_rightFile.DeleteFileIfNeeded(); // will also call Clear()

    m_leftFile = leftFile;
    m_rightFile = rightFile;

    // left file
    m_stcLeft->SetEditable( !m_leftFile.readOnly );
    m_filePickerLeft->SetPath( m_leftFile.filename.GetFullPath() );
    m_staticTextLeft->SetLabel( m_leftFile.title );

    m_stcRight->SetEditable( !m_rightFile.readOnly );
    m_filePickerRight->SetPath( m_rightFile.filename.GetFullPath() );
    m_staticTextRight->SetLabel( m_rightFile.title );
}

void DiffSideBySidePanel::OnNextDiffSequence(wxRibbonButtonBarEvent& event)
{
    m_cur_sequence++; // advance the sequence
    int firstLine = m_sequences.at(m_cur_sequence).first;
    int lastLine  = m_sequences.at(m_cur_sequence).second;
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcLeft);
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcRight);
}

void DiffSideBySidePanel::OnPrevDiffSequence(wxRibbonButtonBarEvent& event)
{
    m_cur_sequence--;
    int firstLine = m_sequences.at(m_cur_sequence).first;
    int lastLine  = m_sequences.at(m_cur_sequence).second;
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcLeft);
    DoDrawSequenceMarkers(firstLine, lastLine, m_stcRight);
}

void DiffSideBySidePanel::OnRefreshDiff(wxRibbonButtonBarEvent& event)
{
    Diff();
}

void DiffSideBySidePanel::DoClean()
{
    // Cleanup
    m_leftRedMarkers.clear();
    m_rightGreenMarkers.clear();
    m_leftPlaceholdersMarkers.clear();
    m_rightPlaceholdersMarkers.clear();
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

    for(int i=line1; i<line2; ++i) {
        ctrl->MarkerAdd(i, MARKER_SEQUENCE);
    }

    // Make sure that the seq lines are visible
    int visibleLine = firstLine-5;
    if ( visibleLine < 0 ) {
        visibleLine = 0;
    }

    ctrl->ScrollToLine(visibleLine);
}

void DiffSideBySidePanel::OnNextDiffUI(wxUpdateUIEvent& event)
{
    event.Enable( CanNextDiff() );
}

void DiffSideBySidePanel::OnPrevDiffUI(wxUpdateUIEvent& event)
{
    event.Enable( CanPrevDiff() );
}

void DiffSideBySidePanel::OnCopyLeftToRightUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_rightFile.readOnly );
}

void DiffSideBySidePanel::OnCopyRightToLeftUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_leftFile.readOnly );
}

void DiffSideBySidePanel::OnCopyLeftToRight(wxRibbonButtonBarEvent& event)
{
    DoCopyCurrentSequence(m_stcLeft, m_stcRight);
    if ( CanNextDiff() ) {
        wxRibbonButtonBarEvent dummy;
        OnNextDiffSequence( dummy );
    }
}

void DiffSideBySidePanel::OnCopyRightToLeft(wxRibbonButtonBarEvent& event)
{
    DoCopyCurrentSequence(m_stcRight, m_stcLeft);
    if ( CanNextDiff() ) {
        wxRibbonButtonBarEvent dummy;
        OnNextDiffSequence( dummy );
    }
}

void DiffSideBySidePanel::DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to)
{
    if ( m_cur_sequence == wxNOT_FOUND )
        return;
    to->SetReadOnly(false);
    int fromStartPos = wxNOT_FOUND;
    int fromEndPos   = wxNOT_FOUND;

    int toStartPos = wxNOT_FOUND;
    int toEndPos   = wxNOT_FOUND;
    int placeHolderMarkerFirstLine = wxNOT_FOUND;
    int placeHolderMarkerLastLine  = wxNOT_FOUND;
    int dummy;
    DoGetPositionsToCopy(from, fromStartPos, fromEndPos, placeHolderMarkerFirstLine, placeHolderMarkerLastLine);
    if ( fromStartPos == wxNOT_FOUND || fromEndPos == wxNOT_FOUND )
        return;

    DoGetPositionsToCopy(to, toStartPos, toEndPos, dummy, dummy);
    if ( toStartPos == wxNOT_FOUND || toEndPos == wxNOT_FOUND )
        return;

    // remove the old markers from the range of lines
    int toLine1 = to->LineFromPosition(toStartPos);
    int toLine2 = to->LineFromPosition(toEndPos);

    for(int i=toLine1; i<toLine2; ++i) {
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
    for(int i=placeHolderMarkerFirstLine; i<placeHolderMarkerLastLine; ++i) {
        to->MarkerAdd(i, PLACE_HOLDER_MARKER);
        from->MarkerAdd(i, PLACE_HOLDER_MARKER);
    }

    // Restore the MARKER_SEQUENCE
    for(int i=toLine1; i<toLine2; ++i) {
        to->MarkerAdd(i, MARKER_SEQUENCE);
    }
    to->SetReadOnly(true);
}

void DiffSideBySidePanel::DoGetPositionsToCopy(wxStyledTextCtrl* stc,
        int& startPos,
        int& endPos,
        int& placeHolderMarkerFirstLine,
        int& placeHolderMarkerLastLine)
{
    startPos = wxNOT_FOUND;
    endPos   = wxNOT_FOUND;
    placeHolderMarkerFirstLine = wxNOT_FOUND;

    int from_line = m_sequences.at(m_cur_sequence).first;
    int to_line   = m_sequences.at(m_cur_sequence).second;

    for(int i=from_line; i<to_line; ++i) {
        if ( (stc->MarkerGet(i) & PLACE_HOLDER_MARKER_MASK) ) {
            placeHolderMarkerFirstLine = i;        // first line of the placholder marker
            placeHolderMarkerLastLine  = to_line;  // last line of the placholder marker
            break;
        }
    }

    startPos = stc->PositionFromLine(from_line);
    endPos   = stc->PositionFromLine(to_line) + stc->LineLength(to_line);
}

void DiffSideBySidePanel::DoSave(wxStyledTextCtrl* stc, const wxFileName& fn)
{
    if ( !stc->IsModified() )
        return;

    // remove all lines that have the 'placeholder' markers
    wxString newContent;
    for(int i=0; i<stc->GetLineCount(); ++i) {
        if ( !(stc->MarkerGet(i) & PLACE_HOLDER_MARKER_MASK) ) {
            newContent << stc->GetLine(i);
        }
    }

    stc->SetReadOnly(false);
    stc->SetText(newContent);
    stc->SaveFile( fn.GetFullPath() );
    stc->SetReadOnly(true);
    stc->SetSavePoint();
    stc->SetModified(false);
}

void DiffSideBySidePanel::OnSaveChanges(wxRibbonButtonBarEvent& event)
{
    DoSave( m_stcLeft,  m_leftFile.filename );
    DoSave( m_stcRight, m_rightFile.filename );
    Diff();
}

void DiffSideBySidePanel::OnSaveChangesUI(wxUpdateUIEvent& event)
{
    event.Enable( m_stcLeft->IsModified() || m_stcRight->IsModified() );
}

bool DiffSideBySidePanel::CanNextDiff()
{
    bool canNext = ((m_cur_sequence+1) < (int)m_sequences.size());
    return !m_sequences.empty() && canNext;
}

bool DiffSideBySidePanel::CanPrevDiff()
{
    bool canPrev = ( (m_cur_sequence-1) >= 0 );
    return !m_sequences.empty() && canPrev;
}

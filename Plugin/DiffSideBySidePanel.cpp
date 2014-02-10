#include "DiffSideBySidePanel.h"
#include "clDTL.h"
#include "editor_config.h"
#include "lexer_configuration.h"
#include "drawingutils.h"
#include <wx/msgdlg.h>

#define RED_MARKER          1
#define GREEN_MARKER        2
#define PLACE_HOLDER_MARKER 3

#define MARKER_SEQUENCE          4
#define MARKER_SEQUENCE_VERTICAL 5

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
        DoClean();
        // Files are the same !
        ::wxMessageBox(_("Files are the same!"));
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
    bool leftOldReadonly  = m_stcLeft->IsEditable();
    bool rightOldReadonly = m_stcRight->IsEditable();

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

    // Restore the 'read-only' states
    m_stcLeft->SetEditable(leftOldReadonly);
    m_stcRight->SetEditable(rightOldReadonly);
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

    m_stcLeft->SetText("");
    m_stcRight->SetText("");
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
    bool canNext = ((m_cur_sequence+1) < (int)m_sequences.size());
    event.Enable(!m_sequences.empty() && canNext);
}

void DiffSideBySidePanel::OnPrevDiffUI(wxUpdateUIEvent& event)
{
    bool canPrev = ( (m_cur_sequence-1) >= 0 );
    event.Enable(!m_sequences.empty() && canPrev);
}

void DiffSideBySidePanel::OnCopyLeftToRightUI(wxUpdateUIEvent& event)
{
    event.Enable( m_stcRight->IsEditable() );
}

void DiffSideBySidePanel::OnCopyRightToLeftUI(wxUpdateUIEvent& event)
{
    event.Enable( m_stcLeft->IsEditable() );
}

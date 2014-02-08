#include "DiffSideBySidePanel.h"
#include "clDTL.h"
#include "editor_config.h"
#include "lexer_configuration.h"
#include "drawingutils.h"

#define RED_MARKER          1
#define GREEN_MARKER        2
#define PLACE_HOLDER_MARKER 3
#define MARKER_SEQUENCE     4

DiffSideBySidePanel::DiffSideBySidePanel(wxWindow* parent)
    : DiffSideBySidePanelBase(parent)
{
}

DiffSideBySidePanel::~DiffSideBySidePanel()
{
}

void DiffSideBySidePanel::SetFiles(const wxFileName& left, const wxFileName& right)
{
    m_filePickerLeft->SetPath( left.GetFullPath() );
    m_filePickerRight->SetPath( right.GetFullPath() );
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
    LexerConfPtr leftLexer  = EditorConfigST::Get()->GetLexerForFile(m_filePickerLeft->GetPath());
    wxASSERT(leftLexer);

    LexerConfPtr rightLexer = EditorConfigST::Get()->GetLexerForFile(m_filePickerRight->GetPath());
    wxASSERT(rightLexer);

    leftLexer->Apply( m_stcLeft, true );
    rightLexer->Apply( m_stcRight, true );

    // Create the markers we need
    m_stcLeft->MarkerDefine(RED_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcLeft->MarkerSetBackground(RED_MARKER, "PINK");
    m_stcLeft->MarkerSetAlpha(RED_MARKER, 50);

    m_stcRight->MarkerDefine(GREEN_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcRight->MarkerSetBackground(GREEN_MARKER, "FOREST GREEN");
    m_stcRight->MarkerSetAlpha(GREEN_MARKER, 50);

    wxString colourPlaceHolder;
    if ( DrawingUtils::IsThemeDark() ) {
        colourPlaceHolder = "GREY";
    } else {
        colourPlaceHolder = "LIGHT GREY";
    }

    m_stcLeft->MarkerDefine(PLACE_HOLDER_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcLeft->MarkerSetBackground(PLACE_HOLDER_MARKER, colourPlaceHolder);
    m_stcLeft->MarkerSetAlpha(PLACE_HOLDER_MARKER, 50);
    m_stcLeft->MarkerDefine(MARKER_SEQUENCE, wxSTC_MARK_FULLRECT);
    m_stcLeft->MarkerSetBackground(MARKER_SEQUENCE, "CYAN");

    m_stcRight->MarkerDefine(PLACE_HOLDER_MARKER, wxSTC_MARK_BACKGROUND);
    m_stcRight->MarkerSetBackground(PLACE_HOLDER_MARKER, colourPlaceHolder);
    m_stcRight->MarkerSetAlpha(PLACE_HOLDER_MARKER, 50);
    m_stcRight->MarkerDefine(MARKER_SEQUENCE, wxSTC_MARK_FULLRECT);
    m_stcRight->MarkerSetBackground(MARKER_SEQUENCE, "CYAN");

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

void DiffSideBySidePanel::SetLeftFileReadOnly(bool b)
{
    m_stcLeft->SetEditable( !b );
}

void DiffSideBySidePanel::SetRightFileReadOnly(bool b)
{
    m_stcRight->SetEditable( !b );
}

void DiffSideBySidePanel::OnNextDiffSequence(wxRibbonButtonBarEvent& event)
{
    m_cur_sequence++; // advance the sequence
    int firstLine = m_sequences.at(m_cur_sequence).first;
    int lastLine  = m_sequences.at(m_cur_sequence).second;
    DoDrawSequenceMarkers(firstLine, lastLine);
}

void DiffSideBySidePanel::OnPrevDiffSequence(wxRibbonButtonBarEvent& event)
{
    m_cur_sequence--;
    int firstLine = m_sequences.at(m_cur_sequence).first;
    int lastLine  = m_sequences.at(m_cur_sequence).second;
    DoDrawSequenceMarkers(firstLine, lastLine);
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
    m_selectedSequence = std::make_pair(-1, -1);
}

void DiffSideBySidePanel::DoDrawSequenceMarkers(int firstLine, int lastLine)
{
    // delete old markers
    m_stcLeft->MarkerDeleteAll(MARKER_SEQUENCE);
    m_stcRight->MarkerDeleteAll(MARKER_SEQUENCE);

    // Add the markers
    for(int i=firstLine; i<lastLine; ++i ) {
        m_stcLeft->MarkerAdd(i, MARKER_SEQUENCE);
        m_stcRight->MarkerAdd(i, MARKER_SEQUENCE);
    }

    // Make sure that the seq lines are visible
    int visibleLine = firstLine-5;
    if ( visibleLine < 0 ) {
        visibleLine = 0;
    }

    m_stcLeft->ScrollToLine(visibleLine);
    m_stcRight->ScrollToLine(visibleLine);
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

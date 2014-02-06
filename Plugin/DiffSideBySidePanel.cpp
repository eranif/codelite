#include "DiffSideBySidePanel.h"
#include "clDTL.h"
#include "editor_config.h"
#include "lexer_configuration.h"

#define RED_MARKER 1
#define GREEN_MARKER 2

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
    // Cleanup
    m_leftGreenMarkers.clear();
    m_rightGreenMarkers.clear();
    m_stcLeft->SetText("");
    m_stcRight->SetText("");

    // Prepare the views
    PrepareViews();

    // Prepare the diff
    clDTL d;
    d.Diff(m_filePickerLeft->GetPath(), m_filePickerRight->GetPath());
    const clDTL::LineInfoVec_t &result  = d.GetResult();

    // Create 2 strings "left" and "right"
    wxString leftContent, rightContent;
    for(size_t i=0; i<result.size(); ++i) {
        // format the lines
        switch(result.at(i).m_type) {
        case clDTL::LINE_ADDED:
            leftContent  << "\n";                   // show an empty line
            rightContent << result.at(i).m_line;    // show the line
            m_leftRedMarkers.push_back( i );
            m_rightGreenMarkers.push_back( i );
            break;

        case clDTL::LINE_REMOVED:
            leftContent  << result.at(i).m_line;
            rightContent << "\n";
            m_leftGreenMarkers.push_back( i );
            m_rightRedMarkers.push_back( i );
            break;

        case clDTL::LINE_COMMON:
            leftContent  << result.at(i).m_line;
            rightContent << result.at(i).m_line;
            break;
        }
    }
    UpdateViews(leftContent, rightContent);
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
    m_stcLeft->MarkerSetBackground(RED_MARKER, "PINK");
    m_stcLeft->MarkerSetAlpha(RED_MARKER, 50);

    m_stcLeft->MarkerSetBackground(GREEN_MARKER, "FOREST GREEN");
    m_stcLeft->MarkerSetAlpha(GREEN_MARKER, 50);

    m_stcRight->MarkerSetBackground(RED_MARKER, "PINK");
    m_stcRight->MarkerSetAlpha(RED_MARKER, 50);

    m_stcRight->MarkerSetBackground(GREEN_MARKER, "FOREST GREEN");
    m_stcRight->MarkerSetAlpha(GREEN_MARKER, 50);
}

void DiffSideBySidePanel::UpdateViews(const wxString& left, const wxString& right)
{
    m_stcLeft->SetText( left );
    m_stcLeft->MarkerDeleteAll(GREEN_MARKER);
    m_stcLeft->MarkerDeleteAll(RED_MARKER);

    m_stcRight->SetText( right );
    m_stcRight->MarkerDeleteAll(GREEN_MARKER);
    m_stcRight->MarkerDeleteAll(RED_MARKER);

    // Show whitespaces
    m_stcRight->SetViewWhiteSpace( wxSTC_WS_VISIBLEALWAYS );
    m_stcLeft->SetViewWhiteSpace( wxSTC_WS_VISIBLEALWAYS );

    // apply the markers
    for(size_t i=0; i<m_leftGreenMarkers.size(); ++i) {
        int line = m_leftGreenMarkers.at(i);
        m_stcLeft->MarkerAdd(line, GREEN_MARKER);
    }
    for(size_t i=0; i<m_leftRedMarkers.size(); ++i) {
        int line = m_leftRedMarkers.at(i);
        m_stcLeft->MarkerAdd(line, RED_MARKER);
    }
    for(size_t i=0; i<m_rightGreenMarkers.size(); ++i) {
        int line = m_rightGreenMarkers.at(i);
        m_stcRight->MarkerAdd(line, GREEN_MARKER);
    }
    for(size_t i=0; i<m_rightRedMarkers.size(); ++i) {
        int line = m_rightRedMarkers.at(i);
        m_stcRight->MarkerAdd(line, RED_MARKER);
    }
}

void DiffSideBySidePanel::OnLeftStcPainted(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    int firstLine = m_stcLeft->GetFirstVisibleLine();
    m_stcRight->SetFirstVisibleLine( firstLine );
}

void DiffSideBySidePanel::OnRightStcPainted(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    int firstLine = m_stcRight->GetFirstVisibleLine();
    m_stcLeft->SetFirstVisibleLine( firstLine );
}

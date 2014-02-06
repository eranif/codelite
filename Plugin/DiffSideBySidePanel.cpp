#include "DiffSideBySidePanel.h"
#include "clDTL.h"
#include "editor_config.h"
#include "lexer_configuration.h"

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
}

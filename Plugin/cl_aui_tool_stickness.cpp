#include "cl_aui_tool_stickness.h"

clAuiToolStickness::clAuiToolStickness(wxAuiToolBar* tb, int toolId)
    : m_tb(tb)
{
    if ( m_tb ) {
        m_item = m_tb->FindTool(toolId);
    }
    
    if ( m_item ) {
        m_item->SetSticky(true);
    }
}

clAuiToolStickness::~clAuiToolStickness()
{
    if ( m_item ) {
        m_item->SetSticky(false);
    }
    
    if ( m_tb ) {
        m_tb->Refresh();
    }
}

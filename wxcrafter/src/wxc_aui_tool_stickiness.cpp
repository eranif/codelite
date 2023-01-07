#include "wxc_aui_tool_stickiness.h"

wxcAuiToolStickiness::wxcAuiToolStickiness(wxAuiToolBar* tb, int toolId)
    : m_tb(tb)
{
    if(m_tb) { m_item = m_tb->FindTool(toolId); }

    if(m_item) { m_item->SetSticky(true); }
}

wxcAuiToolStickiness::~wxcAuiToolStickiness()
{
    if(m_item) {
        m_item->SetSticky(false);
        m_tb->Refresh();
    }
}

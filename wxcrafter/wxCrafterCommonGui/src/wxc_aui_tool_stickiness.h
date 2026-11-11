#pragma once

#include <wx/aui/auibar.h>
#include <wx/bitmap.h>

class wxcAuiToolStickiness
{
    wxAuiToolBar* m_tb;
    wxAuiToolBarItem* m_item;

public:
    wxcAuiToolStickiness(wxAuiToolBar* tb, int toolId);
    virtual ~wxcAuiToolStickiness();
};

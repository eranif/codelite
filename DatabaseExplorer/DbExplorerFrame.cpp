#include "DbExplorerFrame.h"

DbExplorerFrame::DbExplorerFrame(wxWindow* parent, wxWindow* page, const wxString& label)
    : DbExplorerFrameBase(parent)
{
    page->Reparent(this);
    GetSizer()->Add(page, 1, wxEXPAND);
    GetSizer()->Layout();
    SetLabel(label);
}

DbExplorerFrame::~DbExplorerFrame() {}

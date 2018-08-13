#include "DbExplorerFrame.h"
#include "codelite_events.h"
#include "event_notifier.h"

DbExplorerFrame::DbExplorerFrame(wxWindow* parent, wxWindow* page, const wxString& label)
    : DbExplorerFrameBase(parent)
{
    page->Reparent(this);
    GetSizer()->Add(page, 1, wxEXPAND);
    GetSizer()->Layout();
    SetLabel(label);
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, [&](clCommandEvent& event) {
        Close();
        event.Skip();
    });
}

DbExplorerFrame::~DbExplorerFrame() {}

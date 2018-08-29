#include "DbExplorerFrame.h"
#include "DbViewerPanel.h"
#include "codelite_events.h"
#include "event_notifier.h"

DbExplorerFrame::DbExplorerFrame(wxWindow* parent, wxWindow* page, const wxString& label, DbViewerPanel* viewer)
    : DbExplorerFrameBase(parent, wxID_ANY, label, wxDefaultPosition, wxSize(500, 500),
                          wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)
    , m_viewer(viewer)
{
    page->Reparent(this);
    GetSizer()->Add(page, 1, wxEXPAND);
    GetSizer()->Layout();
    SetLabel(label);
    Bind(wxEVT_CLOSE_WINDOW, &DbExplorerFrame::OnClose, this);

    // If this instance does not exists, set the default size
    SetSize(wxDLG_UNIT(this, wxSize(500, 300)));
    if(parent) {
        CentreOnParent();
    }
}

DbExplorerFrame::~DbExplorerFrame() { Unbind(wxEVT_CLOSE_WINDOW, &DbExplorerFrame::OnClose, this); }

void DbExplorerFrame::OnClose(wxCloseEvent& event)
{
    event.Skip();
    m_viewer->RemoveFrame(this);
}

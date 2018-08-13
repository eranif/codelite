#include "TailFrame.h"
#include "TailPanel.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "tail.h"

TailFrame::TailFrame(wxWindow* parent, Tail* plugin)
    : TailFrameBase(parent)
    , m_plugin(plugin)
{
    EventNotifier::Get()->Bind(wxEVT_GOING_DOWN, [&](clCommandEvent& event) {
        event.Skip();
        Close();
    });
}

TailFrame::~TailFrame() {}

void TailFrame::OnClose(wxCloseEvent& event)
{
    TailData d = m_plugin->GetView()->GetTailData();
    m_plugin->CallAfter(&Tail::DockTailWindow, d);
    m_plugin->m_view = NULL;
    event.Skip();
}

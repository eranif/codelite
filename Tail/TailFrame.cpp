#include "TailFrame.h"
#include "TailPanel.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "tail.h"

TailFrame::TailFrame(wxWindow* parent, Tail* plugin)
    : TailFrameBase(parent)
    , m_plugin(plugin)
{
}

TailFrame::~TailFrame() {}

void TailFrame::OnClose(wxCloseEvent& event)
{
    // Remove us from the view
    m_plugin->GetView()->SetFrame(NULL);

    // Redock the view
    TailData d = m_plugin->GetView()->GetTailData();
    m_plugin->CallAfter(&Tail::DockTailWindow, d);
    m_plugin->m_view = NULL;
    event.Skip();
}

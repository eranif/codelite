#include "TailFrame.h"
#include "tail.h"
#include "TailPanel.h"

TailFrame::TailFrame(wxWindow* parent, Tail* plugin)
    : TailFrameBase(parent)
    , m_plugin(plugin)
{
}

TailFrame::~TailFrame() {}

void TailFrame::OnClose(wxCloseEvent& event)
{
    TailData d = m_plugin->GetView()->GetTailData();
    m_plugin->CallAfter(&Tail::DockTailWindow, d);
    m_plugin->m_view = NULL;
    event.Skip();
}

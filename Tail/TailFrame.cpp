#include "TailFrame.h"
#include "tail.h"

TailFrame::TailFrame(wxWindow* parent, Tail* plugin)
    : TailFrameBase(parent)
    , m_plugin(plugin)
{
}

TailFrame::~TailFrame() {}

void TailFrame::OnClose(wxCloseEvent& event)
{
    m_plugin->CallAfter(&Tail::DockTailWindow);
    m_plugin->m_view = NULL;
    event.Skip();
}

#ifndef TAILFRAME_H
#define TAILFRAME_H
#include "TailUI.h"

class Tail;
class TailFrame : public TailFrameBase
{
    Tail* m_plugin;

public:
    TailFrame(wxWindow* parent, Tail* plugin);
    virtual ~TailFrame();

protected:
    virtual void OnClose(wxCloseEvent& event);
};
#endif // TAILFRAME_H

#pragma once
#include "TailUI.hpp"

class Tail;
class TailFrame : public TailFrameBase
{
    Tail* m_plugin;

public:
    TailFrame(wxWindow* parent, Tail* plugin);
    virtual ~TailFrame() = default;

protected:
    virtual void OnClose(wxCloseEvent& event);
};

#ifndef LLDBTOOLTIP_H
#define LLDBTOOLTIP_H
#include "UI.h"

class LLDBPlugin;
class LLDBTooltip : public LLDBTooltipBase
{
    LLDBPlugin *m_plugin;
public:
    LLDBTooltip(wxWindow* parent, LLDBPlugin* plugin);
    virtual ~LLDBTooltip();
};
#endif // LLDBTOOLTIP_H

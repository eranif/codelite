#include "LLDBTooltip.h"
#include "LLDBPlugin.h"

LLDBTooltip::LLDBTooltip(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBTooltipBase(parent)
    , m_plugin(plugin)
{
}

LLDBTooltip::~LLDBTooltip()
{
}


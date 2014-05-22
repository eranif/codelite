#include "clCompilerEvent.h"

clCompilerEvent::~clCompilerEvent()
{
}

clCompilerEvent::clCompilerEvent(const clCompilerEvent& event)
{
    *this = event;
}

clCompilerEvent::clCompilerEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

clCompilerEvent& clCompilerEvent::operator=(const clCompilerEvent& src)
{
    clCommandEvent::operator =(src);
    m_compilers = src.m_compilers;
    return *this;
}

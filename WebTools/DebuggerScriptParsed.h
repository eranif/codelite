#ifndef DEBUGGERSCRIPTPARSED_H
#define DEBUGGERSCRIPTPARSED_H

#include "NodeMessageBase.h" // Base class: NodeMessageBase

class DebuggerScriptParsed : public NodeMessageBase
{
    wxString m_stopReason;

public:
    DebuggerScriptParsed();
    virtual ~DebuggerScriptParsed();

public:
    void Process(const JSONElement& json);
    NodeMessageBase::Ptr_t Clone();
};

#endif // DEBUGGERSCRIPTPARSED_H

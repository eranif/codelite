#ifndef NODEDBGLOCATION_H
#define NODEDBGLOCATION_H

#include "nSerializableObject.h"

class Location : public nSerializableObject
{
    wxString m_scriptId;
    long m_lineNumber = 0;

public:
    Location() {}
    virtual ~Location();
    JSONItem ToJSON(const wxString& name) const;
    void FromJSON(const JSONItem& json);
    void SetLineNumber(long lineNumber) { this->m_lineNumber = lineNumber; }
    void SetScriptId(const wxString& scriptId) { this->m_scriptId = scriptId; }
    long GetLineNumber() const { return m_lineNumber; }
    const wxString& GetScriptId() const { return m_scriptId; }
};

#endif // NODEDBGLOCATION_H

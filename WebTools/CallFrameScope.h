#ifndef CALLFRAMESCOPE_H
#define CALLFRAMESCOPE_H

#include "RemoteObject.h"
#include "nSerializableObject.h"

class CallFrameScope : public nSerializableObject
{
    wxString m_type;
    wxString m_name;
    RemoteObject m_remoteObject;

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON(const wxString& name) const;
    CallFrameScope();
    virtual ~CallFrameScope();

    void SetRemoteObject(const RemoteObject& remoteObject) { this->m_remoteObject = remoteObject; }
    void SetType(const wxString& type) { this->m_type = type; }
    const RemoteObject& GetRemoteObject() const { return m_remoteObject; }
    const wxString& GetType() const { return m_type; }
    wxString GetDisplayName() const;
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
};

#endif // CALLFRAMESCOPE_H

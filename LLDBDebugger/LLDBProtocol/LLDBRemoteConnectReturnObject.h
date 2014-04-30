#ifndef LLDBREMOTECONNECTRETURNOBJECT_H
#define LLDBREMOTECONNECTRETURNOBJECT_H

#include <wx/string.h>

class LLDBConnectReturnObject
{
    bool     m_pivotNeeded;
    wxString m_remoteHostName;

public:
    LLDBConnectReturnObject();
    ~LLDBConnectReturnObject();
    
    void Clear();
    
    void SetPivotNeeded(bool pivotNeeded) {
        this->m_pivotNeeded = pivotNeeded;
    }
    bool IsPivotNeeded() const {
        return m_pivotNeeded;
    }
    void SetRemoteHostName(const wxString& remoteHostName) {
        this->m_remoteHostName = remoteHostName;
    }
    const wxString& GetRemoteHostName() const {
        return m_remoteHostName;
    }
};

#endif // LLDBREMOTECONNECTRETURNOBJECT_H

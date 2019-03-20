#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <wx/event.h>
#include "codelite_exports.h"
#include <unordered_map>
#include <wx/string.h>
#include <vector>

enum class eServiceType {
    kUnknown = -1,
    kCodeCompletion,
};

class WXDLLIMPEXP_CL ServiceProvider : public wxEvtHandler
{
    wxString m_name;
    eServiceType m_type = eServiceType::kUnknown;
    int m_priority = 50;

public:
    typedef std::vector<ServiceProvider*> Vec_t;

public:
    ServiceProvider(const wxString& name, eServiceType type);
    virtual ~ServiceProvider();
    void SetName(const wxString& name) { this->m_name = name; }
    void SetPriority(int priority);
    void SetType(const eServiceType& type) { this->m_type = type; }
    const wxString& GetName() const { return m_name; }
    int GetPriority() const { return m_priority; }
    const eServiceType& GetType() const { return m_type; }
};

#endif // SERVICEPROVIDER_H

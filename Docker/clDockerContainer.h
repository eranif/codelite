#ifndef CLDOCKERCONTAINER_H
#define CLDOCKERCONTAINER_H

#include "wxStringHash.h"
#include <vector>
#include <wx/clntdata.h>
#include <wx/string.h>

class clDockerContainer : public wxClientData
{
public:
    enum { kStateUnknown, kStateRunning, kStatePaused, kStateExited };

protected:
    wxString m_id;
    wxString m_image;
    wxString m_command;
    wxString m_created;
    wxString m_status;
    wxString m_ports;
    wxString m_name;
    int m_state = kStateUnknown;

public:
    clDockerContainer();
    virtual ~clDockerContainer();

    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetCreated(const wxString& created) { this->m_created = created; }
    void SetId(const wxString& id) { this->m_id = id; }
    void SetImage(const wxString& image) { this->m_image = image; }
    void SetPorts(const wxString& ports) { this->m_ports = ports; }
    void SetStatus(const wxString& status) { this->m_status = status; }
    const wxString& GetCommand() const { return m_command; }
    const wxString& GetCreated() const { return m_created; }
    const wxString& GetId() const { return m_id; }
    const wxString& GetImage() const { return m_image; }
    const wxString& GetPorts() const { return m_ports; }
    const wxString& GetStatus() const { return m_status; }

    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }

    void SetState(int state) { this->m_state = state; }
    int GetState() const { return m_state; }
    
    bool Parse(const wxString& line);
    typedef std::vector<clDockerContainer> Vect_t;
    typedef std::unordered_map<wxString, clDockerContainer> Map_t;
};

#endif // CLDOCKERCONTAINER_H

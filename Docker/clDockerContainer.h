#ifndef CLDOCKERCONTAINER_H
#define CLDOCKERCONTAINER_H

#include <vector>
#include <wx/clntdata.h>
#include <wx/string.h>

class clDockerContainer : public wxClientData
{
    wxString m_id;
    wxString m_image;
    wxString m_command;
    wxString m_created;
    wxString m_status;
    wxString m_ports;
    wxString m_name;

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
    bool Parse(const wxString& line);
    typedef std::vector<clDockerContainer> Vect_t;
};

#endif // CLDOCKERCONTAINER_H

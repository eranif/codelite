#ifndef CLDOCKERIMAGE_H
#define CLDOCKERIMAGE_H

#include <vector>
#include <wx/clntdata.h>
#include <wx/string.h>

class clDockerImage : public wxClientData
{
    wxString m_id;
    wxString m_repository;
    wxString m_tag;
    wxString m_created;
    wxString m_size;

public:
    clDockerImage();
    virtual ~clDockerImage();
    
    void SetCreated(const wxString& created) { this->m_created = created; }
    void SetId(const wxString& id) { this->m_id = id; }
    void SetRepository(const wxString& repository) { this->m_repository = repository; }
    void SetSize(const wxString& size) { this->m_size = size; }
    void SetTag(const wxString& tag) { this->m_tag = tag; }
    const wxString& GetCreated() const { return m_created; }
    const wxString& GetId() const { return m_id; }
    const wxString& GetRepository() const { return m_repository; }
    const wxString& GetSize() const { return m_size; }
    const wxString& GetTag() const { return m_tag; }
    bool Parse(const wxString& line);
    typedef std::vector<clDockerImage> Vect_t;
};

#endif // CLDOCKERIMAGE_H

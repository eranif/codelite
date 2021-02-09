#ifndef CARGOTOML_HPP
#define CARGOTOML_HPP

#include <wx/filename.h>

class CargoToml
{
    wxString m_name;

public:
    CargoToml();
    virtual ~CargoToml();
    CargoToml& Load(const wxFileName& fn);

    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
};

#endif // CARGOTOML_HPP

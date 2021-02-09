#include "CargoToml.hpp"
#include <wx/fileconf.h>

CargoToml::CargoToml() {}

CargoToml::~CargoToml() {}

CargoToml& CargoToml::Load(const wxFileName& fn)
{
    wxFileConfig conf("", "", fn.GetFullPath());
    conf.SetPath("/package");
    m_name = conf.Read("name");
    return *this;
}

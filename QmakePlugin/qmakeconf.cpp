#include "qmakeconf.h"

QmakeConf::QmakeConf(const wxString &confPath)
    : wxFileConfig(wxEmptyString, wxEmptyString, confPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE)
{
}

QmakeConf::~QmakeConf()
{
    Flush();
}

wxArrayString QmakeConf::GetAllConfigurations() const
{
    wxArrayString configs;
    wxString      group;
    long          idx;

    bool cont = GetFirstGroup(group, idx);
    while (cont) {
        configs.Add( group );
        cont = GetNextGroup(group, idx);
    }

    return configs;
}

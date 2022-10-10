#include "PlatformCommon.hpp"

#include "Platform.hpp"

#include <wx/arrstr.h>

bool PlatformCommon::WhichWithVersion(const wxString& command, const std::vector<int>& versions,
                                      wxString* command_fullpath)
{
    wxArrayString names;
    names.reserve(versions.size() + 1);

    for(auto ver : versions) {
        names.Add(wxString() << command << "-" << ver);
    }
    names.Add(command);
    for(const wxString& name : names) {
        if(PLATFORM::Which(name, command_fullpath)) {
            return true;
        }
    }
    return false;
}

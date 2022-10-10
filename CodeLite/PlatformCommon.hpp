#ifndef PLAFORM_COMMON_HPP
#define PLAFORM_COMMON_HPP

#include <vector>
#include <wx/string.h>

class PlatformCommon
{
public:
    /**
     * @brief some vendros deliver binaries with "-N" where N is the version number
     * this method attempts to search for command-<N>...command (in this order)
     */
    static bool WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath);

    /**
     * @brief locaate rustup bin folder (on platforms that this is available)
     */
    static bool FindRustupToolchainBinDir(wxString* rustup_bin_dir);
};
#endif // PLAFORM_COMMON_HPP

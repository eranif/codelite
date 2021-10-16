#ifndef REMOTYCONFIG_HPP
#define REMOTYCONFIG_HPP

#include <wx/arrstr.h>
#include <wx/string.h>

class RemotyConfig
{
public:
    RemotyConfig();
    ~RemotyConfig();

    /**
     * @brief read the recent workspaces from the file system
     * @return
     */
    wxArrayString GetRecentWorkspaces() const;
    /**
     * @brief update recent workspaces with workspace
     * placing it at the top ("last used")
     * @param workspace
     */
    void UpdateRecentWorkspaces(const wxString& workspace);
};

#endif // REMOTYCONFIG_HPP

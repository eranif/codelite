#ifndef CLFILESCOLLECTOR_H
#define CLFILESCOLLECTOR_H

#include "codelite_exports.h"
#include "macros.h"

#include <functional>
#include <vector>
#include <wx/filename.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL clFilesScanner
{
public:
    struct EntryData {
        size_t flags = 0;
        wxString fullpath;
        typedef std::vector<EntryData> Vec_t;
    };

    enum eFileAttributes {
        kInvalid = 0,
        kIsFile = (1 << 0),
        kIsFolder = (1 << 1),
        kIsHidden = (1 << 2),
        kIsSymlink = (1 << 3),
    };

public:
    clFilesScanner();
    virtual ~clFilesScanner();

    /**
     * @brief collect all files matching a given pattern from a root folder
     * @param rootFolder the scan root folder
     * @param filesOutput [output] output result full path entries
     * @param filespec files spec
     * @param excludeFolders list of folder to exclude from the search
     * @return number of files found
     */
    size_t Scan(const wxString& rootFolder, std::vector<wxString>& filesOutput, const wxString& filespec = "*",
                const wxString& excludeFilespec = "", const wxStringSet_t& excludeFolders = wxStringSet_t());
    /**
     * @brief same as above, but accepts the ignore directories list in a spec format
     */
    size_t Scan(const wxString& rootFolder, std::vector<wxFileName>& filesOutput, const wxString& filespec,
                const wxString& excludeFilespec, const wxString& excludeFoldersSpec);
    size_t Scan(const wxString& rootFolder, wxArrayString& filesOutput, const wxString& filespec,
                const wxString& excludeFilespec, const wxString& excludeFoldersSpec);
    /**
     * @brief similar to the above, however, whenever a method is found, collect_cb is invoked
     */
    size_t Scan(const wxString& rootFolder, const wxString& filespec, const wxString& excludeFilespec,
                const wxString& excludeFoldersSpec, std::function<bool(const wxString&)>&& collect_cb);
    /**
     * @brief scan folder for files and folders. This function does not recurse into folders. Everything that matches
     * "matchSpec" will get collected.
     */
    size_t ScanNoRecurse(const wxString& rootFolder, clFilesScanner::EntryData::Vec_t& results,
                         const wxString& matchSpec = "*");
};

#endif // CLFILESCOLLECTOR_H

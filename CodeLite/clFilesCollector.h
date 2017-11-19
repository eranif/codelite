#ifndef CLFILESCOLLECTOR_H
#define CLFILESCOLLECTOR_H

#include "macros.h"
#include <wx/string.h>
#include <vector>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL clFilesScanner
{
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
    size_t Scan(const wxString& rootFolder,
                std::vector<wxString>& filesOutput,
                const wxString& filespec = "",
                const wxStringSet_t& excludeFolders = wxStringSet_t());
};

#endif // CLFILESCOLLECTOR_H

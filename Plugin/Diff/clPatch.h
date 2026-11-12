#pragma once

#include "cl_exception.h"
#include "codelite_exports.h"

#include <wx/filename.h>

class WXDLLIMPEXP_SDK clPatch
{
    wxFileName m_patch;

public:
    clPatch();
    virtual ~clPatch() = default;

    /**
     * @brief apply patch specified by 'pathFile' onto filename file
     * @param filename
     * @param workingDirectory execute the patch command from this folder
     * @throws clException on error
     */
    void Patch(const wxFileName& patchFile, const wxString& workingDirectory = "", const wxString& args = "");
};


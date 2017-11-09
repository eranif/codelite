#ifndef CLPATCH_H
#define CLPATCH_H

#include <wx/filename.h>
#include "codelite_exports.h"
#include "cl_exception.h"

class WXDLLIMPEXP_SDK clPatch
{
    wxFileName m_patch;

public:
    clPatch();
    virtual ~clPatch();

    /**
     * @brief apply patch sepcified by 'pathFile' onto filename file
     * @param filename
     * @param workingDirectory execute the patch command from this folder
     * @throws clException on error
     */
    void Patch(const wxFileName& patchFile,
               const wxString& workingDirectory = "",
               const wxString& args = "") ;
};

#endif // CLPATCH_H

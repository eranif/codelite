#ifndef CLTEMPFILE_HPP
#define CLTEMPFILE_HPP

#include "codelite_exports.h"
#include "fileutils.h"

#include <wx/filename.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL clTempFile
{
    wxFileName m_filename;
    bool m_deleteOnDestruct = true;

public:
    /// create a file in the default temp folder using the default values
    clTempFile(const wxString& ext = "txt");

    /// create a temp file in the provided folder using the given extension
    clTempFile(const wxString& folder, const wxString& ext);

    ~clTempFile();
    /**
     * @brief wite content to the temp file
     */
    bool Write(const wxString& content, wxMBConv& conv = wxConvUTF8);
    /**
     * @brief return the temp file path
     */
    const wxFileName& GetFileName() const { return m_filename; }

    /**
     * @brief return the tmp file full path. Wrapped with double quotes if spaces detected
     */
    wxString GetFullPath(bool wrapped_with_quotes = false) const;

    /**
     * @brief do not delete this file on destruct
     */
    void Persist();
};

#endif // CLTEMPFILE_HPP

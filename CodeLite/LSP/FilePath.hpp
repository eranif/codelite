#ifndef FILEPATH_HPP
#define FILEPATH_HPP

#include "codelite_exports.h"
#include <wx/string.h>

namespace LSP
{
class WXDLLIMPEXP_CL FilePath
{
    wxString m_path;
    bool m_isRemoteFile = false;

public:
    FilePath();
    FilePath(const wxString& path);
    virtual ~FilePath();

    void SetIsRemoteFile(bool isRemoteFile) { this->m_isRemoteFile = isRemoteFile; }
    void SetPath(const wxString& path) { this->m_path = path; }

    bool IsRemoteFile() const { return m_isRemoteFile; }
    const wxString& GetPath() const { return m_path; }
};
} // namespace LSP
#endif // FILEPATH_HPP

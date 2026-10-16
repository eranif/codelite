#pragma once

#include <optional>
#include <wx/filename.h>
#include <wx/string.h>

/// Return the path of `file` relative to `folder`, or std::nullopt if `file` is not located under `folder`
inline std::optional<wxFileName> SFTPRelativePath(const wxString& file, const wxString& folder)
{
    wxFileName fn(file);
    if (!fn.MakeRelativeTo(folder)) {
        return std::nullopt;
    }

    // A leading ".." component means that the file is outside of the folder. Do not test the text: a name can start
    // with two dots (`..cache`)
    const wxArrayString& dirs = fn.GetDirs();
    if (!dirs.IsEmpty() && dirs[0] == "..") {
        return std::nullopt;
    }
    return fn;
}

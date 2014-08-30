#ifndef PHPWROKSPACE_STORAGE_INTERFACE_H
#define PHPWROKSPACE_STORAGE_INTERFACE_H

#include <wx/filename.h>
#include <wx/string.h>
#include <vector>
#include <set>

struct FileInfo {
    wxFileName filename;
    wxString   folder;
    size_t     flags;
};
typedef std::vector<FileInfo> FileArray_t;

#endif // PHPWROKSPACE_STORAGE_INTERFACE_H

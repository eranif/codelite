#ifndef NODEFILEMANAGER_H
#define NODEFILEMANAGER_H

#include "wxStringHash.h"
#include <wx/string.h>

class NodeFileManager
{
    std::unordered_map<wxString, wxString> m_files;

public:
    NodeFileManager();
    virtual ~NodeFileManager();
    static NodeFileManager& Get();

    void Clear();
    void AddFile(const wxString& id, const wxString& url);
    wxString GetFilePath(const wxString& id) const;
};

#endif // NODEFILEMANAGER_H

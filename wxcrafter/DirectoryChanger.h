#ifndef DIRECTORY_CHANGER_H
#define DIRECTORY_CHANGER_H

#include <wx/filefn.h>
#include <wx/string.h>

class DirectoryChanger
{
    wxString m_oldPath;

public:
    DirectoryChanger(const wxString& path)
    {
        m_oldPath = ::wxGetCwd();
        ::wxSetWorkingDirectory(path);
    }

    ~DirectoryChanger() { ::wxSetWorkingDirectory(m_oldPath); }
};

#endif // DIRECTORY_CHANGER_H

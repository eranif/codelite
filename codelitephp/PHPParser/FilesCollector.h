#ifndef FILESCOLLECTOR_H
#define FILESCOLLECTOR_H

#include <wx/dir.h>
#include <wx/arrstr.h>
#include <wx/progdlg.h>
#include "macros.h"

#define FOLDER_MARKER "folder.marker"

class FilesCollector : public wxDirTraverser
{
    wxArrayString m_specArray;
    wxArrayString m_filesAndFolders;
    wxProgressDialog* m_progress;
    wxStringSet_t m_excludeFolders;
    
public:
    FilesCollector(const wxString& filespec,
                   const wxString& excludeFolders = "",
                   wxProgressDialog* progress = NULL);
    virtual ~FilesCollector();

    const wxArrayString& GetFilesAndFolders() const { return m_filesAndFolders; }
    wxArrayString& GetFilesAndFolders() { return m_filesAndFolders; }

public:
    virtual wxDirTraverseResult OnDir(const wxString& dirname);
    virtual wxDirTraverseResult OnFile(const wxString& filename);
};

#endif // FILESCOLLECTOR_H

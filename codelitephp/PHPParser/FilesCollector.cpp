#include "FilesCollector.h"
#include <wx/tokenzr.h>
#include <wx/filefn.h>
#include <wx/filename.h>

FilesCollector::FilesCollector(const wxString& filespec, const wxString& excludeFolders, wxProgressDialog* progress)
    : m_progress(progress)
{
    m_specArray = ::wxStringTokenize(filespec, ";", wxTOKEN_STRTOK);
    wxArrayString arrFolders = ::wxStringTokenize(excludeFolders, ";", wxTOKEN_STRTOK);
    m_excludeFolders.insert(arrFolders.begin(), arrFolders.end());
}

FilesCollector::~FilesCollector() {}

wxDirTraverseResult FilesCollector::OnDir(const wxString& dirname)
{
    wxFileName fndir(dirname, "");
    wxString lastPart = fndir.GetDirs().Last();
    
    if(m_excludeFolders.count(lastPart)) {
        return wxDIR_IGNORE;
    }
    
    wxFileName fn(dirname, FOLDER_MARKER);
    m_filesAndFolders.Add(fn.GetFullPath());
    if(m_progress) {
        m_progress->Pulse(wxString::Format("Scanning folder...\n%s", dirname));
    }

    return wxDIR_CONTINUE;
}

wxDirTraverseResult FilesCollector::OnFile(const wxString& filename)
{
    // add the file to our array
    wxFileName fn(filename);

    for(size_t i = 0; i < m_specArray.GetCount(); i++) {
        if(wxMatchWild(m_specArray.Item(i), fn.GetFullName())) {
            m_filesAndFolders.Add(filename);
            break;
        }
    }
    return wxDIR_CONTINUE;
}

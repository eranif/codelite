#include "FilesCollector.h"
#include <wx/tokenzr.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include "fileutils.h"

FilesCollector::FilesCollector(const wxString& filespec, const wxString& excludeFolders, wxProgressDialog* progress)
    : m_progress(progress)
{
    m_specArray = ::wxStringTokenize(filespec.Lower(), ";", wxTOKEN_STRTOK);
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
    wxFileName fn(filename);
    if(FileUtils::WildMatch(m_specArray, fn.GetFullName())) {
        m_filesAndFolders.Add(filename);
    }
    return wxDIR_CONTINUE;
}

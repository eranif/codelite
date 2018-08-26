#include "FilesCollector.h"
#include "fileutils.h"
#include <algorithm>
#include <queue>
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>

FilesCollector::FilesCollector(wxArrayString& filesAndFolders, const wxString& filespec, const wxString& excludeFolders,
                               wxProgressDialog* progress)
    : m_filesAndFolders(filesAndFolders)
    , m_progress(progress)
{
    m_specArray = ::wxStringTokenize(filespec.Lower(), ";", wxTOKEN_STRTOK);
    wxArrayString arrFolders = ::wxStringTokenize(excludeFolders, ";", wxTOKEN_STRTOK);
    m_excludeFolders.insert(arrFolders.begin(), arrFolders.end());
}

FilesCollector::~FilesCollector() {}

bool FilesCollector::IsFileOK(const wxString& filename) const
{
    if(FileUtils::WildMatch(m_specArray, filename)) { return true; }
    return false;
}

void FilesCollector::Collect(const wxString& rootFolder)
{
    if(!wxFileName::DirExists(rootFolder)) {
        m_filesAndFolders.clear();
        return;
    }
    std::queue<wxString> Q;
    Q.push(rootFolder);

    std::vector<wxString> V;
    while(!Q.empty()) {
        wxString dirpath = Q.front();
        Q.pop();

        wxDir dir(dirpath);
        if(!dir.IsOpened()) { continue; }

        wxString filename;
        bool cont = dir.GetFirst(&filename);
        while(cont) {
            // Check to see if this is a folder
            wxString fullpath;
            fullpath << dir.GetNameWithSep() << filename;
            bool isDirectory = wxFileName::DirExists(fullpath);
            if(isDirectory && (m_excludeFolders.count(filename) == 0)) {
                // A directory
                Q.push(fullpath);
                fullpath << wxFileName::GetPathSeparator() << FOLDER_MARKER;
                V.push_back(fullpath);

            } else if(!isDirectory && IsFileOK(filename)) {
                // A file
                V.push_back(fullpath);
            }
            cont = dir.GetNext(&filename);
        }
    }

    m_filesAndFolders.Alloc(V.size());
    std::for_each(V.begin(), V.end(), [&](const wxString& f) { m_filesAndFolders.push_back(f); });
}

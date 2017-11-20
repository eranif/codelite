#include "clFilesCollector.h"
#include <wx/filename.h>
#include <queue>
#include <wx/dir.h>
#include "fileutils.h"
#include <wx/tokenzr.h>
#include "file_logger.h"

clFilesScanner::clFilesScanner() {}

clFilesScanner::~clFilesScanner() {}

size_t clFilesScanner::Scan(const wxString& rootFolder,
                            std::vector<wxString>& filesOutput,
                            const wxString& filespec,
                            const wxStringSet_t& excludeFolders)
{
    filesOutput.clear();
    if(!wxFileName::DirExists(rootFolder)) {
        clDEBUG() << "clFilesScanner: No such dir:" << rootFolder << clEndl;
        return 0;
    }

    wxArrayString specArr = ::wxStringTokenize(filespec.Lower(), ";", wxTOKEN_STRTOK);
    std::queue<wxString> Q;
    Q.push(rootFolder);

    while(!Q.empty()) {
        wxString dirpath = Q.front();
        Q.pop();

        wxDir dir(dirpath);
        if(!dir.IsOpened()) {
            continue;
        }

        wxString filename;
        bool cont = dir.GetFirst(&filename);
        while(cont) {
            // Check to see if this is a folder
            wxString fullpath;
            fullpath << dir.GetNameWithSep() << filename;
            bool isDirectory = wxFileName::DirExists(fullpath);
            if(isDirectory && (excludeFolders.count(fullpath) == 0)) {
                // A directory
                Q.push(fullpath);

            } else if(!isDirectory && FileUtils::WildMatch(specArr, filename)) {
                // A file
                filesOutput.push_back(fullpath);
            }
            cont = dir.GetNext(&filename);
        }
    }
    return filesOutput.size();
}
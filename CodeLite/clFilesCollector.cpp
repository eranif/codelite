#include "clFilesCollector.h"
#include "file_logger.h"
#include "fileutils.h"
#include <queue>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>

clFilesScanner::clFilesScanner() {}

clFilesScanner::~clFilesScanner() {}

size_t clFilesScanner::Scan(const wxString& rootFolder, std::vector<wxString>& filesOutput, const wxString& filespec,
                            const wxString& excludeFilespec, const wxStringSet_t& excludeFolders)
{
    filesOutput.clear();
    if(!wxFileName::DirExists(rootFolder)) {
        clDEBUG() << "clFilesScanner: No such dir:" << rootFolder << clEndl;
        return 0;
    }

    wxArrayString specArr = ::wxStringTokenize(filespec.Lower(), ";,|", wxTOKEN_STRTOK);
    wxArrayString excludeSpecArr = ::wxStringTokenize(excludeFilespec.Lower(), ";,|", wxTOKEN_STRTOK);
    std::queue<wxString> Q;
    Q.push(rootFolder);

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
            // Use FileUtils::RealPath() here to cope with symlinks on Linux
            if(isDirectory && (excludeFolders.count(FileUtils::RealPath(fullpath)) == 0)) {
                // Traverse into this folder
                Q.push(fullpath);
            } else if(!isDirectory && FileUtils::WildMatch(excludeSpecArr, filename)) {
                // Do nothing
            } else if(!isDirectory && FileUtils::WildMatch(specArr, filename)) {
                // Include this file
                filesOutput.push_back(fullpath);
            }
            cont = dir.GetNext(&filename);
        }
    }
    return filesOutput.size();
}

size_t clFilesScanner::ScanNoRecurse(const wxString& rootFolder, clFilesScanner::EntryData::Vec_t& results,
                                     const wxString& matchSpec)
{
    results.clear();
    if(!wxFileName::DirExists(rootFolder)) {
        clDEBUG() << "clFilesScanner::ScanNoRecurse(): No such dir:" << rootFolder << clEndl;
        return 0;
    }
    wxArrayString specArr = ::wxStringTokenize(matchSpec.Lower(), ";,|", wxTOKEN_STRTOK);
    wxDir dir(rootFolder);
    if(!dir.IsOpened()) {
        clDEBUG() << "Failed to open root dir:" << rootFolder;
        return 0;
    }
    wxString dirWithSep = dir.GetNameWithSep();

    wxString filename;
    bool cont = dir.GetFirst(&filename);
    while(cont) {
        if(FileUtils::WildMatch(specArr, filename)) {
            wxString fullpath;
            fullpath << dirWithSep << filename;
            EntryData ed;
            if(FileUtils::IsDirectory(fullpath)) {
                ed.flags |= kIsFolder;
            } else {
                ed.flags |= kIsFile;
            }
            if(FileUtils::IsSymlink(fullpath)) { ed.flags |= kIsSymlink; }
            if(FileUtils::IsHidden(fullpath)) { ed.flags |= kIsHidden; }
            ed.fullpath = fullpath;
            results.push_back(ed);
        }
        cont = dir.GetNext(&filename);
    }
    return results.size();
}

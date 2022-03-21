#include "clFilesCollector.h"

#include "file_logger.h"
#include "fileutils.h"

#include <queue>
#include <vector>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>

clFilesScanner::clFilesScanner() {}

clFilesScanner::~clFilesScanner() {}

size_t clFilesScanner::Scan(const wxString& rootFolder, std::vector<wxFileName>& filesOutput, const wxString& filespec,
                            const wxString& excludeFilespec, const wxString& excludeFoldersSpec)

{
    filesOutput.clear();
    auto cb = [&filesOutput](const wxString& fullpath) -> bool {
        filesOutput.push_back(fullpath);
        return true;
    };
    return Scan(rootFolder, filespec, excludeFilespec, excludeFoldersSpec, std::move(cb));
}

size_t clFilesScanner::Scan(const wxString& rootFolder, wxArrayString& filesOutput, const wxString& filespec,
                            const wxString& excludeFilespec, const wxString& excludeFoldersSpec)

{
    filesOutput.clear();
    auto cb = [&filesOutput](const wxString& fullpath) -> bool {
        filesOutput.push_back(fullpath);
        return true;
    };
    return Scan(rootFolder, filespec, excludeFilespec, excludeFoldersSpec, std::move(cb));
}

namespace
{
bool IsRelPathContainedInSpec(const wxString& rootPath, const wxString& fullPath, const wxStringSet_t& specSet)
{
    wxFileName fp(fullPath);
    fp.MakeRelativeTo(rootPath);

    wxArrayString fpDirs = fp.GetDirs();
    fpDirs.Add(fp.GetFullName()); // Add the last (filename) part into the path array

    const wxString pathSeparators = fp.GetPathSeparators();
    for(const wxString& spec : specSet) {
        // Check if spec matches the beginning of the full path
        if(fp.GetFullPath().StartsWith(spec)) {
            return true;
        }
        // First check if spec is a path-elem (without path separators)
        // Then check if path-elem if found in the array of full path-elements
        if(!spec.Contains(pathSeparators) && (fpDirs.Index(spec) != wxNOT_FOUND)) {
            return true;
        }
    }
    return false;
}
} // namespace

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
    std::unordered_set<wxString> Visited;
    Q.push(rootFolder);
    Visited.insert(rootFolder);

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
            // Use FileUtils::RealPath() here to cope with symlinks on Linux
            bool isExcludeDir =
                isDirectory &&
                (
#if defined(__FreeBSD__)
                    ((FileUtils::IsSymlink(fullpath) && excludeFolders.count(FileUtils::RealPath(fullpath)))
#else
                    (excludeFolders.count(FileUtils::RealPath(fullpath))
#endif
                     || IsRelPathContainedInSpec(rootFolder, fullpath, excludeFolders)));
            if(isDirectory && !isExcludeDir) {
                // Traverse into this folder
                wxString realPath = FileUtils::RealPath(fullpath);
                if(Visited.insert(realPath).second) {
                    Q.push(fullpath);
                }

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

size_t clFilesScanner::Scan(const wxString& rootFolder, const wxString& filespec, const wxString& excludeFilespec,
                            const wxString& excludeFoldersSpec, std::function<bool(const wxString&)>&& collect_cb)
{
    if(!wxFileName::DirExists(rootFolder)) {
        clDEBUG() << "clFilesScanner: No such directory:" << rootFolder << clEndl;
        return 0;
    }

    wxArrayString specArr = ::wxStringTokenize(filespec.Lower(), ";,|", wxTOKEN_STRTOK);
    wxArrayString excludeSpecArr = ::wxStringTokenize(excludeFilespec.Lower(), ";,|", wxTOKEN_STRTOK);
    wxArrayString excludeFoldersSpecArr = ::wxStringTokenize(excludeFoldersSpec.Lower(), ";,|", wxTOKEN_STRTOK);
    std::queue<wxString> Q;
    std::unordered_set<wxString> Visited;

    Q.push(FileUtils::RealPath(rootFolder));
    Visited.insert(FileUtils::RealPath(rootFolder));

    size_t nCount = 0;
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
            bool isFile = !isDirectory;
            // Use FileUtils::RealPath() here to cope with symlinks on Linux
            if(isDirectory /* a folder */ &&
               !FileUtils::WildMatch(excludeFoldersSpecArr, filename) /* does not match the exclude folder spec */) {
                // Traverse into this folder
                wxString real_path = FileUtils::RealPath(fullpath);
                if(Visited.count(real_path) == 0) {
                    Visited.insert(real_path);
                    Q.push(fullpath);
                }
            } else if(isFile && /* a file */
                      !FileUtils::WildMatch(excludeSpecArr, filename) /* does not match the exclude file spec */ &&
                      FileUtils::WildMatch(specArr, filename) /* matches the file spec array */) {
                // Include this file
                if(!collect_cb(fullpath)) {
                    // requested to stop
                    return nCount;
                } else {
                    ++nCount;
                }
            }
            cont = dir.GetNext(&filename);
        }
    }
    return nCount;
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
            if(FileUtils::IsSymlink(fullpath)) {
                ed.flags |= kIsSymlink;
            }
            if(FileUtils::IsHidden(fullpath)) {
                ed.flags |= kIsHidden;
            }
            ed.fullpath = fullpath;
            results.push_back(ed);
        }
        cont = dir.GetNext(&filename);
    }
    return results.size();
}

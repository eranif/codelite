#include "NodeFileManager.h"
#include "file_logger.h"
#include "fileutils.h"
#include <algorithm>
#include <wx/filename.h>
#include <wx/uri.h>
#include <wx/regex.h>
#include <wx/filesys.h>

NodeFileManager::NodeFileManager() {}

NodeFileManager::~NodeFileManager() {}

NodeFileManager& NodeFileManager::Get()
{
    static NodeFileManager mgr;
    return mgr;
}

#define FILE_SCHEME "file://"

void NodeFileManager::Clear()
{
    m_files.clear();
    // Loop over the m_remoteFiles cache and delete the files from the file system
    std::for_each(m_remoteFiles.begin(), m_remoteFiles.end(),
                  [&](const std::unordered_map<wxString, wxString>::value_type& vt) {
                      FileUtils::RemoveFile(vt.second, "NodeFileManager::Clear()");
                  });
    m_remoteFiles.clear();
}

void NodeFileManager::AddFile(const wxString& id, const wxString& url)
{
    wxString filepath = URIToFileName(url);
    m_files.insert({ id, filepath });
}

wxString NodeFileManager::GetFilePath(const wxString& id) const
{
    wxString filePath = DoGetFilePath(id);
    if(!IsFileExists(filePath)) {
        // try the cache
        if(m_remoteFiles.count(id)) { filePath = m_remoteFiles.find(id)->second; }
    }
    return filePath;
}

bool NodeFileManager::IsFileExists(const wxString& id) const
{
    wxFileName fn(DoGetFilePath(id));
    return fn.Exists();
}

void NodeFileManager::CacheRemoteCopy(const wxString& id, const wxString& fileContent)
{
    // remove old content
    if(m_remoteFiles.count(id)) { m_remoteFiles.erase(id); }

    wxString local_copy;
    wxString filePath = DoGetFilePath(id);
    local_copy << clStandardPaths::Get().GetUserDataDir() << "/tmp/webtools/" << filePath;
    wxFileName fn(local_copy);
    // Make sure that the path to the file exists
    fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if(FileUtils::WriteFileContent(fn, fileContent, wxConvUTF8)) {
        clDEBUG() << "Localy copy of file" << filePath << "-->" << fn.GetFullPath();
        m_remoteFiles.insert({ id, fn.GetFullPath() });
    } else {
        clWARNING() << "Failed to write file content:" << fn.GetFullPath();
    }
}

wxString NodeFileManager::DoGetFilePath(const wxString& id) const
{
    std::unordered_map<wxString, wxString>::const_iterator iter = m_files.find(id);
    if(iter == m_files.end()) { return ""; }
    return iter->second;
}

wxString NodeFileManager::URIToFileName(const wxString& uri) { return wxFileSystem::URLToFileName(uri).GetFullPath(); }

wxString NodeFileManager::FileNameToURI(const wxString& uri) { return wxFileSystem::FileNameToURL(wxFileName(uri)); }

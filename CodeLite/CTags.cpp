#include "CTags.hpp"
#include "asyncprocess.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"
#include "readtags.h"

CTags::CTags(const wxString& path)
{
    m_ctagsfile = wxFileName(path, "ctags");
    if(!m_ctagsfile.FileExists()) {
        // try appending the ".codelite" folder
        m_ctagsfile.AppendDir(".codelite");
        if(!m_ctagsfile.FileExists()) {
            clWARNING() << "No such file:" << m_ctagsfile << clEndl;
            m_ctagsfile.Clear();
            return;
        }
    }

    m_file = tagsOpen(m_ctagsfile.GetFullPath().mb_str().data(), &m_fileInfo);
    if(!m_file) {
        clWARNING() << "Failed to open ctags file:" << m_ctagsfile << "." << strerror(m_fileInfo.status.error_number)
                    << clEndl;
        return;
    }
}

CTags::~CTags()
{
    if(IsOpened()) {
        tagsClose(m_file);
        m_ctagsfile.Clear();
    }
}

bool CTags::Generate(const std::vector<wxFileName>& files, const wxString& path)
{
    // create a file with the list of files
    wxString filesList;
    for(const wxFileName& file : files) {
        filesList << file.GetFullPath() << "\n";
    }
    return DoGenerate(filesList, path);
}

wxString CTags::WrapSpaces(const wxString& file)
{
    wxString fixed = file;
    if(fixed.Contains(" ")) {
        fixed.Prepend("\"").Append("\"");
    }
    return fixed;
}

bool CTags::DoGenerate(const wxString& filesContent, const wxString& path)
{
    wxFileName outputFile(path, "ctags");
    if(outputFile.GetDirCount() && outputFile.GetDirs().Last() != ".codelite") {
        outputFile.AppendDir(".codelite");
    }

    wxFileName fnFileList = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "file-list", "txt");
    FileUtils::WriteFileContent(fnFileList, filesContent);

    // Make sure we delete this file when we leave this function
    FileUtils::Deleter d(fnFileList);

    // Write the content into a temporary file
    wxFileName fnTmpTags =
        FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), outputFile.GetName(), outputFile.GetExt());

    // Invode codelite_indexer
    wxString cmd;
    wxString codeliteIndexer = clStandardPaths::Get().GetBinaryFullPath("codelite_indexer");
    cmd << WrapSpaces(codeliteIndexer) << " --batch " << WrapSpaces(fnFileList.GetFullPath()) << " "
        << WrapSpaces(fnTmpTags.GetFullPath());
    clDEBUG() << "CTags:" << cmd << clEndl;
    IProcess::Ptr_t proc(::CreateSyncProcess(cmd));
    if(proc) {
        wxString dummy;
        proc->WaitForTerminate(dummy);
    }

    if(!wxRenameFile(fnTmpTags.GetFullPath(), outputFile.GetFullPath())) {
        clWARNING() << "wxRename error:" << fnTmpTags << "->" << outputFile << clEndl;
        return false;
    }
    clDEBUG() << "Tags file:" << outputFile << clEndl;
    return true;
}

bool CTags::Generate(const std::vector<std::string>& files, const wxString& path)
{
    // create a file with the list of files
    wxString filesList;
    for(const std::string& file : files) {
        filesList << file << "\n";
    }
    return DoGenerate(filesList, path);
}

size_t CTags::FindTags(const wxArrayString& filter, std::vector<TagEntryPtr>& tags, size_t flags)
{
    if(filter.empty()) {
        return 0;
    }

    size_t nCount = FindTags(filter.Item(0), tags, flags);
    if(nCount == 0) {
        return 0;
    }

    std::vector<TagEntryPtr> tmptags;
    tmptags.reserve(nCount);
    for(auto tag : tags) {
        bool ok = true;
        for(size_t i = 1; i < filter.size(); ++i) {
            if(!tag->GetName().Contains(filter[i])) {
                ok = false;
                break;
            }
        }
        if(ok) {
            tmptags.emplace_back(tag);
        }
    }
    tags.swap(tmptags);
    return tmptags.size();
}

size_t CTags::FindTags(const wxString& filter, const wxString& path, std::vector<TagEntryPtr>& tags, size_t flags)
{
    CTags t(path);
    if(!t.IsOpened()) {
        return 0;
    }
    return t.FindTags(filter, tags, flags);
}

size_t CTags::FindTags(const wxArrayString& filter, const wxString& path, std::vector<TagEntryPtr>& tags, size_t flags)
{
    CTags t(path);
    if(!t.IsOpened()) {
        return 0;
    }
    return t.FindTags(filter, tags, flags);
}

size_t CTags::FindTags(const wxString& filter, std::vector<TagEntryPtr>& tags, size_t flags)
{
    if(!IsOpened()) {
        return 0;
    }

    tags.reserve(1000); // assume 1000 entries
    tagEntry entry;
    tagResult result = tagsFind(m_file, &entry, filter.mb_str(wxConvUTF8).data(), flags);
    while(result == TagSuccess) {
        TagEntryPtr t(new TagEntry(entry));
        tags.emplace_back(t);
        result = tagsFindNext(m_file, &entry);
    }
    return tags.size();
}

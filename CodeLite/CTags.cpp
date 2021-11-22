#include "CTags.hpp"
#include "asyncprocess.h"
#include "cl_standard_paths.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileutils.h"
#include "readtags.h"
#include <wx/tokenzr.h>

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

bool CTags::Generate(const std::vector<wxFileName>& files, const wxString& path, const wxString& codelite_indexer)
{
    // create a file with the list of files
    wxString filesList;
    for(const wxFileName& file : files) {
        filesList << file.GetFullPath() << "\n";
    }
    return DoGenerate(filesList, path, codelite_indexer);
}

wxString CTags::WrapSpaces(const wxString& file)
{
    wxString fixed = file;
    if(fixed.Contains(" ")) {
        fixed.Prepend("\"").Append("\"");
    }
    return fixed;
}

bool CTags::DoGenerate(const wxString& filesContent, const wxString& path, const wxString& codelite_indexer)
{
    clDEBUG() << "Generating ctags files" << clEndl;
    wxFileName outputFile(path, "ctags");

    wxFileName fnFileList = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "file-list", "txt");
    FileUtils::WriteFileContent(fnFileList, filesContent);

    // Make sure we delete this file when we leave this function
    FileUtils::Deleter d(fnFileList);

    // Write the content into a temporary file
    wxFileName fnTmpTags =
        FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), outputFile.GetName(), outputFile.GetExt());

    // Pass ctags command line via the environment variable
    wxString ctagsCmd = "--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ";
    ctagsCmd << TagsManagerST::Get()->GetCtagsOptions().ToString();

    clEnvList_t envList = { { "CTAGS_BATCH_CMD", ctagsCmd } };

    // Invoke codelite_indexer
    wxString invokeCmd;
    wxString codeliteIndexer =
        codelite_indexer.empty() ? clStandardPaths::Get().GetBinaryFullPath("codelite_indexer") : codelite_indexer;
    invokeCmd << WrapSpaces(codeliteIndexer) << " --batch " << WrapSpaces(fnFileList.GetFullPath()) << " "
              << WrapSpaces(fnTmpTags.GetFullPath());
    clDEBUG() << "CTags:" << invokeCmd << clEndl;
    IProcess::Ptr_t proc(::CreateSyncProcess(invokeCmd, IProcessCreateDefault, wxEmptyString, &envList));
    if(proc) {
        wxString dummy;
        proc->WaitForTerminate(dummy);
    }

    if(!::wxRenameFile(fnTmpTags.GetFullPath(), outputFile.GetFullPath())) {
        clDEBUG() << "Generating ctags files... ended with an error" << clEndl;
        clWARNING() << "wxRename error:" << fnTmpTags << "->" << outputFile << clEndl;
        return false;
    }
    clDEBUG() << "Tags file:" << outputFile << clEndl;
    clDEBUG() << "Generating ctags files... success" << clEndl;
    return true;
}

bool CTags::Generate(const wxArrayString& files, const wxString& path, const wxString& codelite_indexer)
{
    // create a file with the list of files
    wxString filesList;
    for(const auto& file : files) {
        filesList << file << "\n";
    }
    return DoGenerate(filesList, path, codelite_indexer);
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

TagTreePtr CTags::GetTagsTreeForFile(wxString& fullpath)
{
    fullpath.Clear();
    if(!IsOpened()) {
        return nullptr;
    }

    if(!m_textFile.IsOpened()) {
        if(!m_textFile.Open(m_ctagsfile.GetFullPath())) {
            return nullptr;
        }
    }

    // at eof?
    if(m_curline == (m_textFile.GetLineCount() - 1)) {
        return nullptr;
    }

    std::vector<TagEntry> tags;
    tags.reserve(1000); // pre allocate memory
    size_t i = m_curline;
    for(; i < m_textFile.GetLineCount(); ++i) {
        m_curline = i;
        wxString& line = m_textFile.GetLine(i);
        line.Trim(false).Trim();
        if(line.empty()) {
            continue;
        }
        // construct a tag from the line
        TagEntry t;
        t.FromLine(line);
        // add it to the vector
        tags.push_back(t);

        if(fullpath.empty()) {
            fullpath = tags[0].GetFile();
        }

        if(fullpath != tags.back().GetFile()) {
            // we moved to another file, remove the last tag and leave the loop
            tags.pop_back();
            break;
        }
    }
    if(tags.empty()) {
        return nullptr;
    }
    return TreeFromTags(tags);
}

TagTreePtr CTags::TreeFromTags(std::vector<TagEntry>& tags)
{
    // Load the records and build a language tree
    TagEntry root;
    root.SetName(wxT("<ROOT>"));

    TagTreePtr tree(new TagTree(wxT("<ROOT>"), root));

    for(auto& tag : tags) {
        // Add the tag to the tree, locals are not added to the
        // tree
        if(tag.GetKind() != wxT("local")) {
            tree->AddEntry(tag);
        }
    }
    return tree;
}

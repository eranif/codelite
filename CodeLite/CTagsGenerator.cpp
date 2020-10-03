#include "CTagsGenerator.hpp"
#include "asyncprocess.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"

CTagsGenerator::CTagsGenerator() {}

CTagsGenerator::~CTagsGenerator() {}

bool CTagsGenerator::Generate(const std::vector<wxFileName>& files, const wxFileName& outputFile)
{
    // create a file with the list of files
    wxString filesList;
    for(const wxFileName& file : files) {
        filesList << file.GetFullPath() << "\n";
    }

    return DoGenerate(filesList, outputFile);
}

wxString CTagsGenerator::WrapSpaces(const wxString& file) const
{
    wxString fixed = file;
    if(fixed.Contains(" ")) {
        fixed.Prepend("\"").Append("\"");
    }
    return fixed;
}

bool CTagsGenerator::DoGenerate(const wxString& filesContent, const wxFileName& outputFile)
{
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
    clDEBUG() << "CTagsGenerator:" << cmd << clEndl;
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

bool CTagsGenerator::Generate(const std::vector<std::string>& files, const wxFileName& outputFile)
{
    // create a file with the list of files
    wxString filesList;
    for(const std::string& file : files) {
        filesList << file << "\n";
    }
    return DoGenerate(filesList, outputFile);
}

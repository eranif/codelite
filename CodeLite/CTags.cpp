#include "CTags.hpp"

#include "asyncprocess.h"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileutils.h"
#include "readtags.h"

#include <wx/tokenzr.h>

wxString CTags::WrapSpaces(const wxString& file)
{
    wxString fixed = file;
    if(fixed.Contains(" ")) {
        fixed.Prepend("\"").Append("\"");
    }
    return fixed;
}

bool CTags::DoGenerate(const wxString& filesContent, const wxString& codelite_indexer, const wxString& ctags_args,
                       wxString* output)
{
    clDEBUG() << "Generating ctags files" << clEndl;

    // ensure temp directory exists
    wxFileName::Mkdir(clStandardPaths::Get().GetTempDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // create 2 files: 1 containing list of files to parse and the second is the output file from ctags
    clTempFile fn_output_file("txt");
    clTempFile fn_list_of_files("list");
    fn_list_of_files.Write(filesContent);

    // Pass ctags command line via the environment variable
    wxString ctagsCmd = ctags_args;
    if(ctagsCmd.empty()) {
        ctagsCmd = "--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ";
    }

    ctagsCmd << " --language-force=c++ "; // force c++
    ctagsCmd << TagsManagerST::Get()->GetCtagsOptions().ToString();

    clEnvList_t envList = { { "CTAGS_BATCH_CMD", ctagsCmd } };

    // Invoke codelite_indexer
    wxString execute_command;
    execute_command << WrapSpaces(codelite_indexer) << " --batch " << WrapSpaces(fn_list_of_files.GetFullPath()) << " "
                    << WrapSpaces(fn_output_file.GetFullPath());

    clDEBUG() << "CTags:" << execute_command << clEndl;
    IProcess::Ptr_t proc(::CreateSyncProcess(execute_command, IProcessCreateDefault, wxEmptyString, &envList));
    if(proc) {
        wxString dummy;
        proc->WaitForTerminate(dummy);
    }

    if(output && !FileUtils::ReadFileContent(fn_output_file.GetFullPath(), *output)) {
        clERROR() << "Failed to read temporary ctags output file" << fn_output_file.GetFullPath() << endl;
        return false;
    }

    if(output && output->empty()) {
        int i = 0;
        ++i;
    }
    clDEBUG() << "Generating ctags files... success" << clEndl;
    return true;
}

size_t CTags::ParseFiles(const vector<wxString>& files, const wxString& codelite_indexer, vector<TagEntryPtr>& tags)
{
    wxString filesList;
    for(const auto& file : files) {
        filesList << file << "\n";
    }
    wxString content;
    if(!DoGenerate(filesList, codelite_indexer, wxEmptyString, &content)) {
        return 0;
    }

    tags.clear();
    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
    tags.reserve(lines.size());

    // convert the lines into tags
    for(wxString& line : lines) {
        line.Trim(false).Trim();
        if(line.empty()) {
            continue;
        }

        // construct a tag from the line
        tags.emplace_back(new TagEntry());
        tags.back()->FromLine(line);
    }
    return tags.size();
}

size_t CTags::ParseFile(const wxString& file, const wxString& codelite_indexer, vector<TagEntryPtr>& tags)
{
    return ParseFiles({ file }, codelite_indexer, tags);
}

size_t CTags::ParseBuffer(const wxFileName& filename, const wxString& buffer, const wxString& codelite_indexer,
                          vector<TagEntryPtr>& tags)
{
    // create a temporary file with the content we want to parse
    clTempFile temp_file("cpp");
    temp_file.Write(buffer);
    // parse the file
    ParseFile(temp_file.GetFileName().GetFullPath(), codelite_indexer, tags);
    // set the file name to the correct file
    for(TagEntryPtr tag : tags) {
        tag->SetFile(filename.GetFullPath());
    }
    return tags.size();
}

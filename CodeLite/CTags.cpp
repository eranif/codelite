#include "CTags.hpp"

#include "asyncprocess.h"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"
#include "readtags.h"

#include <wx/stopwatch.h>
#include <wx/tokenzr.h>

namespace
{
void WrapInShell(wxString& cmd)
{
    wxString command;
#ifdef __WXMSW__
    wxChar* shell = wxGetenv(wxT("COMSPEC"));
    if(!shell)
        shell = (wxChar*)wxT("CMD.EXE");
    command << shell << wxT(" /C ");
    if(cmd.StartsWith("\"") && !cmd.EndsWith("\"")) {
        command << "\"" << cmd << "\"";
    } else {
        command << cmd;
    }
    cmd = command;
#else
    command << wxT("/bin/sh -c '");
    // escape any single quoutes
    cmd.Replace("'", "\\'");
    command << cmd << wxT("'");
    cmd = command;
#endif
}
} // namespace

wxString CTags::WrapSpaces(const wxString& file)
{
    wxString fixed = file;
    if(fixed.Contains(" ")) {
        fixed.Prepend("\"").Append("\"");
    }
    return fixed;
}
namespace
{
wxString fix_macro_entry(const wxString& macro)
{
    wxString fixed = macro;
    fixed.Replace("%", "__arg_");
    fixed.Replace("\"", "\\\"");
    return fixed;
}
} // namespace

bool CTags::DoGenerate(const wxString& filesContent, const wxString& codelite_indexer, const wxStringMap_t& macro_table,
                       const wxString& ctags_args, wxString* output)
{
    clDEBUG() << "Generating ctags files" << clEndl;

    // Pass ctags command line via the environment variable
    wxString ctagsCmd = ctags_args;
    if(ctagsCmd.empty()) {
        ctagsCmd = "--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ";
    }

    // TODO :
    // build the -D.. list here
    wxString macro_replacements;
    for(const auto& vt : macro_table) {
        if(vt.second.empty()) {
            // simple -D
            macro_replacements << "-D" << vt.first;
        } else {
            wxString fixed_macro_name = fix_macro_entry(vt.first);
            wxString fixed_macro_value = fix_macro_entry(vt.second);
            macro_replacements << "-D\"" << fixed_macro_name << "=" << fixed_macro_value << "\"";
        }
        macro_replacements << " ";
    }

    ctagsCmd << " " << macro_replacements << " --language-force=c++ --fields-c++=+{template} -f - ";
    wxStopWatch sw;

    sw.Start();
    // Split the list of files
    clTempFile file_list("txt");
    file_list.Write(filesContent);

    wxFileName output_file = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "tags_out", "tags");
    wxString command_to_run;
    command_to_run << WrapSpaces(codelite_indexer) << " " << ctagsCmd << " -L " << WrapSpaces(file_list.GetFullPath());
    command_to_run << " > " << WrapSpaces(output_file.GetFullPath());
    WrapInShell(command_to_run);

    // delete the output file
    FileUtils::Deleter d(output_file.GetFullPath());

    clDEBUG() << "Executing" << command_to_run << endl;
    ProcUtils::SafeExecuteCommand(command_to_run);
    long elapsed = sw.Time();

    FileUtils::ReadFileContent(output_file.GetFullPath(), *output);

    clDEBUG() << "Parsing took:" << (elapsed / 1000) << "secs," << (elapsed % 1000) << "ms" << endl;
    clDEBUG() << "Generating ctags files... Success" << clEndl;
    return true;
}

size_t CTags::ParseFiles(const vector<wxString>& files, const wxString& codelite_indexer,
                         const wxStringMap_t& macro_table, vector<TagEntryPtr>& tags)
{
    wxString filesList;
    for(const auto& file : files) {
        filesList << file << "\n";
    }
    wxString content;
    if(!DoGenerate(filesList, codelite_indexer, macro_table, wxEmptyString, &content)) {
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

size_t CTags::ParseFile(const wxString& file, const wxString& codelite_indexer, const wxStringMap_t& macro_table,
                        vector<TagEntryPtr>& tags)
{
    return ParseFiles({ file }, codelite_indexer, macro_table, tags);
}

size_t CTags::ParseBuffer(const wxFileName& filename, const wxString& buffer, const wxString& codelite_indexer,
                          const wxStringMap_t& macro_table, vector<TagEntryPtr>& tags)
{
    // create a temporary file with the content we want to parse
    clTempFile temp_file("cpp");
    temp_file.Write(buffer);
    // parse the file
    ParseFile(temp_file.GetFileName().GetFullPath(), codelite_indexer, macro_table, tags);
    // set the file name to the correct file
    for(TagEntryPtr tag : tags) {
        tag->SetFile(filename.GetFullPath());
    }
    return tags.size();
}

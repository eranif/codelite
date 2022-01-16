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

    // prepare the options file
    // one option per line
    vector<wxString> options_arr;
    options_arr.reserve(500);
    options_arr = { "--excmd=pattern", "--sort=no", "--fields=aKmSsnit", "--c-kinds=+p", "--C++-kinds=+p" };

    for(const auto& vt : macro_table) {
        wxString macro_replacements;
        wxString fixed_macro_name = fix_macro_entry(vt.first);
        wxString fixed_macro_value = fix_macro_entry(vt.second);

        if(fixed_macro_value.empty()) {
            // simple -D
            macro_replacements << "-D" << fixed_macro_name;
        } else {
            wxString fixed_macro_name = fix_macro_entry(vt.first);
            wxString fixed_macro_value = fix_macro_entry(vt.second);
            macro_replacements << "-D" << fixed_macro_name << "=" << fixed_macro_value;
        }
        options_arr.push_back(macro_replacements);
    }

    options_arr.push_back("--language-force=c++");
    options_arr.push_back("--fields-c++=+{template}+{properties}");

    // write the options into a file
    wxFileName ctags_options_file(clStandardPaths::Get().GetUserDataDir(), "options.ctags");
    wxString ctags_options_file_content;
    for(const wxString& option : options_arr) {
        ctags_options_file_content << option << "\n";
    }
    FileUtils::WriteFileContent(ctags_options_file.GetFullPath(), ctags_options_file_content);

    wxStopWatch sw;

    sw.Start();
    // Split the list of files
    clTempFile file_list("txt");
    file_list.Write(filesContent);

    wxFileName output_file = FileUtils::CreateTempFileName(clStandardPaths::Get().GetTempDir(), "tags_out", "tags");
    wxString command_to_run;
    command_to_run << WrapSpaces(codelite_indexer) << " --options=" << WrapSpaces(ctags_options_file.GetFullPath())
                   << " -L " << WrapSpaces(file_list.GetFullPath()) << " -f - > "
                   << WrapSpaces(output_file.GetFullPath());
    WrapInShell(command_to_run);
    clDEBUG() << "Running command:" << command_to_run << endl;

    // delete the output file
    FileUtils::Deleter d(output_file.GetFullPath());

    ProcUtils::SafeExecuteCommand(command_to_run);
    FileUtils::ReadFileContent(output_file.GetFullPath(), *output);

    long elapsed = sw.Time();

    clDEBUG() << "Parsing took:" << (elapsed / 1000) << "secs," << (elapsed % 1000) << "ms" << endl;
    clDEBUG() << "Generating ctags files... Success" << endl;
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
    TagEntryPtr prev_scoped_tag = nullptr;
    for(wxString& line : lines) {
        line.Trim(false).Trim();
        if(line.empty()) {
            continue;
        }

        // construct a tag from the line
        tags.emplace_back(new TagEntry());
        auto tag = tags.back();
        tag->FromLine(line);

        if(tag->IsEnumerator()                                // looking at an enumerator
           && prev_scoped_tag                                 // we have a previously seen scope
           && prev_scoped_tag->GetFile() == tag->GetFile()    /// and they are on the same file
           && prev_scoped_tag->GetName() == tag->GetParent()) // and it belongs to it
        {
            // remove one part of the scope
            wxArrayString scopes = ::wxStringTokenize(tag->GetScope(), ":", wxTOKEN_STRTOK);
            if(scopes.size()) {
                scopes.pop_back(); // remove the last part of the scope
                wxString new_scope;
                for(const wxString& scope : scopes) {
                    if(!new_scope.empty()) {
                        new_scope << "::";
                    }
                    new_scope << scope;
                }
                // update the scope
                tag->SetScope(new_scope.empty() ? "<global>" : new_scope);
            }
        }

        if(tag->IsEnum()) {
            prev_scoped_tag = tag;
        }
    }

    if(tags.empty()) {
        clDEBUG() << "0 tags, ctags output:" << content << endl;
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

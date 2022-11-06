#include "CTags.hpp"

#include "asyncprocess.h"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"
#include "readtags.h"

#include <set>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>

namespace
{
void WrapInShell(wxString& cmd)
{
    wxString command;
#ifdef __WXMSW__
    wxString shell = wxGetenv("COMSPEC");
    if(shell.IsEmpty()) {
        shell = "CMD.EXE";
    }
    command << shell << " /C ";
    if(cmd.StartsWith("\"") && !cmd.EndsWith("\"")) {
        command << "\"" << cmd << "\"";
    } else {
        command << cmd;
    }
    cmd = command;
#else
    command << "/bin/sh -c '";
    // escape any single quoutes
    cmd.Replace("'", "\\'");
    command << cmd << "'";
    cmd = command;
#endif
}
} // namespace

thread_local bool is_initialised = false;
thread_local bool is_macrodef_supported = false;

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
    //fixed.Replace("\"", "\\\"");
    return fixed;
}
} // namespace

bool CTags::DoGenerate(const wxString& filesContent, const wxString& codelite_indexer, const wxStringMap_t& macro_table,
                       const wxString& ctags_kinds, wxString* output)
{
    Initialise(codelite_indexer);
    clDEBUG() << "Generating ctags files" << clEndl;

    // prepare the options file
    // one option per line
    std::vector<wxString> options_arr;
    options_arr.reserve(500);
    wxString fields_cxx = "--fields-c++=+{template}+{properties}";
    if(is_macrodef_supported) {
        fields_cxx << "+{macrodef}";
    }

    options_arr = { "--extras=-p",       "--excmd=pattern",      "--sort=no",
                    "--fields=aKmSsnit", "--language-force=c++", fields_cxx };

    wxString kinds_string;
    if(ctags_kinds.empty()) {
        // default
        kinds_string << " --c-kinds=+pxz --C++-kinds=+pxz ";
    } else {
        kinds_string << " --c-kinds=" << ctags_kinds << " --C++-kinds=" << ctags_kinds << " ";
    }

    // we want the macros ordered, so we push them into std::set
    std::set<wxString> macros;
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
        macros.insert(macro_replacements);
    }

    // write the options into a file
    wxFileName ctags_options_file(clStandardPaths::Get().GetUserDataDir(),
                                  wxString() << "options-" << wxThread::GetCurrentId() << ".ctags");
    //FileUtils::Deleter d{ ctags_options_file };

    wxString ctags_options_file_content;
    for(const wxString& option : options_arr) {
        ctags_options_file_content << option << "\n";
    }

    // append the macros
    for(const auto& macro : macros) {
        ctags_options_file_content << macro << "\n";
    }
    ctags_options_file_content.Trim();
    FileUtils::WriteFileContent(ctags_options_file.GetFullPath(), ctags_options_file_content);

    // start timer
    wxStopWatch sw;

    sw.Start();
    // Split the list of files
    wxFileName file_list(clStandardPaths::Get().GetTempDir(),
                         wxString() << "file-list-" << wxThread::GetCurrentId() << ".txt");
    FileUtils::WriteFileContent(file_list, filesContent);

    wxString command_to_run;
    command_to_run << WrapSpaces(codelite_indexer) << " --options=" << WrapSpaces(ctags_options_file.GetFullPath())
                   << kinds_string << " -L " << WrapSpaces(file_list.GetFullPath()) << " -f - ";
    WrapInShell(command_to_run);
    clDEBUG() << "Running command:" << command_to_run << endl;

    *output = ProcUtils::SafeExecuteCommand(command_to_run);

    long elapsed = sw.Time();

    clDEBUG() << "Parsing took:" << (elapsed / 1000) << "secs," << (elapsed % 1000) << "ms" << endl;
    clDEBUG() << "Generating ctags files... Success" << endl;
    return true;
}

size_t CTags::ParseFiles(const std::vector<wxString>& files, const wxString& codelite_indexer,
                         const wxStringMap_t& macro_table, std::vector<TagEntryPtr>& tags)
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
                        std::vector<TagEntryPtr>& tags)
{
    return ParseFiles({ file }, codelite_indexer, macro_table, tags);
}

size_t CTags::ParseBuffer(const wxFileName& filename, const wxString& buffer, const wxString& codelite_indexer,
                          const wxStringMap_t& macro_table, std::vector<TagEntryPtr>& tags)
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

size_t CTags::ParseLocals(const wxFileName& filename, const wxString& buffer, const wxString& codelite_indexer,
                          const wxStringMap_t& macro_table, std::vector<TagEntryPtr>& tags)
{
    wxString content;
    {
        clTempFile temp_file("cpp");
        temp_file.Write(buffer);

        wxString filesList;
        filesList << temp_file.GetFullPath() << "\n";

        // we want locals + functions (to resolve the scope)
        if(!DoGenerate(filesList, codelite_indexer, macro_table, "lzpvfm", &content)) {
            return 0;
        }
    }

    tags.clear();
    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
    tags.reserve(lines.size());

    // convert the lines into tags
    for(auto& line : lines) {
        line.Trim().Trim(false);
        if(line.empty()) {
            continue;
        }

        // construct a tag from the line
        tags.emplace_back(new TagEntry());
        auto tag = tags.back();
        tag->FromLine(line);
        tag->SetFile(filename.GetFullPath());
    }

    if(tags.empty()) {
        clDEBUG() << "0 local tags, ctags output:" << content << endl;
    }
    return tags.size();
}

void CTags::Initialise(const wxString& codelite_indexer)
{
    if(is_initialised) {
        return;
    }

    is_initialised = true;
    // check whether we have `macrodef` supported
    wxString output;
    std::vector<wxString> command = { codelite_indexer, "--list-fields=c++" };
    auto process = ::CreateAsyncProcess(nullptr, command, IProcessCreateSync, wxEmptyString, nullptr, wxEmptyString);
    if(process) {
        process->WaitForTerminate(output);
    }

    wxArrayString lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for(const auto& line : lines) {
        if(line.Contains("macrodef")) {
            is_macrodef_supported = true;
            break;
        }
    }
}

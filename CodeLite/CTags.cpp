#include "CTags.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "StringUtils.h"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"

#include <wx/stopwatch.h>
#include <wx/tokenzr.h>

thread_local bool is_initialised = false;
thread_local bool is_macrodef_supported = false;

namespace
{

/**
 * @brief Returns the language name associated with a file type.
 *
 * Maps a FileExtManager::FileType value to a lower-case language identifier
 * used by the application. If the file type is not recognized, no language
 * name is returned.
 *
 * @param file_type FileExtManager::FileType The file type to translate into a language name.
 * @return std::optional<wxString> The corresponding language name as a wxString, or std::nullopt if the file type is
 * not supported.
 */
std::optional<wxString> GetLang(FileExtManager::FileType file_type)
{
    switch (file_type) {
    case FileExtManager::TypeSourceC:
    case FileExtManager::TypeSourceCpp:
    case FileExtManager::TypeHeader:
        return "c++";
    case FileExtManager::TypeCSharp:
        return "c#";
    case FileExtManager::TypePhp:
        return "php";
    case FileExtManager::TypeLua:
        return "lua";
    case FileExtManager::TypeRuby:
        return "ruby";
    case FileExtManager::TypeRust:
        return "rust";
    case FileExtManager::TypeCMake:
        return "cmake";
    case FileExtManager::TypeCSS:
        return "css";
    case FileExtManager::TypeTypeScript:
        return "typescript";
    case FileExtManager::TypePython:
        return "python";
    case FileExtManager::TypeJava:
        return "java";
    case FileExtManager::TypeTcl:
        return "tcl";
    default:
        return std::nullopt;
    }
}
} // namespace

std::optional<wxString>
CTags::DoCxxGenerate(const wxString& filesContent, const wxString& ctags_exe, const wxString& ctags_kinds)
{
    Initialise(ctags_exe);
    clDEBUG() << "Generating ctags files" << clEndl;

    // prepare the options file
    // one option per line
    std::vector<wxString> options_arr;
    wxString fields_cxx = "--fields-c++=+{template}+{properties}";
    if (is_macrodef_supported) {
        fields_cxx << "+{macrodef}";
    }

    options_arr = {
        "--extras=-p", "--excmd=pattern", "--sort=no", "--fields=aKmSsnit", "--language-force=c++", fields_cxx};

    wxString kinds_string;
    if (ctags_kinds.empty()) {
        // default
        kinds_string << " --c-kinds=+pxz --C++-kinds=+pxz ";
    } else {
        kinds_string << " --c-kinds=" << ctags_kinds << " --C++-kinds=" << ctags_kinds << " ";
    }

    // write the options into a file
    wxFileName ctags_options_file(
        clStandardPaths::Get().GetUserDataDir(), wxString() << "options-" << wxThread::GetCurrentId() << ".ctags");
    FileUtils::Deleter d{ctags_options_file};

    wxString ctags_options_file_content;
    for (const wxString& option : options_arr) {
        ctags_options_file_content << option << "\n";
    }

    // append the macros
    ctags_options_file_content.Trim();
    if (!FileUtils::WriteFileContent(ctags_options_file.GetFullPath(), ctags_options_file_content)) {
        clDEBUG() << "Failed to write ctags options file: " << ctags_options_file.GetFullPath() << endl;
        return std::nullopt;
    }

    // start timer
    wxStopWatch sw;

    sw.Start();
    // Split the list of files
    wxFileName file_list(
        clStandardPaths::Get().GetTempDir(), wxString() << "file-list-" << wxThread::GetCurrentId() << ".txt");
    if (!FileUtils::WriteFileContent(file_list, filesContent)) {
        clDEBUG() << "Failed to write ctags file list: " << file_list.GetFullPath() << endl;
        return std::nullopt;
    }

    wxString command_to_run;
    command_to_run << StringUtils::WrapWithDoubleQuotes(ctags_exe)
                   << " --options=" << StringUtils::WrapWithDoubleQuotes(ctags_options_file.GetFullPath())
                   << kinds_string << " -L " << StringUtils::WrapWithDoubleQuotes(file_list.GetFullPath()) << " -f - ";
    ProcUtils::WrapInShell(command_to_run);
    clDEBUG() << "Running command:" << command_to_run << endl;

    auto output = ProcUtils::SafeExecuteCommand(command_to_run);
    long elapsed = sw.Time();

    clDEBUG() << "ctags generation took:" << (elapsed / 1000) << "secs," << (elapsed % 1000) << "ms" << endl;
    clDEBUG() << "Generating ctags files... Success" << endl;
    return output;
}

std::vector<TagEntryPtr> CTags::ParseCxxFiles(const std::vector<wxString>& files, const wxString& ctags_exe)
{
    wxString filesList;
    for (const auto& file : files) {
        filesList << file << "\n";
    }
    auto result = DoCxxGenerate(filesList, ctags_exe);
    if (!result.has_value()) {
        return {};
    }

    auto content = std::move(result.value());
    std::vector<TagEntryPtr> tags;
    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
    tags.reserve(lines.size());

    // convert the lines into tags
    TagEntryPtr prev_scoped_tag = nullptr;
    for (wxString& line : lines) {
        line.Trim(false).Trim();
        if (line.empty()) {
            continue;
        }

        // construct a tag from the line
        tags.emplace_back(new TagEntry());
        auto tag = tags.back();
        tag->FromLine(line);

        if (tag->IsEnumerator()                                // looking at an enumerator
            && prev_scoped_tag                                 // we have a previously seen scope
            && prev_scoped_tag->GetFile() == tag->GetFile()    /// and they are on the same file
            && prev_scoped_tag->GetName() == tag->GetParent()) // and it belongs to it
        {
            // remove one part of the scope
            wxArrayString scopes = ::wxStringTokenize(tag->GetScope(), ":", wxTOKEN_STRTOK);
            if (scopes.size()) {
                scopes.pop_back(); // remove the last part of the scope
                wxString new_scope;
                for (const wxString& scope : scopes) {
                    if (!new_scope.empty()) {
                        new_scope << "::";
                    }
                    new_scope << scope;
                }
                // update the scope
                tag->SetScope(new_scope.empty() ? "<global>" : new_scope);
            }
        }

        if (tag->IsEnum()) {
            prev_scoped_tag = tag;
        }
    }

    if (tags.empty()) {
        clDEBUG() << "0 tags, ctags output:" << content << endl;
    }
    return tags;
}

std::vector<TagEntryPtr> CTags::ParseCxxFile(const wxString& file, const wxString& ctags_exe)
{
    return ParseCxxFiles({file}, ctags_exe);
}

std::vector<TagEntryPtr>
CTags::ParseCxxBuffer(const wxFileName& filename, const wxString& buffer, const wxString& ctags_exe)
{
    // create a temporary file with the content we want to parse
    clTempFile temp_file("cpp");
    temp_file.Write(buffer);
    // parse the file
    auto tags = ParseCxxFile(temp_file.GetFileName().GetFullPath(), ctags_exe);
    // set the file name to the correct file
    const wxString full_path = filename.GetFullPath();
    for (const auto& tag : tags) {
        tag->SetFile(full_path);
    }
    return tags;
}

std::vector<TagEntryPtr>
CTags::ParseCxxLocals(const wxFileName& filename, const wxString& buffer, const wxString& ctags_exe)
{
    wxString content;
    {
        clTempFile temp_file("cpp");
        temp_file.Write(buffer);

        wxString filesList;
        filesList << temp_file.GetFullPath() << "\n";

        // we want locals + functions (to resolve the scope)
        auto result = DoCxxGenerate(filesList, ctags_exe, "lzpvfm");
        if (!result.has_value()) {
            return {};
        }
        content = std::move(result.value());
    }

    std::vector<TagEntryPtr> tags;
    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
    tags.reserve(lines.size());

    // convert the lines into tags
    for (auto& line : lines) {
        line.Trim(false).Trim();
        if (line.empty()) {
            continue;
        }

        // construct a tag from the line
        tags.emplace_back(new TagEntry());
        auto tag = tags.back();
        tag->FromLine(line);
        tag->SetFile(filename.GetFullPath());
    }

    if (tags.empty()) {
        clDEBUG() << "0 local tags, ctags output:" << content << endl;
    }
    return tags;
}

void CTags::Initialise(const wxString& ctags_exe)
{
    if (is_initialised) {
        return;
    }

    // check whether we have `macrodef` supported
    wxString output;
    std::vector<wxString> command = {ctags_exe, "--list-fields=c++"};
    auto process = ::CreateAsyncProcess(nullptr, command, IProcessCreateSync, wxEmptyString, nullptr, wxEmptyString);
    if (process) {
        process->WaitForTerminate(output);
    }

    wxArrayString lines = ::wxStringTokenize(output, "\n", wxTOKEN_STRTOK);
    for (const auto& line : lines) {
        if (line.Contains("macrodef")) {
            is_macrodef_supported = true;
            break;
        }
    }
    is_initialised = true;
}

#include "CTags.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "Platform/Platform.hpp"
#include "StringUtils.h"
#include "assistant/Process.hpp"
#include "assistant/common/json.hpp"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"

#include <algorithm>
#include <mutex>
#include <wx/stopwatch.h>
#include <wx/tokenzr.h>

static std::atomic_bool is_macrodef_supported{false};
static std::once_flag s_initialise_once;
static const wxString s_ctags_executable = wxEmptyString;

using assistant::json;

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
wxString NormalizeSymbolLanguage(const wxString& language)
{
    const wxString lang = language.Lower();
    if (lang == "bash" || lang == "shell") {
        return "sh";
    }
    if (lang == "cpp") {
        return "c++";
    }
    if (lang == "py") {
        return "python";
    }
    if (lang == "rb") {
        return "ruby";
    }
    return lang;
}
} // namespace

bool CTags::IsSupportedSymbolLanguage(const wxString& language)
{
    static const std::unordered_set<wxString> supported = {
        "c++", "c", "rust", "python", "php", "ruby", "tcl", "java", "sh"};
    if (language.empty()) {
        return true;
    }

    const wxString lang = language.Lower();
    return std::find(supported.begin(), supported.end(), lang) != supported.end();
}

std::optional<CTags::SymbolKind>
CTags::MapSymbolKind(const wxString& kind, const wxString& scope, const wxString& kind_from_ctags)
{
    const wxString k = kind.Lower();
    const wxString c = kind_from_ctags.Lower();
    if (k == "class")
        return SymbolKind::kClass;
    if (k == "struct")
        return SymbolKind::kStruct;
    if (k == "trait")
        return SymbolKind::kTrait;
    if (k == "prototype" || c == "prototype")
        return SymbolKind::kPrototype;
    if (k == "method" || c == "method")
        return SymbolKind::kMethod;
    if (k == "function") {
        if (scope.empty() || scope == "<global>")
            return SymbolKind::kGlobalMethod;
        return SymbolKind::kMethod;
    }
    return std::nullopt;
}

std::optional<wxString> CTags::DoSymbolGenerate(const wxString& file, const wxString& ctags_exe)
{
    if (!wxFileExists(file)) {
        clWARNING() << "Symbol file does not exist: " << file << endl;
        return std::nullopt;
    }

    std::vector<wxString> cmdarr{ctags_exe, "--output-format=json", "--fields=+nKsSe", "--extras=+q", "-f", "-", file};
    auto command = StringUtils::ToStdStrings(cmdarr);
    clDEBUG() << "Running command:" << StringUtils::Join(cmdarr) << endl;
    auto result = assistant::Process::RunProcessAndWait(command);
    clDEBUG() << "Running command...done" << endl;

    if (!result.ok) {
        return std::nullopt;
    }
    return wxString::FromUTF8(result.out);
}

std::vector<CTags::SymbolInfo> CTags::ParseSymbolOutput(const wxString& content, const wxString& filename)
{
    std::vector<SymbolInfo> out;
    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
    for (auto& line : lines) {
        line.Trim(false).Trim();
        if (line.empty())
            continue;
        try {
            json entry = json::parse(line.ToStdString(wxConvUTF8));
            if (!entry.is_object() || !entry.contains("kind") || !entry.contains("name") || !entry.contains("line")) {
                continue;
            }

            wxString kind = wxString::FromUTF8(entry["kind"].get<std::string>());
            wxString name = wxString::FromUTF8(entry["name"].get<std::string>());
            wxString scope;
            if (entry.contains("scope") && entry["scope"].is_string()) {
                scope = wxString::FromUTF8(entry["scope"].get<std::string>());
            }

            auto mapped_kind = MapSymbolKind(kind, scope, kind);
            if (!mapped_kind) {
                continue;
            }

            SymbolInfo info;
            info.name = name;
            info.kind = *mapped_kind;
            info.line = entry["line"].get<int>();
            info.file = filename;
            if (entry.contains("end") && entry["end"].is_number_integer()) {
                info.end_line = entry["end"].get<int>();
            }
            if ((info.kind == SymbolKind::kMethod || info.kind == SymbolKind::kGlobalMethod ||
                 info.kind == SymbolKind::kPrototype) &&
                entry.contains("signature") && entry["signature"].is_string()) {
                info.signature = wxString::FromUTF8(entry["signature"].get<std::string>());
            }
            out.push_back(std::move(info));
        } catch (...) {
            continue;
        }
    }
    return out;
}

clStatusOr<std::vector<CTags::SymbolInfo>> CTags::ParseBufferSymbols(const wxString& filename, const wxString& buffer)
{
    if (s_ctags_executable.empty()) {
        return StatusNotFound("Please install universal-ctags and try again");
    }

    auto ext = FileExtManager::GetFileExtenstion(filename, buffer);
    if (!ext.has_value()) {
        return StatusInvalidArgument(wxString() << _("Failed to resolve file extension for file: ") << filename);
    }

    clTempFile tmpfile{ext.value()};
    if (!tmpfile.Write(buffer)) {
        return StatusIOError(wxString() << _("Failed to write temporary file: ") << tmpfile.GetFullPath()
                                        << _(" to disk"));
    }
    return ParseFileSymbols(tmpfile.GetFullPath());
}

clStatusOr<std::vector<CTags::SymbolInfo>> CTags::ParseFileSymbols(const wxString& file)
{
    if (s_ctags_executable.empty()) {
        return StatusNotFound("Please install universal-ctags and try again");
    }

    wxString file_content;
    if (!FileUtils::ReadFileContent(file, file_content)) {
        return StatusIOError(wxString() << _("Failed to read file content: ") << file << _(" from disk"));
    }

    auto ext = FileExtManager::GetFileExtenstion(file, file_content);
    std::unique_ptr<clTempFile> tmpfile;
    if (ext.has_value() && wxFileName{file}.GetExt().empty()) {
        tmpfile = std::make_unique<clTempFile>(ext.value());
        if (!tmpfile->Write(file_content)) {
            return StatusIOError(wxString()
                                 << _("Failed to write temporary file: ") << tmpfile->GetFullPath() << _(" to disk"));
        }
    }

    auto content = DoSymbolGenerate(tmpfile.get() ? tmpfile->GetFullPath() : file, s_ctags_executable);
    if (!content)
        return std::vector<CTags::SymbolInfo>{};

    clDEBUG() << "Parsing symbols..." << endl;
    auto symbols = ParseSymbolOutput(*content, file);
    clDEBUG() << "Parsing symbols...done" << endl;

    clDEBUG() << "Sorting symbols..." << endl;
    std::sort(symbols.begin(), symbols.end(), [](const SymbolInfo& lhs, const SymbolInfo& rhs) {
        if (lhs.line != rhs.line) {
            return lhs.line < rhs.line;
        }
        return lhs.name < rhs.name;
    });
    clDEBUG() << "Sorting symbols...done" << endl;
    return symbols;
}

std::optional<CTags::SymbolRangeInfo> CTags::FindSymbolsRangeNearLine(const std::vector<SymbolInfo>& symbols, int line)
{
    if (symbols.empty() || line <= 0) {
        return std::nullopt;
    }

    auto it = std::upper_bound(
        symbols.begin(), symbols.end(), line, [](int value, const SymbolInfo& sym) { return value < sym.line; });

    if (it == symbols.begin()) {
        return std::nullopt;
    }

    --it;
    SymbolRangeInfo result;
    result.symbol = *it;
    result.start_line = it->line;
    if (it->end_line.has_value()) {
        result.end_line = it->end_line;
    } else {
        auto next_it = std::next(it);
        if (next_it != symbols.end()) {
            result.end_line = next_it->line;
        }
    }
    return result;
}

std::optional<wxString>
CTags::DoCxxGenerate(const wxString& filesContent, const wxString& ctags_exe, const wxString& ctags_kinds)
{
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

    std::vector<wxString> kinds_arr;
    if (ctags_kinds.empty()) {
        // default
        kinds_arr = {"--c-kinds=+pxz", "--C++-kinds=+pxz"};
    } else {
        kinds_arr = {"--c-kinds=" + ctags_kinds, "--C++-kinds=" + ctags_kinds};
    }

    // start timer
    wxStopWatch sw;
    sw.Start();

    // Split the list of files
    clTempFile file_list(clStandardPaths::Get().GetTempDir(), "txt");
    if (!FileUtils::WriteFileContent(file_list.GetFullPath(), filesContent)) {
        clWARNING() << "Failed to write ctags file list: " << file_list.GetFullPath(true) << endl;
        return std::nullopt;
    }

    std::vector<wxString> cmdarr{ctags_exe};
    cmdarr.insert(cmdarr.end(), options_arr.begin(), options_arr.end());
    cmdarr.insert(cmdarr.end(), kinds_arr.begin(), kinds_arr.end());
    cmdarr.push_back("-o");
    cmdarr.push_back("-");
    cmdarr.push_back("-L");
    cmdarr.push_back(file_list.GetFullPath(true));
    clDEBUG() << "Running command:" << StringUtils::Join(cmdarr, " ") << endl;

    auto command = StringUtils::ToStdStrings(cmdarr);
    auto result = assistant::Process::RunProcessAndWait(command);
    if (!result.ok) {
        clWARNING() << "Failed to execute command. stderr:" << result.err << endl;
        return std::nullopt;
    }

    wxString output = wxString::FromUTF8(result.out);
    long elapsed = sw.Time();

    clDEBUG() << "STDOUT:" << result.out << endl;
    clDEBUG() << "STDERR:" << result.err << endl;
    clDEBUG() << "ctags generation took:" << (elapsed / 1000) << "secs," << (elapsed % 1000) << "ms" << endl;
    clDEBUG() << "Generating ctags files... Success" << endl;
    return output;
}

clStatusOr<std::vector<TagEntryPtr>> CTags::ParseCxxFiles(const std::vector<wxString>& files)
{
    if (s_ctags_executable.empty()) {
        return StatusNotFound("Please install universal-ctags and try again");
    }

    wxString filesList = StringUtils::Join(files, "\n");
    filesList << "\n";

    auto result = DoCxxGenerate(filesList, s_ctags_executable);
    if (!result.has_value()) {
        return std::vector<TagEntryPtr>{};
    }

    auto content = result.value();
    std::vector<TagEntryPtr> tags;
    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
    clDEBUG() << "Processing" << lines.size() << "lines" << endl;
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

    for (auto t : tags) {
        clDEBUG() << "Tag:" << t->GetName() << ", Kind:" << t->GetKind() << endl;
    }
    return tags;
}

clStatusOr<std::vector<TagEntryPtr>> CTags::ParseCxxFile(const wxString& file) { return ParseCxxFiles({file}); }

clStatusOr<std::vector<TagEntryPtr>> CTags::ParseCxxBuffer(const wxFileName& filename, const wxString& buffer)
{
    if (s_ctags_executable.empty()) {
        return StatusNotFound("Please install universal-ctags and try again");
    }

    // create a temporary file with the content we want to parse
    clTempFile temp_file("cpp");
    temp_file.Write(buffer);

    // parse the file
    auto res = ParseCxxFile(temp_file.GetFileName().GetFullPath());
    if (!res.ok()) {
        return res.status();
    }

    // set the file name to the correct file
    auto tags = res.value();
    const wxString full_path = filename.GetFullPath();
    for (auto tag : tags) {
        tag->SetFile(full_path);
    }
    return tags;
}

void CTags::Initialise()
{
    std::call_once(s_initialise_once, []() {
        auto res = LocateExe();
        if (res.ok()) {
            const_cast<wxString&>(s_ctags_executable) = res.value();
            clSYSTEM() << "ctags executable found:" << s_ctags_executable << endl;
        } else {
            clWARNING() << res.error_message() << endl;
            return;
        }

        // check whether we have `macrodef` supported
        wxString output;
        std::vector<wxString> command = {s_ctags_executable, "--list-fields=c++"};
        auto process =
            ::CreateAsyncProcess(nullptr, command, IProcessCreateSync, wxEmptyString, nullptr, wxEmptyString);
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
    });
}

clStatusOr<wxString> CTags::LocateExe()
{
#ifdef __WXMAC__
    // On macOS, we get a default ctags under /usr/bin/ctags which is very ancient
    // skip it by ignoring system PATH.
    static bool use_system_path{false};
#else
    static bool use_system_path{true};
#endif

    static thread_local std::optional<wxString> ctags_exe{std::nullopt};
    if (!ctags_exe.has_value()) {
        ctags_exe = ThePlatform->Which("ctags", use_system_path);
    }

    if (!ctags_exe.has_value()) {
        return StatusNotFound("Could not locate ctags. Please install it and try again");
    }

    wxString path = ctags_exe.value();
    return path;
}

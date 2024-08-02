#include "Settings.hpp"

#include "GCCMetadata.hpp"
#include "JSON.h"
#include "Platform/Platform.hpp"
#include "clTempFile.hpp"
#include "clangd/CompileCommandsJSON.h"
#include "clangd/CompileFlagsTxt.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "fileutils.h"
#include "procutils.h"
#include "tags_options_data.h"

#include <set>
#include <wx/string.h>
#include <wx/tokenzr.h>

namespace
{
FileLogger& operator<<(FileLogger& logger, const std::vector<std::pair<wxString, wxString>>& vec)
{
    wxString buffer;
    buffer << "[";
    for (const auto& d : vec) {
        buffer << "{" << d.first << ", " << d.second << "}, ";
    }
    buffer << "]";
    logger << buffer;
    return logger;
}

std::vector<std::pair<wxString, wxString>> to_vector_of_pairs(const std::vector<wxString>& arr)
{
    std::vector<std::pair<wxString, wxString>> result;
    result.reserve(arr.size());
    for (const wxString& line : arr) {
        wxString k = line.BeforeFirst('=');
        wxString v = line.AfterFirst('=');
        result.emplace_back(std::make_pair(k, v));
    }
    return result;
}

void write_to_json(JSONItem& json_arr, const std::vector<std::pair<wxString, wxString>>& arr)
{
    for (const auto& entry : arr) {
        auto type = json_arr.AddObject(wxEmptyString);
        type.addProperty("key", entry.first);
        type.addProperty("value", entry.second);
    }
}
} // namespace

CTagsdSettings::CTagsdSettings()
{
    // set some defaults
    m_tokens = to_vector_of_pairs(TagsOptionsData::GetDefaultTokens());
    m_types = to_vector_of_pairs(TagsOptionsData::GetDefaultTypes());
}

CTagsdSettings::~CTagsdSettings() {}

void CTagsdSettings::Load(const wxFileName& filepath)
{
    JSON config_file(filepath);
    m_settings_dir = filepath.GetPath();

    if (!config_file.isOk()) {
        clWARNING() << "Could not locate configuration file:" << filepath << endl;
        CreateDefault(filepath);

    } else {
        auto config = config_file.toElement();
        m_search_path = config["search_path"].toArrayString();
        m_file_mask = config["file_mask"].toString(m_file_mask);
        m_ignore_spec = config["ignore_spec"].toString(m_ignore_spec);
        m_codelite_indexer = config["codelite_indexer"].toString();
        m_limit_results = config["limit_results"].toSize_t(m_limit_results);
        CreateDefault(filepath); // generate the default tokens and types
    }

    build_search_path(filepath);
    if (m_types.empty() || m_tokens.empty() || m_search_path.empty()) {
        CreateDefault(filepath);
    }

    LOG_IF_TRACE { clDEBUG1() << "search path...........:" << m_search_path << endl; }
    LOG_IF_TRACE { clDEBUG1() << "tokens................:" << m_tokens << endl; }
    LOG_IF_TRACE { clDEBUG1() << "types.................:" << m_types << endl; }
    LOG_IF_TRACE { clDEBUG1() << "file_mask.............:" << m_file_mask << endl; }
    LOG_IF_TRACE { clDEBUG1() << "codelite_indexer......:" << m_codelite_indexer << endl; }
    LOG_IF_TRACE { clDEBUG1() << "ignore_spec...........:" << m_ignore_spec << endl; }
    LOG_IF_TRACE { clDEBUG1() << "limit_results.........:" << m_limit_results << endl; }
    LOG_IF_TRACE { clDEBUG1() << "Settings dir is set to:" << m_settings_dir << endl; }

    // conver the tokens to wxArrayString
    wxArrayString wxarr;
    wxarr.reserve(m_tokens.size());
    for (const auto& p : m_tokens) {
        wxarr.Add(p.first + "=" + p.second);
    }

    // update ctags manager
    TagsOptionsData tod;
    tod.SetCcNumberOfDisplayItems(m_limit_results);
    tod.SetTokens(wxJoin(wxarr, '\n'));
    TagsManagerST::Get()->SetCtagsOptions(tod);
}

void CTagsdSettings::Save(const wxFileName& filepath)
{
    JSON config_file(cJSON_Object);
    auto config = config_file.toElement();
    config.addProperty("file_mask", m_file_mask);
    config.addProperty("ignore_spec", m_ignore_spec);
    config.addProperty("codelite_indexer", m_codelite_indexer);
    config.addProperty("limit_results", m_limit_results);
    config.addProperty("search_path", m_search_path);

    auto types = config.AddArray("types");
    write_to_json(types, m_types);

    auto tokens = config.AddArray("tokens");
    write_to_json(tokens, m_tokens);
    config_file.save(filepath);
}

void CTagsdSettings::build_search_path(const wxFileName& filepath)
{
    // check the root folder for compile_flags.txt file or compile_commands.json
    wxFileName fn(filepath);
    fn.RemoveLastDir();

    wxString path = fn.GetPath();

    wxFileName compile_flags_txt(path, "compile_flags.txt");
    wxFileName compile_commands_json(path, "compile_commands.json");

    std::set<wxString> S{ m_search_path.begin(), m_search_path.end() };
    if (compile_flags_txt.FileExists()) {
        // we are using the compile_flags.txt file method
        CompileFlagsTxt cft(compile_flags_txt);
        S.insert(cft.GetIncludes().begin(), cft.GetIncludes().end());
    } else if (compile_commands_json.FileExists()) {
        CompileCommandsJSON ccj(compile_commands_json.GetFullPath());
        S.insert(ccj.GetIncludes().begin(), ccj.GetIncludes().end());
    }

    // Add the paths found in the compile_flags.txt/compile_commands.json files
    m_search_path.clear();
    for (const auto& path : S) {
        // add the custom paths
        m_search_path.Add(path);
    }

    wxString basename;
#ifdef __WXMSW__
    basename = "clang";
#elif defined(__WXMAC__)
    basename = "clang";
#else
    basename = "gcc";
#endif

    wxString command;

    // Common compiler paths - should be placed at top of the include path!
    if (ThePlatform->Which(basename, &command)) {
        GCCMetadata md{ basename };

        md.Load(command, wxEmptyString, false);
        m_search_path.insert(m_search_path.end(), md.GetSearchPaths().begin(), md.GetSearchPaths().end());
    }
}

void CTagsdSettings::CreateDefault(const wxFileName& filepath)
{
    m_tokens = to_vector_of_pairs(TagsOptionsData::GetDefaultTokens());
    m_types = to_vector_of_pairs(TagsOptionsData::GetDefaultTypes());
    Save(filepath);
}

wxStringMap_t CTagsdSettings::GetMacroTable() const
{
    wxStringMap_t table;
    table.reserve(m_tokens.size());
    for (const auto& p : m_tokens) {
        table.insert(p);
    }
    return table;
}

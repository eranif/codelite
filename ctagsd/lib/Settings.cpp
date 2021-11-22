#include "CompileCommandsJSON.h"
#include "CompileFlagsTxt.h"
#include "JSON.h"
#include "Settings.hpp"
#include "file_logger.h"
#include <set>

using namespace std;
namespace
{
FileLogger& operator<<(FileLogger& logger, const wxStringMap_t& m)
{
    wxString s;
    s << "{";
    for(const auto& vt : m) {
        s << "{ " << vt.first << ", " << vt.second << " },";
    }
    if(s.EndsWith(",")) {
        s.RemoveLast();
    }
    s << "}";
    logger.Append(s, logger.GetRequestedLogLevel());
    return logger;
}
} // namespace

CTagsdSettings::CTagsdSettings() {}

CTagsdSettings::~CTagsdSettings() {}

void CTagsdSettings::Load(const wxFileName& filepath)
{
    JSON config_file(filepath);
    if(!config_file.isOk()) {
        clWARNING() << "Could not locate configuration file:" << filepath << endl;
    } else {
        auto config = config_file.toElement();
        m_search_path = config["search_path"].toArrayString();
        m_tokens = config["tokens"].toStringMap();
        m_file_mask = config["file_mask"].toString(m_file_mask);
        m_ignore_spec = config["ignore_spec"].toString(m_ignore_spec);
        m_codelite_indexer = config["codelite_indexer"].toString();
    }

    build_search_path(filepath);

    clDEBUG() << "search path.......:" << m_search_path << endl;
    clDEBUG() << "tokens............:" << m_tokens << endl;
    clDEBUG() << "file_mask.........:" << m_file_mask << endl;
    clDEBUG() << "codelite_indexer..:" << m_codelite_indexer << endl;
    clDEBUG() << "ignore_spec.......:" << m_ignore_spec << endl;
}

void CTagsdSettings::Save(const wxFileName& filepath)
{
    JSON config_file(cJSON_Object);
    auto config = config_file.toElement();
    config.addProperty("search_path", m_search_path);
    config.addProperty("tokens", m_tokens);
    config.addProperty("file_mask", m_file_mask);
    config.addProperty("ignore_spec", m_ignore_spec);
    config.addProperty("codelite_indexer", m_codelite_indexer);
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

    set<wxString> S{ m_search_path.begin(), m_search_path.end() };
    if(compile_flags_txt.FileExists()) {
        // we are using the compile_flags.txt file method
        CompileFlagsTxt cft(compile_flags_txt);
        S.insert(cft.GetIncludes().begin(), cft.GetIncludes().end());
    } else if(compile_commands_json.FileExists()) {
        CompileCommandsJSON ccj(compile_commands_json.GetFullPath());
        S.insert(ccj.GetIncludes().begin(), ccj.GetIncludes().end());
    }
    m_search_path.clear();
    m_search_path.reserve(S.size());
    for(const auto& path : S) {
        m_search_path.Add(path);
    }
}

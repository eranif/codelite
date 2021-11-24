#include "Settings.hpp"
#include "CompileCommandsJSON.h"
#include "CompileFlagsTxt.h"
#include "GCCMetadata.hpp"
#include "JSON.h"
#include "clTempFile.hpp"
#include "file_logger.h"
#include "procutils.h"
#include "wx/tokenzr.h"
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
        m_types = config["types"].toStringMap();
        m_file_mask = config["file_mask"].toString(m_file_mask);
        m_ignore_spec = config["ignore_spec"].toString(m_ignore_spec);
        m_codelite_indexer = config["codelite_indexer"].toString();
    }

    build_search_path(filepath);

    clDEBUG() << "search path.......:" << m_search_path << endl;
    clDEBUG() << "tokens............:" << m_tokens << endl;
    clDEBUG() << "tokens............:" << m_types << endl;
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
    config.addProperty("types", m_types);
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

#if defined(__WXGTK__) || defined(__WXOSX__)
    wxString cxx = "/usr/bin/g++";

#ifdef __WXOSX__
    cxx = "/usr/bin/clang++";
#endif

    // Common compiler paths - should be placed at top of the include path!
    wxString command;

    // GCC prints parts of its output to stdout and some to stderr
    // redirect all output to stdout
    wxString working_directory;
    clTempFile tmpfile;
    tmpfile.Write(wxEmptyString);
    command << "/bin/bash -c '" << cxx << " -v -x c++ /dev/null -fsyntax-only > " << tmpfile.GetFullPath() << " 2>&1'";

    ProcUtils::SafeExecuteCommand(command);

    wxString content;
    FileUtils::ReadFileContent(tmpfile.GetFullPath(), content);
    wxArrayString outputArr = wxStringTokenize(content, wxT("\n\r"), wxTOKEN_STRTOK);

    // Analyze the output
    bool collect(false);
    wxArrayString search_paths;
    for(wxString& line : outputArr) {
        line.Trim().Trim(false);

        // search the scan starting point
        if(line.Contains(wxT("#include <...> search starts here:"))) {
            collect = true;
            continue;
        }

        if(line.Contains(wxT("End of search list."))) {
            break;
        }

        if(collect) {
            line.Replace("(framework directory)", wxEmptyString);
            // on Mac, (framework directory) appears also,
            // but it is harmless to use it under all OSs
            wxFileName includePath(line, wxEmptyString);
            includePath.Normalize();
            search_paths.Add(includePath.GetPath());
        }
    }

    S.insert(search_paths.begin(), search_paths.end());
#endif

    m_search_path.clear();
    m_search_path.reserve(S.size());
    for(const auto& path : S) {
        m_search_path.Add(path);
    }
}

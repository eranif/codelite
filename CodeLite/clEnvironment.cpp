#include "clEnvironment.hpp"

#include "cl_standard_paths.h"
#include "fileutils.h"
#include "xml/archive.h"
#include "xml/xmlutils.h"

#include <unordered_set>
#include <wx/any.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#include <wx/xml/xml.h>

namespace
{
static const wxString VARIABLE_REG_EXPR = R"#(\$[\(\{]?([\w]+)[\\/\)\}]?)#";

bool is_env_variable(const wxString& str, wxString* env_name)
{
    if (str.empty() || str[0] != '$') {
        return false;
    }
    env_name->reserve(str.length());

    // start from 1 to skip the prefix $
    for (size_t i = 1; i < str.length(); ++i) {
        wxChar ch = str[i];
        if (ch == '(' || ch == ')' || ch == '{' || ch == '}')
            continue;
        env_name->Append(ch);
    }
    return true;
}

/**
 * @brief expand an environment variable. use `env_map` to resolve any variables
 * accepting value in the form of (one example):
 * $PATH;C:\bin;$(LD_LIBRARY_PATH);${PYTHONPATH}
 */
wxString expand_env_variable(const wxString& value, const wxEnvVariableHashMap& env_map)
{
    // split the value into its parts
    wxArrayString parts = wxStringTokenize(value, wxPATH_SEP, wxTOKEN_STRTOK);
    wxString resolved;
    for (const wxString& part : parts) {
        wxString env_name;
        if (is_env_variable(part, &env_name)) {
            // try the environment variables first
            if (env_map.find(env_name) != env_map.end()) {
                resolved << env_map.find(env_name)->second;
            }
        } else {
            // literal
            resolved << part;
        }
        resolved << wxPATH_SEP;
    }
    if (!resolved.empty()) {
        resolved.RemoveLast();
    }
    return resolved;
}

/// Return vector of [{var_name, pattern}]
/// where:
/// given this example: ${HOME}/path/to
/// var_name -> the variable name e.g. `HOME`
/// pattern -> `${HOME}`
std::vector<std::pair<wxString, wxString>> FindVariablesInString(wxString str)
{
    wxRegEx re{VARIABLE_REG_EXPR};
    std::vector<std::pair<wxString, wxString>> result;
    bool cont = true;
    while (cont) {
        cont = false;
        if (re.Matches(str)) {
            wxString pattern;
            wxString varname;
            size_t offset = wxString::npos;
            {
                size_t start, len;
                if (re.GetMatch(&start, &len, 0)) {
                    pattern = str.Mid(start, len);
                    offset = start + len;
                }
            }
            {
                size_t start, len;
                if (re.GetMatch(&start, &len, 1)) {
                    varname = str.Mid(start, len);
                }
            }

            if (!varname.empty() && !pattern.empty()) {
                result.push_back({varname, pattern});
                cont = true;
                str = str.Mid(offset);
            }
        }
    }
    return result;
}

} // namespace

clEnvironment::clEnvironment()
{
    wxXmlDocument doc;
    wxFileName config{clStandardPaths::Get().GetUserDataDir(), "environment_variables.xml"};
    config.AppendDir("config");
    if (!doc.Load(config.GetFullPath()))
        return;

    wxXmlNode* node = XmlUtils::FindFirstByTagName(doc.GetRoot(), "ArchiveObject");
    if (node) {
        Archive arc;
        arc.SetXmlNode(node);

        wxString activeSet;
        clEnvList_t list;
        if (arc.Read("m_activeSet", activeSet)) {
            wxStringMap_t global_env_map;
            if (arc.Read("m_envVarSets", global_env_map) && global_env_map.count(activeSet)) {
                wxString envstr = global_env_map[activeSet];
                list = FileUtils::CreateEnvironment(envstr);
            }
        }

        if (!list.empty()) {
            ApplyFromList(&list);
        }
    }
}

clEnvironment::clEnvironment(const clEnvList_t* envlist)
    : m_env(envlist)
{
    ApplyFromList(m_env);
}

clEnvironment::~clEnvironment()
{
    for (const auto& p : m_old_env) {
        if (p.second.IsNull()) {
            // remove this environment
            ::wxUnsetEnv(p.first);
        } else {
            wxString strvalue;
            if (p.second.GetAs(&strvalue)) {
                ::wxSetEnv(p.first, strvalue);
            }
        }
    }
}

void clEnvironment::ApplyFromList(const clEnvList_t* envlist)
{
    if (!envlist) {
        return;
    }

    std::unordered_set<wxString> V;

    // keep track of the previous values
    for (const auto& [varname, varvalue] : *envlist) {
        const auto& [iter, succeeded] = V.insert(varname);
        if (succeeded) {
            wxString old_value;
            if (wxGetEnv(varname, &old_value)) {
                m_old_env.push_back({varname, old_value});
            } else {
                m_old_env.push_back({varname, wxAny{}});
            }
        }
    }

    for (auto [varname, varvalue] : *envlist) {
        auto vars = FindVariablesInString(varvalue);

        for (const auto& p : vars) {
            // the variable name, e.g. "HOME"
            const wxString& name = p.first;

            // the pattern, e.g. "$(HOME)"
            const wxString& pattern = p.second;

            wxString value;
            wxGetEnv(name, &value);
            varvalue.Replace(pattern, value);
        }

        // update the environment
        ::wxSetEnv(varname, varvalue);
    }
}

clEnvList_t BuildEnvFromString(const wxString& env_str) {
    clEnvList_t result;
    wxArrayString lines = ::wxStringTokenize(env_str, "\r\n", wxTOKEN_STRTOK);
    for (wxString& line : lines) {
        wxString key = line.BeforeFirst('=');
        wxString value = line.AfterFirst('=');
        if (key.empty()) {
            continue;
        }
        result.push_back({key, value});
    }
    return result;
}

clEnvList_t ResolveEnvList(const clEnvList_t& env_list)
{
    wxEnvVariableHashMap current_env;
    ::wxGetEnvMap(&current_env);

    for (auto [env_var_name, env_var_value] : env_list) {
        env_var_value = expand_env_variable(env_var_value, current_env);
        current_env.erase(env_var_name);
        current_env.insert({env_var_name, env_var_value});
    }

    clEnvList_t result;
    result.reserve(current_env.size());

    // convert the hash map into list and return it
    for (const auto& [env_var_name, env_var_value] : current_env) {
        result.push_back({env_var_name, env_var_value});
    }
    return result;
}

clEnvList_t ResolveEnvList(const wxString& envstr)
{
    auto source_list = BuildEnvFromString(envstr);
    return ResolveEnvList(source_list);
}

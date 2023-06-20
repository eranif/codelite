#include "clEnvironment.hpp"

#include "StringUtils.h"
#include "archive.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fileutils.h"

#include <wx/any.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/xml/xml.h>

namespace
{
static const wxString VARIARBLE_REG_EXPR = R"#(\$[\(\{]?([\w]+)[\\/\)\}]?)#";

/// Return vector of [{var_name, pattern}]
/// where:
/// given this example: ${HOME}/path/to
/// var_name -> the variable name e.g. `HOME`
/// pattern -> `${HOME}`
std::vector<std::pair<wxString, wxString>> FindVariablesInString(wxString str)
{
    wxRegEx re{ VARIARBLE_REG_EXPR };
    std::vector<std::pair<wxString, wxString>> result;
    bool cont = true;
    while(cont) {
        cont = false;
        if(re.Matches(str)) {
            wxString pattern;
            wxString varname;
            size_t offset = wxString::npos;
            {
                size_t start, len;
                if(re.GetMatch(&start, &len, 0)) {
                    pattern = str.Mid(start, len);
                    offset = start + len;
                }
            }
            {
                size_t start, len;
                if(re.GetMatch(&start, &len, 1)) {
                    varname = str.Mid(start, len);
                }
            }

            if(!varname.empty() && !pattern.empty()) {
                result.push_back({ varname, pattern });
                cont = true;
                str = str.Mid(offset);
            }
        }
    }
    return result;
}

wxXmlNode* FindFirstByTagName(const wxXmlNode* parent, const wxString& tagName)
{
    if(!parent) {
        return nullptr;
    }

    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == tagName) {
            return child;
        }
        child = child->GetNext();
    }
    return nullptr;
}
} // namespace

clEnvironment::clEnvironment()
{
    wxXmlDocument doc;
    wxFileName config{ clStandardPaths::Get().GetUserDataDir(), "environment_variables.xml" };
    config.AppendDir("config");
    if(!doc.Load(config.GetFullPath()))
        return;

    wxXmlNode* node = FindFirstByTagName(doc.GetRoot(), "ArchiveObject");
    if(node) {
        Archive arc;
        arc.SetXmlNode(node);

        wxString activeSet;
        clEnvList_t list;
        if(arc.Read("m_activeSet", activeSet)) {
            wxStringMap_t global_env_map;
            if(arc.Read("m_envVarSets", global_env_map) && global_env_map.count(activeSet)) {
                wxString envstr = global_env_map[activeSet];
                list = FileUtils::CreateEnvironment(envstr);
            }
        }

        if(!list.empty()) {
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
    for(const auto& p : m_old_env) {
        if(p.second.IsNull()) {
            // remove this environment
            ::wxUnsetEnv(p.first);
        } else {
            wxString strvalue;
            if(p.second.GetAs(&strvalue)) {
                ::wxSetEnv(p.first, strvalue);
            }
        }
    }
}

void clEnvironment::ApplyFromList(const clEnvList_t* envlist)
{
    if(!envlist) {
        return;
    }

    std::unordered_set<wxString> V;

    // keep track of the previous values
    for(const auto& [varname, varvalue] : *envlist) {
        const auto& [iter, succeeded] = V.insert(varname);
        if(succeeded) {
            wxString old_value;
            if(wxGetEnv(varname, &old_value)) {
                m_old_env.push_back({ varname, old_value });
            } else {
                m_old_env.push_back({ varname, wxAny{} });
            }
        }
    }

    for(auto [varname, varvalue] : *envlist) {
        auto vars = FindVariablesInString(varvalue);

        for(const auto& p : vars) {
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

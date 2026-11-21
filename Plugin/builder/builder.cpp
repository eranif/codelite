//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : builder.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "builder.h"

#include "build_settings_config.h"
#include "buildmanager.h"
#include "environmentconfig.h"
#include "envvarlist.h"
#include "macros.h"
#include "project.h"
#include "workspace.h"

namespace
{
/// A variable name character, the same as in MacroManager::FindVariable
bool IsVarNameChar(wxUniChar ch) { return ch == '_' || wxIsalnum(ch); }

/// Return the end of the variable name that starts at `pos` (`pos` when there is no name there)
size_t VarNameEnd(const wxString& text, size_t pos)
{
    while (pos < text.length() && IsVarNameChar(text[pos])) {
        ++pos;
    }
    return pos;
}

/// Return true if `text` contains `word` as a whole word (not as a part of a longer name)
bool ContainsWord(const wxString& text, const wxString& word)
{
    if (word.empty()) {
        return false;
    }
    size_t pos = text.find(word);
    while (pos != wxString::npos) {
        const size_t end = pos + word.length();
        const bool start_ok = pos == 0 || !IsVarNameChar(text[pos - 1]);
        const bool end_ok = end >= text.length() || !IsVarNameChar(text[end]);
        if (start_ok && end_ok) {
            return true;
        }
        pos = text.find(word, pos + 1);
    }
    return false;
}
} // namespace

Builder::Builder(const wxString& name)
    : m_name(name)
    , m_isActive(false)
{
    // Override values from configuration file
    BuilderConfigPtr config = BuildSettingsConfigST::Get()->GetBuilderConfig(m_name);
    if (config) {
        m_isActive = config->GetIsActive();

    } else {
        m_isActive = (m_name == "Default");
    }
}

wxString Builder::NormalizeConfigName(const wxString& confgName)
{
    wxString normalized(confgName);
    TrimString(normalized);
    normalized.Replace(wxT(" "), wxT("_"));
    return normalized;
}

wxString Builder::ToMakefileVariableRefs(const wxString& value)
{
    wxString result;
    result.reserve(value.length());
    size_t i = 0;
    while (i < value.length()) {
        const wxUniChar ch = value[i];
        if (ch == '%') {
            // %NAME% -> $(NAME). The name must not start with a digit, so percent-encoded text like "a%20b%20c" is
            // not changed
            const size_t end = VarNameEnd(value, i + 1);
            if (end > i + 1 && end < value.length() && value[end] == '%' && !wxIsdigit(value[i + 1])) {
                result << "$(" << value.Mid(i + 1, end - i - 1) << ")";
                i = end + 1;
            } else {
                result << ch;
                ++i;
            }
            continue;
        }

        if (ch != '$' || i + 1 >= value.length()) {
            result << ch;
            ++i;
            continue;
        }

        const wxUniChar next = value[i + 1];
        if (next == '$') {
            // "$$" is an escaped "$": keep it
            result << "$$";
            i += 2;

        } else if (next == '{') {
            // ${NAME} -> $(NAME)
            const size_t end = VarNameEnd(value, i + 2);
            if (end > i + 2 && end < value.length() && value[end] == '}') {
                result << "$(" << value.Mid(i + 2, end - i - 2) << ")";
                i = end + 1;
            } else {
                result << ch;
                ++i;
            }

        } else if (IsVarNameChar(next)) {
            // $NAME -> $(NAME)
            const size_t end = VarNameEnd(value, i + 1);
            result << "$(" << value.Mid(i + 1, end - i - 1) << ")";
            i = end;

        } else {
            // "$(NAME)" or anything else: keep as is
            result << ch;
            ++i;
        }
    }
    return result;
}

wxString Builder::CreateMakefileEnvVarsSection(const wxString& makefile,
                                               const wxString& project,
                                               const wxString& config,
                                               const wxString& assign)
{
    EnvVarList vars;
    EnvironmentConfig::Instance()->ReadObject("Variables", &vars);
    EnvMap varMap = vars.GetVariables(wxEmptyString, true, project, config);

    EnvVarEntries variables;
    variables.reserve(varMap.GetCount());
    for (size_t i = 0; i < varMap.GetCount(); ++i) {
        wxString name, value;
        varMap.Get(i, name, value);
        variables.emplace_back(name, value);
    }
    return CreateMakefileEnvVarsSection(makefile, variables, assign);
}

wxString
Builder::CreateMakefileEnvVarsSection(const wxString& makefile, const EnvVarEntries& variables, const wxString& assign)
{
    // name -> value, in the makefile syntax. The same name can appear more than once
    EnvVarEntries entries;
    entries.reserve(variables.size());
    for (const auto& [name, value] : variables) {
        entries.emplace_back(name, ToMakefileVariableRefs(value));
    }

    // Keep PATH and the variables that the makefile uses. A whole word match also finds the uses that are not a plain
    // "$(NAME)" (for example "$(NAME:.c=.o)" or "ifdef NAME"). A false match only writes one more variable.
    std::vector<bool> keep(entries.size(), false);
    for (size_t i = 0; i < entries.size(); ++i) {
        const wxString& name = entries[i].first;
        keep[i] = name.CmpNoCase("PATH") == 0 || ContainsWord(makefile, name);
    }

    // Keep the variables that a kept variable uses, until nothing changes
    bool changed = true;
    while (changed) {
        changed = false;
        for (size_t i = 0; i < entries.size(); ++i) {
            if (keep[i]) {
                continue;
            }
            for (size_t j = 0; j < entries.size(); ++j) {
                if (keep[j] && ContainsWord(entries[j].second, entries[i].first)) {
                    keep[i] = true;
                    changed = true;
                    break;
                }
            }
        }
    }

    wxString text;
    text << "##\n";
    text << "## User defined environment variables\n";
    text << "## (only the ones that this makefile uses: CodeLite sets all of them when it runs the build)\n";
    text << "##\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        if (keep[i]) {
            text << entries[i].first << assign << entries[i].second << "\n";
        }
    }
    return text;
}

void Builder::SetActive()
{
    std::list<wxString> builderNames;
    BuildManagerST::Get()->GetBuilders(builderNames);
    for (wxString builderName : builderNames) {
        if (BuilderPtr builder = BuildManagerST::Get()->GetBuilder(builderName))
            builder->m_isActive = (builder->m_name == m_name);
    }
}

Builder::OptimalBuildConfig Builder::GetOptimalBuildConfig(const wxString& projectType) const
{
    OptimalBuildConfig conf;
    conf.intermediateDirectory = "$(ConfigurationName)";
    conf.outputFile = "$(IntermediateDirectory)/";
    conf.command = "$(OutputFile)";

    if (projectType == PROJECT_TYPE_STATIC_LIBRARY || projectType == PROJECT_TYPE_DYNAMIC_LIBRARY) {
        conf.outputFile << "lib";
    }
    conf.outputFile << "$(ProjectName)" << GetOutputFileSuffix(projectType);

    return conf;
}

wxString Builder::GetOutputFileSuffix(const wxString& projectType) const
{
    if (projectType == PROJECT_TYPE_EXECUTABLE) {
#ifdef __WXMSW__
        return ".exe";
#endif
    } else if (projectType == PROJECT_TYPE_STATIC_LIBRARY) {
        return GetStaticLibSuffix();
    } else if (projectType == PROJECT_TYPE_DYNAMIC_LIBRARY) {
#ifdef __WXMSW__
        return ".dll";
#elif defined(__WXGTK__)
        return ".so";
#elif defined(__WXOSX__)
        return ".dylib";
#endif
    }
    return wxEmptyString;
}

#include "CodeFormatterManager.hpp"

#include "fileutils.h"
#include "fmtBlack.hpp"
#include "fmtCMakeFormat.hpp"
#include "fmtClangFormat.hpp"
#include "fmtJQ.hpp"
#include "fmtPHPCBF.hpp"
#include "fmtPHPCSFixer.hpp"
#include "fmtRustfmt.hpp"
#include "fmtShfmtFormat.hpp"
#include "fmtXmlLint.hpp"
#include "fmtYQ.hpp"
#include "json_utils.h"

#include <wx/filename.h>

CodeFormatterManager::~CodeFormatterManager() { clear(); }

std::shared_ptr<GenericFormatter> CodeFormatterManager::GetFormatter(const wxString& filepath) const
{
    auto type = FileExtManager::GetType(filepath);
    for (auto f : m_formatters) {
        if (f->IsEnabled() && f->CanHandle(type)) {
            return f;
        }
    }
    return nullptr;
}

void CodeFormatterManager::clear() { m_formatters.clear(); }

void CodeFormatterManager::initialize_defaults()
{
    clear();
    push_back(new fmtClangFormat);
    push_back(new fmtPHPCBF);
    push_back(new fmtPHPCSFixer);
    push_back(new fmtJQ);
    push_back(new fmtXmlLint);
    push_back(new fmtRustfmt);
    push_back(new fmtBlack);
    push_back(new fmtYQ);
    push_back(new fmtCMakeFormat);
    push_back(new fmtShfmtFormat);
}

void CodeFormatterManager::push_back(GenericFormatter* formatter)
{
    std::shared_ptr<GenericFormatter> ptr{ formatter };
    m_formatters.push_back(ptr);
}

size_t CodeFormatterManager::GetAllNames(wxArrayString* names) const
{
    if (!names) {
        return 0;
    }

    names->reserve(m_formatters.size());
    for (auto f : m_formatters) {
        names->Add(f->GetName());
    }
    return names->size();
}

void CodeFormatterManager::Load()
{
    wxFileName config_file{ clStandardPaths::Get().GetUserDataDir(), "code-formatters.json" };
    config_file.AppendDir("config");

    if (!config_file.FileExists()) {
        return;
    }

    wxString content;
    if (!FileUtils::ReadFileContent(config_file, content))
    {
        initialize_defaults();
        return;
    }
    const auto json = nlohmann::json::parse(StringUtils::ToStdString(content), nullptr, false);
    if (json.is_discarded() || !json.is_array()) {
        initialize_defaults();
        return;
    }

    clear();
    for (const auto& elem : json) {
        GenericFormatter* fmtr = new GenericFormatter();
        fmtr->FromJSON(elem);
        push_back(fmtr);
    }
}

void CodeFormatterManager::Save()
{
    wxFileName config_file{ clStandardPaths::Get().GetUserDataDir(), "code-formatters.json" };
    config_file.AppendDir("config");
    nlohmann::json json = nlohmann::json::array();
    for (auto fmtr : m_formatters) {
        json.push_back(fmtr->ToJSON());
    }
    FileUtils::WriteFileContentRaw(config_file, json.dump(2));
}

std::shared_ptr<GenericFormatter> CodeFormatterManager::GetFormatterByName(const wxString& name) const
{
    for (auto f : m_formatters) {
        if (f->GetName() == name) {
            return f;
        }
    }
    return nullptr;
}

bool CodeFormatterManager::CanFormat(const wxString& filepath) const
{
    auto file_type = FileExtManager::GetType(filepath);
    for (auto f : m_formatters) {
        if (f->IsEnabled() && f->CanHandle(file_type)) {
            return true;
        }
    }
    return false;
}

void CodeFormatterManager::RestoreDefaults()
{
    clear();
    initialize_defaults();
}

void CodeFormatterManager::ClearRemoteCommands()
{
    for (auto f : m_formatters) {
        f->SetRemoteCommand(wxEmptyString, wxEmptyString, {});
    }
}

std::shared_ptr<GenericFormatter> CodeFormatterManager::GetFormatterByContent(const wxString& content) const
{
    FileExtManager::FileType type;
    if (!FileExtManager::GetContentType(content, type)) {
        return nullptr;
    }

    for (auto f : m_formatters) {
        if (f->IsEnabled() && f->CanHandle(type)) {
            return f;
        }
    }
    return nullptr;
}

bool CodeFormatterManager::AddCustom(GenericFormatter* formatter)
{
    auto where =
        std::find_if(m_formatters.begin(), m_formatters.end(), [formatter](std::shared_ptr<GenericFormatter> fmtr) {
            return fmtr->GetName() == formatter->GetName();
        });
    if (where != m_formatters.end()) {
        return false;
    }
    push_back(formatter);
    return true;
}

bool CodeFormatterManager::DeleteFormatter(const wxString& name)
{
    auto where = std::find_if(m_formatters.begin(),
                              m_formatters.end(),
                              [&name](std::shared_ptr<GenericFormatter> fmtr) { return fmtr->GetName() == name; });
    if (where == m_formatters.end()) {
        // not found
        return false;
    }
    m_formatters.erase(where);
    return true;
}

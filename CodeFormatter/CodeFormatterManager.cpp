#include "CodeFormatterManager.hpp"

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

#include <algorithm>
#include <wx/filename.h>

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
    push_back(std::make_shared<fmtClangFormat>());
    push_back(std::make_shared<fmtPHPCBF>());
    push_back(std::make_shared<fmtPHPCSFixer>());
    push_back(std::make_shared<fmtJQ>());
    push_back(std::make_shared<fmtXmlLint>());
    push_back(std::make_shared<fmtRustfmt>());
    push_back(std::make_shared<fmtBlack>());
    push_back(std::make_shared<fmtYQ>());
    push_back(std::make_shared<fmtCMakeFormat>());
    push_back(std::make_shared<fmtShfmtFormat>());
}

void CodeFormatterManager::push_back(std::shared_ptr<GenericFormatter> formatter)
{
    m_formatters.push_back(std::move(formatter));
}

wxArrayString CodeFormatterManager::GetAllNames() const
{
    wxArrayString names;
    names.reserve(m_formatters.size());
    for (const auto& f : m_formatters) {
        names.Add(f->GetName());
    }
    return names;
}

void CodeFormatterManager::Load()
{
    wxFileName config_file{ clStandardPaths::Get().GetUserDataDir(), "code-formatters.json" };
    config_file.AppendDir("config");

    if (!config_file.FileExists()) {
        return;
    }

    JSON root{ config_file };
    if (!root.isOk() || !root.toElement().isArray()) {
        initialize_defaults();
        return;
    }

    clear();
    auto arr = root.toElement();
    int count = arr.arraySize();
    for (int i = 0; i < count; ++i) {
        auto formatter = std::make_shared<GenericFormatter>();
        formatter->FromJSON(arr[i]);
        push_back(std::move(formatter));
    }
}

void CodeFormatterManager::Save()
{
    wxFileName config_file{ clStandardPaths::Get().GetUserDataDir(), "code-formatters.json" };
    config_file.AppendDir("config");
    JSON root{JsonType::Array};
    auto arr = root.toElement();
    for (auto fmtr : m_formatters) {
        arr.arrayAppend(fmtr->ToJSON());
    }
    root.save(config_file);
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

bool CodeFormatterManager::AddCustom(std::shared_ptr<GenericFormatter> formatter)
{
    const auto where = std::ranges::find(m_formatters, formatter->GetName(), &GenericFormatter::GetName);
    if (where != m_formatters.end()) {
        return false;
    }
    push_back(std::move(formatter));
    return true;
}

bool CodeFormatterManager::DeleteFormatter(const wxString& name)
{
    const auto where = std::ranges::find(m_formatters, name, &GenericFormatter::GetName);
    if (where == m_formatters.end()) {
        // not found
        return false;
    }
    m_formatters.erase(where);
    return true;
}

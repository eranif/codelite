#include "CodeFormatterManager.hpp"

#include "LSP/LSPManager.hpp"
#include "fmtBlack.hpp"
#include "fmtCMakeFormat.hpp"
#include "fmtClangFormat.hpp"
#include "fmtJQ.hpp"
#include "fmtLSP.hpp"
#include "fmtRustfmt.hpp"
#include "fmtShfmtFormat.hpp"
#include "fmtXmlLint.hpp"
#include "fmtYQ.hpp"

#include <algorithm>
#include <wx/filename.h>

namespace
{
/// Can `formatter` format a file of type `type` right now? The LSP formatter also needs a running language server
/// that supports `textDocument/formatting`
bool can_format(const GenericFormatter& formatter, FileExtManager::FileType type)
{
    if (!formatter.IsEnabled() || !formatter.CanHandle(type)) {
        return false;
    }

    if (!formatter.IsLSPFormatter()) {
        return true;
    }

    auto server = LSP::Manager::GetInstance().GetServerForFileType(type);
    return server && server->IsDocumentFormattingSupported();
}

/// Formatters that CodeLite no longer ships. They are dropped from the saved configuration when it is loaded
const wxStringSet_t removed_formatters = {
    // replaced by the language server (PHPantom picks phpcbf / php-cs-fixer from the project itself)
    "PHPCBF",
    "PHP-CS-Fixer",
};
} // namespace

std::shared_ptr<GenericFormatter> CodeFormatterManager::GetFormatter(const wxString& filepath) const
{
    auto type = FileExtManager::GetType(filepath);
    for (auto f : m_formatters) {
        if (can_format(*f, type)) {
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
    push_back(std::make_shared<fmtJQ>());
    push_back(std::make_shared<fmtXmlLint>());
    push_back(std::make_shared<fmtRustfmt>());
    push_back(std::make_shared<fmtBlack>());
    push_back(std::make_shared<fmtYQ>());
    push_back(std::make_shared<fmtCMakeFormat>());
    push_back(std::make_shared<fmtShfmtFormat>());
    // last, so the formatters above win when they are enabled
    push_back(std::make_shared<fmtLSP>());
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
    wxFileName config_file{clStandardPaths::Get().GetUserDataDir(), "code-formatters.json"};
    config_file.AppendDir("config");

    if (!config_file.FileExists()) {
        return;
    }

    JSON root{config_file};
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
        if (removed_formatters.contains(formatter->GetName())) {
            continue;
        }
        push_back(std::move(formatter));
    }

    // configurations saved before the LSP formatter existed do not have it
    if (std::ranges::none_of(m_formatters, &GenericFormatter::IsLSPFormatter)) {
        push_back(std::make_shared<fmtLSP>());
    }
}

void CodeFormatterManager::Save()
{
    wxFileName config_file{clStandardPaths::Get().GetUserDataDir(), "code-formatters.json"};
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
    // used for batch formatting: the LSP formatter is skipped, it can only format open files
    auto file_type = FileExtManager::GetType(filepath);
    for (auto f : m_formatters) {
        if (!f->IsLSPFormatter() && f->IsEnabled() && f->CanHandle(file_type)) {
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
        if (can_format(*f, type)) {
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

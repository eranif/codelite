#include "CodeFormatterManager.hpp"

#include "JSON.h"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "fmtBlack.hpp"
#include "fmtCMakeForamt.hpp"
#include "fmtClangFormat.hpp"
#include "fmtJQ.hpp"
#include "fmtPHPCBF.hpp"
#include "fmtPHPCSFixer.hpp"
#include "fmtRustfmt.hpp"
#include "fmtXmlLint.hpp"
#include "fmtYQ.hpp"

#include <wx/filename.h>

CodeFormatterManager::CodeFormatterManager() {}

CodeFormatterManager::~CodeFormatterManager() { clear(); }

std::shared_ptr<GenericFormatter> CodeFormatterManager::GetFormatter(const wxString& filepath) const
{
    auto type = FileExtManager::GetType(filepath);
    for(auto f : m_formatters) {
        if(f->IsEnabled() && f->CanHandle(type)) {
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
    push_back(new fmtCMakeForamt);
}

void CodeFormatterManager::push_back(GenericFormatter* formatter)
{
    std::shared_ptr<GenericFormatter> ptr{ formatter };
    m_formatters.push_back(ptr);
}

size_t CodeFormatterManager::GetAllNames(wxArrayString* names) const
{
    if(!names) {
        return 0;
    }

    names->reserve(m_formatters.size());
    for(auto f : m_formatters) {
        names->Add(f->GetName());
    }
    return names->size();
}

void CodeFormatterManager::Load()
{
    wxFileName config_file{ clStandardPaths::Get().GetUserDataDir(), "code-formatters.json" };
    config_file.AppendDir("config");
    JSON root(config_file);
    if(!root.isOk() || !root.toElement().isArray()) {
        initialize_defaults();
        return;
    }

    clear();
    auto arr = root.toElement();
    int count = arr.arraySize();
    for(int i = 0; i < count; ++i) {
        GenericFormatter* fmtr = new GenericFormatter();
        fmtr->FromJSON(arr[i]);
        push_back(fmtr);
    }
}

void CodeFormatterManager::Save()
{
    wxFileName config_file{ clStandardPaths::Get().GetUserDataDir(), "code-formatters.json" };
    config_file.AppendDir("config");
    JSON root{ cJSON_Array };
    auto arr = root.toElement();
    for(auto fmtr : m_formatters) {
        arr.arrayAppend(fmtr->ToJSON());
    }
    root.save(config_file);
}

std::shared_ptr<GenericFormatter> CodeFormatterManager::GetFormatterByName(const wxString& name) const
{
    for(auto f : m_formatters) {
        if(f->GetName() == name) {
            return f;
        }
    }
    return nullptr;
}

bool CodeFormatterManager::CanFormat(const wxString& filepath) const
{
    auto file_type = FileExtManager::GetType(filepath);
    for(auto f : m_formatters) {
        if(f->IsEnabled() && f->CanHandle(file_type)) {
            return true;
        }
    }
    return false;
}

bool CodeFormatterManager::CanFormatByContent(const wxString& content) const
{
    FileExtManager::FileType file_type;
    if(!FileExtManager::GetContentType(content, file_type)) {
        return false;
    }

    for(auto f : m_formatters) {
        if(f->IsEnabled() && f->CanHandle(file_type)) {
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
    for(auto f : m_formatters) {
        f->SetRemoteCommand(wxEmptyString);
    }
}

std::shared_ptr<GenericFormatter> CodeFormatterManager::GetFormatterByContent(const wxString& content) const
{
    FileExtManager::FileType type;
    if(!FileExtManager::GetContentType(content, type)) {
        return nullptr;
    }

    for(auto f : m_formatters) {
        if(f->IsEnabled() && f->CanHandle(type)) {
            return f;
        }
    }
    return nullptr;
}

#pragma once

#include "cl_config.h"

#include <wx/arrstr.h>

namespace llm
{
class Config : clConfigItem
{
public:
    Config();
    virtual ~Config();
    void SetSelectedModelName(const wxString& selectedModel) { m_selectedModel = selectedModel; }
    const wxString& GetSelectedModel() const { return m_selectedModel; }

    const wxArrayString& GetHistory() const { return m_history; }
    void AddHistory(const wxString& prompt);

    void Load();
    void Save();

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

private:
    wxString m_selectedModel;
    mutable wxArrayString m_history;
};
} // namespace llm
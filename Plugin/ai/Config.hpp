#pragma once

#include "assistant/attributes.hpp"
#include "cl_config.h"
#include "codelite_exports.h"

#include <mutex>
#include <wx/arrstr.h>

namespace llm
{
class WXDLLIMPEXP_SDK Config : clConfigItem
{
public:
    Config();
    virtual ~Config();

    void SetSelectedModelName(const wxString& selectedModel)
    {
        std::scoped_lock lk{m_mutex};
        m_selectedModel = selectedModel;
    }
    wxString GetSelectedModel() const
    {
        std::scoped_lock lk{m_mutex};
        return m_selectedModel;
    }

    wxArrayString GetHistory() const
    {
        std::scoped_lock lk{m_mutex};
        return m_history;
    }

    void AddHistory(const wxString& prompt);

    void Load();
    void Save();

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

private:
    mutable std::mutex m_mutex;
    wxString m_selectedModel GUARDED_BY(m_mutex);
    mutable wxArrayString m_history GUARDED_BY(m_mutex);
};
} // namespace llm
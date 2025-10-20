#pragma once

#include "assistant/attributes.hpp"
#include "cl_config.h"
#include "codelite_exports.h"
#include "macros.h"

#include <mutex>
#include <wx/arrstr.h>

namespace llm
{
enum class PromptKind {
    kCommentGeneration,
    kGitCommitMessage,
    kReleaseNotesGenerate,
    kReleaseNotesMerge,
    kGitChangesCodeReview,
    kMax, // Must be last
};

inline wxString GetPromptString(PromptKind kind)
{
    switch (kind) {
    case llm::PromptKind::kReleaseNotesMerge:
        return "Git Release Notes: Merge";
    case llm::PromptKind::kReleaseNotesGenerate:
        return "Git Release Notes: Generate";
    case llm::PromptKind::kCommentGeneration:
        return "Comment Generation Prompt";
    case llm::PromptKind::kGitCommitMessage:
        return "Git Commit Message Prompt";
    case llm::PromptKind::kGitChangesCodeReview:
        return "Code Review Prompt";
    case llm::PromptKind::kMax:
        return "";
    }
    return "";
}

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

    wxString GetPrompt(PromptKind kind) const;
    void SetPrompt(PromptKind kind, const wxString& prompt);

private:
    mutable std::mutex m_mutex;
    wxString m_selectedModel GUARDED_BY(m_mutex);
    wxStringMap_t m_prompts;
    mutable wxArrayString m_history GUARDED_BY(m_mutex);
};
} // namespace llm
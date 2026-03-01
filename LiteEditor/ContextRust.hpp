#ifndef CONTEXTRUST_HPP
#define CONTEXTRUST_HPP

#include "generic_context.h"
class ContextRust : public ContextGeneric
{
    bool m_eventsBound = false;

public:
    ContextRust();
    explicit ContextRust(clEditor* Editor);
    ~ContextRust() override;

protected:
    void OnCommentSelection(wxCommandEvent& event);
    void OnCommentLine(wxCommandEvent& event);

public:
    bool IsStringTriggerCodeComplete(const wxString& str) const override;
    int GetActiveKeywordSet() const override;
    int DoGetCalltipParamterIndex() override;
    wxMenu* GetMenu() override;
    void AddMenuDynamicContent(wxMenu* menu) override;
    void ApplySettings() override;
    void AutoIndent(const wxChar&) override;
    wxString CallTipContent() override;
    wxString GetCurrentScopeName() override;
    TagEntryPtr GetTagAtCaret(bool scoped, bool impl) override;
    bool IsCommentOrString(long pos) override;
    bool IsDefaultContext() const override;
    std::shared_ptr<ContextBase> NewInstance(clEditor* container) override;
    void OnCallTipClick(wxStyledTextEvent& event) override;
    void OnCalltipCancel() override;
    void OnDbgDwellEnd(wxStyledTextEvent& event) override;
    void OnDbgDwellStart(wxStyledTextEvent& event) override;
    void OnDwellEnd(wxStyledTextEvent& event) override;
    void OnEnterHit() override;
    void OnFileSaved() override;
    void OnKeyDown(wxKeyEvent& event) override;
    void OnSciUpdateUI(wxStyledTextEvent& event) override;
    void RemoveMenuDynamicContent(wxMenu* menu) override;
    void RetagFile() override;
    void SemicolonShift() override;
    void SetActive() override;
    bool IsAtBlockComment() const override;
    bool IsAtLineComment() const override;
    void ProcessIdleActions() override;

    bool IsComment(long pos) const;
};

#endif // CONTEXTRUST_HPP

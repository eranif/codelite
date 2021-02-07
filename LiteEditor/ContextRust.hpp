#ifndef CONTEXTRUST_HPP
#define CONTEXTRUST_HPP

#include "generic_context.h"
class ContextRust : public ContextGeneric
{
public:
    ContextRust();
    ContextRust(clEditor* Editor);
    virtual ~ContextRust();

public:
    bool IsStringTriggerCodeComplete(const wxString& str) const;
    virtual int GetActiveKeywordSet() const;
    virtual int DoGetCalltipParamterIndex();
    virtual wxMenu* GetMenu();
    virtual void AddMenuDynamicContent(wxMenu* menu);
    virtual void ApplySettings();
    virtual void AutoIndent(const wxChar&);
    virtual wxString CallTipContent();
    virtual wxString GetCurrentScopeName();
    virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
    virtual void GotoPreviousDefintion();
    virtual bool IsComment(long pos);
    virtual bool IsCommentOrString(long pos);
    virtual bool IsDefaultContext() const;
    virtual ContextBase* NewInstance(clEditor* container);
    virtual void OnCallTipClick(wxStyledTextEvent& event);
    virtual void OnCalltipCancel();
    virtual void OnDbgDwellEnd(wxStyledTextEvent& event);
    virtual void OnDbgDwellStart(wxStyledTextEvent& event);
    virtual void OnDwellEnd(wxStyledTextEvent& event);
    virtual void OnDwellStart(wxStyledTextEvent& event);
    virtual void OnEnterHit();
    virtual void OnFileSaved();
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnSciUpdateUI(wxStyledTextEvent& event);
    virtual void RemoveMenuDynamicContent(wxMenu* menu);
    virtual void RetagFile();
    virtual void SemicolonShift();
    virtual void SetActive();
    virtual bool IsAtBlockComment() const;
    virtual bool IsAtLineComment() const;
    void ProcessIdleActions();
};

#endif // CONTEXTRUST_HPP

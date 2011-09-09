#ifndef CONTEXTHTML_H
#define CONTEXTHTML_H

#include "context_base.h" // Base class: ContextBase

class ContextHtml : public ContextBase 
{
public:
	ContextHtml();
	ContextHtml(LEditor *Editor);
	virtual ~ContextHtml();

public:
	virtual int  DoGetCalltipParamterIndex();
	virtual wxMenu* GetMenu();
	virtual void AddMenuDynamicContent(wxMenu* menu);
	virtual void ApplySettings();
	virtual void AutoIndent(const wxChar&);
	virtual wxString CallTipContent();
	virtual void CodeComplete(long pos = wxNOT_FOUND);
	virtual void CompleteWord();
	virtual wxString GetCurrentScopeName();
	virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
	virtual void GoHyperlink(int start, int end, int type, bool alt);
	virtual void GotoDefinition();
	virtual void GotoPreviousDefintion();
	virtual bool IsComment(long pos);
	virtual bool IsCommentOrString(long pos);
	virtual bool IsDefaultContext() const;
	virtual ContextBase* NewInstance(LEditor* container);
	virtual void OnCallTipClick(wxScintillaEvent& event);
	virtual void OnCalltipCancel();
	virtual void OnDbgDwellEnd(wxScintillaEvent& event);
	virtual void OnDbgDwellStart(wxScintillaEvent& event);
	virtual void OnDwellEnd(wxScintillaEvent& event);
	virtual void OnDwellStart(wxScintillaEvent& event);
	virtual void OnEnterHit();
	virtual void OnFileSaved();
	virtual void OnKeyDown(wxKeyEvent& event);
	virtual void OnSciUpdateUI(wxScintillaEvent& event);
	virtual void OnUserTypedXChars(const wxString& word);
	virtual void RemoveMenuDynamicContent(wxMenu* menu);
	virtual void RetagFile();
	virtual void SemicolonShift();
	virtual void SetActive();
};

#endif // CONTEXTHTML_H

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_base.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef CONTEXT_BASE_H
#define CONTEXT_BASE_H

#include "wx/string.h"
#include "wx/wxscintilla.h"
#include "smart_ptr.h"
#include "wx/filename.h"
#include "lexer_configuration.h"
#include <vector>
#include "entry.h"

class LEditor;

/**
 * \ingroup LiteEditor
 * \brief This class defines the language context of the editor
 *
 * \version 1.0
 * first version
 *
 * \date 04-30-2007
 *
 * \author Eran
 *
 */
class ContextBase : public wxEvtHandler
{
protected:
	LEditor *m_container;
	wxString m_name;
	wxString m_selectedWord;
	std::vector<wxMenuItem*> m_dynItems;

protected:
	void SetName(const wxString &name){m_name = name;}
	void DoApplySettings(LexerConfPtr lexPtr);
	void PrependMenuItem(wxMenu* menu, const wxString &text, wxObjectEventFunction func, int eventId = -1);
	void PrependMenuItem(wxMenu* menu, const wxString &text, int id);
	void PrependMenuItemSeparator(wxMenu* menu);

public:

	// ctor-dtor
	ContextBase(LEditor *container);
	ContextBase(const wxString &name);
	virtual ~ContextBase();

	/**
	 * Return the context parent control
	 */
	LEditor &GetCtrl() { return *m_container; }

	/**
	 * Return the context name
	 */
	const wxString &GetName() const { return m_name; }

	// every Context derived class must implement the following methods
	virtual ContextBase *NewInstance(LEditor *container) = 0;
	virtual void ApplySettings() = 0;

	// functions with default implementation:
	virtual bool IsDefaultContext() const {return true;}
	virtual void OnCallTipClick(wxScintillaEvent& event){event.Skip();}
	virtual void OnCalltipCancel(){};
	virtual void OnDwellEnd(wxScintillaEvent & event){event.Skip();}
	virtual void OnDbgDwellEnd(wxScintillaEvent & event){event.Skip();}
	virtual void OnDwellStart(wxScintillaEvent & event){event.Skip();}
	virtual void OnDbgDwellStart(wxScintillaEvent & event){event.Skip();}
	virtual void OnKeyDown(wxKeyEvent &event) {event.Skip();}
	virtual void AddMenuDynamicContent(wxMenu *WXUNUSED(menu)) {}
	virtual void RemoveMenuDynamicContent(wxMenu *WXUNUSED(menu)) {}
	virtual void OnSciUpdateUI(wxScintillaEvent& WXUNUSED(event)){}
	virtual void OnFileSaved(){}
	virtual void OnEnterHit(){}
	virtual void RetagFile(){}
	virtual void OnUserTypedXChars(const wxString &WXUNUSED(word)){}
	virtual wxString CallTipContent(){return wxEmptyString;}
    virtual void SetActive(){}
	virtual bool IsCommentOrString(long WXUNUSED(pos)) { return false; }
	virtual void AutoIndent(const wxChar&);
	virtual void CompleteWord(){}
	virtual void CodeComplete(long pos = wxNOT_FOUND) {wxUnusedVar(pos);}
	virtual void GotoDefinition(){}
	virtual void GotoPreviousDefintion(){}
    virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl){return NULL;}
	virtual wxString GetCurrentScopeName() {return wxEmptyString;}
	virtual void SemicolonShift(){}
	virtual int  DoGetCalltipParamterIndex();
	
    // ctrl-click style navigation support
	virtual int  GetHyperlinkRange(int pos, int &start, int &end);
    virtual void GoHyperlink(int start, int end, int type, bool alt);

	//override this method if you wish to provide context based right click menu
	virtual wxMenu *GetMenu() ;
};

typedef SmartPtr<ContextBase> ContextBasePtr;
#endif // CONTEXT_BASE_H


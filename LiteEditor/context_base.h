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
#include <wx/stc/stc.h>
#include "smart_ptr.h"
#include "wx/filename.h"
#include "lexer_configuration.h"
#include <vector>
#include "entry.h"
#include <set>
#include "macros.h"

class clEditor;

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
    clEditor* m_container;
    wxString m_name;
    wxString m_selectedWord;
    std::vector<wxMenuItem*> m_dynItems;
    wxStringSet_t m_completionTriggerStrings;

protected:
    void SetName(const wxString& name) { m_name = name; }
    void DoApplySettings(LexerConf::Ptr_t lexPtr);
    void PrependMenuItem(wxMenu* menu, const wxString& text, wxObjectEventFunction func, int eventId = -1);
    void PrependMenuItem(wxMenu* menu, const wxString& text, int id);
    void PrependMenuItemSeparator(wxMenu* menu);

    /**
     * @brief search for the next 'what' in the editor
     */
    int FindNext(const wxString& what, int& pos, bool wholePage = false);
    /**
     * @brief search for the next 'what' in the editor
     */
    int FindPrev(const wxString& what, int& pos, bool wholePage = false);

public:
    // ctor-dtor
    ContextBase(clEditor* container);
    ContextBase(const wxString& name);
    virtual ~ContextBase();

    /**
     * @brief return true if str is a string that should trigger a code completion
     */
    virtual bool IsStringTriggerCodeComplete(const wxString& str) const;

    /**
     * @brief when the context is requested to display a code completion for keywords
     * this functions determines which keyword set to display ( there are up to 5, indexed from: 0-4)
     */
    virtual int GetActiveKeywordSet() const { return 0; }

    /**
     * Return the context parent control
     */
    clEditor& GetCtrl() { return *m_container; }

    clEditor& GetCtrl() const { return *m_container; }
    /**
     * Return the context name
     */
    const wxString& GetName() const { return m_name; }

    // every Context derived class must implement the following methods
    virtual ContextBase* NewInstance(clEditor* container) = 0;
    virtual void ApplySettings() = 0;

    // functions with default implementation:
    virtual bool IsDefaultContext() const { return true; }
    virtual void OnCallTipClick(wxStyledTextEvent& event) { event.Skip(); }
    virtual void OnCalltipCancel(){};
    virtual void OnDwellEnd(wxStyledTextEvent& event) { event.Skip(); }
    virtual void OnDbgDwellEnd(wxStyledTextEvent& event) { event.Skip(); }
    virtual void OnDwellStart(wxStyledTextEvent& event) { event.Skip(); }
    virtual void OnDbgDwellStart(wxStyledTextEvent& event) { event.Skip(); }
    virtual void OnKeyDown(wxKeyEvent& event) { event.Skip(); }
    virtual void AddMenuDynamicContent(wxMenu* WXUNUSED(menu)) {}
    virtual void RemoveMenuDynamicContent(wxMenu* WXUNUSED(menu)) {}
    virtual void OnSciUpdateUI(wxStyledTextEvent& WXUNUSED(event)) {}
    virtual void OnFileSaved() {}
    virtual void OnEnterHit() {}
    virtual void RetagFile() {}
    virtual void OnUserTypedXChars(const wxString& word);
    virtual wxString CallTipContent() { return wxEmptyString; }
    virtual void SetActive() {}
    virtual bool IsCommentOrString(long WXUNUSED(pos)) { return false; }
    /**
     * @brief return true if the caret is at a block comment
     */
    virtual bool IsAtBlockComment() const { return false; }
    /**
     * @brief return true if the caret is at a line comment
     */
    virtual bool IsAtLineComment() const { return false; }
    virtual void AutoIndent(const wxChar&);
    virtual void AutoAddComment();
    virtual void CompleteWord() {}
    virtual void CodeComplete(long pos = wxNOT_FOUND) { wxUnusedVar(pos); }
    virtual void GotoDefinition() {}
    virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl) { return NULL; }
    virtual wxString GetCurrentScopeName() { return wxEmptyString; }
    virtual void SemicolonShift() {}
    virtual int DoGetCalltipParamterIndex();

    // ctrl-click style navigation support
    virtual int GetHyperlinkRange(int pos, int& start, int& end);
    virtual void GoHyperlink(int start, int end, int type, bool alt);

    // override this method if you wish to provide context based right click menu
    virtual wxMenu* GetMenu();

    /**
     * @brief colour tokens in the current editor
     * @param workspaceTokens list of token that are associated with the workspace
     */
    virtual void ColourContextTokens(const wxString& workspaceTokensStr, const wxString& localsTokensStr)
    {
        wxUnusedVar(workspaceTokensStr);
        wxUnusedVar(localsTokensStr);
    }

    /**
     * @brief process any idle actions by the context
     */
    virtual void ProcessIdleActions() {}
};

typedef SmartPtr<ContextBase> ContextBasePtr;
#endif // CONTEXT_BASE_H

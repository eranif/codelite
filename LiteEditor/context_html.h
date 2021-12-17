//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : context_html.h
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

#ifndef CONTEXTHTML_H
#define CONTEXTHTML_H

#include "context_base.h" // Base class: ContextBase

class ContextHtml : public ContextBase
{
public:
    ContextHtml();
    ContextHtml(clEditor *Editor);
    virtual ~ContextHtml();

public:
    virtual int GetActiveKeywordSet() const;
    virtual int  DoGetCalltipParamterIndex();
    virtual wxMenu* GetMenu();
    virtual void AddMenuDynamicContent(wxMenu* menu);
    virtual void ApplySettings();
    virtual void AutoIndent(const wxChar&);
    virtual wxString CallTipContent();
    virtual wxString GetCurrentScopeName();
    virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
    virtual bool GotoDefinition();
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
    
};

#endif // CONTEXTHTML_H

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
    explicit ContextHtml(clEditor *Editor);
    ~ContextHtml() override = default;

public:
    int GetActiveKeywordSet() const override;
    int DoGetCalltipParamterIndex() override;
    wxMenu* GetMenu() override;
    void AddMenuDynamicContent(wxMenu* menu) override;
    void ApplySettings() override;
    void AutoIndent(const wxChar&) override;
    wxString CallTipContent() override;
    wxString GetCurrentScopeName() override;
    TagEntryPtr GetTagAtCaret(bool scoped, bool impl) override;
    bool GotoDefinition() override;
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

    bool IsComment(long pos) const;
};

#endif // CONTEXTHTML_H

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_cpp.h
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
#ifndef CONTEXT_CPP_H
#define CONTEXT_CPP_H

#include "LSP/LSPEvent.h"
#include "Cxx/cpptoken.h"
#include "cl_command_event.h"
#include "context_base.h"
#include "ctags_manager.h"
#include "database/entry.h"
#include "macros.h"

#include <map>

class RefactorSource;

class ContextCpp : public ContextBase
{
    std::map<wxString, int> m_propertyInt;
    wxMenu* m_rclickMenu;

    static wxBitmap m_cppFileBmp;
    static wxBitmap m_hFileBmp;
    static wxBitmap m_otherFileBmp;

protected:
    void OnShowCodeNavMenu(clCodeCompletionEvent& e);
    void OnCodeCompleteFiles(clCodeCompletionEvent& event);
    void OnSymbolDeclaraionFound(LSPEvent& event);

private:
    bool TryOpenFile(const wxFileName& fileName, bool lookInEntireWorkspace = true);
    bool IsJavaScript() const;

    void DisplayFilesCompletionBox(const wxString& word);
    bool DoGetFunctionBody(long curPos, long& blockStartPos, long& blockEndPos, wxString& content);
    void Initialize();
    bool DoCodeComplete(long pos);
    void DoCreateFile(const wxFileName& fn);
    void DoUpdateCalltipHighlight();
    size_t DoGetEntriesForHeaderAndImpl(std::vector<TagEntryPtr>& prototypes, std::vector<TagEntryPtr>& functions,
                                        wxString& otherfile);

public:
    /**
     * @brief
     * @return
     */
    bool IsAtBlockComment() const override;
    /**
     * @brief
     * @return
     */
    bool IsAtLineComment() const override;
    ContextCpp(clEditor* container);
    bool IsDefaultContext() const override;

    virtual ~ContextCpp();
    ContextCpp();
    ContextBase* NewInstance(clEditor* container) override;
    bool CompleteWord() override;
    bool CodeComplete(long pos = wxNOT_FOUND) override;
    bool GotoDefinition() override;
    wxString GetCurrentScopeName() override;
    void AutoIndent(const wxChar&) override;
    bool IsCommentOrString(long pos) override;
    virtual bool IsComment(long pos);
    void AddMenuDynamicContent(wxMenu* menu) override;
    void RemoveMenuDynamicContent(wxMenu* menu) override;
    void ApplySettings() override;
    void RetagFile() override;
    wxString CallTipContent() override;
    void SetActive() override;
    void SemicolonShift() override;
    void ProcessIdleActions() override;

    // override swapfiles features
    virtual void SwapFiles(const wxFileName& fileName);

    // Event handlers
    void OnDwellEnd(wxStyledTextEvent& event) override;
    bool GetHoverTip(int pos) override;
    void OnDbgDwellEnd(wxStyledTextEvent& event) override;
    void OnDbgDwellStart(wxStyledTextEvent& event) override;
    void OnSciUpdateUI(wxStyledTextEvent& event) override;
    void OnFileSaved() override;
    void AutoAddComment() override;

    // Capture menu events
    // return this context specific right click menu
    wxMenu* GetMenu() override;
    virtual void OnSwapFiles(wxCommandEvent& event);
    virtual void OnInsertDoxyComment(wxCommandEvent& event);
    virtual void OnCommentSelection(wxCommandEvent& event);
    virtual void OnCommentLine(wxCommandEvent& event);
    virtual void OnGenerateSettersGetters(wxCommandEvent& event);
    virtual void OnFindImpl(wxCommandEvent& event);
    virtual void OnFindDecl(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event) override;
    virtual void OnUpdateUI(wxUpdateUIEvent& event);
    virtual void OnContextOpenDocument(wxCommandEvent& event);
    virtual void OnAddIncludeFile(wxCommandEvent& e);
    virtual void OnAddForwardDecl(wxCommandEvent& e);
    virtual void OnMoveImpl(wxCommandEvent& e);
    virtual void OnAddImpl(wxCommandEvent& e);
    virtual void OnAddMultiImpl(wxCommandEvent& e);
    virtual void OnRetagFile(wxCommandEvent& e);
    virtual void OnUserTypedXChars(const wxString& word);
    void OnCallTipClick(wxStyledTextEvent& e) override;
    void OnCalltipCancel() override;
    DECLARE_EVENT_TABLE()

private:
    wxString GetWordUnderCaret();
    wxString GetFileImageString(const wxString& ext);
    wxString GetImageString(const TagEntry& entry);
    wxString GetExpression(long pos, bool onlyWord, clEditor* editor = NULL, bool forCC = true);
    bool DoGotoSymbol(TagEntryPtr tag);
    bool IsIncludeStatement(const wxString& line, wxString* fileName = NULL, wxString* fileNameUpToCaret = NULL);
    void RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
    int FindLineToAddInclude();
    void MakeCppKeywordsTags(const wxString& word, std::vector<TagEntryPtr>& tags);
    void DoOpenWorkspaceFile();
    bool DoGetSingatureRange(int line, int& start, int& end, clEditor* ctrl);
    /**
     * @brief add missing implementaions. If line_number is provided
     * then only add the function found on this line number
     */
    void DoAddFunctionImplementation(int line_number = wxNOT_FOUND);

public:
    void DoMakeDoxyCommentString(DoxygenComment& dc, const wxString& blockPrefix, wxChar keywordPrefix);
    /**
     * \brief replace list of tokens representd by li with 'word'
     * \param li
     * \return
     */
    static void ReplaceInFiles(const wxString& word, const CppToken::Vec_t& li);

private:
    /**
     * \brief try to find a swapped file for this rhs. The logic is based on the C++ coding conventions
     * a swapped file for a.cpp will be a.h or a.hpp
     * \param rhs input
     * \param lhs output
     * \return true if such sibling file exist, false otherwise
     */
    bool FindSwappedFile(const wxFileName& rhs, wxStringSet_t& others);
    bool FindSwappedFile(const wxFileName& rhs, wxString& lhs);

    /**
     * @brief format editor
     * @param editor
     */
    void DoFormatEditor(clEditor* editor);
};

#endif // CONTEXT_CPP_H

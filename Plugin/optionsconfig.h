//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : optionsconfig.h
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

#ifndef OPTIONS_CONFIG_H
#define OPTIONS_CONFIG_H

#include "clBitset.hpp"
#include "clEditorConfig.h"
#include "codelite_exports.h"
#include "configuration_object.h"

#include <memory>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/string.h>
#include <wx/xml/xml.h>

class WXDLLIMPEXP_SDK OptionsConfig : public ConfObject
{
public:
    enum {
        Opt_Unused10,
        Opt_Unused11,
        Opt_Unused12,
        Opt_AutoCompleteCurlyBraces,
        Opt_AutoCompleteNormalBraces,
        Opt_SmartAddFiles,
        Opt_IconSet_FreshFarm,
        Opt_TabStyleTRAPEZOID,
        Opt_IconSet_Classic,
        Opt_AutoCompleteDoubleQuotes,
        Opt_NavKey_Shift,
        Opt_NavKey_Alt,
        Opt_NavKey_Control,
        Opt_IconSet_Classic_Dark,
        Opt_Mark_Debugger_Line,
        Opt_TabNoXButton,
        Opt_TabColourPersistent,
        Opt_TabColourDark,
        Opt_Use_CodeLite_Terminal,
        Opt_Unused14,
        Opt_Unused15,
        Opt_AllowCaretAfterEndOfLine,
        Opt_HideDockingWindowCaption,
        Opt_WrapQuotes,
        Opt_WrapBrackets,
        Opt_WrapCmdWithDoubleQuotes,
        Opt_FoldHighlightActiveBlock,
        Opt_EnsureCaptionsVisible,
        Opt_DisableMouseCtrlZoom,
        Opt_UseBlockCaret,
        Opt_TabStyleMinimal,
        Opt_TabNoPath,
        Opt2_MouseScrollSwitchTabs,
        Opt2_SortTabsDropdownAlphabetically,
        Opt2_PlaceNavBarAtTheTop,
        Opt2_DisableCtrlTabForTabSwitching,
        Opt2_SortNavBarDropdown,
    };

    enum { nbTabHt_Tiny = 1, nbTabHt_Short, nbTabHt_Medium, nbTabHt_Tall };

protected:
    bool m_displayFoldMargin;
    bool m_underlineFoldLine;
    bool m_scrollBeyondLastLine;
    wxString m_foldStyle;
    bool m_displayBookmarkMargin;
    wxString m_bookmarkShape;
    wxString m_bookmarkBgColours;
    wxString m_bookmarkFgColours;
    wxString m_bookmarkLabels;
    bool m_highlightCaretLine;
    bool m_highlightCaretLineWithColour;
    bool m_clearHighlightedWordsOnFind;
    bool m_displayLineNumbers;
    bool m_relativeLineNumbers;
    bool m_showIndentationGuidelines;
    wxColour m_caretLineColour;
    bool m_indentUsesTabs;
    int m_indentWidth;
    int m_tabWidth;
    int m_iconsSize;
    wxFontEncoding m_fileFontEncoding;
    int m_showWhitespaces;
    bool m_foldCompact;
    bool m_foldAtElse;
    bool m_foldPreprocessor;
    bool m_highlightMatchedBraces;
    wxColour m_foldBgColour;
    bool m_autoAdjustHScrollBarWidth;
    int m_caretWidth;
    int m_caretBlinkPeriod;
    bool m_copyLineEmptySelection;
    wxString m_programConsoleCommand;
    wxString m_eolMode;
    bool m_trackEditorChanges;
    bool m_hideOutputPaneOnUserClick;
    bool m_hideOutputPaneNotIfBuild;
    bool m_hideOutputPaneNotIfSearch;
    bool m_hideOutputPaneNotIfReplace;
    bool m_hideOutputPaneNotIfReferences;
    bool m_hideOutputPaneNotIfOutput;
    bool m_hideOutputPaneNotIfTrace;
    bool m_hideOutputPaneNotIfTasks;
    bool m_hideOutputPaneNotIfBuildQ;
    bool m_hideOutputPaneNotIfCppCheck;
    bool m_hideOutputPaneNotIfSvn;
    bool m_hideOutputPaneNotIfCscope;
    bool m_hideOutputPaneNotIfGit;
    bool m_hideOutputPaneNotIfDebug;
    bool m_hideOutputPaneNotIfMemCheck;
    bool m_findBarAtBottom;
    bool m_showReplaceBar;
    bool m_TrimLine;
    bool m_AppendLF;
    bool m_disableSmartIndent;
    bool m_disableSemicolonShift;
    int m_caretLineAlpha;
    bool m_dontAutoFoldResults;
    bool m_dontOverrideSearchStringWithSelection;
    bool m_findNextOrPreviousUseSelection;
    bool m_showDebugOnRun;
    bool m_caretUseCamelCase;
    bool m_dontTrimCaretLine;
    bool m_wordWrap;
    int m_dockingStyle;
    bool m_mswTheme;
    wxString m_preferredLocale;
    bool m_useLocale;
    bool m_trimOnlyModifiedLines;
    clBitset m_options;
    wxColour m_debuggerMarkerLine;
    wxDirection m_workspaceTabsDirection; // Up/Down/Left/Right
    wxDirection m_outputTabsDirection;    // Up/Down
    bool m_indentedComments;
    int m_nbTabHeight; // Should notebook tabs be too tall, too short or...
    wxString m_webSearchPrefix;
    bool m_smartParen = true;
    bool m_lineNumberHighlightCurrent = false;
    bool m_showRightMarginIndicator = false;
    int m_rightMarginColumn = 120;

public:
    // Helpers
    void EnableOption(size_t flag, bool b);
    bool HasOption(size_t flag) const;

public:
    OptionsConfig() {}
    OptionsConfig(wxXmlNode* node);
    virtual ~OptionsConfig();

    void SetOutputTabsDirection(const wxDirection& outputTabsDirection)
    {
        this->m_outputTabsDirection = outputTabsDirection;
    }
    void SetWorkspaceTabsDirection(const wxDirection& workspaceTabsDirection)
    {
        this->m_workspaceTabsDirection = workspaceTabsDirection;
    }
    const wxDirection& GetOutputTabsDirection() const { return m_outputTabsDirection; }
    const wxDirection& GetWorkspaceTabsDirection() const { return m_workspaceTabsDirection; }
    wxString GetEOLAsString() const;
    //-------------------------------------
    // Setters/Getters
    //-------------------------------------

    void SetSmartParen(bool smartParen) { this->m_smartParen = smartParen; }
    bool IsSmartParen() const { return m_smartParen; }
    void SetTabColourMatchesTheme(bool b) { EnableOption(Opt_TabColourPersistent, !b); }
    bool IsTabColourMatchesTheme() const;
    void SetTabColourDark(bool b) { EnableOption(Opt_TabColourDark, b); }
    bool IsTabColourDark() const;
    void SetTabHasXButton(bool b) { EnableOption(Opt_TabNoXButton, !b); }
    bool IsTabHasXButton() const { return !HasOption(Opt_TabNoXButton); }
    void SetTabShowPath(bool b) { EnableOption(Opt_TabNoPath, !b); }
    bool IsTabShowPath() const { return !HasOption(Opt_TabNoPath); }
    bool IsMouseScrollSwitchTabs() const { return HasOption(Opt2_MouseScrollSwitchTabs); }
    void SetMouseScrollSwitchTabs(bool b) { EnableOption(Opt2_MouseScrollSwitchTabs, b); }
    bool IsSortTabsDropdownAlphabetically() const { return HasOption(Opt2_SortTabsDropdownAlphabetically); }
    void SetSortTabsDropdownAlphabetically(bool b) { EnableOption(Opt2_SortTabsDropdownAlphabetically, b); }
    bool IsNavBarTop() const { return HasOption(Opt2_PlaceNavBarAtTheTop); }
    void SetNavBarTop(bool b) { EnableOption(Opt2_PlaceNavBarAtTheTop, b); }
    bool IsCtrlTabEnabled() const { return !HasOption(Opt2_DisableCtrlTabForTabSwitching); }
    void SetCtrlTabEnabled(bool b) { EnableOption(Opt2_DisableCtrlTabForTabSwitching, !b); }
    bool IsSortNavBarDropdown() const { return HasOption(Opt2_SortNavBarDropdown); }
    void SetSortNavBarDropdown(bool b) { EnableOption(Opt2_SortNavBarDropdown, b); }
    void SetTrimOnlyModifiedLines(bool trimOnlyModifiedLines) { this->m_trimOnlyModifiedLines = trimOnlyModifiedLines; }
    bool GetTrimOnlyModifiedLines() const { return m_trimOnlyModifiedLines; }
    void SetPreferredLocale(const wxString& preferredLocale) { this->m_preferredLocale = preferredLocale; }
    wxString GetPreferredLocale() const { return m_preferredLocale; }
    void SetUseLocale(bool useLocale) { this->m_useLocale = useLocale; }
    bool GetUseLocale() const { return m_useLocale; }
    void SetMswTheme(bool mswTheme) { this->m_mswTheme = mswTheme; }
    bool GetMswTheme() const { return m_mswTheme; }
    void SetDockingStyle(int dockingStyle) { this->m_dockingStyle = dockingStyle; }
    int GetDockingStyle() const { return m_dockingStyle; }
    void SetCaretUseCamelCase(bool caretUseCamelCase) { this->m_caretUseCamelCase = caretUseCamelCase; }
    bool GetCaretUseCamelCase() const { return m_caretUseCamelCase; }
    void SetDontAutoFoldResults(bool dontAutoFoldResults) { this->m_dontAutoFoldResults = dontAutoFoldResults; }
    bool GetDontAutoFoldResults() const { return m_dontAutoFoldResults; }
    void SetDontOverrideSearchStringWithSelection(bool dontOverrideSearchStringWithSelection)
    {
        m_dontOverrideSearchStringWithSelection = dontOverrideSearchStringWithSelection;
    }
    bool GetDontOverrideSearchStringWithSelection() const { return m_dontOverrideSearchStringWithSelection; }
    void SetFindNextOrPreviousUseSelection(bool findNextOrPreviousUseSelection)
    {
        m_findNextOrPreviousUseSelection = findNextOrPreviousUseSelection;
    }
    bool GetFindNextOrPreviousUseSelection() const { return m_findNextOrPreviousUseSelection; }
    void SetShowDebugOnRun(bool showDebugOnRun) { this->m_showDebugOnRun = showDebugOnRun; }
    bool GetShowDebugOnRun() const { return m_showDebugOnRun; }
    bool GetDisableSemicolonShift() const { return m_disableSemicolonShift; }
    void SetDisableSemicolonShift(bool disableSemicolonShift) { this->m_disableSemicolonShift = disableSemicolonShift; }
    void SetDisableSmartIndent(bool disableSmartIndent) { this->m_disableSmartIndent = disableSmartIndent; }
    bool GetDisableSmartIndent() const { return m_disableSmartIndent; }
    void SetTrimLine(bool trimLine) { this->m_TrimLine = trimLine; }
    bool GetTrimLine() const { return m_TrimLine; }
    void SetAppendLF(bool appendLF) { this->m_AppendLF = appendLF; }
    bool GetAppendLF() const { return m_AppendLF; }
    void SetFindBarAtBottom(bool findBarAtBottom) { this->m_findBarAtBottom = findBarAtBottom; }
    bool GetFindBarAtBottom() const { return m_findBarAtBottom; }
    void SetShowReplaceBar(bool show) { m_showReplaceBar = show; }
    bool GetShowReplaceBar() const { return m_showReplaceBar; }
    void SetHideOutputPaneOnUserClick(bool hideOutputPaneOnUserClick)
    {
        this->m_hideOutputPaneOnUserClick = hideOutputPaneOnUserClick;
    }
    bool GetHideOutputPaneOnUserClick() const { return m_hideOutputPaneOnUserClick; }
    void SetHideOutputPaneNotIfBuild(bool HideOutputPaneNotIfBuild)
    {
        this->m_hideOutputPaneNotIfBuild = HideOutputPaneNotIfBuild;
    }
    bool GetHideOutputPaneNotIfBuild() const { return m_hideOutputPaneNotIfBuild; }
    void SetHideOutputPaneNotIfSearch(bool HideOutputPaneNotIfSearch)
    {
        this->m_hideOutputPaneNotIfSearch = HideOutputPaneNotIfSearch;
    }
    bool GetHideOutputPaneNotIfSearch() const { return m_hideOutputPaneNotIfSearch; }
    void SetHideOutputPaneNotIfReplace(bool HideOutputPaneNotIfReplace)
    {
        this->m_hideOutputPaneNotIfReplace = HideOutputPaneNotIfReplace;
    }
    bool GetHideOutputPaneNotIfReplace() const { return m_hideOutputPaneNotIfReplace; }
    void SetHideOutputPaneNotIfReferences(bool HideOutputPaneNotIfReferences)
    {
        this->m_hideOutputPaneNotIfReferences = HideOutputPaneNotIfReferences;
    }
    bool GetHideOutputPaneNotIfReferences() const { return m_hideOutputPaneNotIfReferences; }
    void SetHideOutputPaneNotIfOutput(bool HideOutputPaneNotIfOutput)
    {
        this->m_hideOutputPaneNotIfOutput = HideOutputPaneNotIfOutput;
    }
    bool GetHideOutputPaneNotIfOutput() const { return m_hideOutputPaneNotIfOutput; }
    void SetHideOutputPaneNotIfTrace(bool HideOutputPaneNotIfTrace)
    {
        this->m_hideOutputPaneNotIfTrace = HideOutputPaneNotIfTrace;
    }
    bool GetHideOutputPaneNotIfTrace() const { return m_hideOutputPaneNotIfTrace; }
    void SetHideOutputPaneNotIfTasks(bool HideOutputPaneNotIfTasks)
    {
        this->m_hideOutputPaneNotIfTasks = HideOutputPaneNotIfTasks;
    }
    bool GetHideOutputPaneNotIfTasks() const { return m_hideOutputPaneNotIfTasks; }
    void SetHideOutputPaneNotIfBuildQ(bool HideOutputPaneNotIfBuildQ)
    {
        this->m_hideOutputPaneNotIfBuildQ = HideOutputPaneNotIfBuildQ;
    }
    bool GetHideOutputPaneNotIfBuildQ() const { return m_hideOutputPaneNotIfBuildQ; }
    void SetHideOutputPaneNotIfCppCheck(bool HideOutputPaneNotIfCppCheck)
    {
        this->m_hideOutputPaneNotIfCppCheck = HideOutputPaneNotIfCppCheck;
    }
    bool GetHideOutputPaneNotIfCppCheck() const { return m_hideOutputPaneNotIfCppCheck; }
    void SetHideOutputPaneNotIfSvn(bool HideOutputPaneNotIfSvn)
    {
        this->m_hideOutputPaneNotIfSvn = HideOutputPaneNotIfSvn;
    }
    bool GetHideOutputPaneNotIfSvn() const { return m_hideOutputPaneNotIfSvn; }
    void SetHideOutputPaneNotIfCscope(bool HideOutputPaneNotIfCscope)
    {
        this->m_hideOutputPaneNotIfCscope = HideOutputPaneNotIfCscope;
    }
    bool GetHideOutputPaneNotIfCscope() const { return m_hideOutputPaneNotIfCscope; }
    void SetHideOutputPaneNotIfGit(bool HideOutputPaneNotIfGit)
    {
        this->m_hideOutputPaneNotIfGit = HideOutputPaneNotIfGit;
    }
    bool GetHideOutputPaneNotIfGit() const { return m_hideOutputPaneNotIfGit; }
    void SetHideOutputPaneNotIfDebug(bool HideOutputPaneNotIfDebug)
    {
        this->m_hideOutputPaneNotIfDebug = HideOutputPaneNotIfDebug;
    }
    bool GetHideOutputPaneNotIfDebug() const { return m_hideOutputPaneNotIfDebug; }
    void SetHideOutputPaneNotIfMemCheck(bool HideOutputPaneNotIfMemCheck)
    {
        this->m_hideOutputPaneNotIfMemCheck = HideOutputPaneNotIfMemCheck;
    }
    bool GetHideOutputPaneNotIfMemCheck() const { return m_hideOutputPaneNotIfMemCheck; }

    void SetTrackChanges(bool b) { this->m_trackEditorChanges = b; }
    bool IsTrackChanges() const { return m_trackEditorChanges; }

    bool GetIndentedComments() const { return m_indentedComments; }
    bool GetDisplayFoldMargin() const { return m_displayFoldMargin; }
    bool GetUnderlineFoldLine() const { return m_underlineFoldLine; }
    bool GetScrollBeyondLastLine() const { return m_scrollBeyondLastLine; }
    wxString GetFoldStyle() const { return m_foldStyle; }
    bool GetDisplayBookmarkMargin() const { return m_displayBookmarkMargin; }
    wxString GetBookmarkShape() const { return m_bookmarkShape; }
    wxColour GetBookmarkFgColour(size_t index) const;

    wxColour GetBookmarkBgColour(size_t index) const;

    wxString GetBookmarkLabel(size_t index) const;

    bool GetClearHighlightedWordsOnFind() const { return m_clearHighlightedWordsOnFind; }

    bool GetHighlightCaretLine() const { return m_highlightCaretLine; }
    bool IsHighlightCaretLineWithColour() const { return m_highlightCaretLineWithColour; }
    void SetHighlightCaretLineWithColour(bool b) { m_highlightCaretLineWithColour = b; }
    bool GetDisplayLineNumbers() const { return m_displayLineNumbers; }
    bool GetRelativeLineNumbers() const { return m_relativeLineNumbers; }
    bool GetShowIndentationGuidelines() const { return m_showIndentationGuidelines; }
    wxColour GetCaretLineColour() const { return m_caretLineColour; }

    void SetIndentedComments(bool b) { m_indentedComments = b; }
    void SetDisplayFoldMargin(bool b) { m_displayFoldMargin = b; }
    void SetUnderlineFoldLine(bool b) { m_underlineFoldLine = b; }
    void SetScrollBeyondLastLine(bool b) { m_scrollBeyondLastLine = b; }
    void SetFoldStyle(wxString s) { m_foldStyle = s; }
    void SetDisplayBookmarkMargin(bool b) { m_displayBookmarkMargin = b; }
    void SetBookmarkShape(wxString s) { m_bookmarkShape = s; }
    void SetBookmarkFgColour(wxColour c, size_t index);

    void SetBookmarkBgColour(wxColour c, size_t index);

    void SetBookmarkLabel(const wxString& label, size_t index);

    void SetClearHighlightedWordsOnFind(bool b) { m_clearHighlightedWordsOnFind = b; }

    void SetHighlightCaretLine(bool b) { m_highlightCaretLine = b; }
    void SetDisplayLineNumbers(bool b) { m_displayLineNumbers = b; }
    void SetRelativeLineNumbers(bool b) { m_relativeLineNumbers = b; }
    void SetShowIndentationGuidelines(bool b) { m_showIndentationGuidelines = b; }
    void SetCaretLineColour(wxColour c) { m_caretLineColour = c; }

    void SetIndentUsesTabs(bool indentUsesTabs) { this->m_indentUsesTabs = indentUsesTabs; }
    bool GetIndentUsesTabs() const { return m_indentUsesTabs; }
    void SetIndentWidth(int indentWidth) { this->m_indentWidth = indentWidth; }
    int GetIndentWidth() const { return m_indentWidth; }
    void SetTabWidth(int tabWidth) { this->m_tabWidth = tabWidth; }
    int GetTabWidth() const { return m_tabWidth; }

    void SetIconsSize(int iconsSize) { this->m_iconsSize = iconsSize; }
    int GetIconsSize() const { return m_iconsSize; }

    void SetFileFontEncoding(const wxString& strFileFontEncoding);
    wxFontEncoding GetFileFontEncoding() const { return m_fileFontEncoding; }

    void SetShowWhitespaces(int showWhitespaces) { this->m_showWhitespaces = showWhitespaces; }
    int GetShowWhitespaces() const { return m_showWhitespaces; }

    void SetFoldAtElse(bool foldAtElse) { this->m_foldAtElse = foldAtElse; }
    void SetFoldCompact(bool foldCompact) { this->m_foldCompact = foldCompact; }
    bool GetFoldAtElse() const { return m_foldAtElse; }
    bool GetFoldCompact() const { return m_foldCompact; }
    void SetFoldPreprocessor(bool foldPreprocessor) { this->m_foldPreprocessor = foldPreprocessor; }
    bool GetFoldPreprocessor() const { return m_foldPreprocessor; }

    void SetShowRightMarginIndicator(bool showRightMarginIndicator)
    {
        this->m_showRightMarginIndicator = showRightMarginIndicator;
    }
    void SetRightMarginColumn(int rightMarginColumn) { this->m_rightMarginColumn = rightMarginColumn; }
    bool IsShowRightMarginIndicator() const { return m_showRightMarginIndicator; }
    int GetRightMarginColumn() const { return m_rightMarginColumn; }
    void SetHighlightMatchedBraces(bool highlightMatchedBraces)
    {
        this->m_highlightMatchedBraces = highlightMatchedBraces;
    }
    bool GetHighlightMatchedBraces() const { return m_highlightMatchedBraces; }

    void SetAutoAddMatchedCurlyBraces(bool autoAddMatchedBraces)
    {
        EnableOption(Opt_AutoCompleteCurlyBraces, autoAddMatchedBraces);
    }

    bool GetAutoAddMatchedCurlyBraces() const { return HasOption(Opt_AutoCompleteCurlyBraces); }

    void SetAutoAddMatchedNormalBraces(bool b) { EnableOption(Opt_AutoCompleteNormalBraces, b); }

    bool GetAutoAddMatchedNormalBraces() const { return HasOption(Opt_AutoCompleteNormalBraces); }
    bool GetAutoCompleteDoubleQuotes() const { return HasOption(Opt_AutoCompleteDoubleQuotes); }
    void SetAutoCompleteDoubleQuotes(bool b) { EnableOption(Opt_AutoCompleteDoubleQuotes, b); }
    void SetHighlightFoldWhenActive(bool b) { EnableOption(Opt_FoldHighlightActiveBlock, b); }
    bool IsHighlightFoldWhenActive() const { return HasOption(Opt_FoldHighlightActiveBlock); }
    void SetFoldBgColour(const wxColour& foldBgColour) { this->m_foldBgColour = foldBgColour; }
    const wxColour& GetFoldBgColour() const { return m_foldBgColour; }
    void SetAutoAdjustHScrollBarWidth(bool autoAdjustHScrollBarWidth)
    {
        this->m_autoAdjustHScrollBarWidth = autoAdjustHScrollBarWidth;
    }
    bool GetAutoAdjustHScrollBarWidth() const { return m_autoAdjustHScrollBarWidth; }
    void SetCaretBlinkPeriod(int caretBlinkPeriod) { this->m_caretBlinkPeriod = caretBlinkPeriod; }
    void SetCaretWidth(int caretWidth) { this->m_caretWidth = caretWidth; }
    int GetCaretBlinkPeriod() const { return m_caretBlinkPeriod; }
    int GetCaretWidth() const { return m_caretWidth; }

    void SetCopyLineEmptySelection(const bool copyLineEmptySelection)
    {
        m_copyLineEmptySelection = copyLineEmptySelection;
    }
    bool GetCopyLineEmptySelection() const { return m_copyLineEmptySelection; }

    void SetProgramConsoleCommand(const wxString& programConsoleCommand)
    {
        this->m_programConsoleCommand = programConsoleCommand;
    }

    const wxString& GetProgramConsoleCommand() const { return m_programConsoleCommand; }

    void SetEolMode(const wxString& eolMode) { this->m_eolMode = eolMode; }
    const wxString& GetEolMode() const { return m_eolMode; }
    void SetCaretLineAlpha(int caretLineAlpha) { this->m_caretLineAlpha = caretLineAlpha; }
    int GetCaretLineAlpha() const { return m_caretLineAlpha; }
    bool GetDontTrimCaretLine() const { return m_dontTrimCaretLine; }

    bool GetWordWrap() const { return m_wordWrap; }
    void SetWordWrap(bool wordWrap) { this->m_wordWrap = wordWrap; }

    void SetDebuggerMarkerLine(const wxColour& debuggerMarkerLine) { this->m_debuggerMarkerLine = debuggerMarkerLine; }
    const wxColour& GetDebuggerMarkerLine() const { return m_debuggerMarkerLine; }

    void SetShowDockingWindowCaption(bool show) { EnableOption(Opt_HideDockingWindowCaption, !show); }
    void SetEnsureCaptionsVisible(bool b) { EnableOption(Opt_EnsureCaptionsVisible, b); }
    bool IsEnsureCaptionsVisible() const { return HasOption(Opt_EnsureCaptionsVisible); }
    bool IsShowDockingWindowCaption() const { return !HasOption(Opt_HideDockingWindowCaption); }
    bool IsWrapSelectionWithQuotes() const { return HasOption(Opt_WrapQuotes); }
    bool IsWrapSelectionBrackets() const { return HasOption(Opt_WrapBrackets); }
    void SetWrapSelectionWithQuotes(bool b) { return EnableOption(Opt_WrapQuotes, b); }
    void SetWrapSelectionBrackets(bool b) { return EnableOption(Opt_WrapBrackets, b); }
    void SetNotebookTabHeight(int nbTabHeight) { m_nbTabHeight = nbTabHeight; }
    int GetNotebookTabHeight() const { return m_nbTabHeight; }

    void MSWWrapCmdWithDoubleQuotes(bool b) { EnableOption(Opt_WrapCmdWithDoubleQuotes, b); }
    bool MSWIsWrapCmdWithDoubleQuotes() const { return true; }
    bool IsMouseZoomEnabled() const { return !HasOption(Opt_DisableMouseCtrlZoom); }
    void SetMouseZoomEnabled(bool b) { EnableOption(Opt_DisableMouseCtrlZoom, !b); }

    const wxString& GetWebSearchPrefix() const { return m_webSearchPrefix; }
    void SetWebSearchPrefix(const wxString& webSearchPrefix) { this->m_webSearchPrefix = webSearchPrefix; }

    void UpdateFromEditorConfig(const clEditorConfigSection& section);

    void SetLineNumberHighlightCurrent(bool lineNumberHighlightCurrent)
    {
        this->m_lineNumberHighlightCurrent = lineNumberHighlightCurrent;
    }
    bool IsLineNumberHighlightCurrent() const { return m_lineNumberHighlightCurrent; }
    /**
     * Return an XML representation of this object
     * \return XML node
     */
    wxXmlNode* ToXml() const;
};

using OptionsConfigPtr = std::shared_ptr<OptionsConfig>;

#endif // OPTIONS_CONFIG_H

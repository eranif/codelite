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

#include "clEditorConfig.h"
#include "codelite_exports.h"
#include "configuration_object.h"
#include "wx/colour.h"
#include "wx/font.h"
#include "wx/string.h"
#include "wx/xml/xml.h"

class WXDLLIMPEXP_SDK OptionsConfig : public ConfObject
{
public:
    enum {
        Opt_Unused10 = (1 << 0),
        Opt_Unused11 = (1 << 1),
        Opt_Unused12 = (1 << 2),
        Opt_AutoCompleteCurlyBraces = (1 << 3),
        Opt_AutoCompleteNormalBraces = (1 << 4),
        Opt_SmartAddFiles = (1 << 5),
        Opt_IconSet_FreshFarm = (1 << 6),
        Opt_TabStyleTRAPEZOID = (1 << 7),
        Opt_IconSet_Classic = (1 << 8),
        Opt_AutoCompleteDoubleQuotes = (1 << 9),
        Opt_NavKey_Shift = (1 << 10), // (No longer actively used)
        Opt_NavKey_Alt = (1 << 11),
        Opt_NavKey_Control = (1 << 12),
        Opt_IconSet_Classic_Dark = (1 << 13),
        Opt_Mark_Debugger_Line = (1 << 14),
        Opt_TabNoXButton = (1 << 15),
        Opt_TabColourPersistent = (1 << 16),
        Opt_TabColourDark = (1 << 17),
        Opt_Use_CodeLite_Terminal = (1 << 18),
        Opt_Unused14 = (1 << 19),
        Opt_Unused15 = (1 << 20),
        Opt_AllowCaretAfterEndOfLine = (1 << 21),
        Opt_HideDockingWindowCaption = (1 << 22),
        Opt_WrapQuotes = (1 << 23),
        Opt_WrapBrackets = (1 << 24),
        Opt_WrapCmdWithDoubleQuotes = (1 << 25),
        Opt_FoldHighlightActiveBlock = (1 << 26),
        Opt_EnsureCaptionsVisible = (1 << 27),
        Opt_DisableMouseCtrlZoom = (1 << 28),
        Opt_UseBlockCaret = (1 << 29),
        Opt_TabStyleMinimal = (1 << 30),
    };

    enum {
        Opt2_MouseScrollSwitchTabs = (1 << 0),
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
    bool m_clearHighlitWordsOnFind;
    bool m_displayLineNumbers;
    bool m_showIndentationGuidelines;
    wxColour m_caretLineColour;
    bool m_indentUsesTabs;
    int m_indentWidth;
    int m_tabWidth;
    int m_iconsSize;
    wxFontEncoding m_fileFontEncoding;
    int m_showWhitspaces;
    bool m_foldCompact;
    bool m_foldAtElse;
    bool m_foldPreprocessor;
    int m_edgeMode;
    int m_edgeColumn;
    wxColour m_edgeColour;
    bool m_highlightMatchedBraces;
    wxColour m_foldBgColour;
    bool m_autoAdjustHScrollBarWidth;
    int m_caretWidth;
    int m_caretBlinkPeriod;
    wxString m_programConsoleCommand;
    wxString m_eolMode;
    bool m_hideChangeMarkerMargin;
    bool m_hideOutpuPaneOnUserClick;
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
    bool m_showDebugOnRun;
    bool m_caretUseCamelCase;
    bool m_dontTrimCaretLine;
    bool m_wordWrap;
    int m_dockingStyle;
    bool m_mswTheme;
    wxString m_preferredLocale;
    bool m_useLocale;
    bool m_trimOnlyModifiedLines;
    size_t m_options;
    size_t m_options2;
    wxColour m_debuggerMarkerLine;
    wxDirection m_workspaceTabsDirection; // Up/Down/Left/Right
    wxDirection m_outputTabsDirection;    // Up/Down
    bool m_indentedComments;
    int m_nbTabHeight; // Should notebook tabs be too tall, too short or...

public:
    // Helpers
    void EnableOption(size_t flag, bool b)
    {
        if(b) {
            m_options |= flag;
        } else {
            m_options &= ~flag;
        }
    }

    bool HasOption(size_t flag) const { return m_options & flag; }
    void EnableOption2(size_t flag, bool b)
    {
        if(b) {
            m_options2 |= flag;
        } else {
            m_options2 &= ~flag;
        }
    }

    bool HasOption2(size_t flag) const { return m_options2 & flag; }

public:
    OptionsConfig() {}
    OptionsConfig(wxXmlNode* node);
    virtual ~OptionsConfig(void);

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
    void SetTabColourMatchesTheme(bool b) { EnableOption(Opt_TabColourPersistent, !b); }
    bool IsTabColourMatchesTheme() const;
    void SetTabColourDark(bool b) { EnableOption(Opt_TabColourDark, b); }
    bool IsTabColourDark() const;
    void SetTabHasXButton(bool b) { EnableOption(Opt_TabNoXButton, !b); }
    bool IsTabHasXButton() const { return !HasOption(Opt_TabNoXButton); }
    bool IsMouseScrollSwitchTabs() const { return HasOption2(Opt2_MouseScrollSwitchTabs); }
    void SetMouseScrollSwitchTabs(bool b) { EnableOption2(Opt2_MouseScrollSwitchTabs, b); }

    void SetOptions(size_t options) { this->m_options = options; }
    size_t GetOptions() const { return m_options; }
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
    void SetShowDebugOnRun(bool showDebugOnRun) { this->m_showDebugOnRun = showDebugOnRun; }
    bool GetShowDebugOnRun() const { return m_showDebugOnRun; }
    bool GetDisableSemicolonShift() const { return m_disableSemicolonShift; }
    void SetDisableSemicolonShift(bool disableSemicolonShift) { this->m_disableSemicolonShift = disableSemicolonShift; }
    void SetDisableSmartIndent(bool disableSmartIndent) { this->m_disableSmartIndent = disableSmartIndent; }
    bool GetDisableSmartIndent() const { return m_disableSmartIndent; }
    void SetTrimLine(const bool& trimLine) { this->m_TrimLine = trimLine; }
    const bool& GetTrimLine() const { return m_TrimLine; }
    void SetAppendLF(const bool& appendLF) { this->m_AppendLF = appendLF; }
    const bool& GetAppendLF() const { return m_AppendLF; }
    void SetFindBarAtBottom(const bool& findBarAtBottom) { this->m_findBarAtBottom = findBarAtBottom; }
    const bool& GetFindBarAtBottom() const { return m_findBarAtBottom; }
    void SetShowReplaceBar(bool show) { m_showReplaceBar = show; }
    bool GetShowReplaceBar() const { return m_showReplaceBar; }
    void SetHideOutpuPaneOnUserClick(const bool& hideOutpuPaneOnUserClick)
    {
        this->m_hideOutpuPaneOnUserClick = hideOutpuPaneOnUserClick;
    }
    const bool& GetHideOutpuPaneOnUserClick() const { return m_hideOutpuPaneOnUserClick; }
    void SetHideOutputPaneNotIfBuild(const bool& HideOutpuPaneNotIfBuild)
    {
        this->m_hideOutputPaneNotIfBuild = HideOutpuPaneNotIfBuild;
    }
    const bool& GetHideOutputPaneNotIfBuild() const { return m_hideOutputPaneNotIfBuild; }
    void SetHideOutputPaneNotIfSearch(const bool& HideOutpuPaneNotIfSearch)
    {
        this->m_hideOutputPaneNotIfSearch = HideOutpuPaneNotIfSearch;
    }
    const bool& GetHideOutputPaneNotIfSearch() const { return m_hideOutputPaneNotIfSearch; }
    void SetHideOutputPaneNotIfReplace(const bool& HideOutpuPaneNotIfReplace)
    {
        this->m_hideOutputPaneNotIfReplace = HideOutpuPaneNotIfReplace;
    }
    const bool& GetHideOutputPaneNotIfReplace() const { return m_hideOutputPaneNotIfReplace; }
    void SetHideOutputPaneNotIfReferences(const bool& HideOutpuPaneNotIfReferences)
    {
        this->m_hideOutputPaneNotIfReferences = HideOutpuPaneNotIfReferences;
    }
    const bool& GetHideOutputPaneNotIfReferences() const { return m_hideOutputPaneNotIfReferences; }
    void SetHideOutputPaneNotIfOutput(const bool& HideOutpuPaneNotIfOutput)
    {
        this->m_hideOutputPaneNotIfOutput = HideOutpuPaneNotIfOutput;
    }
    const bool& GetHideOutputPaneNotIfOutput() const { return m_hideOutputPaneNotIfOutput; }
    void SetHideOutputPaneNotIfTrace(const bool& HideOutpuPaneNotIfTrace)
    {
        this->m_hideOutputPaneNotIfTrace = HideOutpuPaneNotIfTrace;
    }
    const bool& GetHideOutputPaneNotIfTrace() const { return m_hideOutputPaneNotIfTrace; }
    void SetHideOutputPaneNotIfTasks(const bool& HideOutpuPaneNotIfTasks)
    {
        this->m_hideOutputPaneNotIfTasks = HideOutpuPaneNotIfTasks;
    }
    const bool& GetHideOutputPaneNotIfTasks() const { return m_hideOutputPaneNotIfTasks; }
    void SetHideOutputPaneNotIfBuildQ(const bool& HideOutpuPaneNotIfBuildQ)
    {
        this->m_hideOutputPaneNotIfBuildQ = HideOutpuPaneNotIfBuildQ;
    }
    const bool& GetHideOutputPaneNotIfBuildQ() const { return m_hideOutputPaneNotIfBuildQ; }
    void SetHideOutputPaneNotIfCppCheck(const bool& HideOutpuPaneNotIfCppCheck)
    {
        this->m_hideOutputPaneNotIfCppCheck = HideOutpuPaneNotIfCppCheck;
    }
    const bool& GetHideOutputPaneNotIfCppCheck() const { return m_hideOutputPaneNotIfCppCheck; }
    void SetHideOutputPaneNotIfSvn(const bool& HideOutpuPaneNotIfSvn)
    {
        this->m_hideOutputPaneNotIfSvn = HideOutpuPaneNotIfSvn;
    }
    const bool& GetHideOutputPaneNotIfSvn() const { return m_hideOutputPaneNotIfSvn; }
    void SetHideOutputPaneNotIfCscope(const bool& HideOutpuPaneNotIfCscope)
    {
        this->m_hideOutputPaneNotIfCscope = HideOutpuPaneNotIfCscope;
    }
    const bool& GetHideOutputPaneNotIfCscope() const { return m_hideOutputPaneNotIfCscope; }
    void SetHideOutputPaneNotIfGit(const bool& HideOutpuPaneNotIfGit)
    {
        this->m_hideOutputPaneNotIfGit = HideOutpuPaneNotIfGit;
    }
    const bool& GetHideOutputPaneNotIfGit() const { return m_hideOutputPaneNotIfGit; }
    void SetHideOutputPaneNotIfDebug(const bool& HideOutpuPaneNotIfDebug)
    {
        this->m_hideOutputPaneNotIfDebug = HideOutpuPaneNotIfDebug;
    }
    const bool& GetHideOutputPaneNotIfDebug() const { return m_hideOutputPaneNotIfDebug; }
    void SetHideOutputPaneNotIfMemCheck(const bool& HideOutpuPaneNotIfMemCheck)
    {
        this->m_hideOutputPaneNotIfMemCheck = HideOutpuPaneNotIfMemCheck;
    }
    const bool& GetHideOutputPaneNotIfMemCheck() const { return m_hideOutputPaneNotIfMemCheck; }
    void SetHideChangeMarkerMargin(bool hideChangeMarkerMargin)
    {
        this->m_hideChangeMarkerMargin = hideChangeMarkerMargin;
    }

    bool GetHideChangeMarkerMargin() const { return m_hideChangeMarkerMargin; }

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

    bool GetClearHighlitWordsOnFind() const { return m_clearHighlitWordsOnFind; }

    bool GetHighlightCaretLine() const { return m_highlightCaretLine; }
    bool GetDisplayLineNumbers() const { return m_displayLineNumbers; }
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

    void SetClearHighlitWordsOnFind(bool b) { m_clearHighlitWordsOnFind = b; }

    void SetHighlightCaretLine(bool b) { m_highlightCaretLine = b; }
    void SetDisplayLineNumbers(bool b) { m_displayLineNumbers = b; }
    void SetShowIndentationGuidelines(bool b) { m_showIndentationGuidelines = b; }
    void SetCaretLineColour(wxColour c) { m_caretLineColour = c; }

    void SetIndentUsesTabs(const bool& indentUsesTabs) { this->m_indentUsesTabs = indentUsesTabs; }
    const bool& GetIndentUsesTabs() const { return m_indentUsesTabs; }
    void SetIndentWidth(const int& indentWidth) { this->m_indentWidth = indentWidth; }
    const int& GetIndentWidth() const { return m_indentWidth; }
    void SetTabWidth(const int& tabWidth) { this->m_tabWidth = tabWidth; }
    const int& GetTabWidth() const { return m_tabWidth; }

    void SetIconsSize(const int& iconsSize) { this->m_iconsSize = iconsSize; }
    const int& GetIconsSize() const { return m_iconsSize; }

    void SetFileFontEncoding(const wxString& strFileFontEncoding);
    wxFontEncoding GetFileFontEncoding() const { return m_fileFontEncoding; }

    void SetShowWhitspaces(const int& showWhitspaces) { this->m_showWhitspaces = showWhitspaces; }
    const int& GetShowWhitspaces() const { return m_showWhitspaces; }

    void SetFoldAtElse(const bool& foldAtElse) { this->m_foldAtElse = foldAtElse; }
    void SetFoldCompact(const bool& foldCompact) { this->m_foldCompact = foldCompact; }
    const bool& GetFoldAtElse() const { return m_foldAtElse; }
    const bool& GetFoldCompact() const { return m_foldCompact; }
    void SetFoldPreprocessor(const bool& foldPreprocessor) { this->m_foldPreprocessor = foldPreprocessor; }
    const bool& GetFoldPreprocessor() const { return m_foldPreprocessor; }

    void SetEdgeColour(const wxColour& edgeColour) { this->m_edgeColour = edgeColour; }
    void SetEdgeColumn(int edgeColumn) { this->m_edgeColumn = edgeColumn; }
    void SetEdgeMode(int edgeMode) { this->m_edgeMode = edgeMode; }

    const wxColour& GetEdgeColour() const { return m_edgeColour; }
    int GetEdgeColumn() const { return m_edgeColumn; }
    int GetEdgeMode() const { return m_edgeMode; }

    void SetHighlightMatchedBraces(const bool& highlightMatchedBraces)
    {
        this->m_highlightMatchedBraces = highlightMatchedBraces;
    }
    const bool& GetHighlightMatchedBraces() const { return m_highlightMatchedBraces; }

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
    void SetAutoAdjustHScrollBarWidth(const bool& autoAdjustHScrollBarWidth)
    {
        this->m_autoAdjustHScrollBarWidth = autoAdjustHScrollBarWidth;
    }
    const bool& GetAutoAdjustHScrollBarWidth() const { return m_autoAdjustHScrollBarWidth; }
    void SetCaretBlinkPeriod(const int& caretBlinkPeriod) { this->m_caretBlinkPeriod = caretBlinkPeriod; }
    void SetCaretWidth(const int& caretWidth) { this->m_caretWidth = caretWidth; }
    const int& GetCaretBlinkPeriod() const { return m_caretBlinkPeriod; }
    const int& GetCaretWidth() const { return m_caretWidth; }

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

    void UpdateFromEditorConfig(const clEditorConfigSection& section);

    /**
     * Return an XML representation of this object
     * \return XML node
     */
    wxXmlNode* ToXml() const;
};

typedef SmartPtr<OptionsConfig> OptionsConfigPtr;

#endif // OPTIONS_CONFIG_H

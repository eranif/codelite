//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : optionsconfig.cpp
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
#include "cl_defs.h"
#include "editor_config.h"
#include "macros.h"
#include "optionsconfig.h"
#include "wx_xml_compatibility.h"
#include "xmlutils.h"
#include <wx/fontmap.h>
#include <wx/intl.h>

#ifdef __WXMSW__
#include <wx/msw/uxtheme.h>
#endif

const wxString defaultBookmarkLabels = wxString(';', CL_N0_OF_BOOKMARK_TYPES - 1) + "Find";

wxString SetDefaultBookmarkColours()
{
    // (Confusingly, the 'foreground' is actually just the rim of the marker; the background is the central bulk)
    // NB: We want the 'find' colour always to be the most significant, so add any future extra items *before* the last
    // one
    const wxString arr[] = { "#FF0080", "#0000FF", "#FF0000", "#00FF00", "#FFFF00" };
    wxString output;
    for(size_t n = 0; n < CL_N0_OF_BOOKMARK_TYPES; ++n) {
        if(n < sizeof(arr) / sizeof(wxString)) {
            output << arr[n] << ';';
        } else {
            output << "#FF0080" << ';';
        }
    }

    return output;
}

OptionsConfig::OptionsConfig(wxXmlNode* node)
    : m_displayFoldMargin(false)
    , m_underlineFoldLine(true)
    , m_scrollBeyondLastLine(true)
    , m_foldStyle(wxT("Arrows with Background Colour"))
    , m_displayBookmarkMargin(true)
    , m_bookmarkShape(wxT("Small Arrow"))
    , m_bookmarkBgColours(SetDefaultBookmarkColours())
    , m_bookmarkFgColours(SetDefaultBookmarkColours())
    , m_bookmarkLabels(defaultBookmarkLabels)
    , m_highlightCaretLine(true)
    , m_clearHighlitWordsOnFind(true)
    , m_displayLineNumbers(true)
    , m_showIndentationGuidelines(false)
    , m_caretLineColour(wxT("LIGHT BLUE"))
    , m_indentUsesTabs(true)
    , m_indentWidth(4)
    , m_tabWidth(4)
    , m_iconsSize(16)
    , m_showWhitspaces(0 /*wxSCI_WS_INVISIBLE*/)
    , m_foldCompact(false)
    , m_foldAtElse(false)
    , m_foldPreprocessor(false)
    , m_edgeMode(0 /*wxSCI_EDGE_NONE*/)
    , m_edgeColumn(80)
    , m_edgeColour(wxColour(wxT("LIGHT GREY")))
    , m_highlightMatchedBraces(true)
    , m_foldBgColour(wxColour(240, 240, 240))
    , m_autoAdjustHScrollBarWidth(true)
    , m_caretWidth(1)
    , m_caretBlinkPeriod(500)
    , m_programConsoleCommand(TERMINAL_CMD)
    , m_eolMode(wxT("Default"))
    , m_hideChangeMarkerMargin(false)
    , m_hideOutpuPaneOnUserClick(false)
    , m_hideOutputPaneNotIfBuild(false)
    , m_hideOutputPaneNotIfSearch(true)
    , m_hideOutputPaneNotIfReplace(false)
    , m_hideOutputPaneNotIfReferences(false)
    , m_hideOutputPaneNotIfOutput(false)
    , m_hideOutputPaneNotIfTrace(false)
    , m_hideOutputPaneNotIfTasks(false)
    , m_hideOutputPaneNotIfBuildQ(true)
    , m_hideOutputPaneNotIfCppCheck(true)
    , m_hideOutputPaneNotIfSvn(true)
    , m_hideOutputPaneNotIfCscope(false)
    , m_hideOutputPaneNotIfGit(true)
    , m_hideOutputPaneNotIfDebug(true)
    , m_hideOutputPaneNotIfMemCheck(true)
    , m_findBarAtBottom(true)
    , m_showReplaceBar(true)
    , m_TrimLine(true)
    , m_AppendLF(true)
    , m_disableSmartIndent(false)
    , m_disableSemicolonShift(false)
    , m_caretLineAlpha(30)
    , m_dontAutoFoldResults(true)
    , m_showDebugOnRun(true)
    , m_caretUseCamelCase(true)
    , m_wordWrap(false)
    , m_dockingStyle(0)
    , m_preferredLocale(wxT("en_US"))
    , m_useLocale(0)
    , m_trimOnlyModifiedLines(true)
    , m_options(Opt_AutoCompleteCurlyBraces | Opt_AutoCompleteNormalBraces | Opt_NavKey_Shift | Opt_WrapBrackets |
                Opt_WrapQuotes | Opt_AutoCompleteDoubleQuotes | Opt_FoldHighlightActiveBlock |
                Opt_WrapCmdWithDoubleQuotes | Opt_TabStyleMinimal)
    , m_options2(0)
    , m_workspaceTabsDirection(wxUP)
    , m_outputTabsDirection(wxUP)
    , m_indentedComments(false)
    , m_nbTabHeight(nbTabHt_Medium)
{
    m_debuggerMarkerLine = DrawingUtils::LightColour("LIME GREEN", 8.0);
    m_mswTheme = false;
#ifdef __WXMSW__
    int major, minor;
    wxGetOsVersion(&major, &minor);

    if(wxUxThemeEngine::GetIfActive() && major >= 6 /* Win 7 and up */) { m_mswTheme = true; }
#endif

    // set the default font name to be wxFONTENCODING_UTF8
    SetFileFontEncoding(wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8));
    if(node) {
        SetFileFontEncoding(
            XmlUtils::ReadString(node, wxT("FileFontEncoding"), wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8)));

        m_displayFoldMargin = XmlUtils::ReadBool(node, wxT("DisplayFoldMargin"), m_displayFoldMargin);
        m_underlineFoldLine = XmlUtils::ReadBool(node, wxT("UnderlineFoldedLine"), m_underlineFoldLine);
        m_foldStyle = XmlUtils::ReadString(node, wxT("FoldStyle"), m_foldStyle);
        m_displayBookmarkMargin = XmlUtils::ReadBool(node, wxT("DisplayBookmarkMargin"), m_displayBookmarkMargin);
        m_bookmarkShape = XmlUtils::ReadString(node, wxT("BookmarkShape"), m_bookmarkShape);
        m_bookmarkBgColours =
            XmlUtils::ReadString(node, wxT("BookmarkBgColours"), ""); // No default; we'll deal with this later
        m_bookmarkFgColours = XmlUtils::ReadString(node, wxT("BookmarkFgColours"), "");
        m_bookmarkLabels = XmlUtils::ReadString(node, wxT("BookmarkLabels"), defaultBookmarkLabels);
        m_clearHighlitWordsOnFind = XmlUtils::ReadBool(node, wxT("ClearHighlitWordsOnFind"), m_clearHighlitWordsOnFind);
        m_highlightCaretLine = XmlUtils::ReadBool(node, wxT("HighlightCaretLine"), m_highlightCaretLine);
        m_displayLineNumbers = XmlUtils::ReadBool(node, wxT("ShowLineNumber"), m_displayLineNumbers);
        m_showIndentationGuidelines = XmlUtils::ReadBool(node, wxT("IndentationGuides"), m_showIndentationGuidelines);
        m_caretLineColour =
            XmlUtils::ReadString(node, wxT("CaretLineColour"), m_caretLineColour.GetAsString(wxC2S_HTML_SYNTAX));
        m_indentUsesTabs = XmlUtils::ReadBool(node, wxT("IndentUsesTabs"), m_indentUsesTabs);
        m_indentWidth = XmlUtils::ReadLong(node, wxT("IndentWidth"), m_indentWidth);
        m_tabWidth = XmlUtils::ReadLong(node, wxT("TabWidth"), m_tabWidth);
        m_iconsSize = XmlUtils::ReadLong(node, wxT("ToolbarIconSize"), m_iconsSize);
        m_showWhitspaces = XmlUtils::ReadLong(node, wxT("ShowWhitespaces"), m_showWhitspaces);
        m_foldCompact = XmlUtils::ReadBool(node, wxT("FoldCompact"), m_foldCompact);
        m_foldAtElse = XmlUtils::ReadBool(node, wxT("FoldAtElse"), m_foldAtElse);
        m_foldPreprocessor = XmlUtils::ReadBool(node, wxT("FoldPreprocessor"), m_foldPreprocessor);
        m_edgeMode = XmlUtils::ReadLong(node, wxT("EdgeMode"), m_edgeMode);
        m_edgeColumn = XmlUtils::ReadLong(node, wxT("EdgeColumn"), m_edgeColumn);
        m_edgeColour = XmlUtils::ReadString(node, wxT("EdgeColour"), m_edgeColour.GetAsString(wxC2S_HTML_SYNTAX));
        m_highlightMatchedBraces = XmlUtils::ReadBool(node, wxT("HighlightMatchedBraces"), m_highlightMatchedBraces);
        m_foldBgColour = XmlUtils::ReadString(node, wxT("FoldBgColour"), m_foldBgColour.GetAsString(wxC2S_HTML_SYNTAX));
        m_autoAdjustHScrollBarWidth =
            XmlUtils::ReadBool(node, wxT("AutoAdjustHScrollBarWidth"), m_autoAdjustHScrollBarWidth);
        m_caretBlinkPeriod = XmlUtils::ReadLong(node, wxT("CaretBlinkPeriod"), m_caretBlinkPeriod);
        m_caretWidth = XmlUtils::ReadLong(node, wxT("CaretWidth"), m_caretWidth);
        m_programConsoleCommand = XmlUtils::ReadString(node, wxT("ConsoleCommand"), m_programConsoleCommand);
        m_eolMode = XmlUtils::ReadString(node, wxT("EOLMode"), m_eolMode);
        m_hideChangeMarkerMargin = XmlUtils::ReadBool(node, wxT("HideChangeMarkerMargin"));
        m_scrollBeyondLastLine = XmlUtils::ReadBool(node, wxT("ScrollBeyondLastLine"), m_scrollBeyondLastLine);
        m_hideOutpuPaneOnUserClick = XmlUtils::ReadBool(node, wxT("HideOutputPaneOnUserClick"));
        m_hideOutputPaneNotIfBuild = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfBuild"));
        m_hideOutputPaneNotIfSearch = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfSearch"));
        m_hideOutputPaneNotIfReplace = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfReplace"));
        m_hideOutputPaneNotIfReferences = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfReferences"));
        m_hideOutputPaneNotIfOutput = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfOutput"));
        m_hideOutputPaneNotIfTrace = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfTrace"));
        m_hideOutputPaneNotIfTasks = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfTasks"));
        m_hideOutputPaneNotIfBuildQ = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfBuildQ"));
        m_hideOutputPaneNotIfCppCheck = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfCppCheck"));
        m_hideOutputPaneNotIfSvn = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfSvn"));
        m_hideOutputPaneNotIfCscope = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfCscope"));
        m_hideOutputPaneNotIfGit = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfGit"));
        m_hideOutputPaneNotIfDebug = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfDebug"));
        m_hideOutputPaneNotIfMemCheck = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfMemCheck"));
        m_findBarAtBottom = XmlUtils::ReadBool(node, wxT("FindBarAtBottom"), m_findBarAtBottom);
        m_showReplaceBar = XmlUtils::ReadBool(node, wxT("ShowReplaceBar"), m_showReplaceBar);
        m_disableSmartIndent = XmlUtils::ReadBool(node, wxT("DisableSmartIndent"), m_disableSmartIndent);
        m_disableSemicolonShift = XmlUtils::ReadBool(node, wxT("DisableSemicolonShift"), m_disableSemicolonShift);
        m_caretLineAlpha = XmlUtils::ReadLong(node, wxT("CaretLineAlpha"), m_caretLineAlpha);
        m_dontAutoFoldResults = XmlUtils::ReadBool(node, wxT("DontAutoFoldResults"), m_dontAutoFoldResults);
        m_showDebugOnRun = XmlUtils::ReadBool(node, wxT("ShowDebugOnRun"), m_showDebugOnRun);
        m_caretUseCamelCase = XmlUtils::ReadBool(node, wxT("m_caretUseCamelCase"), m_caretUseCamelCase);
        m_wordWrap = XmlUtils::ReadBool(node, wxT("m_wordWrap"), m_wordWrap);
        m_dockingStyle = XmlUtils::ReadLong(node, wxT("m_dockingStyle"), m_dockingStyle);
        m_nbTabHeight = XmlUtils::ReadLong(node, wxT("m_nbTabHeight"), m_nbTabHeight);
        m_mswTheme = XmlUtils::ReadBool(node, wxT("m_mswTheme"), m_mswTheme);
        m_preferredLocale = XmlUtils::ReadString(node, wxT("m_preferredLocale"), m_preferredLocale);
        m_useLocale = XmlUtils::ReadBool(node, wxT("m_useLocale"), m_useLocale);
        m_trimOnlyModifiedLines = XmlUtils::ReadBool(node, wxT("m_trimOnlyModifiedLines"), m_trimOnlyModifiedLines);
        m_options = XmlUtils::ReadLong(node, wxT("m_options"), m_options);
        m_options2 = XmlUtils::ReadLong(node, wxT("m_options2"), m_options2);
        m_debuggerMarkerLine = XmlUtils::ReadString(node, wxT("m_debuggerMarkerLine"),
                                                    m_debuggerMarkerLine.GetAsString(wxC2S_HTML_SYNTAX));
        m_indentedComments = XmlUtils::ReadBool(node, wxT("IndentedComments"), m_indentedComments);

        // These hacks will likely be changed in the future. If so, we'll be able to remove the #include
        // "editor_config.h" too
        long trim = EditorConfigST::Get()->GetInteger(wxT("EditorTrimEmptyLines"), 0);
        m_TrimLine = (trim > 0);

        long appendLf = EditorConfigST::Get()->GetInteger(wxT("EditorAppendLf"), 0);
        m_AppendLF = (appendLf > 0);

        long dontTrimCaretLine = EditorConfigST::Get()->GetInteger(wxT("DontTrimCaretLine"), 0);
        m_dontTrimCaretLine = (dontTrimCaretLine > 0);

        m_outputTabsDirection =
            (wxDirection)XmlUtils::ReadLong(node, "OutputTabsDirection", (int)m_outputTabsDirection);
        m_workspaceTabsDirection =
            (wxDirection)XmlUtils::ReadLong(node, "WorkspaceTabsDirection", (int)m_workspaceTabsDirection);
    }
#ifdef __WXMSW__
    if(!(wxUxThemeEngine::GetIfActive() && major >= 6 /* Win 7 and up */)) { m_mswTheme = false; }
#endif

    // Transitional calls. These checks are relevant for 2 years i.e. until the beginning of 2016
    if(m_bookmarkFgColours.empty()) {
        // This must be the first time with multiple BMs, so rescue any old user-set value
        m_bookmarkFgColours = SetDefaultBookmarkColours();
        wxString oldcolour = XmlUtils::ReadString(node, "BookmarkFgColour", "#FF0080");
        SetBookmarkFgColour(oldcolour, 0);
    }
    if(m_bookmarkBgColours.empty()) {
        m_bookmarkBgColours = SetDefaultBookmarkColours();
        wxString oldcolour = XmlUtils::ReadString(node, "BookmarkBgColour", "#FF0080");
        SetBookmarkBgColour(oldcolour, 0);
    }
}

OptionsConfig::~OptionsConfig(void) {}

wxXmlNode* OptionsConfig::ToXml() const
{
    wxXmlNode* n = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Options"));
    n->AddProperty(wxT("DisplayFoldMargin"), BoolToString(m_displayFoldMargin));
    n->AddProperty(wxT("UnderlineFoldedLine"), BoolToString(m_underlineFoldLine));
    n->AddProperty(wxT("FoldStyle"), m_foldStyle);
    n->AddProperty(wxT("DisplayBookmarkMargin"), BoolToString(m_displayBookmarkMargin));
    n->AddProperty(wxT("BookmarkShape"), m_bookmarkShape);
    n->AddProperty(wxT("BookmarkBgColours"), m_bookmarkBgColours);
    n->AddProperty(wxT("BookmarkFgColours"), m_bookmarkFgColours);
    n->AddProperty(wxT("BookmarkLabels"), m_bookmarkLabels);
    n->AddProperty(wxT("ClearHighlitWordsOnFind"), BoolToString(m_clearHighlitWordsOnFind));
    n->AddProperty(wxT("HighlightCaretLine"), BoolToString(m_highlightCaretLine));
    n->AddProperty(wxT("ShowLineNumber"), BoolToString(m_displayLineNumbers));
    n->AddProperty(wxT("IndentationGuides"), BoolToString(m_showIndentationGuidelines));
    n->AddProperty(wxT("CaretLineColour"), m_caretLineColour.GetAsString(wxC2S_HTML_SYNTAX));
    n->AddProperty(wxT("IndentUsesTabs"), BoolToString(m_indentUsesTabs));
    n->AddProperty(wxT("FoldCompact"), BoolToString(m_foldCompact));
    n->AddProperty(wxT("FoldAtElse"), BoolToString(m_foldAtElse));
    n->AddProperty(wxT("FoldPreprocessor"), BoolToString(m_foldPreprocessor));
    n->AddProperty(wxT("HighlightMatchedBraces"), BoolToString(m_highlightMatchedBraces));
    n->AddProperty(wxT("FoldBgColour"), m_foldBgColour.GetAsString(wxC2S_HTML_SYNTAX));
    n->AddProperty(wxT("AutoAdjustHScrollBarWidth"), BoolToString(m_autoAdjustHScrollBarWidth));
    n->AddProperty(wxT("HideChangeMarkerMargin"), BoolToString(m_hideChangeMarkerMargin));
    n->AddProperty(wxT("ScrollBeyondLastLine"), BoolToString(m_scrollBeyondLastLine));
    n->AddProperty(wxT("HideOutputPaneOnUserClick"), BoolToString(m_hideOutpuPaneOnUserClick));
    n->AddProperty(wxT("HideOutputPaneNotIfBuild"), BoolToString(m_hideOutputPaneNotIfBuild));
    n->AddProperty(wxT("HideOutputPaneNotIfSearch"), BoolToString(m_hideOutputPaneNotIfSearch));
    n->AddProperty(wxT("HideOutputPaneNotIfReplace"), BoolToString(m_hideOutputPaneNotIfReplace));
    n->AddProperty(wxT("HideOutputPaneNotIfReferences"), BoolToString(m_hideOutputPaneNotIfReferences));
    n->AddProperty(wxT("HideOutputPaneNotIfOutput"), BoolToString(m_hideOutputPaneNotIfOutput));
    n->AddProperty(wxT("HideOutputPaneNotIfTrace"), BoolToString(m_hideOutputPaneNotIfTrace));
    n->AddProperty(wxT("HideOutputPaneNotIfTasks"), BoolToString(m_hideOutputPaneNotIfTasks));
    n->AddProperty(wxT("HideOutputPaneNotIfBuildQ"), BoolToString(m_hideOutputPaneNotIfBuildQ));
    n->AddProperty(wxT("HideOutputPaneNotIfCppCheck"), BoolToString(m_hideOutputPaneNotIfCppCheck));
    n->AddProperty(wxT("HideOutputPaneNotIfSvn"), BoolToString(m_hideOutputPaneNotIfSvn));
    n->AddProperty(wxT("HideOutputPaneNotIfCscope"), BoolToString(m_hideOutputPaneNotIfCscope));
    n->AddProperty(wxT("HideOutputPaneNotIfGit"), BoolToString(m_hideOutputPaneNotIfGit));
    n->AddProperty(wxT("HideOutputPaneNotIfDebug"), BoolToString(m_hideOutputPaneNotIfDebug));
    n->AddProperty(wxT("HideOutputPaneNotIfMemCheck"), BoolToString(m_hideOutputPaneNotIfMemCheck));
    n->AddProperty(wxT("FindBarAtBottom"), BoolToString(m_findBarAtBottom));
    n->AddProperty(wxT("ShowReplaceBar"), BoolToString(m_showReplaceBar));
    n->AddProperty(wxT("DisableSmartIndent"), BoolToString(m_disableSmartIndent));
    n->AddProperty(wxT("DisableSemicolonShift"), BoolToString(m_disableSemicolonShift));
    n->AddProperty(wxT("DontAutoFoldResults"), BoolToString(m_dontAutoFoldResults));
    n->AddProperty(wxT("ShowDebugOnRun"), BoolToString(m_showDebugOnRun));
    n->AddProperty(wxT("ConsoleCommand"), m_programConsoleCommand);
    n->AddProperty(wxT("EOLMode"), m_eolMode);
    n->AddProperty(wxT("m_caretUseCamelCase"), BoolToString(m_caretUseCamelCase));
    n->AddProperty(wxT("m_wordWrap"), BoolToString(m_wordWrap));
    n->AddProperty(wxT("m_dockingStyle"), wxString::Format(wxT("%d"), m_dockingStyle));
    n->AddProperty(wxT("m_nbTabHeight"), wxString::Format(wxT("%d"), m_nbTabHeight));
    n->AddProperty(wxT("m_mswTheme"), BoolToString(m_mswTheme));
    n->AddProperty(wxT("m_preferredLocale"), m_preferredLocale);
    n->AddProperty(wxT("m_useLocale"), BoolToString(m_useLocale));
    n->AddProperty(wxT("m_trimOnlyModifiedLines"), BoolToString(m_trimOnlyModifiedLines));
    n->AddProperty(wxT("m_debuggerMarkerLine"), m_debuggerMarkerLine.GetAsString(wxC2S_HTML_SYNTAX));
    n->AddProperty(wxT("OutputTabsDirection"), wxString() << (int)m_outputTabsDirection);
    n->AddProperty(wxT("WorkspaceTabsDirection"), wxString() << (int)m_workspaceTabsDirection);
    n->AddProperty(wxT("IndentedComments"), BoolToString(m_indentedComments));

    wxString tmp;
    tmp << m_indentWidth;
    n->AddProperty(wxT("IndentWidth"), tmp);

    tmp.clear();
    tmp << m_tabWidth;
    n->AddProperty(wxT("TabWidth"), tmp);

    tmp.clear();
    tmp << m_iconsSize;
    n->AddProperty(wxT("ToolbarIconSize"), tmp);

    tmp.clear();
    tmp << m_showWhitspaces;
    n->AddProperty(wxT("ShowWhitespaces"), tmp);

    tmp.clear();
    tmp << m_edgeMode;
    n->AddProperty(wxT("EdgeMode"), tmp);

    tmp.clear();
    tmp << m_edgeColumn;
    n->AddProperty(wxT("EdgeColumn"), tmp);

    n->AddProperty(wxT("EdgeColour"), m_edgeColour.GetAsString(wxC2S_HTML_SYNTAX));

    tmp.clear();
    tmp << m_caretWidth;
    n->AddProperty(wxT("CaretWidth"), tmp);

    tmp.clear();
    tmp << m_caretBlinkPeriod;
    n->AddProperty(wxT("CaretBlinkPeriod"), tmp);

    tmp.clear();
    tmp << m_caretLineAlpha;
    n->AddProperty(wxT("CaretLineAlpha"), tmp);

    tmp.clear();
    tmp = wxFontMapper::GetEncodingName(m_fileFontEncoding);
    n->AddProperty(wxT("FileFontEncoding"), tmp);

    tmp.Clear();
    tmp << m_options;
    n->AddProperty(wxT("m_options"), tmp);

    tmp.Clear();
    tmp << m_options2;
    n->AddProperty(wxT("m_options2"), tmp);
    return n;
}

void OptionsConfig::SetFileFontEncoding(const wxString& strFileFontEncoding)
{
    this->m_fileFontEncoding = wxFontMapper::Get()->CharsetToEncoding(strFileFontEncoding, false);

    if(wxFONTENCODING_SYSTEM == this->m_fileFontEncoding) { this->m_fileFontEncoding = wxFONTENCODING_UTF8; }
}

wxString OptionsConfig::GetEOLAsString() const
{
    if(GetEolMode() == wxT("Unix (LF)")) {
        return "\n";

    } else if(GetEolMode() == wxT("Mac (CR)")) {
        return "\r";

    } else if(GetEolMode() == wxT("Windows (CRLF)")) {
        return "\r\n";

    } else {
        return "\n";
    }
}

wxColour OptionsConfig::GetBookmarkFgColour(size_t index) const
{
    wxColour col;
    wxArrayString arr = wxSplit(m_bookmarkFgColours, ';');
    if(index < arr.GetCount()) { return wxColour(arr.Item(index)); }

    return col;
}

void OptionsConfig::SetBookmarkFgColour(wxColour c, size_t index)
{
    wxArrayString arr = wxSplit(m_bookmarkFgColours, ';');
    if(index < arr.GetCount()) {
        arr.Item(index) = c.GetAsString(wxC2S_HTML_SYNTAX);
        m_bookmarkFgColours = wxJoin(arr, ';');
    }
}

wxColour OptionsConfig::GetBookmarkBgColour(size_t index) const
{
    wxColour col;
    wxArrayString arr = wxSplit(m_bookmarkBgColours, ';');
    if(index < arr.GetCount()) { return wxColour(arr.Item(index)); }

    return col;
}

void OptionsConfig::SetBookmarkBgColour(wxColour c, size_t index)
{
    wxArrayString arr = wxSplit(m_bookmarkBgColours, ';');
    if(index < arr.GetCount()) {
        arr.Item(index) = c.GetAsString(wxC2S_HTML_SYNTAX);
        m_bookmarkBgColours = wxJoin(arr, ';');
    }
}

wxString OptionsConfig::GetBookmarkLabel(size_t index) const
{
    wxArrayString arr = wxSplit(m_bookmarkLabels, ';');
    if(index < arr.GetCount()) { return arr.Item(index); }

    return "";
}

void OptionsConfig::SetBookmarkLabel(const wxString& label, size_t index)
{
    wxArrayString arr = wxSplit(m_bookmarkLabels, ';');
    if(index < arr.GetCount()) {
        arr.Item(index) = label;
        m_bookmarkLabels = wxJoin(arr, ';');
    }
}

void OptionsConfig::UpdateFromEditorConfig(const clEditorConfigSection& section)
{
    if(section.IsInsertFinalNewlineSet()) { this->SetAppendLF(section.IsInsertFinalNewline()); }
    if(section.IsSetEndOfLineSet()) {
        // Convert .editorconfig to CodeLite strings
        wxString eolMode = "Unix (LF)"; // default
        if(section.GetEndOfLine() == "crlf") {
            eolMode = "Windows (CRLF)";
        } else if(section.GetEndOfLine() == "cr") {
            eolMode = "Mac (CR)";
        }
        this->SetEolMode(eolMode);
    }
    if(section.IsTabWidthSet()) { this->SetTabWidth(section.GetTabWidth()); }
    if(section.IsIndentStyleSet()) { this->SetIndentUsesTabs(section.GetIndentStyle() == "tab"); }
    if(section.IsTabWidthSet()) { this->SetTabWidth(section.GetTabWidth()); }
    if(section.IsIndentSizeSet()) { this->SetIndentWidth(section.GetIndentSize()); }
    if(section.IsCharsetSet()) {
        // TODO: fix the locale here
    }
}

bool OptionsConfig::IsTabColourDark() const
{
#if USE_AUI_NOTEBOOK
    return false;
#else
    return HasOption(Opt_TabColourDark);
#endif
}

bool OptionsConfig::IsTabColourMatchesTheme() const
{
#if USE_AUI_NOTEBOOK
    return true;
#else
    return !HasOption(Opt_TabColourPersistent);
#endif
}

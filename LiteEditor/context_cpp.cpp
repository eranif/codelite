//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_cpp.cpp
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

#include "AddFunctionsImpDlg.h"
#include "CxxVariableScanner.h"
#include "SelectProjectsDlg.h"
#include "addincludefiledlg.h"
#include "algorithm"
#include "browse_record.h"
#include "buildtabsettingsdata.h"
#include "clEditorStateLocker.h"
#include "clSelectSymbolDialog.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "cl_editor_tip_window.h"
#include "code_completion_api.h"
#include "code_completion_manager.h"
#include "codelite_events.h"
#include "commentconfigdata.h"
#include "context_cpp.h"
#include "cpptoken.h"
#include "ctags_manager.h"
#include "debuggerasciiviewer.h"
#include "debuggerconfigtool.h"
#include "debuggermanager.h"
#include "debuggersettings.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileview.h"
#include "findusagetab.h"
#include "frame.h"
#include "globals.h"
#include "implement_parent_virtual_functions.h"
#include "language.h"
#include "manager.h"
#include "movefuncimpldlg.h"
#include "navigationmanager.h"
#include "new_quick_watch_dlg.h"
#include "parse_thread.h"
#include "pluginmanager.h"
#include "precompiled_header.h"
#include "refactorengine.h"
#include "refactorindexbuildjob.h"
#include "renamesymboldlg.h"
#include "setters_getters_dlg.h"
#include "symbols_dialog.h"
#include "workspacetab.h"
#include "wx/regex.h"
#include "wx/tokenzr.h"
#include "wx/xrc/xmlres.h"
#include "wxCodeCompletionBoxManager.h"
#include <parse_thread.h>
#include <wx/choicdlg.h>
#include <wx/file.h>
#include <wx/progdlg.h>
#include <wx/regex.h>
#include "clFileSystemWorkspace.hpp"

//#define __PERFORMANCE
#include "performance.h"

// Set of macros to allow use to disable any context code when we are using
// the C++ lexer for Java Script
#define CHECK_JS_RETURN_TRUE() \
    if(IsJavaScript()) return true
#define CHECK_JS_RETURN_FALSE() \
    if(IsJavaScript()) return false
#define CHECK_JS_RETURN_VOID() \
    if(IsJavaScript()) return
#define CHECK_JS_RETURN_NULL() \
    if(IsJavaScript()) return NULL

static bool IsSource(const wxString& ext)
{
    wxString e(ext);
    e = e.MakeLower();
    return e == "cpp" || e == "cxx" || e == "c" || e == "c++" || e == "cc" || e == "ipp";
}

static bool IsHeader(const wxString& ext)
{
    wxString e(ext);
    e = e.MakeLower();
    return e == wxT("hpp") || e == wxT("h") || e == wxT("hxx");
}

#define VALIDATE_PROJECT(ctrl) \
    if(ctrl.GetProject().IsEmpty()) { return; }

#define VALIDATE_PROJECT_FALSE(ctrl) \
    if(ctrl.GetProject().IsEmpty()) { return false; }

#define IS_CXX_WORKSPACE_OPENED() (::clIsCxxWorkspaceOpened())

#define VALIDATE_WORKSPACE() \
    if(!IS_CXX_WORKSPACE_OPENED()) { return; }

#define VALIDATE_WORKSPACE_FALSE() \
    if(!IS_CXX_WORKSPACE_OPENED()) { return false; }

#define VALIDATE_WORKSPACE_NULL() \
    if(!IS_CXX_WORKSPACE_OPENED()) { return NULL; }

struct SFileSort {
    bool operator()(const wxFileName& one, const wxFileName& two)
    {
        return two.GetFullName().Cmp(one.GetFullName()) > 0;
    }
};

//----------------------------------------------------------------------------------

wxBitmap ContextCpp::m_cppFileBmp = wxNullBitmap;
wxBitmap ContextCpp::m_hFileBmp = wxNullBitmap;
wxBitmap ContextCpp::m_otherFileBmp = wxNullBitmap;

BEGIN_EVENT_TABLE(ContextCpp, wxEvtHandler)
EVT_UPDATE_UI(XRCID("find_decl"), ContextCpp::OnUpdateUI)
EVT_UPDATE_UI(XRCID("find_impl"), ContextCpp::OnUpdateUI)
EVT_UPDATE_UI(XRCID("go_to_function_start"), ContextCpp::OnUpdateUI)
EVT_UPDATE_UI(XRCID("go_to_next_function"), ContextCpp::OnUpdateUI)
EVT_UPDATE_UI(XRCID("insert_doxy_comment"), ContextCpp::OnUpdateUI)
EVT_UPDATE_UI(XRCID("setters_getters"), ContextCpp::OnUpdateUI)
EVT_UPDATE_UI(XRCID("move_impl"), ContextCpp::OnUpdateUI)

EVT_MENU(XRCID("swap_files"), ContextCpp::OnSwapFiles)
EVT_MENU(XRCID("comment_selection"), ContextCpp::OnCommentSelection)
EVT_MENU(XRCID("comment_line"), ContextCpp::OnCommentLine)
EVT_MENU(XRCID("find_decl"), ContextCpp::OnFindDecl)
EVT_MENU(XRCID("find_impl"), ContextCpp::OnFindImpl)
EVT_MENU(XRCID("go_to_function_start"), ContextCpp::OnGotoFunctionStart)
EVT_MENU(XRCID("go_to_next_function"), ContextCpp::OnGotoNextFunction)
EVT_MENU(XRCID("insert_doxy_comment"), ContextCpp::OnInsertDoxyComment)
EVT_MENU(XRCID("move_impl"), ContextCpp::OnMoveImpl)
EVT_MENU(XRCID("add_impl"), ContextCpp::OnAddImpl)
EVT_MENU(XRCID("add_multi_impl"), ContextCpp::OnAddMultiImpl)
EVT_MENU(XRCID("add_virtual_impl"), ContextCpp::OnOverrideParentVritualFunctions)
EVT_MENU(XRCID("add_pure_virtual_impl"), ContextCpp::OnOverrideParentVritualFunctions)
EVT_MENU(XRCID("setters_getters"), ContextCpp::OnGenerateSettersGetters)
EVT_MENU(XRCID("add_include_file"), ContextCpp::OnAddIncludeFile)
EVT_MENU(XRCID("add_forward_decl"), ContextCpp::OnAddForwardDecl)
EVT_MENU(XRCID("rename_symbol"), ContextCpp::OnRenameGlobalSymbol)
EVT_MENU(XRCID("find_references"), ContextCpp::OnFindReferences)
EVT_MENU(XRCID("sync_signatures"), ContextCpp::OnSyncSignatures)
EVT_MENU(XRCID("retag_file"), ContextCpp::OnRetagFile)
EVT_MENU(XRCID("open_include_file"), ContextCpp::OnContextOpenDocument)
END_EVENT_TABLE()

ContextCpp::ContextCpp(clEditor* container)
    : ContextBase(container)
    , m_rclickMenu(NULL)
{
    Initialize();
    SetName("c++");
    EventNotifier::Get()->Connect(wxEVT_CC_SHOW_QUICK_NAV_MENU,
                                  clCodeCompletionEventHandler(ContextCpp::OnShowCodeNavMenu), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SELECTION_MADE, &ContextCpp::OnCodeCompleteFiles, this);
}

ContextCpp::ContextCpp()
    : ContextBase(wxT("c++"))
    , m_rclickMenu(NULL)
{
    EventNotifier::Get()->Connect(wxEVT_CC_SHOW_QUICK_NAV_MENU,
                                  clCodeCompletionEventHandler(ContextCpp::OnShowCodeNavMenu), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SELECTION_MADE, &ContextCpp::OnCodeCompleteFiles, this);
}

ContextCpp::~ContextCpp()
{
    EventNotifier::Get()->Disconnect(wxEVT_CC_SHOW_QUICK_NAV_MENU,
                                     clCodeCompletionEventHandler(ContextCpp::OnShowCodeNavMenu), NULL, this);
    wxDELETE(m_rclickMenu);
}

ContextBase* ContextCpp::NewInstance(clEditor* container) { return new ContextCpp(container); }

void ContextCpp::OnDwellEnd(wxStyledTextEvent& event)
{
    clEditor& rCtrl = GetCtrl();
    rCtrl.DoCancelCalltip();
    event.Skip();
}

bool ContextCpp::GetHoverTip(int pos)
{
    CHECK_JS_RETURN_FALSE();

    clEditor& rCtrl = GetCtrl();

    VALIDATE_PROJECT_FALSE(rCtrl);

    // before we start, make sure we are the visible window
    if(clMainFrame::Get()->GetMainBook()->GetActiveEditor(true) != &rCtrl) { return false; }

    int end = rCtrl.WordEndPosition(pos, true);
    int word_start = rCtrl.WordStartPosition(pos, true);

    // get the expression we are standing on it
    if(IsCommentOrString(pos)) return false;

    // get the token
    wxString word = rCtrl.GetTextRange(word_start, end);
    if(word.IsEmpty()) { return false; }

    int foundPos(wxNOT_FOUND);
    if(rCtrl.PreviousChar(word_start, foundPos) == wxT('~')) word.Prepend(wxT("~"));

    // get the expression we are hovering over
    wxString expr = GetExpression(end, false);

    // get the full text of the current page
    wxString text = rCtrl.GetTextRange(0, pos);

    // now we are ready to process the scope and build our tips
    std::vector<wxString> tips;
    int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition()) + 1;
    TagsManagerST::Get()->GetHoverTip(rCtrl.GetFileName(), line, expr, word, text, tips);

    // display a tooltip
    wxString tooltip;
    if(tips.size() > 0) {

        tooltip << tips[0];
        for(size_t i = 1; i < tips.size(); i++)
            tooltip << wxT("\n") << tips[i];

        // cancel any old calltip and display the new one
        rCtrl.DoCancelCalltip();

        tooltip.Trim().Trim(false);
        if(tooltip.IsEmpty()) { return false; }
        rCtrl.DoShowCalltip(-1, "", tooltip, true);
        return true;
    } else {
        return false;
    }
}

wxString ContextCpp::GetFileImageString(const wxString& ext)
{
    if(IsSource(ext)) { return wxT("?15"); }
    if(IsHeader(ext)) { return wxT("?16"); }
    return wxT("?17");
}

wxString ContextCpp::GetImageString(const TagEntry& entry)
{
    if(entry.GetKind() == wxT("class")) return wxT("?1");

    if(entry.GetKind() == wxT("struct")) return wxT("?2");

    if(entry.GetKind() == wxT("namespace")) return wxT("?3");

    if(entry.GetKind() == wxT("variable")) return wxT("?4");

    if(entry.GetKind() == wxT("typedef")) return wxT("?5");

    if(entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("private"))) return wxT("?6");

    if(entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("public"))) return wxT("?7");

    if(entry.GetKind() == wxT("member") && entry.GetAccess().Contains(wxT("protected"))) return wxT("?8");

    // member with no access? (maybe part of namespace??)
    if(entry.GetKind() == wxT("member")) return wxT("?7");

    if((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) &&
       entry.GetAccess().Contains(wxT("private")))
        return wxT("?9");

    if((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) &&
       (entry.GetAccess().Contains(wxT("public")) || entry.GetAccess().IsEmpty()))
        return wxT("?10");

    if((entry.GetKind() == wxT("function") || entry.GetKind() == wxT("prototype")) &&
       entry.GetAccess().Contains(wxT("protected")))
        return wxT("?11");

    if(entry.GetKind() == wxT("macro")) return wxT("?12");

    if(entry.GetKind() == wxT("enum")) return wxT("?13");

    if(entry.GetKind() == wxT("enumerator")) return wxT("?14");

    return wxEmptyString;
}

void ContextCpp::AutoIndent(const wxChar& nChar)
{
    clEditor& rCtrl = GetCtrl();

    if(rCtrl.GetDisableSmartIndent()) { return; }

    if(rCtrl.GetLineIndentation(rCtrl.GetCurrentLine()) && nChar == wxT('\n')) { return; }

    int curpos = rCtrl.GetCurrentPos();
    if(IsComment(curpos) && nChar == wxT('\n')) {
        AutoAddComment();
        return;
    }

    if(IsCommentOrString(curpos)) {
        ContextBase::AutoIndent(nChar);
        return;
    }

    int line = rCtrl.LineFromPosition(curpos);
    if(nChar == wxT('\n')) {

        int prevpos(wxNOT_FOUND);
        int foundPos(wxNOT_FOUND);

        wxString word;
        wxChar ch = rCtrl.PreviousChar(curpos, prevpos);
        word = rCtrl.PreviousWord(curpos, foundPos);

        bool isPreLinePreProcessLine(false);
        if(line) {
            wxString lineStr = rCtrl.GetLine(line - 1);
            lineStr.Trim().Trim(false);
            isPreLinePreProcessLine = lineStr.StartsWith(wxT("#"));
        }

        // user hit ENTER after 'else'
        if(word == wxT("else") && !isPreLinePreProcessLine) {
            int prevLine = rCtrl.LineFromPosition(prevpos);
            rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
            rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
            rCtrl.ChooseCaretX(); // set new column as "current" column
            return;
        }

        // User typed 'ENTER' immediatly after closing brace ')'
        if(prevpos != wxNOT_FOUND && ch == wxT(')')) {

            long openBracePos(wxNOT_FOUND);
            int posWordBeforeOpenBrace(wxNOT_FOUND);

            if(rCtrl.MatchBraceBack(wxT(')'), prevpos, openBracePos)) {
                rCtrl.PreviousChar(openBracePos, posWordBeforeOpenBrace);
                if(posWordBeforeOpenBrace != wxNOT_FOUND) {
                    word = rCtrl.PreviousWord(posWordBeforeOpenBrace, foundPos);

                    // c++ expression with single line and should be treated separatly
                    if(word == wxT("if") || word == wxT("while") || word == wxT("for")) {
                        int prevLine = rCtrl.LineFromPosition(prevpos);
                        rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
                        rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
                        rCtrl.ChooseCaretX(); // set new column as "current" column
                        return;
                    }
                }
            }
        }

        // User typed 'ENTER' immediatly after colons ':'
        if(prevpos != wxNOT_FOUND && ch == wxT(':')) {
            int posWordBeforeColons(wxNOT_FOUND);

            rCtrl.PreviousChar(prevpos, posWordBeforeColons);
            if(posWordBeforeColons != wxNOT_FOUND) {
                word = rCtrl.PreviousWord(posWordBeforeColons, foundPos);
                int prevLine = rCtrl.LineFromPosition(posWordBeforeColons);
                wxUnusedVar(prevLine);
                // If we found one of the following keywords, un-indent their line by (foldLevel - 1)*indentSize
                if(word == wxT("public") || word == wxT("private") || word == wxT("protected")) {

                    ContextBase::AutoIndent(nChar);

                    // Indent this line according to the block indentation level
                    // But do this only if "Fold PreProcessors" switch is OFF
                    // Otherwise, these keywords will be somewhat miss-aligned
                    if(!GetCtrl().GetOptions()->GetFoldPreprocessor()) {
                        int foldLevel =
                            (rCtrl.GetFoldLevel(prevLine) & wxSTC_FOLDLEVELNUMBERMASK) - wxSTC_FOLDLEVELBASE;
                        if(foldLevel) {
                            rCtrl.SetLineIndentation(prevLine, ((foldLevel - 1) * rCtrl.GetIndent()));
                            rCtrl.ChooseCaretX();
                        }
                    }
                    return;
                }
            }
        }

        // use the previous line indentation level
        if(prevpos == wxNOT_FOUND || ch != wxT('{') || IsCommentOrString(prevpos)) {
            ContextBase::AutoIndent(nChar);
            return;
        }

        // Open brace? increase indent size
        int prevLine = rCtrl.LineFromPosition(prevpos);
        rCtrl.SetLineIndentation(line, rCtrl.GetIndent() + rCtrl.GetLineIndentation(prevLine));
        rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));

    } else if(nChar == wxT('}')) {

        long matchPos = wxNOT_FOUND;
        if(!rCtrl.MatchBraceBack(wxT('}'), rCtrl.PositionBefore(curpos), matchPos)) return;
        int secondLine = rCtrl.LineFromPosition(matchPos);
        if(secondLine == line) return;
        rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(secondLine));

    } else if(nChar == wxT('{')) {
        wxString lineString = rCtrl.GetLine(line);
        lineString.Trim().Trim(false);

        int matchPos = wxNOT_FOUND;
        wxChar previousChar = rCtrl.PreviousChar(rCtrl.PositionBefore(curpos), matchPos);
        if(previousChar != wxT('{') && lineString == wxT("{")) {
            // indent this line accroding to the previous line
            int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
            rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line - 1));
            rCtrl.ChooseCaretX();
        }
    }

    // set new column as "current" column
    rCtrl.ChooseCaretX();
}

bool ContextCpp::IsCommentOrString(long pos)
{
    int style;
    style = GetCtrl().GetStyleAt(pos);
    return (style == wxSTC_C_COMMENT || style == wxSTC_C_COMMENTLINE || style == wxSTC_C_COMMENTDOC ||
            style == wxSTC_C_COMMENTLINEDOC || style == wxSTC_C_COMMENTDOCKEYWORD ||
            style == wxSTC_C_COMMENTDOCKEYWORDERROR || style == wxSTC_C_STRING || style == wxSTC_C_STRINGEOL ||
            style == wxSTC_C_CHARACTER || style == wxSTC_C_STRINGRAW);
}

//=============================================================================
// >>>>>>>>>>>>>>>>>>>>>>>> CodeCompletion API - START
//=============================================================================

// user pressed ., -> or ::
bool ContextCpp::CodeComplete(long pos)
{
    CHECK_JS_RETURN_FALSE();
    VALIDATE_WORKSPACE_FALSE();
    long from = pos;
    if(from == wxNOT_FOUND) { from = GetCtrl().GetCurrentPos(); }
    return DoCodeComplete(from);
}

void ContextCpp::RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
    CHECK_JS_RETURN_VOID();
    for(size_t i = 0; i < src.size(); i++) {
        if(i == 0) {
            target.push_back(src.at(0));
        } else {
            if(src.at(i)->GetName() != target.at(target.size() - 1)->GetName()) { target.push_back(src.at(i)); }
        }
    }
}

wxString ContextCpp::GetWordUnderCaret()
{
    clEditor& rCtrl = GetCtrl();
    // Get the partial word that we have
    long pos = rCtrl.GetCurrentPos();
    long start = rCtrl.WordStartPosition(pos, true);
    long end = rCtrl.WordEndPosition(pos, true);
    return rCtrl.GetTextRange(start, end);
}

void ContextCpp::OnContextOpenDocument(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    wxUnusedVar(event);

    wxString fileName;
    clEditor& rCtrl = GetCtrl();
    wxString line = rCtrl.GetCurLine();
    if(IsIncludeStatement(line, &fileName)) {
        m_selectedWord = fileName;
    } else {
        m_selectedWord.clear();
    }

    // If the event contains a new selection, use it instead of the m_selectedWord
    if(event.GetString().IsEmpty() == false) m_selectedWord = event.GetString();

    DoOpenWorkspaceFile();
}

void ContextCpp::RemoveMenuDynamicContent(wxMenu* menu)
{
    std::vector<wxMenuItem*>::iterator iter = m_dynItems.begin();
    for(; iter != m_dynItems.end(); iter++) {
        menu->Destroy((*iter));
    }
    m_dynItems.clear();
    m_selectedWord.Empty();
}

void ContextCpp::AddMenuDynamicContent(wxMenu* menu)
{
    // if we are placed over an include statement,
    // add an option in the menu to open it
    wxString fileName;

    clEditor& rCtrl = GetCtrl();

    wxString menuItemText;
    wxString line = rCtrl.GetCurLine();
    menuItemText.Clear();

    if(IsIncludeStatement(line, &fileName)) {

        PrependMenuItemSeparator(menu);
        menuItemText << _("Open Include File \"") << fileName << wxT("\"");

        PrependMenuItem(menu, menuItemText, wxCommandEventHandler(ContextCpp::OnContextOpenDocument),
                        XRCID("open_include_file"));
        m_selectedWord = fileName;

    } else {
        int pos = rCtrl.GetCurrentPos();
        if(IsCommentOrString(pos)) { return; }

        wxString word = rCtrl.GetWordAtCaret();
        if(word.IsEmpty() == false) {
            PrependMenuItemSeparator(menu);

            menuItemText << _("Add Forward Declaration for \"") << word << "\"";
            PrependMenuItem(menu, menuItemText, XRCID("add_forward_decl"));

            menuItemText.Clear();
            menuItemText << _("Add Include File for \"") << word << wxT("\"");
            PrependMenuItem(menu, menuItemText, XRCID("add_include_file"));

            m_selectedWord = word;
        }
    }
}

void ContextCpp::OnAddForwardDecl(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();
    wxUnusedVar(e);
    clEditor& rCtrl = GetCtrl();

    // get expression
    int pos = rCtrl.GetCurrentPos();

    if(IsCommentOrString(pos)) return;

    // get the scope
    wxString text = rCtrl.GetTextRange(0, rCtrl.GetCurrentPos());

    wxString word = m_selectedWord;
    if(word.IsEmpty()) {
        // try the word under the caret
        word = rCtrl.GetWordAtCaret();
        if(word.IsEmpty()) { return; }
    }

    int lineNumber = wxNOT_FOUND;
    wxString lineToAdd;
    TagsManagerST::Get()->InsertForwardDeclaration(word, text, lineToAdd, lineNumber);
    if(lineNumber == wxNOT_FOUND) {
        // Append it to the end of the file
        rCtrl.AppendText(rCtrl.GetEolString() + lineToAdd);

    } else {
        int pos = rCtrl.PositionFromLine(lineNumber);
        rCtrl.InsertText(pos, lineToAdd + rCtrl.GetEolString());
    }
}

void ContextCpp::OnAddIncludeFile(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();
    wxUnusedVar(e);
    clEditor& rCtrl = GetCtrl();

    // get expression
    int pos = rCtrl.GetCurrentPos();

    if(IsCommentOrString(pos)) return;

    int word_end = rCtrl.WordEndPosition(pos, true);
    wxString expr = GetExpression(word_end, false);

    // get the scope
    wxString text = rCtrl.GetTextRange(0, word_end);

    wxString word = m_selectedWord;
    if(word.IsEmpty()) {
        // try the word under the caret
        word = rCtrl.GetWordAtCaret();
        if(word.IsEmpty()) { return; }
    }

    std::vector<TagEntryPtr> tags;
    int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition()) + 1;
    TagsManagerST::Get()->FindImplDecl(rCtrl.GetFileName(), line, expr, word, text, tags, false);
    if(tags.empty()) return;

    std::map<wxString, bool> tmpmap;

    wxArrayString options;

    // remove duplicate file entries
    for(std::vector<TagEntryPtr>::size_type i = 0; i < tags.size(); i++) {
        tmpmap[tags.at(i)->GetFile()] = true;
    }

    // convert the map to wxArrayString
    std::map<wxString, bool>::iterator iter = tmpmap.begin();
    for(; iter != tmpmap.end(); iter++) {
        options.Add(iter->first);
    }

    // we now got list of tags that matches 'word'
    wxString choice;
    if(options.GetCount() > 1) {
        // multiple matches
        choice = wxGetSingleChoice(_("Select File to Include:"), _("Add Include File"), options, &GetCtrl());
    } else {
        choice = options.Item(0);
    }

    if(choice.IsEmpty()) { return; }

    // check to see if this file is a workspace file
    AddIncludeFileDlg dlg(clMainFrame::Get(), choice, rCtrl.GetText(), FindLineToAddInclude());
    if(dlg.ShowModal() == wxID_OK) {
        // add the line to the current document
        wxString lineToAdd = dlg.GetLineToAdd();
        int line = dlg.GetLine();

        long pos = rCtrl.PositionFromLine(line);
        rCtrl.InsertText(pos, lineToAdd + rCtrl.GetEolString());
    }
}

bool ContextCpp::IsIncludeStatement(const wxString& line, wxString* fileName, wxString* fileNameUpToCaret)
{
    CHECK_JS_RETURN_FALSE();
    wxString tmpLine(line);
    wxString tmpLine1(line);

    // If we are on an include statement, popup a file list
    // completion list
    tmpLine = tmpLine.Trim();
    tmpLine = tmpLine.Trim(false);
    tmpLine.Replace(wxT("\t"), wxT(" "));

    static wxRegEx reIncludeFile(wxT("include *[\\\"\\<]{1}([a-zA-Z0-9_/\\.\\+\\-]*)"));
    if(tmpLine.StartsWith(wxT("#"), &tmpLine1)) {
        if(reIncludeFile.Matches(tmpLine1)) {
            if(fileNameUpToCaret) {
                // 'line' contains the entire current line
                // we want the part up until the caret
                int caretpos = GetCtrl().GetCurrentPos();
                int lineStartPos = GetCtrl().PositionFromLine(GetCtrl().GetCurrentLine());
                if(lineStartPos > caretpos) return false;

                wxString partialLine = GetCtrl().GetTextRange(lineStartPos, caretpos);

                // Get the partial file name (up to the caret)
                size_t where = partialLine.find_first_of("<\"");
                if(where == wxString::npos) return false;
                ++where; // Skip the < or " character found

                partialLine = partialLine.Mid(where);
                // partialLine = partialLine.AfterLast('/');
                *fileNameUpToCaret = partialLine;
            }

            if(fileName) { *fileName = reIncludeFile.GetMatch(tmpLine1, 1); }
            return true;
        }
    }
    return false;
}

bool ContextCpp::CompleteWord()
{
    CHECK_JS_RETURN_FALSE();
    clEditor& rCtrl = GetCtrl();

    VALIDATE_WORKSPACE_FALSE();

    wxString word;
    wxString fileName;
    wxString line = rCtrl.GetCurLine();
    if(IsIncludeStatement(line, NULL, &fileName)) {
        DisplayFilesCompletionBox(fileName);
        return true;
    }

    // Make sure we are not on a comment section
    if(IsCommentOrString(rCtrl.GetCurrentPos())) return false;

    // Get the partial word that we have
    long pos = rCtrl.GetCurrentPos();
    long start = rCtrl.WordStartPosition(pos, true);
    word = rCtrl.GetTextRange(start, pos);

    if(word.IsEmpty()) {
        // incase the 'word' is empty, test the word to the left of the current pos
        wxChar ch1 = rCtrl.SafeGetChar(pos - 1);
        wxChar ch2 = rCtrl.SafeGetChar(pos - 2);

        if(ch1 == wxT('.') || (ch2 == wxT('-') && ch1 == wxT('>')) || (ch2 == wxT(':') && ch1 == wxT(':'))) {
            return CodeComplete();
        }
        return false;
    }

    // get the current expression
    wxString expr = GetExpression(rCtrl.GetCurrentPos(), true);

    DoSetProjectPaths();
    return CodeCompletionManager::Get().WordCompletion(&GetCtrl(), expr, word);
}

void ContextCpp::DisplayFilesCompletionBox(const wxString& word)
{
    CHECK_JS_RETURN_VOID();
    wxString list;

    wxString fileName(word);
    wxArrayString files;
    TagsManagerST::Get()->GetFilesForCC(fileName, files);
    files.Sort();

    if(!files.IsEmpty()) {
        // Show completion box for files
        wxCodeCompletionBoxEntry::Vec_t entries;
        wxCodeCompletionBox::BmpVec_t bitmaps;
        bitmaps.push_back(m_cppFileBmp);
        bitmaps.push_back(m_hFileBmp);
        bitmaps.push_back(m_otherFileBmp);
        // Make sure that the file list is unique
        wxStringSet_t matches;
        for(size_t i = 0; i < files.GetCount(); ++i) {
            wxFileName fn(files.Item(i));
            if(matches.count(files.Item(i))) continue; // we already have this file in the list, don't add another one
            matches.insert(files.Item(i));

            int imgID = 0;
            switch(FileExtManager::GetType(fn.GetFullName())) {
            case FileExtManager::TypeSourceC:
            case FileExtManager::TypeSourceCpp:
                imgID = 0;
                break;
            case FileExtManager::TypeHeader:
                imgID = 1;
                break;
            default:
                imgID = 2; // other
                break;
            }

            if(FileExtManager::GetType(fn.GetFullName()) == FileExtManager::TypeHeader ||
               FileExtManager::GetType(fn.GetFullName()) == FileExtManager::TypeOther) {
                entries.push_back(wxCodeCompletionBoxEntry::New(files.Item(i), imgID));
            }
        }
        wxCodeCompletionBoxManager::Get().ShowCompletionBox(&GetCtrl(), entries, bitmaps, wxCodeCompletionBox::kNone,
                                                            wxNOT_FOUND, this);
    }
}

//=============================================================================
// <<<<<<<<<<<<<<<<<<<<<<<<<<< CodeCompletion API - END
//=============================================================================

struct ContextCpp_ClientData : public wxClientData {
    TagEntryPtr m_ptr;

    ContextCpp_ClientData(TagEntryPtr ptr) { m_ptr = ptr; }
    virtual ~ContextCpp_ClientData() {}
};

TagEntryPtr ContextCpp::GetTagAtCaret(bool scoped, bool impl)
{
    CHECK_JS_RETURN_NULL();
    VALIDATE_WORKSPACE_NULL();
    
    clEditor& rCtrl = GetCtrl();

    //	Make sure we are not on a comment section
    if(IsCommentOrString(rCtrl.GetCurrentPos())) return NULL;

    // Get the word under the cursor OR the selected word
    int word_start = -1, word_end = -1;
    rCtrl.wxStyledTextCtrl::GetSelection(&word_start, &word_end);
    if(word_start == word_end) {
        word_start = rCtrl.WordStartPos(word_start, true);
        word_end = rCtrl.WordEndPos(word_end, true);
    }
    wxString word = rCtrl.GetTextRange(word_start, word_end);
    if(word.IsEmpty()) return NULL;

    std::vector<TagEntryPtr> tags;
    if(scoped) {
        // get tags that make sense in current scope and expression
        wxFileName fname = rCtrl.GetFileName();
        wxString expr = GetExpression(word_end, false);
        wxString text = rCtrl.GetTextRange(0, word_end);
        int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition()) + 1;
        TagsManagerST::Get()->FindImplDecl(fname, line, expr, word, text, tags, impl);
        if(!impl && tags.empty()) {
            // try again, this time allow impls
            // this will find inline definitions, which have no separate declaration
            TagsManagerST::Get()->FindImplDecl(fname, line, expr, word, text, tags, true);
        }
    } else {
        // get all tags that match the name (ignore scope)
        TagsManagerST::Get()->FindSymbol(word, tags);
    }

    if(tags.empty()) {
        // Test for local variable first
        CppToken token = TagsManagerST::Get()->FindLocalVariable(
            rCtrl.GetFileName(),                    // file name
            word_start,                             // the word start position
            rCtrl.LineFromPosition(word_start) + 1, // current line
            word,
            rCtrl.GetModify() ? rCtrl.GetText()
                              : wxString()); // pass the modified text or none if the file is already saved
        if(token.getOffset() != wxString::npos) {
            // we got a match in the local scope, display it
            clEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(rCtrl.GetFileName().GetFullPath(),
                                                                           rCtrl.GetProject(), 0, token.getOffset());
            if(editor) { editor->SetSelection(token.getOffset(), token.getOffset() + token.getName().length()); }
            return NULL;
        }
        return NULL;
    }

    if(tags.size() == 1) // only one tag found
        return tags[0];

    // popup a dialog offering the results to the user
    clSelectSymbolDialogEntry::List_t entries;
    std::for_each(tags.begin(), tags.end(), [&](TagEntryPtr tag) {
        clSelectSymbolDialogEntry e;
        e.bmp = wxCodeCompletionBox::GetBitmap(tag);
        e.name = tag->GetFullDisplayName();
        e.clientData = new ContextCpp_ClientData(tag);

        wxString helpString;
        wxFileName fn(tag->GetFile());
        helpString << fn.GetFullName() << ":" << tag->GetLine();
        e.help = helpString;
        entries.push_back(e);
    });

    clSelectSymbolDialog dlg(EventNotifier::Get()->TopFrame(), entries);
    if(dlg.ShowModal() != wxID_OK) { return NULL; }
    ContextCpp_ClientData* cd = dynamic_cast<ContextCpp_ClientData*>(dlg.GetSelection());
    return cd->m_ptr;
}

bool ContextCpp::DoGotoSymbol(TagEntryPtr tag)
{
    CHECK_JS_RETURN_FALSE();
    if(tag) {
        clEditor* editor =
            clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine() - 1);
        if(editor) { editor->FindAndSelectV(tag->GetPattern(), tag->GetName()); }
        return true;
    } else {
        return false;
    }
}

bool ContextCpp::GotoDefinition()
{
    CHECK_JS_RETURN_FALSE();
    return DoGotoSymbol(GetTagAtCaret(false, false));
}

void ContextCpp::SwapFiles(const wxFileName& fileName)
{
    CHECK_JS_RETURN_VOID();

    wxStringSet_t file_options;
    FindSwappedFile(fileName, file_options);
    wxString file_to_open;
    if(file_options.size() > 1) {
        // More than one option
        wxArrayString fileArr;
        std::for_each(file_options.begin(), file_options.end(), [&](const wxString& s) { fileArr.Add(s); });
        file_to_open = ::wxGetSingleChoice(_("Multiple candidates found. Select a file to open:"),
                                           _("Swap Header/Source Implementation"), fileArr, 0);

        if(file_to_open.IsEmpty())
            // Cancel clicked
            return;

        TryOpenFile(file_to_open, false);
        return;

    } else if(!file_options.empty()) {

        file_to_open = *file_options.begin();
        if(TryOpenFile(file_to_open, false)) { return; }
    }

    // We failed to locate matched file, offer the user to create one
    // check to see if user already provided an answer
    wxFileName otherFile = fileName;
    otherFile.SetExt(FileExtManager::GetType(fileName.GetFullName()) == FileExtManager::TypeHeader ? "cpp" : "h");

    wxStandardID res = ::PromptForYesNoDialogWithCheckbox(_("No matched file was found, would you like to create one?"),
                                                          "CreateSwappedFile", _("Create"), _("Don't Create"),
                                                          _("Remember my answer and don't ask me again"),
                                                          wxYES_NO | wxCANCEL | wxICON_QUESTION | wxCANCEL_DEFAULT);
    if(res == wxID_YES) { DoCreateFile(otherFile); }
}

bool ContextCpp::FindSwappedFile(const wxFileName& rhs, wxStringSet_t& others)
{
    CHECK_JS_RETURN_FALSE();

    others.clear();
    wxString ext = rhs.GetExt();
    wxStringSet_t exts;

    // replace the file extension
    if(FileExtManager::GetType(rhs.GetFullName()) == FileExtManager::TypeSourceC ||
       FileExtManager::GetType(rhs.GetFullName()) == FileExtManager::TypeSourceCpp) {
        // try to find a header file
        exts.insert("h");
        exts.insert("hpp");
        exts.insert("hxx");
        exts.insert("h++");
        exts.insert("hh");

    } else {
        // try to find a implementation file
        exts.insert("cpp");
        exts.insert("cxx");
        exts.insert("cc");
        exts.insert("c++");
        exts.insert("c");
        exts.insert("ipp");
    }

    // Try to locate a file in the same folder first
    std::for_each(exts.begin(), exts.end(), [&](const wxString& ext) {
        wxFileName otherFile = rhs;
        otherFile.SetExt(ext);
        if(otherFile.FileExists()) { others.insert(otherFile.GetFullPath()); }
    });

    // if we found a match on the same folder, don't bother continue searching
    if(others.empty()) {

        // Get a list of workspace files
        std::vector<wxFileName> files;
        ManagerST::Get()->GetWorkspaceFiles(files, true);

        for(size_t i = 0; i < files.size(); ++i) {
            const wxFileName& workspaceFile = files.at(i);
            if((workspaceFile.GetName() == rhs.GetName()) && exts.count(workspaceFile.GetExt().Lower())) {
                others.insert(workspaceFile.GetFullPath());
            }
        }
    }
    return !others.empty();
}

bool ContextCpp::FindSwappedFile(const wxFileName& rhs, wxString& lhs)
{
    CHECK_JS_RETURN_FALSE();
    wxFileName otherFile(rhs);

    wxString ext = rhs.GetExt();
    wxArrayString exts;

    // replace the file extension
    if(IsSource(ext)) {
        // try to find a header file
        exts.Add("h");
        exts.Add("hpp");
        exts.Add("hxx");
        exts.Add("h++");

    } else {
        // try to find a implementation file
        exts.Add("cpp");
        exts.Add("cxx");
        exts.Add("cc");
        exts.Add("c++");
        exts.Add("c");
    }

    std::vector<wxFileName> files;
    ManagerST::Get()->GetWorkspaceFiles(files, true);

    for(size_t j = 0; j < exts.GetCount(); j++) {
        otherFile.SetExt(exts.Item(j));

        if(otherFile.FileExists()) {
            // we got a match
            lhs = otherFile.GetFullPath();
            return true;
        }

        for(size_t i = 0; i < files.size(); i++) {
            if(files.at(i).GetFullName() == otherFile.GetFullName()) {
                lhs = files.at(i).GetFullPath();
                return true;
            }
        }
    }
    return false;
}

bool ContextCpp::TryOpenFile(const wxFileName& fileName, bool lookInEntireWorkspace)
{
    if(fileName.FileExists()) {
        // we got a match
        wxString proj = ManagerST::Get()->GetProjectNameByFile(fileName.GetFullPath());
        return clMainFrame::Get()->GetMainBook()->OpenFile(fileName.GetFullPath(), proj, wxNOT_FOUND, wxNOT_FOUND,
                                                           (enum OF_extra)(OF_PlaceNextToCurrent | OF_AddJump));
    }

    if(!lookInEntireWorkspace) return false;

    // ok, the file does not exist in the current directory, try to find elsewhere
    // whithin the workspace files
    std::vector<wxFileName> files;
    ManagerST::Get()->GetWorkspaceFiles(files, true);

    for(size_t i = 0; i < files.size(); i++) {
        if(files.at(i).GetFullName() == fileName.GetFullName()) {
            wxString proj = ManagerST::Get()->GetProjectNameByFile(files.at(i).GetFullPath());
            return clMainFrame::Get()->GetMainBook()->OpenFile(files.at(i).GetFullPath(), proj, wxNOT_FOUND,
                                                               wxNOT_FOUND,
                                                               (enum OF_extra)(OF_PlaceNextToCurrent | OF_AddJump));
        }
    }

    return false;
}

//-----------------------------------------------
// Menu event handlers
//-----------------------------------------------
void ContextCpp::OnSwapFiles(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    wxUnusedVar(event);
    SwapFiles(GetCtrl().GetFileName());
}

void ContextCpp::DoMakeDoxyCommentString(DoxygenComment& dc, const wxString& blockPrefix, wxChar keywordPrefix)
{
    CHECK_JS_RETURN_VOID();
    clEditor& editor = GetCtrl();
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    wxString blockStart = blockPrefix;
    blockStart << "\n";

    // prepend the prefix to the
    wxString sKeywordPrefix;
    sKeywordPrefix << keywordPrefix;

    wxString classPattern = data.GetClassPattern();
    wxString funcPattern = data.GetFunctionPattern();

    // Make sure we are using the correct keyword prefix
    classPattern.Replace("@", sKeywordPrefix);
    classPattern.Replace("\\", sKeywordPrefix);
    funcPattern.Replace("@", sKeywordPrefix);
    funcPattern.Replace("\\", sKeywordPrefix);

    // replace $(Name) here **before** the call to ExpandAllVariables()
    classPattern.Replace(wxT("$(Name)"), dc.name);
    funcPattern.Replace(wxT("$(Name)"), dc.name);

    classPattern = ExpandAllVariables(classPattern, clCxxWorkspaceST::Get(), editor.GetProjectName(), wxEmptyString,
                                      editor.GetFileName().GetFullPath());
    funcPattern = ExpandAllVariables(funcPattern, clCxxWorkspaceST::Get(), editor.GetProjectName(), wxEmptyString,
                                     editor.GetFileName().GetFullPath());

    dc.comment.Replace(wxT("$(ClassPattern)"), classPattern);
    dc.comment.Replace(wxT("$(FunctionPattern)"), funcPattern);

    // close the comment
    dc.comment << wxT(" */\n");
    dc.comment.Prepend(blockStart);
}

void ContextCpp::OnInsertDoxyComment(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    wxUnusedVar(event);
    clEditor& editor = GetCtrl();

    VALIDATE_WORKSPACE();

    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    // We decide whether to use @ or \ based on the current class pattern
    wxChar keyPrefix = data.IsUseQtStyle() ? '\\' : '@';

    int curpos = editor.GetCurrentPos();
    int newPos = curpos; // the new position to place the caret after the insertion of the doxy block
    int curline = editor.GetCurrentLine();
    int insertPos = editor.PositionFromLine(curline);
    int endPos = curpos;

    if(editor.SafeGetChar(curpos - 1) == ';' || editor.SafeGetChar(curpos - 1) == '{') {
        endPos = curpos;
    } else {
        // start moving from this position until we find '{'
        for(int i = curpos; i < editor.GetLength(); ++i) {
            endPos = i;
            int ch = editor.SafeGetChar(i);
            if(ch == '{' || ch == ';') {
                ++endPos; // include this char as well
                break;
            }
        }
    }

    wxString text = editor.GetTextRange(0, endPos);
    TagEntryPtrVector_t tags = TagsManagerST::Get()->ParseBuffer(text);
    if(!tags.empty()) {
        // the last tag is our function
        TagEntryPtr t = tags.at(tags.size() - 1);
        // get doxygen comment based on file and line
        DoxygenComment dc = TagsManagerST::Get()->DoCreateDoxygenComment(t, keyPrefix);
        // do we have a comment?
        if(dc.comment.IsEmpty()) return;

        DoMakeDoxyCommentString(dc, data.GetCommentBlockPrefix(), keyPrefix);
        // To make the doxy block fit in, we need to prepend each line
        // with the exact whitespace of the current line
        wxString lineContent = editor.GetLine(editor.GetCurrentLine());
        wxString whitespace;
        for(size_t i = 0; i < lineContent.length(); ++i) {
            if(lineContent[i] == ' ' || lineContent == '\t') {
                whitespace << lineContent[i];
            } else {
                break;
            }
        }

        // Prepare the doxy block
        wxArrayString lines = ::wxStringTokenize(dc.comment, "\n", wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); ++i) {
            lines.Item(i).Prepend(whitespace);
        }

        // Join the lines back
        wxString doxyBlock = ::clJoinLinesWithEOL(lines, GetCtrl().GetEOL());
        doxyBlock << GetCtrl().GetEolString();
        doxyBlock.Replace("|", ""); // Remove any marker position

        // remove any selection
        editor.ClearSelections();
        editor.InsertText(insertPos, doxyBlock);

        // Try to place the caret after the @brief
        wxRegEx reBrief("[@\\]brief[ \t]*");
        if(reBrief.IsValid() && reBrief.Matches(doxyBlock)) {
            wxString match = reBrief.GetMatch(doxyBlock);
            // Get the index
            int where = doxyBlock.Find(match);
            if(where != wxNOT_FOUND) {
                where += match.length();
                int caretPos = insertPos + where;
                editor.SetCaretAt(caretPos);
            }
        } else {
            newPos += doxyBlock.length();
            editor.SetCaretAt(newPos);
        }
        return;
    }
}

void ContextCpp::OnCommentSelection(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetCtrl().CommentBlockSelection("/*", "*/");
}

void ContextCpp::OnCommentLine(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetCtrl().ToggleLineComment("//", wxSTC_C_COMMENTLINE);
}

void ContextCpp::OnGenerateSettersGetters(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    wxUnusedVar(event);
    clEditor& editor = GetCtrl();

    VALIDATE_WORKSPACE();

    long pos = editor.GetCurrentPos();

    if(IsCommentOrString(pos)) { return; }

    TagsManager* tagmgr = TagsManagerST::Get();
    std::vector<TagEntryPtr> tags;
    // get the scope name that the caret is currently at

    wxString text = editor.GetTextRange(0, pos);
    wxString scopeName = tagmgr->GetScopeName(text);
    tagmgr->TagsByScope(scopeName, wxT("member"), tags, false, false);
    if(tags.empty()) { return; }

    std::vector<TagEntryPtr> classtags;
    tagmgr->FindByPath(scopeName, classtags);
    if(classtags.empty() || classtags.size() > 1) return;

    TagEntryPtr tag = classtags.at(0);
    if(tag->GetFile().CmpNoCase(editor.GetFileName().GetFullPath()) != 0) {

        wxString msg;
        msg << _("This file does not seem to contain the declaration for '") << tag->GetName() << wxT("'\n");
        msg << _("The declaration of '") << tag->GetName() << _("' is located at '") << tag->GetFile() << wxT("'\n");
        msg << _("Would you like CodeLite to open this file for you?");

        if(wxMessageBox(msg, _("CodeLite"), wxYES_NO) == wxYES) {
            wxString projectName = ManagerST::Get()->GetProjectNameByFile(tag->GetFile());
            clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), projectName, tag->GetLine());
        }
        return;
    }

    int lineno = editor.LineFromPosition(editor.GetCurrentPos()) + 1;

    // get the file name and line where to insert the setters getters
    SettersGettersDlg dlg(EventNotifier::Get()->TopFrame());
    if(!dlg.Init(tags, tag->GetFile(), lineno)) {
        ::wxMessageBox(_("Seems like you have all the getters/setters you need..."), _("codelite"));
        return;
    }

    if(dlg.ShowModal() == wxID_OK) {
        clEditorStateLocker locker(editor.GetCtrl());
        wxString code = dlg.GetGenCode();
        if(code.IsEmpty() == false) { editor.InsertTextWithIndentation(code, lineno); }
        if(dlg.GetFormatText()) { DoFormatEditor(&GetCtrl()); }
    }
}

void ContextCpp::OnKeyDown(wxKeyEvent& event)
{
    clEditor& ctrl = GetCtrl();
    if(ctrl.GetFunctionTip()->IsActive()) {
        switch(event.GetKeyCode()) {
        case WXK_UP:
            ctrl.GetFunctionTip()->SelectPrev(DoGetCalltipParamterIndex());
            return;

        case WXK_DOWN:
            ctrl.GetFunctionTip()->SelectNext(DoGetCalltipParamterIndex());
            return;
        }
    }
    event.Skip();
}

void ContextCpp::OnFindImpl(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    CodeCompletionManager::Get().GotoImpl(&GetCtrl());
}

void ContextCpp::OnFindDecl(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    CodeCompletionManager::Get().GotoDecl(&GetCtrl());
}

void ContextCpp::OnUpdateUI(wxUpdateUIEvent& event)
{
    if(IsJavaScript()) {
        event.Enable(false);
        return;
    }

    bool workspaceOpen = IS_CXX_WORKSPACE_OPENED();
    bool projectAvailable = (GetCtrl().GetProjectName().IsEmpty() == false);

    if(event.GetId() == XRCID("insert_doxy_comment")) {
        event.Enable(projectAvailable);
    } else if(event.GetId() == XRCID("setters_getters")) {
        event.Enable(projectAvailable);
    } else if(event.GetId() == XRCID("go_to_function_start")) {
        event.Enable(workspaceOpen);
    } else if(event.GetId() == XRCID("go_to_next_function")) {
        event.Enable(workspaceOpen);
    } else if(event.GetId() == XRCID("find_decl")) {
        event.Enable(workspaceOpen);
    } else if(event.GetId() == XRCID("find_impl")) {
        event.Enable(workspaceOpen);
    } else if(event.GetId() == XRCID("move_impl")) {
        event.Enable(projectAvailable && GetCtrl().GetSelectedText().IsEmpty() == false);
    } else {
        event.Skip();
    }
}

void ContextCpp::SetActive()
{
    wxStyledTextEvent dummy;
    OnSciUpdateUI(dummy);
}

void ContextCpp::OnSciUpdateUI(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    clEditor& ctrl = GetCtrl();

    static long lastPos(wxNOT_FOUND);

    // get the current position
    long curpos = ctrl.GetCurrentPos();
    if(curpos != lastPos) {
        lastPos = curpos;

        // update the calltip highlighting if needed
        DoUpdateCalltipHighlight();
    }
}

void ContextCpp::OnDbgDwellEnd(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    // remove the debugger indicator
    GetCtrl().SetIndicatorCurrent(DEBUGGER_INDICATOR);
    GetCtrl().IndicatorClearRange(0, GetCtrl().GetLength());
}

void ContextCpp::OnDbgDwellStart(wxStyledTextEvent& event)
{
    static wxRegEx reCppIndentifier(wxT("[a-zA-Z_][a-zA-Z0-9_]*"));

    // the tip is already up
    if(ManagerST::Get()->GetDebuggerTip() && ManagerST::Get()->GetDebuggerTip()->IsShown()) return;

    // We disply the tooltip only if the control key is down
    DebuggerInformation info;

    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(!dbgr) { return; }

    DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), info);
    if(info.showTooltipsOnlyWithControlKeyIsDown && wxGetMouseState().ControlDown() == false) return;

    wxString word;
    clEditor& ctrl = GetCtrl();
    int pos = event.GetPosition();
    if(pos != wxNOT_FOUND) {

        if(IsCommentOrString(pos)) { return; }

        long end(0);
        long sel_start(0), sel_end(0);

        end = ctrl.WordEndPosition(pos, true);

        // if thers is no selected text, use the word calculated from the caret position
        if(!ctrl.GetSelectedText().IsEmpty()) {
            // selection is not empty, use it
            sel_start = ctrl.GetSelectionStart();
            sel_end = ctrl.GetSelectionEnd();
            word = ctrl.GetTextRange(sel_start, sel_end);

            // Mark the code we are going to try and show tip for
            GetCtrl().SetIndicatorCurrent(DEBUGGER_INDICATOR);
            GetCtrl().IndicatorFillRange(sel_start, sel_end - sel_start);

        } else {
            word = GetExpression(end, false, &GetCtrl(), false);
            word.Trim().Trim(false);

            // Mark the code we are going to try and show tip for
            GetCtrl().SetIndicatorCurrent(DEBUGGER_INDICATOR);
            GetCtrl().IndicatorFillRange(end - word.length(), word.Length());
        }

        if(word.IsEmpty()) { return; }
    } else {
        return;
    }

    if(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
        if(ManagerST::Get()->GetDebuggerTip()->IsShown() && ManagerST::Get()->GetDebuggerTip()->m_expression == word) {
            // a 'Quick Show dialog' is already shown for this word
            // dont show another tip
            return;

        } else {
            ManagerST::Get()->GetDebuggerTip()->HideDialog();
        }
    }
    dbgr->ResolveType(word, DBG_USERR_QUICKWACTH);
}

int ContextCpp::FindLineToAddInclude()
{
    clEditor& ctrl = GetCtrl();

    int maxLineToScan = ctrl.GetLineCount();
    if(maxLineToScan > 500) { maxLineToScan = 500; }

    int lineno = wxNOT_FOUND;
    bool found = false;
    int i = 0;
    // Start by skipping any initial copyright block and include-guard
    int initialblock = 0;
    for(int i = 0; i < maxLineToScan; i++) {
        wxString line = ctrl.GetLine(i).Trim().Trim(false);
        if(!(line.empty() || line.StartsWith("//") || IsComment(ctrl.PositionFromLine(i)) ||
             line.StartsWith("#ifndef") || line.StartsWith("#define"))) {
            break;
        }
        initialblock = i;
    }

    // Try to find the end of the #include list
    for(; i < maxLineToScan; i++) {
        wxString line = ctrl.GetLine(i).Trim().Trim(false);
        if(line.empty()) { continue; }
        if(IsIncludeStatement(line)) {
            lineno = i;
            found = true;
        } else if(found) {
            // Only return here if we've found at least 1
            return lineno + 1;
        }
    }

    if(lineno != wxNOT_FOUND) {
        return lineno + 1;
    } else {
        return initialblock ? initialblock + 1 : wxNOT_FOUND;
    }
}

void ContextCpp::OnMoveImpl(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();

    wxUnusedVar(e);
    clEditor& rCtrl = GetCtrl();
    VALIDATE_WORKSPACE();

    // get expression
    int pos = rCtrl.GetCurrentPos();
    int word_end = rCtrl.WordEndPosition(pos, true);
    int word_start = rCtrl.WordStartPosition(pos, true);

    // get the scope
    wxString word = rCtrl.GetTextRange(word_start, word_end);

    if(word.IsEmpty()) return;

    std::vector<TagEntryPtr> tags;
    int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition()) + 1;

    // get this scope name
    int startPos(0);
    wxString scopeText = rCtrl.GetTextRange(startPos, rCtrl.GetCurrentPos());

    // get the scope name from the text
    wxString scopeName = TagsManagerST::Get()->GetScopeName(scopeText);
    if(scopeName.IsEmpty()) { scopeName = wxT("<global>"); }

    // Find the tag
    TagsManagerST::Get()->TagsByScopeAndName(scopeName, word, tags, ExactMatch);
    if(tags.empty()) return;

    TagEntryPtr tag;
    bool match(false);
    for(std::vector<TagEntryPtr>::size_type i = 0; i < tags.size(); i++) {
        if(tags.at(i)->GetName() == word && tags.at(i)->GetLine() == line && tags.at(i)->GetKind() == wxT("function") &&
           tags.at(i)->GetScope() == scopeName) {
            // we got a match
            tag = tags.at(i);
            match = true;
            break;
        }
    }

    if(match) {

        long curPos = word_end;
        long blockEndPos(wxNOT_FOUND);
        long blockStartPos(wxNOT_FOUND);
        wxString content;

        if(DoGetFunctionBody(curPos, blockStartPos, blockEndPos, content)) {

            // create the functions body
            wxString body = TagsManagerST::Get()->FormatFunction(tag, FunctionFormat_Impl);
            // remove the empty content provided by this function
            body = body.BeforeLast(wxT('{'));
            body = body.Trim().Trim(false);
            body.Prepend(wxT("\n"));
            body << content << wxT("\n");

            wxString targetFile;
            FindSwappedFile(rCtrl.GetFileName(), targetFile);
            MoveFuncImplDlg dlg(EventNotifier::Get()->TopFrame(), body, targetFile);
            if(dlg.ShowModal() == wxID_OK) {
                // get the updated data
                targetFile = dlg.GetFileName();
                body = dlg.GetText();

                // Place the implementation in its new home
                clEditor* implEditor = clMainFrame::Get()->GetMainBook()->OpenFile(targetFile);
                if(implEditor) {

                    // Ensure that the file state is remained
                    int insertedLine = wxNOT_FOUND;
                    {
                        clEditorStateLocker locker(implEditor->GetCtrl());

                        wxString sourceContent = implEditor->GetText();
                        TagsManagerST::Get()->InsertFunctionImpl(scopeName, body, targetFile, sourceContent,
                                                                 insertedLine);
                        implEditor->SetText(sourceContent);
                        DoFormatEditor(implEditor);

                        // Remove the current body and replace it with ';'
                        rCtrl.SetTargetEnd(blockEndPos);
                        rCtrl.SetTargetStart(blockStartPos);
                        rCtrl.ReplaceTarget(wxT(";"));
                    }
                    if(insertedLine != wxNOT_FOUND) { implEditor->CenterLine(insertedLine); }
                }
            }
        }
    }
}

bool ContextCpp::DoGetFunctionBody(long curPos, long& blockStartPos, long& blockEndPos, wxString& content)
{
    CHECK_JS_RETURN_FALSE();
    clEditor& rCtrl = GetCtrl();
    blockStartPos = wxNOT_FOUND;
    blockEndPos = wxNOT_FOUND;

    // scan for the functions' start block
    while(true) {
        curPos = rCtrl.PositionAfter(curPos);

        // eof?
        if(curPos == rCtrl.GetLength()) { break; }

        // comment?
        if(IsCommentOrString(curPos)) { continue; }

        // valid character
        wxChar ch = rCtrl.GetCharAt(curPos);
        if(ch == wxT(';')) {
            // no implementation to move
            break;
        }

        if(ch == wxT('{')) {
            blockStartPos = curPos;
            break;
        }
    }

    // collect the functions' block
    if(blockStartPos != wxNOT_FOUND) {
        int depth(1);
        content << wxT("{");
        while(depth > 0) {
            curPos = rCtrl.PositionAfter(curPos);
            // eof?
            if(curPos == rCtrl.GetLength()) { break; }

            // comment?
            wxChar ch = rCtrl.GetCharAt(curPos);
            if(IsCommentOrString(curPos)) {
                content << ch;
                continue;
            }

            switch(ch) {
            case wxT('{'):
                depth++;
                break;
            case wxT('}'):
                depth--;
                break;
            }
            content << ch;
        }

        if(depth == 0) { blockEndPos = rCtrl.PositionAfter(curPos); }
    }

    return (blockEndPos > blockStartPos) && (blockEndPos != wxNOT_FOUND) && (blockStartPos != wxNOT_FOUND);
}

void ContextCpp::OnOverrideParentVritualFunctions(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();
    clEditor& rCtrl = GetCtrl();
    VALIDATE_WORKSPACE();

    // Get the text from the file start point until the current position
    int pos = rCtrl.GetCurrentPos();
    wxString context = rCtrl.GetTextRange(0, pos);
    bool onlyPure = e.GetId() == XRCID("add_pure_virtual_impl");

    wxString scopeName = TagsManagerST::Get()->GetScopeName(context);
    if(scopeName.IsEmpty() || scopeName == wxT("<global>")) {
        wxMessageBox(_("Cant resolve scope properly. Found <") + scopeName + wxT(">"), _("CodeLite"),
                     wxICON_INFORMATION | wxOK);
        return;
    }

    // get map of all unimlpemented methods
    std::vector<TagEntryPtr> protos;
    TagsManagerST::Get()->GetUnOverridedParentVirtualFunctions(scopeName, onlyPure, protos);

    // No methods to add?
    if(protos.empty()) return;

    // Locate the swapped file
    wxString targetFile(rCtrl.GetFileName().GetFullPath());
    FindSwappedFile(rCtrl.GetFileName(), targetFile);

    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    // get doxygen comment based on file and line
    wxChar keyPrefix = '@';
    if(data.IsUseQtStyle()) { keyPrefix = '\\'; }

    ImplementParentVirtualFunctionsDialog dlg(wxTheApp->GetTopWindow(), scopeName, protos, keyPrefix, this);
    dlg.SetTargetFile(targetFile);
    if(dlg.ShowModal() == wxID_OK) {
        wxString implFile = dlg.GetTargetFile();
        wxString impl = dlg.GetImpl();
        wxString decl;

        int oldLine = rCtrl.LineFromPos(rCtrl.GetCurrentPos());
        wxString headerContent;

        // add the declarations (public, protected, private)
        headerContent = GetCtrl().GetText();
        decl = dlg.GetDecl("public");
        if(!decl.IsEmpty()) {
            if(TagsManagerST::Get()->InsertFunctionDecl(scopeName, decl, headerContent, 0))
                rCtrl.SetText(headerContent);
            else
                rCtrl.InsertText(rCtrl.GetCurrentPos(), decl); // Insert at the caret position
        }

        // protected
        headerContent = GetCtrl().GetText();
        decl = dlg.GetDecl("protected");
        if(!decl.IsEmpty()) {
            if(TagsManagerST::Get()->InsertFunctionDecl(scopeName, decl, headerContent, 1))
                rCtrl.SetText(headerContent);
            else
                rCtrl.InsertText(rCtrl.GetCurrentPos(), decl); // Insert at the caret position
        }

        // private
        headerContent = GetCtrl().GetText();
        decl = dlg.GetDecl("private");
        if(!decl.IsEmpty()) {
            if(TagsManagerST::Get()->InsertFunctionDecl(scopeName, decl, headerContent, 2))
                rCtrl.SetText(headerContent);
            else
                rCtrl.InsertText(rCtrl.GetCurrentPos(), decl); // Insert at the caret position
        }

        if(dlg.IsFormatAfterInsert()) DoFormatEditor(&GetCtrl());

        rCtrl.GotoLine(rCtrl.GetLineCount() > oldLine ? oldLine : rCtrl.GetLineCount());

        // Open the implementation file and format it if needed
        clEditor* implEditor = clMainFrame::Get()->GetMainBook()->OpenFile(implFile);
        if(implEditor) {
            int insertedLine = wxNOT_FOUND;
            wxString sourceContent = implEditor->GetText();
            TagsManagerST::Get()->InsertFunctionImpl(scopeName, impl, implFile, sourceContent, insertedLine);
            implEditor->SetText(sourceContent);
            if(dlg.IsFormatAfterInsert()) DoFormatEditor(implEditor);
        }
    }

    // Restore this file to be the active one
    clMainFrame::Get()->GetMainBook()->OpenFile(GetCtrl().GetFileName().GetFullPath());
}

void ContextCpp::OnAddMultiImpl(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();

    wxUnusedVar(e);
    clEditor& rCtrl = GetCtrl();
    VALIDATE_WORKSPACE();

    // get the text from the file start point until the current position
    int pos = rCtrl.GetCurrentPos();
    wxString context = rCtrl.GetTextRange(0, pos);

    wxString scopeName = TagsManagerST::Get()->GetScopeName(context);
    if(scopeName.IsEmpty() || scopeName == wxT("<global>")) {
        wxMessageBox(_("'Add Functions Implementation' can only work inside valid scope, got (") + scopeName + wxT(")"),
                     _("CodeLite"), wxICON_INFORMATION | wxOK);
        return;
    }

    // get map of all unimlpemented methods
    std::map<wxString, TagEntryPtr> protos;
    TagsManagerST::Get()->GetUnImplementedFunctions(scopeName, protos);

    if(protos.empty()) {
        ::wxMessageBox(_("All your functions seems to have an implementation!"));
        return;
    }

    TagEntryPtrVector_t tags;
    std::map<wxString, TagEntryPtr>::const_iterator iter = protos.begin();
    for(; iter != protos.end(); ++iter) {
        tags.push_back(iter->second);
    }

    // Sort the functions according to their line number (asc)
    std::sort(tags.begin(), tags.end(), [&](TagEntryPtr a, TagEntryPtr b) { return (a->GetLine() < b->GetLine()); });

    wxString targetFile;
    FindSwappedFile(rCtrl.GetFileName(), targetFile);

    AddFunctionsImpDlg dlg(wxTheApp->GetTopWindow(), tags, targetFile);
    if(dlg.ShowModal() == wxID_OK) {
        // get the updated data
        targetFile = dlg.GetFileName();
        wxString body = dlg.GetText();
        int insertedLine = wxNOT_FOUND;

        // Open the C++ file
        clEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(targetFile, wxEmptyString, 0);
        if(!editor) { return; }

        // Inser the new functions at the proper location
        wxString sourceContent = editor->GetText();
        TagsManagerST::Get()->InsertFunctionImpl(scopeName, body, targetFile, sourceContent, insertedLine);

        {
            clEditorStateLocker locker(editor->GetCtrl());
            editor->SetText(sourceContent);
        }

        if(insertedLine != wxNOT_FOUND) { editor->CenterLine(insertedLine); }
    }
}

void ContextCpp::OnAddImpl(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();

    wxUnusedVar(e);
    clEditor& rCtrl = GetCtrl();
    VALIDATE_WORKSPACE();

    // get expression
    int pos = rCtrl.GetCurrentPos();
    int word_end = rCtrl.WordEndPosition(pos, true);
    int word_start = rCtrl.WordStartPosition(pos, true);

    // get the scope
    wxString word = rCtrl.GetTextRange(word_start, word_end);

    if(word.IsEmpty()) return;

    int foundPos(wxNOT_FOUND);
    if(rCtrl.PreviousChar(word_start, foundPos) == wxT('~')) word.Prepend(wxT("~"));

    std::vector<TagEntryPtr> tags;
    int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPosition()) + 1;

    // get this scope name
    int startPos(0);
    wxString scopeText = rCtrl.GetTextRange(startPos, rCtrl.GetCurrentPos());

    // get the scope name from the text
    wxString scopeName = TagsManagerST::Get()->GetScopeName(scopeText);
    if(scopeName.IsEmpty()) { scopeName = wxT("<global>"); }

    TagsManagerST::Get()->FindSymbol(word, tags);
    if(tags.empty()) return;

    TagEntryPtr tag;
    bool match(false);
    for(std::vector<TagEntryPtr>::size_type i = 0; i < tags.size(); i++) {
        if(tags.at(i)->GetName() == word && tags.at(i)->GetLine() == line &&
           tags.at(i)->GetKind() == wxT("prototype") && tags.at(i)->GetScope() == scopeName) {
            // we got a match
            tag = tags.at(i);
            match = true;
            break;
        }
    }

    if(match) {

        long curPos = word_end;
        long blockEndPos(wxNOT_FOUND);
        long blockStartPos(wxNOT_FOUND);
        wxString content;

        if(DoGetFunctionBody(curPos, blockStartPos, blockEndPos, content)) {
            // function already has body ...
            wxMessageBox(_("Function '") + tag->GetName() + _("' already has a body"), _("CodeLite"),
                         wxICON_WARNING | wxOK);
            return;
        }

        // create the functions body
        // replace the function signature with the normalized one, so default values
        // will not appear in the function implementation
        wxString newSig = TagsManagerST::Get()->NormalizeFunctionSig(
            tag->GetSignature(), Normalize_Func_Name | Normalize_Func_Reverse_Macro);
        tag->SetSignature(newSig);
        wxString body = TagsManagerST::Get()->FormatFunction(tag, FunctionFormat_Impl);

        wxString targetFile;
        FindSwappedFile(rCtrl.GetFileName(), targetFile);

        // if no swapped file is found, use the current file
        if(targetFile.IsEmpty()) { targetFile = rCtrl.GetFileName().GetFullPath(); }

        MoveFuncImplDlg dlg(EventNotifier::Get()->TopFrame(), body, targetFile);
        dlg.SetTitle(_("Add Function Implementation"));
        if(dlg.ShowModal() == wxID_OK) {
            // get the updated data
            targetFile = dlg.GetFileName();
            body = dlg.GetText();
            int insertedLine = wxNOT_FOUND;

            // Open the C++ file
            clEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(targetFile, wxEmptyString, 0);
            if(!editor) { return; }

            // Inser the new functions at the proper location
            wxString sourceContent = editor->GetText();
            TagsManagerST::Get()->InsertFunctionImpl(scopeName, body, targetFile, sourceContent, insertedLine);

            {
                clEditorStateLocker locker(editor->GetCtrl());
                editor->SetText(sourceContent);
            }

            if(insertedLine != wxNOT_FOUND) { editor->CenterLine(insertedLine); }
        }
    }
}

void ContextCpp::DoFormatEditor(clEditor* editor)
{
    clSourceFormatEvent formatEvent(wxEVT_FORMAT_STRING);
    formatEvent.SetInputString(editor->GetText());
    formatEvent.SetFileName(editor->GetFileName().GetFullPath());
    EventNotifier::Get()->ProcessEvent(formatEvent);
    if(!formatEvent.GetFormattedString().IsEmpty()) { editor->SetText(formatEvent.GetFormattedString()); }
}

void ContextCpp::OnFileSaved()
{
    PERF_FUNCTION();

    if(!IsJavaScript()) {
        VariableList var_list;

        wxArrayString varList;
        wxArrayString projectTags;
        VALIDATE_WORKSPACE();

        // if there is nothing to color, go ahead and return
        if(!(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_VARS)) { return; }

        // Start a colour request
        ParseRequest* parsingRequest = new ParseRequest(ManagerST::Get());
        parsingRequest->setDbFile(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().GetFullPath());
        parsingRequest->setType(ParseRequest::PR_SUGGEST_HIGHLIGHT_WORDS);
        parsingRequest->setFile(GetCtrl().GetFileName().GetFullPath());
        ParseThreadST::Get()->Add(parsingRequest);

        // Update preprocessor visualization
        ManagerST::Get()->UpdatePreprocessorFile(&GetCtrl());
    }
}

void ContextCpp::ApplySettings()
{
    //-----------------------------------------------
    // Load laguage settings from configuration file
    //-----------------------------------------------
    SetName(wxT("C++"));

    // Set the key words and the lexer
    LexerConf::Ptr_t lexPtr;
    // Read the configuration file
    if(EditorConfigST::Get()->IsOk()) { lexPtr = EditorConfigST::Get()->GetLexer(wxT("C++")); }

    // Update the control
    clEditor& rCtrl = GetCtrl();
    rCtrl.SetLexer((int)lexPtr->GetLexerId());

    wxString keyWords = lexPtr->GetKeyWords(0);
    wxString doxyKeyWords = lexPtr->GetKeyWords(2);
    wxString jsKeywords = lexPtr->GetKeyWords(1);

    // C/C++ keywords
    keyWords.Replace(wxT("\n"), wxT(" "));
    keyWords.Replace(wxT("\r"), wxT(" "));

    jsKeywords.Replace(wxT("\n"), wxT(" "));
    jsKeywords.Replace(wxT("\r"), wxT(" "));

    // A javascript file?
    if(!IsJavaScript()) {
        rCtrl.SetKeyWords(0, keyWords);
    } else {
        rCtrl.SetKeyWords(0, jsKeywords);
    }

    // Doxygen keywords
    doxyKeyWords.Replace(wxT("\n"), wxT(" "));
    doxyKeyWords.Replace(wxT("\r"), wxT(" "));
    rCtrl.SetKeyWords(2, doxyKeyWords);

    DoApplySettings(lexPtr);

    // create all images used by the cpp context
    if(!m_cppFileBmp.IsOk()) {
        // Initialise the file bitmaps
        BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();
        m_cppFileBmp = bmpLoader->LoadBitmap(wxT("mime-cpp"));
        m_hFileBmp = bmpLoader->LoadBitmap(wxT("mime-h"));
        m_otherFileBmp = bmpLoader->LoadBitmap(wxT("mime-txt"));
    }

    // delete uneeded commands
    rCtrl.CmdKeyClear('/', wxSTC_KEYMOD_CTRL);
    rCtrl.CmdKeyClear('/', wxSTC_KEYMOD_CTRL | wxSTC_KEYMOD_SHIFT);

    // update word characters to allow '~' as valid word character
    rCtrl.SetWordChars(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"));

    // Error
    wxFont guiFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    BuildTabSettingsData cmpColoursOptions;

    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &cmpColoursOptions);
    rCtrl.StyleSetBackground(eAnnotationStyleError, DrawingUtils::LightColour(cmpColoursOptions.GetErrorColour(), 9.0));
    rCtrl.StyleSetForeground(eAnnotationStyleError, cmpColoursOptions.GetErrorColour());
    // rCtrl.StyleSetFont(eAnnotationStyleError, guiFont);

    // Warning
    rCtrl.StyleSetBackground(eAnnotationStyleWarning,
                             DrawingUtils::LightColour(cmpColoursOptions.GetErrorColour(), 9.0));
    rCtrl.StyleSetForeground(eAnnotationStyleWarning, cmpColoursOptions.GetWarnColour());
    // rCtrl.StyleSetFont(eAnnotationStyleWarning, guiFont);
}

void ContextCpp::Initialize()
{
    if(!IsJavaScript()) {
        m_completionTriggerStrings.insert(".");
        m_completionTriggerStrings.insert("->");
        m_completionTriggerStrings.insert("::");

    } else {
        m_completionTriggerStrings.insert(".");
    }
}

void ContextCpp::AutoAddComment()
{
    clEditor& rCtrl = GetCtrl();

    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    int curpos = rCtrl.GetCurrentPos();
    int line = rCtrl.LineFromPosition(curpos);
    int cur_style = rCtrl.GetStyleAt(curpos);
    wxString text = rCtrl.GetLine(line - 1).Trim(false);

    bool dontadd = false;
    switch(cur_style) {
    case wxSTC_C_COMMENTLINE:
    case wxSTC_C_COMMENTLINEDOC:
        dontadd = !text.StartsWith(wxT("//")) || !data.GetContinueCppComment();
        break;
    case wxSTC_C_COMMENT:
    case wxSTC_C_COMMENTDOC:
        dontadd = !data.GetAddStarOnCComment();
        break;
    default:
        dontadd = true;
        break;
    }
    if(dontadd) {
        ContextBase::AutoIndent(wxT('\n'));
        return;
    }

    wxString toInsert;
    switch(cur_style) {
    case wxSTC_C_COMMENTLINE:
    case wxSTC_C_COMMENTLINEDOC: {
        if(text.StartsWith(wxT("//"))) {
            // try to parse the comment text and indentation
            unsigned i = (text.Length() > 2 && text[2] == wxT('!')) ? 3 : 2; // support "//!" for doxygen
            i = text.find_first_not_of(wxT('/'), i);
            i = text.find_first_not_of(wxT(" \t"), i);
            if(i == wxString::npos) i = text.Length() - 1;
            // we want to avoid duplicating line-long comments such as those
            // that sometime start a comment block; if there's something more on
            // the line, after our match, then we can assume that we do not have
            // a line-long comment; we do want to duplicate the comments on
            // otherwise blank lines, however, to allow comment blocks with
            // blank lines in the comment text, so we will still accept the
            // match if it is less than half the typical line length
            // (i.e. 80/2=40) (a guesstimate that it's not a line-long
            // comment); otherwise, we'll use a default value
            if(i < text.Length() - 1 || i < 40) {
                toInsert = text.substr(0, i);
            } else {
                if(cur_style == wxSTC_C_COMMENTLINEDOC && i >= 3)
                    toInsert = text.substr(0, 3) + wxT(" ");
                else
                    toInsert = wxT("// ");
            }
        }
    } break;
    case wxSTC_C_COMMENT:
    case wxSTC_C_COMMENTDOC: {

        CommentConfigData data;
        EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

        // Check the text typed before this char
        int startPos = rCtrl.PositionBefore(curpos);
        startPos -= 3; // for "/**"
        if(startPos >= 0) {
            wxString textTyped = rCtrl.GetTextRange(startPos, rCtrl.PositionBefore(curpos));
            if(((textTyped == "/**") || (textTyped == "/*!")) && data.IsAutoInsert() && !IsJavaScript()) {

                // Let the plugins/codelite check if they can provide a doxy comment
                // for the current entry
                wxCommandEvent dummy;
                // Parse the source file
                wxString text = rCtrl.GetTextRange(curpos, rCtrl.GetLength());
                TagEntryPtrVector_t tags = TagsManagerST::Get()->ParseBuffer(text);
                if(!tags.empty()) {
                    TagEntryPtr t = tags[0];

                    wxChar keyPrefix = (textTyped == "/*!") ? '\\' : '@';
                    // get doxygen comment based on file and line
                    DoxygenComment dc = TagsManagerST::Get()->DoCreateDoxygenComment(t, keyPrefix);
                    // do we have a comment?
                    if(dc.comment.IsEmpty()) return;

                    DoMakeDoxyCommentString(dc, textTyped, keyPrefix);
                    // To make the doxy block fit in, we need to prepend each line
                    // with the exact whitespace of the line that starts with "/**"
                    int lineStartPos = rCtrl.PositionFromLine(rCtrl.LineFromPos(startPos));
                    wxString whitespace = rCtrl.GetTextRange(lineStartPos, startPos);

                    // Prepare the doxy block
                    wxArrayString lines = ::wxStringTokenize(dc.comment, "\n", wxTOKEN_STRTOK);
                    for(size_t i = 0; i < lines.GetCount(); ++i) {
                        if(i) { // don't add it to the first line (it already exists in the editor)
                            lines.Item(i).Prepend(whitespace);
                        }
                    }

                    // Join the lines back
                    wxString doxyBlock = ::clJoinLinesWithEOL(lines, rCtrl.GetEOL());
                    doxyBlock.Replace("|", ""); // Remove any marker position
                    rCtrl.SetSelection(startPos, curpos);
                    rCtrl.ReplaceSelection(doxyBlock);

                    // Try to place the caret after the @brief
                    wxRegEx reBrief("[@\\]brief[ \t]*");
                    if(reBrief.IsValid() && reBrief.Matches(doxyBlock)) {
                        wxString match = reBrief.GetMatch(doxyBlock);
                        // Get the index
                        int where = doxyBlock.Find(match);
                        if(where != wxNOT_FOUND) {
                            where += match.length();
                            int caretPos = startPos + where;
                            rCtrl.SetCaretAt(caretPos);
                        }
                    } else {
                        rCtrl.SetCaretAt(startPos);
                    }
                    return;
                }
            }
        }

        if(rCtrl.GetStyleAt(rCtrl.PositionBefore(rCtrl.PositionBefore(curpos))) == cur_style) {
            toInsert = rCtrl.GetCharAt(rCtrl.GetLineIndentPosition(line - 1)) == wxT('*') ? wxT("* ") : wxT(" * ");
        }
        break;
    }
    }
    rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line - 1));
    int insertPos = rCtrl.GetLineIndentPosition(line);
    rCtrl.InsertText(insertPos, toInsert);
    rCtrl.SetCaretAt(insertPos + toInsert.Length());
    rCtrl.ChooseCaretX(); // set new column as "current" column
}

bool ContextCpp::IsComment(long pos)
{
    int style;
    style = GetCtrl().GetStyleAt(pos);
    return (style == wxSTC_C_COMMENT || style == wxSTC_C_COMMENTLINE || style == wxSTC_C_COMMENTDOC ||
            style == wxSTC_C_COMMENTLINEDOC || style == wxSTC_C_COMMENTDOCKEYWORD ||
            style == wxSTC_C_COMMENTDOCKEYWORDERROR);
}

void ContextCpp::OnRenameGlobalSymbol(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();
    VALIDATE_WORKSPACE();

    clEditor& rCtrl = GetCtrl();
    // get expression
    int pos = rCtrl.GetCurrentPos();
    int word_start = rCtrl.WordStartPosition(pos, true);
    int word_end = rCtrl.WordEndPosition(pos, true);

    // Read the word that we want to refactor
    wxString word = rCtrl.GetTextRange(word_start, word_end);
    if(word.IsEmpty()) return;

    // Save all files before refactoring
    if(!clMainFrame::Get()->GetMainBook()->SaveAll(true, false)) return;

    // Get list of projects to work on
    wxArrayString projectsCandidateList, projects;
    clCxxWorkspaceST::Get()->GetProjectList(projectsCandidateList);
    if(projectsCandidateList.IsEmpty()) return;

    if(projectsCandidateList.GetCount() > 1) {
        SelectProjectsDlg projectSelectorDlg(EventNotifier::Get()->TopFrame());
        if(projectSelectorDlg.ShowModal() != wxID_OK) { return; }
        projects = projectSelectorDlg.GetProjects();
        if(projects.IsEmpty()) { return; }
    } else {
        // we have excatly one project. Skip the 'Project Selector' dialog
        projects.swap(projectsCandidateList);
    }

    wxArrayString filesArr;
    for(size_t i = 0; i < projects.GetCount(); ++i) {
        ManagerST::Get()->GetProjectFiles(projects.Item(i), filesArr);
    }

    // Convert the array into wxFileList_t
    wxFileList_t files;
    files.reserve(filesArr.GetCount());
    for(size_t i = 0; i < filesArr.GetCount(); ++i) {
        files.push_back(wxFileName(filesArr.Item(i)));
    }

    // Invoke the RefactorEngine
    if(RefactoringEngine::Instance()->IsBusy()) {
        ::wxMessageBox(_("Refactoring engine is busy with another request. Please try again later"), "CodeLite",
                       wxOK | wxICON_WARNING);
        return;
    }

    RefactoringEngine::Instance()->RenameGlobalSymbol(word, rCtrl.GetFileName(), rCtrl.LineFromPosition(pos + 1),
                                                      word_start, files);
}

void ContextCpp::ReplaceInFiles(const wxString& word, const CppToken::Vec_t& li)
{
    int off = 0;
    wxString fileName(wxEmptyString);
    bool success(false);

    // Disable the "Limit opened buffers" feature for during replacements
    clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(false);

    // Try to maintain as far as possible the editor and line within it that the user started from.
    // Otherwise a different editor may be selected, and the original one will have scrolled to the last replacement
    int current_line = wxSTC_INVALID_POSITION;
    clEditor* current = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(current) { current_line = current->GetCurrentLine(); }

    clEditor* previous = NULL;
    for(CppToken::Vec_t::const_iterator iter = li.begin(); iter != li.end(); ++iter) {
        CppToken cppToken = *iter;
        wxString file_name(cppToken.getFilename());
        if(fileName == file_name) {
            // update next token offset in case we are still in the same file
            cppToken.setOffset(cppToken.getOffset() + off);
        } else {
            // switched file
            off = 0;
            fileName = file_name;
        }

        // Open the file only once
        clEditor* editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
        if(!editor || editor->GetFileName().GetFullPath() != file_name) {
            editor = clMainFrame::Get()->GetMainBook()->OpenFile(file_name, wxEmptyString, 0);
            // We've loaded a new editor, so start a new bulk undo action for it
            // (this can only be done per editor, not per refactor :( )
            // First end any previous one
            if(previous) { previous->EndUndoAction(); }
            editor->BeginUndoAction();
            previous = editor;
        }

        if(editor) {
            editor->SetSelection(cppToken.getOffset(), cppToken.getOffset() + cppToken.getName().length());
            if(editor->GetSelectionStart() != editor->GetSelectionEnd()) {
                editor->ReplaceSelection(word);
                off += word.Len() - cppToken.getName().length();
                success = true; // Flag that there's been at least one replacement
            }
        }
    }

    // The last editor won't have this done otherwise
    if(previous) { previous->EndUndoAction(); }

    if(current) {
        clMainFrame::Get()->GetMainBook()->SelectPage(current);
        if(current_line != wxSTC_INVALID_POSITION) { current->GotoLine(current_line); }
    }

    // re-enable the feature again
    clMainFrame::Get()->GetMainBook()->SetUseBuffereLimit(true);

    if(success) { clGetManager()->GetStatusBar()->SetMessage(_("Symbol renamed")); }
}

void ContextCpp::OnRetagFile(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();
    VALIDATE_WORKSPACE();

    wxUnusedVar(e);
    clEditor& editor = GetCtrl();
    if(editor.GetModify()) {
        wxMessageBox(wxString::Format(_("Please save the file before retagging it")));
        return;
    }

    RetagFile();
    editor.SetActive();
}

void ContextCpp::RetagFile()
{
    CHECK_JS_RETURN_VOID();
    if(ManagerST::Get()->GetRetagInProgress()) return;

    clEditor& editor = GetCtrl();
    ManagerST::Get()->RetagFile(editor.GetFileName().GetFullPath());

    // incase this file is not cache this function does nothing
    TagsManagerST::Get()->ClearCachedFile(editor.GetFileName().GetFullPath());
}

void ContextCpp::OnUserTypedXChars(const wxString& word)
{
    // user typed more than 3 chars, display completion box with C++ keywords
    if(IsCommentOrString(GetCtrl().GetCurrentPos())) { return; }

    if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_CPP_KEYWORD_ASISST) {
        std::vector<TagEntryPtr> tags;
        MakeCppKeywordsTags(word, tags);
        if(tags.empty() == false) {
            GetCtrl().ShowCompletionBox(tags,  // list of tags
                                        word); // do not automatically insert word if there is only single choice
        }
    }
}

void ContextCpp::MakeCppKeywordsTags(const wxString& word, std::vector<TagEntryPtr>& tags)
{
    // C++ keywords are handled differently
    if(!IsJavaScript()) return;

    LexerConf::Ptr_t lexPtr;
    // Read the configuration file
    if(EditorConfigST::Get()->IsOk()) { lexPtr = EditorConfigST::Get()->GetLexer(this->GetName()); }

    wxString cppWords;

    if(lexPtr) {
        cppWords = lexPtr->GetKeyWords(1);

    } else {
        cppWords = "abstract boolean break byte case catch char class "
                   "const continue debugger default delete do double else enum export extends "
                   "final finally float for function goto if implements import in instanceof "
                   "int interface long native new package private protected public "
                   "return short static super switch synchronized this throw throws "
                   "transient try typeof var void volatile while with";
    }

    wxString s1(word);
    std::set<wxString> uniqueWords;
    wxArrayString wordsArr = wxStringTokenize(cppWords, wxT(" \r\t\n"));
    for(size_t i = 0; i < wordsArr.GetCount(); i++) {

        // Dont add duplicate words
        if(uniqueWords.find(wordsArr.Item(i)) != uniqueWords.end()) continue;

        uniqueWords.insert(wordsArr.Item(i));
        wxString s2(wordsArr.Item(i));
        if(s2.StartsWith(s1) || s2.Lower().StartsWith(s1.Lower())) {
            TagEntryPtr tag(new TagEntry());
            tag->SetName(wordsArr.Item(i));
            tag->SetKind(wxT("cpp_keyword"));
            tags.push_back(tag);
        }
    }
}

wxString ContextCpp::CallTipContent()
{
    // if we have an active call tip, return its content
    if(GetCtrl().GetFunctionTip()->IsActive()) return GetCtrl().GetFunctionTip()->GetText();

    return wxEmptyString;
}

bool ContextCpp::DoCodeComplete(long pos)
{
    CHECK_JS_RETURN_FALSE();
    clDEBUG1() << "ContextCpp::DoCodeComplete(" << pos << ") is called" << clEndl;
    long currentPosition = pos;
    bool showFuncProto = false;
    int pos1, pos2, end;
    clEditor& editor = GetCtrl();
    wxChar ch = editor.PreviousChar(pos, pos1);

    //	Make sure we are not on a comment section
    if(IsCommentOrString(editor.PositionBefore(pos))) { return false; }

    // Search for first non-whitespace wxChar
    clDEBUG1() << "Triggering char is:" << ch << clEndl;
    switch(ch) {
    case '.':
        // Class / Struct completion
        editor.PreviousChar(pos1, end);
        break;
    case '>':
        // Check previous character if is '-'
        // We open drop box as well
        if(editor.PreviousChar(pos1, pos2) == '-') {
            editor.PreviousChar(pos2, end);
        } else {
            return false;
        }
        break;
    case ':':
        // Check previous character if is ':'
        // We open drop box as well
        if(editor.PreviousChar(pos1, pos2) == wxT(':')) {
            editor.PreviousChar(pos2, end);
        } else {
            return false;
        }
        break;
    case '(':
        showFuncProto = true;
        // is this setting is on?
        if(!(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISP_FUNC_CALLTIP)) { return false; }
        editor.PreviousChar(pos1, end);
        break;
    default:
        return false;
    }

    // get expression
    wxString expr = GetExpression(currentPosition, false);

    // get the scope
    // Optimize the text for large files
    int line = editor.LineFromPosition(editor.GetCurrentPosition()) + 1;
    int startPos(0);

    // enable faster scope name resolving if needed
    if(!(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_ACCURATE_SCOPE_RESOLVING)) {
        TagEntryPtr t = TagsManagerST::Get()->FunctionFromFileLine(editor.GetFileName(), line);
        if(t) {
            startPos = editor.PositionFromLine(t->GetLine() - 1);
            if(startPos > currentPosition) { startPos = 0; }
        }
    }

    wxString text = editor.GetTextRange(startPos, currentPosition);

    // collect all text from 0 - first scope found
    // this will help us detect statements like 'using namespace foo;'
    if(startPos) { //> 0
        // get the first function on this file
        int endPos(0);
        int endPos1(0);
        int endPos2(0);
        TagEntryPtr t2 = TagsManagerST::Get()->FirstFunctionOfFile(editor.GetFileName());
        if(t2) {
            endPos1 = editor.PositionFromLine(t2->GetLine() - 1);
            if(endPos1 > 0 && endPos1 <= startPos) { endPos = endPos1; }
        }

        TagEntryPtr t3 = TagsManagerST::Get()->FirstScopeOfFile(editor.GetFileName());
        if(t3) {
            endPos2 = editor.PositionFromLine(t3->GetLine() - 1);
            if(endPos2 > 0 && endPos2 <= startPos && endPos2 < endPos1) { endPos = endPos2; }
        }

        wxString globalText = editor.GetTextRange(0, endPos);
        globalText.Append(wxT(";"));
        text.Prepend(globalText);
    }

    if(showFuncProto) {
        clDEBUG1() << "Function prototype is requested..." << clEndl;
        // for function prototype, the last char entered was '(', this will break
        // the logic of the Getexpression() method to workaround this, we search for
        // expression one char before the current position
        expr = GetExpression(editor.PositionBefore(currentPosition), false);

        // display function tooltip
        int word_end = editor.WordEndPosition(end, true);
        int word_start = editor.WordStartPosition(end, true);

        // get the token
        wxString word = editor.GetTextRange(word_start, word_end);
        clDEBUG1() << "Function prototype is requested for:" << expr << "|" << word << clEndl;
        return CodeCompletionManager::Get().Calltip(&editor, line, expr, text, word);

    } else {
        DoSetProjectPaths();
        return CodeCompletionManager::Get().CodeComplete(&editor, line, expr, text);
    }
}

int ContextCpp::GetHyperlinkRange(int pos, int& start, int& end)
{
    clEditor& rCtrl = GetCtrl();
    int lineNum = rCtrl.LineFromPosition(pos);
    wxString fileName;
    wxString line = rCtrl.GetLine(lineNum);
    if(IsIncludeStatement(line, &fileName)) {
        start = rCtrl.PositionFromLine(lineNum) + line.find(fileName);
        end = start + fileName.size();
        return start <= pos && pos <= end ? XRCID("open_include_file") : wxID_NONE;
    }
    return ContextBase::GetHyperlinkRange(pos, start, end);
}

void ContextCpp::GoHyperlink(int start, int end, int type, bool alt)
{
    (void)alt;

    if(type == XRCID("open_include_file")) {
        m_selectedWord = GetCtrl().GetTextRange(start, end);
        DoOpenWorkspaceFile();
    } else {
        if(type == XRCID("find_tag")) {
            wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_impl"));
            clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
        }
    }
}

void ContextCpp::DoOpenWorkspaceFile()
{
    wxFileName fileName(m_selectedWord);
    wxString tmpName(m_selectedWord);

    tmpName.Replace(wxT("\\"), wxT("/"));
    if(tmpName.Contains(wxT(".."))) tmpName = fileName.GetFullName();

#ifdef __WXMSW__
    // On windows, files are case in-sensitive
    tmpName.MakeLower();
#endif

    std::vector<wxFileName> files, files2;

#ifdef __WXMSW__
    wxString lcNameOnly = fileName.GetFullName();
    lcNameOnly.MakeLower();
    TagsManagerST::Get()->GetFiles(lcNameOnly, files);
#else
    TagsManagerST::Get()->GetFiles(fileName.GetFullName(), files);
#endif

    // filter out the all files that does not have an exact match
    for(size_t i = 0; i < files.size(); i++) {
        wxString curFileName = files.at(i).GetFullPath();

#ifdef __WXMSW__
        // On windows, files are case in-sensitive
        curFileName.MakeLower();
#endif

        curFileName.Replace(wxT("\\"), wxT("/"));
        if(curFileName.EndsWith(tmpName)) { files2.push_back(files.at(i)); }
    }

    wxString fileToOpen;
    if(files2.size() > 1) {
        wxArrayString choices;
        wxStringSet_t uniqueFileSet;
        for(size_t i = 0; i < files2.size(); i++) {
            wxString fullPath = files2.at(i).GetFullPath();
            wxString fullPathLc = (wxGetOsVersion() & wxOS_WINDOWS) ? fullPath.Lower() : fullPath;

            // Dont add duplicate entries.
            // On Windows, we have a non case sensitive file system
            if(uniqueFileSet.count(fullPathLc) == 0) {
                uniqueFileSet.insert(fullPathLc);
                choices.Add(fullPath);
            }
        }

        fileToOpen = wxGetSingleChoice(_("Select file to open:"), _("Select file"), choices, &GetCtrl());
    } else if(files2.size() == 1) {
        fileToOpen = files2.at(0).GetFullPath();
    }

    if(fileToOpen.IsEmpty() == false) { clMainFrame::Get()->GetMainBook()->OpenFile(fileToOpen); }
}

void ContextCpp::DoCreateFile(const wxFileName& fn)
{
    // get the file name from the user
    wxString new_file = wxGetTextFromUser(_("New File Name:"), _("Create File"), fn.GetFullPath(), clMainFrame::Get());
    if(new_file.IsEmpty()) {
        // user clicked cancel
        return;
    }

    // if the project is part of a project, add this file to the same project
    // (under the same virtual folder as well)
    if(GetCtrl().GetProject().IsEmpty() == false) {
        ProjectPtr p = ManagerST::Get()->GetProject(GetCtrl().GetProject());
        if(p) {
            wxString vd = p->GetVDByFileName(GetCtrl().GetFileName().GetFullPath());
            vd.Prepend(p->GetName() + wxT(":"));

            if(vd.IsEmpty() == false) {
                clMainFrame::Get()->GetWorkspaceTab()->GetFileView()->CreateAndAddFile(new_file, vd);
            }
        }
    } else {
        // just a plain file
        wxFile file;
        if(!file.Create(new_file.GetData(), true)) return;

        if(file.IsOpened()) { file.Close(); }
    }

    TryOpenFile(wxFileName(new_file));
}

void ContextCpp::OnGotoFunctionStart(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    int line_number = GetCtrl().LineFromPosition(GetCtrl().GetCurrentPos());
    TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(GetCtrl().GetFileName(), line_number);
    if(tag) {
        // move the caret to the function start
        BrowseRecord jumpfrom = GetCtrl().CreateBrowseRecord();
        GetCtrl().SetCaretAt(GetCtrl().PositionFromLine(tag->GetLine() - 1));
        // add an entry to the navigation manager
        NavMgr::Get()->AddJump(jumpfrom, GetCtrl().CreateBrowseRecord());
    }
}

void ContextCpp::OnGotoNextFunction(wxCommandEvent& event)
{
    CHECK_JS_RETURN_VOID();
    int line_number = GetCtrl().LineFromPosition(GetCtrl().GetCurrentPos());
    TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(GetCtrl().GetFileName(), line_number + 1, true);
    if(tag) {
        // move the caret to the function start
        BrowseRecord jumpfrom = GetCtrl().CreateBrowseRecord();
        GetCtrl().SetCaretAt(GetCtrl().PositionFromLine(tag->GetLine() - 1));
        // add an entry to the navigation manager
        NavMgr::Get()->AddJump(jumpfrom, GetCtrl().CreateBrowseRecord());
    }
}

void ContextCpp::OnCallTipClick(wxStyledTextEvent& e) { e.Skip(); }

void ContextCpp::OnCalltipCancel() {}

void ContextCpp::DoUpdateCalltipHighlight()
{
    CHECK_JS_RETURN_VOID();
    clEditor& ctrl = GetCtrl();
    if(ctrl.GetFunctionTip()->IsActive()) { ctrl.GetFunctionTip()->Highlight(DoGetCalltipParamterIndex()); }
}

void ContextCpp::SemicolonShift()
{
    int foundPos(wxNOT_FOUND);
    int semiColonPos(wxNOT_FOUND);
    clEditor& ctrl = GetCtrl();
    if(ctrl.NextChar(ctrl.GetCurrentPos(), semiColonPos) == wxT(')')) {

        // test to see if we are inside a 'for' statement
        long openBracePos(wxNOT_FOUND);
        int posWordBeforeOpenBrace(wxNOT_FOUND);

        if(ctrl.MatchBraceBack(wxT(')'), semiColonPos, openBracePos)) {
            ctrl.PreviousChar(openBracePos, posWordBeforeOpenBrace);
            if(posWordBeforeOpenBrace != wxNOT_FOUND) {
                wxString word = ctrl.PreviousWord(posWordBeforeOpenBrace, foundPos);

                // c++ expression with single line and should be treated separatly
                if(word == wxT("for")) return;

                // At the current pos, we got a ';'
                // at semiColonPos we got ;
                // switch
                ctrl.DeleteBack();
                ctrl.SetCurrentPos(semiColonPos);
                ctrl.InsertText(semiColonPos, wxT(";"));
                ctrl.SetCaretAt(semiColonPos + 1);
                ctrl.GetFunctionTip()->Deactivate();
            }
        }
    }
}
void ContextCpp::DoSetProjectPaths()
{
    wxArrayString projects;
    wxArrayString projectPaths;
    ManagerST::Get()->GetProjectList(projects);
    for(size_t i = 0; i < projects.GetCount(); i++) {
        ProjectPtr p = ManagerST::Get()->GetProject(projects.Item(i));
        if(p) { projectPaths.Add(p->GetFileName().GetPath()); }
    }
    TagsManagerST::Get()->SetProjectPaths(projectPaths);
}

wxString ContextCpp::GetCurrentScopeName()
{
    if(IsJavaScript()) { return wxEmptyString; }

    TagEntryPtr tag =
        TagsManagerST::Get()->FunctionFromFileLine(GetCtrl().GetFileName(), GetCtrl().GetCurrentLine() + 1);
    if(tag) { return tag->GetParent(); }
    return wxEmptyString;
}

wxString ContextCpp::GetExpression(long pos, bool onlyWord, clEditor* editor, bool forCC)
{
    if(IsJavaScript()) { return wxEmptyString; }

    bool cont(true);
    int depth(0);

    clEditor* ctrl(NULL);
    if(!editor) {
        ctrl = &GetCtrl();
    } else {
        ctrl = editor;
    }

    int position(pos);
    int at(position);
    bool prevGt(false);
    while(cont && depth >= 0) {
        wxChar ch = ctrl->PreviousChar(position, at, true);
        position = at;
        // Eof?
        if(ch == 0) {
            at = 0;
            break;
        }

        // Comment?
        int style = ctrl->GetStyleAt(position);
        if(style == wxSTC_C_COMMENT || style == wxSTC_C_COMMENTLINE || style == wxSTC_C_COMMENTDOC ||
           style == wxSTC_C_COMMENTLINEDOC || style == wxSTC_C_COMMENTDOCKEYWORD ||
           style == wxSTC_C_COMMENTDOCKEYWORDERROR || style == wxSTC_C_STRING || style == wxSTC_C_STRINGEOL ||
           style == wxSTC_C_CHARACTER) {
            continue;
        }

        switch(ch) {
        case wxT(';'):
            // dont include this token
            at = ctrl->PositionAfter(at);
            cont = false;
            break;
        case wxT('-'):
            if(prevGt) {
                prevGt = false;
                // if previous char was '>', we found an arrow so reduce the depth
                // which was increased
                depth--;
            } else {
                if(depth <= 0) {
                    // dont include this token
                    at = ctrl->PositionAfter(at);
                    cont = false;
                }
            }
            break;
        case wxT(' '):
        case wxT('\n'):
        case wxT('\v'):
        case wxT('\t'):
        case wxT('\r'):
            prevGt = false;
            if(depth <= 0) {
                cont = false;
                break;
            }
            break;
        case wxT('{'):
            prevGt = false;
            cont = false;
            break;
        case wxT('='):
            prevGt = false;
            cont = false;
            // dont include this token
            at = ctrl->PositionAfter(at);
            break;
        case wxT('('):
        case wxT('['):
            depth--;
            prevGt = false;
            if(depth < 0) {
                // dont include this token
                at = ctrl->PositionAfter(at);
                cont = false;
            }
            break;
        case ',':
        case '*':
        case '&':
        case '!':
        case '~':
        case '+':
        case '^':
        case '|':
        case '%':
        case '?':
        case '/':
            prevGt = false;
            if(depth <= 0) {
                // dont include this token
                at = ctrl->PositionAfter(at);
                cont = false;
            }
            break;
        case wxT('>'):
            prevGt = true;
            depth++;
            break;
        case wxT('<'):
            prevGt = false;
            depth--;
            if(depth < 0) {

                // dont include this token
                at = ctrl->PositionAfter(at);
                cont = false;
            }
            break;
        case wxT(')'):
        case wxT(']'):
            prevGt = false;
            depth++;
            break;
        default:
            prevGt = false;
            break;
        }
    }

    if(at < 0) at = 0;
    wxString expr = ctrl->GetTextRange(at, pos);
    if(!forCC) {
        // If we do not require the expression for CodeCompletion
        // return the un-touched buffer
        return expr;
    }

    // remove comments from it
    CppScanner sc;
    sc.SetText(_C(expr));
    wxString expression;
    while((sc.yylex()) != 0) {
        wxString token = _U(sc.YYText());
        expression += token;
        expression += wxT(" ");
    }
    return expression;
}

void ContextCpp::OnFindReferences(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();
    VALIDATE_WORKSPACE();

    clEditor& rCtrl = GetCtrl();
    // get expression
    int pos = rCtrl.GetCurrentPos();
    int word_start = rCtrl.WordStartPosition(pos, true);
    int word_end = rCtrl.WordEndPosition(pos, true);

    // Read the word that we want to refactor
    wxString word = rCtrl.GetTextRange(word_start, word_end);
    if(word.IsEmpty()) return;

    // Save all files before 'find usage'
    if(!clMainFrame::Get()->GetMainBook()->SaveAll(true, false)) return;

    // Invoke the RefactorEngine
    if(RefactoringEngine::Instance()->IsBusy()) {
        ::wxMessageBox(_("Refactoring engine is busy with another request. Please try again later"), "CodeLite",
                       wxOK | wxICON_WARNING);
        return;
    }

    // Get list of files to search in
    std::vector<wxFileName> files;
    ManagerST::Get()->GetWorkspaceFiles(files, true);

    // Invoke the RefactorEngine
    RefactoringEngine::Instance()->FindReferences(word, rCtrl.GetFileName(), rCtrl.LineFromPosition(pos + 1),
                                                  word_start, files);
}

bool ContextCpp::IsDefaultContext() const { return false; }

void ContextCpp::OnSyncSignatures(wxCommandEvent& e)
{
    CHECK_JS_RETURN_VOID();
    VALIDATE_WORKSPACE();

    clEditor& rCtrl = GetCtrl();

    // get expression
    int pos = rCtrl.GetCurrentPos();
    int word_start = rCtrl.WordStartPosition(pos, true);
    int word_end = rCtrl.WordEndPosition(pos, true);

    // Read the word that we want to refactor
    wxString word = rCtrl.GetTextRange(word_start, word_end);
    if(word.IsEmpty()) return;

    // Save all files before 'find usage'
    if(!clMainFrame::Get()->GetMainBook()->SaveAll(true, false)) return;

    int line = rCtrl.GetCurrentLine() + 1;

    // get the full text of the current page
    wxString text = rCtrl.GetTextRange(0, pos);
    wxString expr = GetExpression(word_end, false);
    TagEntryPtr tag = RefactoringEngine::Instance()->SyncSignature(rCtrl.GetFileName(), line, pos, word, text, expr);
    if(!tag) return;

    // Locate the function start and end pos
    clEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, 0);
    if(!editor) return;

    int end, start;
    if(DoGetSingatureRange(tag->GetLine() - 1, start, end, editor)) {
        editor->SetSelection(start, end);
        editor->ReplaceSelection(tag->GetSignature());
    }
}

bool ContextCpp::DoGetSingatureRange(int line, int& start, int& end, clEditor* ctrl)
{
    CHECK_JS_RETURN_FALSE();
    start = wxNOT_FOUND;
    end = wxNOT_FOUND;

    int nStart = ctrl->PositionFromLine(line);
    int nLen = ctrl->GetLength();
    int nCur = nStart;

    while(nCur < nLen) {
        wxChar ch = ctrl->SafeGetChar(nCur);
        if(IsCommentOrString(nCur)) {
            nCur++;
            continue;
        }

        if(ch == wxT('(')) {
            start = nCur;
            nCur++;
            break;
        }
        nCur++;
    }

    if(start == wxNOT_FOUND) return false;

    // search for the function end position
    int nDepth = 1;
    while((nCur < nLen) && nDepth > 0) {

        wxChar ch = ctrl->SafeGetChar(nCur);
        if(ctrl->GetContext()->IsCommentOrString(nCur)) {
            nCur++;
            continue;
        }

        switch(ch) {
        case wxT('('):
            nDepth++;
            break;
        case wxT(')'):
            nDepth--;
            if(nDepth == 0) {
                nCur++;
                end = nCur;
            }
            break;
        default:
            break;
        }
        nCur++;
    }

    if(end == wxNOT_FOUND) return false;

    return true;
}

bool ContextCpp::IsJavaScript() const
{
    return (m_container->GetFileName().GetExt().CmpNoCase("js") == 0 ||
            m_container->GetFileName().GetExt().CmpNoCase("javascript") == 0);
}

bool ContextCpp::IsAtBlockComment() const
{
    int curpos = GetCtrl().GetCurrentPos();
    int cur_style = GetCtrl().GetStyleAt(curpos);
    return cur_style == wxSTC_C_COMMENTDOC || cur_style == wxSTC_C_COMMENT;
}

bool ContextCpp::IsAtLineComment() const
{
    int curpos = GetCtrl().GetCurrentPos();
    int cur_style = GetCtrl().GetStyleAt(curpos);
    return cur_style == wxSTC_C_COMMENTLINE || cur_style == wxSTC_C_COMMENTLINEDOC;
}

void ContextCpp::OnShowCodeNavMenu(clCodeCompletionEvent& e)
{
    clEditor* editor = dynamic_cast<clEditor*>(e.GetEditor());
    if(!editor || editor != &GetCtrl()) {
        e.Skip();
        return;
    }

    wxMenu menu(_("Find Symbol"));
    menu.Append(XRCID("find_decl"), _("Go to Declaration"));
    menu.Append(XRCID("find_impl"), _("Go to Implementation"));
    editor->PopupMenu(&menu);
}

void ContextCpp::ColourContextTokens(const wxString& workspaceTokensStr, const wxString& localsTokensStr)
{
    clEditor& ctrl = GetCtrl();
    size_t cc_flags = TagsManagerST::Get()->GetCtagsOptions().GetFlags();

    //------------------------------------------
    // Classes
    //------------------------------------------
    wxString flatStrClasses = cc_flags & CC_COLOUR_VARS ? workspaceTokensStr : "";
    ctrl.SetKeyWords(1, flatStrClasses);
    ctrl.SetKeywordClasses(flatStrClasses);

    wxString flatStrLocals = cc_flags & CC_COLOUR_VARS ? localsTokensStr : "";
    ctrl.SetKeyWords(3, flatStrLocals);
    ctrl.SetKeywordLocals(flatStrLocals);
}

wxMenu* ContextCpp::GetMenu()
{
    wxMenu* menu = NULL;
    if(!IsJavaScript()) {
        // load the context menu from the resource manager
        menu = wxXmlResource::Get()->LoadMenu(wxT("editor_right_click"));
        wxMenuItem* item = menu->FindItem(XRCID("grep_current_workspace"));
        if(item) { item->SetBitmap(wxXmlResource::Get()->LoadBitmap("m_bmpFindInFiles")); }
    } else {
        menu = wxXmlResource::Get()->LoadMenu(wxT("editor_right_click_default"));
    }
    return menu;
}

void ContextCpp::OnCodeCompleteFiles(clCodeCompletionEvent& event)
{
    if(event.GetEventObject() == this) {
        const wxString& selection = event.GetWord();
        wxString origWordChars = GetCtrl().GetWordChars();
        // for proper string selection, we want to replace all the #include statement
        // including any / and .
        // to do that, we temporary replace the word-chars of the wxSTC control to include
        // these chars, perform the selection and then restore the word chars
        wxString newWordChars = origWordChars;
        newWordChars << "./-$";
        GetCtrl().SetWordChars(newWordChars);
        int startPos = GetCtrl().WordStartPos(GetCtrl().GetCurrentPos(), true);
        int endPos = GetCtrl().GetCurrentPos();
        GetCtrl().SetSelection(startPos, endPos);
        GetCtrl().ReplaceSelection(selection);
        GetCtrl().SetCaretAt(startPos + selection.Len());
        GetCtrl().CallAfter(&wxStyledTextCtrl::SetFocus);

        // Restore the original word chars
        GetCtrl().SetWordChars(origWordChars);

    } else {
        // not ours
        event.Skip();
    }
}

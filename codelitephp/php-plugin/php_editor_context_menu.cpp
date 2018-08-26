#include <wx/regex.h>
#include "php_editor_context_menu.h"
#include "php_utils.h"
#include <wx/msgdlg.h>
#include <plugin.h>
#include <wx/app.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>
#include "php_code_completion.h"
#include <event_notifier.h>
#include "PHPRefactoring.h"
#include "PHPEntityBase.h"
#include "cl_command_event.h"
#include <codelite_events.h>
#include "clSTCLineKeeper.h"
#include "PHPSourceFile.h"
#include "PHPEntityClass.h"
#include "PHPSettersGettersDialog.h"
#include "clEditorStateLocker.h"
#include <wx/regex.h>
#include "globals.h"
#include "editor_config.h"

PHPEditorContextMenu* PHPEditorContextMenu::ms_instance = 0;

PHPEditorContextMenu::PHPEditorContextMenu()
    : m_manager(NULL)
{
    m_comment_line_1 = wxT("//");
    m_comment_line_2 = wxT("#");
    m_start_comment = wxT("/*");
    m_close_comment = wxT("*/");
}

PHPEditorContextMenu::~PHPEditorContextMenu()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &PHPEditorContextMenu::OnContextMenu, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR_MARGIN, &PHPEditorContextMenu::OnMarginContextMenu, this);

    wxTheApp->Disconnect(wxID_OPEN_PHP_FILE,
                         wxID_FIND_REFERENCES,
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(PHPEditorContextMenu::OnPopupClicked),
                         NULL,
                         this);
    wxTheApp->Disconnect(wxID_ADD_DOXY_COMMENT,
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(PHPEditorContextMenu::OnInsertDoxyComment),
                         NULL,
                         this);
    wxTheApp->Disconnect(wxID_GENERATE_GETTERS_SETTERS,
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(PHPEditorContextMenu::OnGenerateSettersGetters),
                         NULL,
                         this);
    wxTheApp->Unbind(wxEVT_MENU, &PHPEditorContextMenu::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Unbind(wxEVT_MENU, &PHPEditorContextMenu::OnCommentSelection, this, XRCID("comment_selection"));
}

void PHPEditorContextMenu::ConnectEvents()
{
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &PHPEditorContextMenu::OnContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR_MARGIN, &PHPEditorContextMenu::OnMarginContextMenu, this);
    // The below Connect catches *all* the menu events there is no need to
    // call it per menu entry
    wxTheApp->Connect(wxID_OPEN_PHP_FILE,
                      wxID_FIND_REFERENCES,
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(PHPEditorContextMenu::OnPopupClicked),
                      NULL,
                      this);
    wxTheApp->Connect(wxID_ADD_DOXY_COMMENT,
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(PHPEditorContextMenu::OnInsertDoxyComment),
                      NULL,
                      this);
    wxTheApp->Connect(wxID_GENERATE_GETTERS_SETTERS,
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(PHPEditorContextMenu::OnGenerateSettersGetters),
                      NULL,
                      this);
    wxTheApp->Bind(wxEVT_MENU, &PHPEditorContextMenu::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Bind(wxEVT_MENU, &PHPEditorContextMenu::OnCommentSelection, this, XRCID("comment_selection"));
}

PHPEditorContextMenu* PHPEditorContextMenu::Instance()
{
    if(ms_instance == 0) {
        ms_instance = new PHPEditorContextMenu();
    }
    return ms_instance;
}

void PHPEditorContextMenu::Release()
{
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void PHPEditorContextMenu::DoBuildMenu(wxMenu* menu, IEditor* editor)
{
    // Add the default actions:
    // If we are placed over an include/include_once/require/require_once statement,
    // add an option in the menu to open it
    wxString includeWhat;

    // if this is not a PHP section than the above menu items are all we can offer
    int styleAtPos = editor->GetStyleAtPos(editor->GetSelectionStart());
    if(!IsPHPSection(styleAtPos)) return;

    menu->PrependSeparator();
    menu->Prepend(wxID_GOTO_DEFINITION, _("Goto definition"));

    wxMenu* refactoringMenu = new wxMenu;
    refactoringMenu->Append(wxID_ADD_DOXY_COMMENT, _("Insert Doxygen Comment"));
    refactoringMenu->Append(wxID_GENERATE_GETTERS_SETTERS, _("Generate Setters / Getters"));

    menu->AppendSeparator();
    menu->Append(wxID_ANY, _("Code Generation"), refactoringMenu);

    if(IsIncludeOrRequireStatement(includeWhat)) {
        menu->PrependSeparator();
        menu->Prepend(wxID_OPEN_PHP_FILE, wxString::Format(_("Open '%s'"), includeWhat.c_str()));
    }
}

bool PHPEditorContextMenu::IsPHPCommentOrString(int styleAtPos) const
{
    if((styleAtPos == wxSTC_HPHP_HSTRING) || (styleAtPos == wxSTC_HPHP_SIMPLESTRING) ||
       (styleAtPos == wxSTC_HPHP_COMMENT) || (styleAtPos == wxSTC_HPHP_COMMENTLINE))
        return true;
    return false;
}

bool PHPEditorContextMenu::IsPHPSection(int styleAtPos) const
{
    if((styleAtPos == wxSTC_HPHP_DEFAULT) || (styleAtPos == wxSTC_HPHP_HSTRING) ||
       (styleAtPos == wxSTC_HPHP_SIMPLESTRING) || (styleAtPos == wxSTC_HPHP_WORD) ||
       (styleAtPos == wxSTC_HPHP_NUMBER) || (styleAtPos == wxSTC_HPHP_VARIABLE) || (styleAtPos == wxSTC_HPHP_COMMENT) ||
       (styleAtPos == wxSTC_HPHP_COMMENTLINE) || (styleAtPos == wxSTC_HPHP_HSTRING_VARIABLE) ||
       (styleAtPos == wxSTC_HPHP_OPERATOR))
        return true;
    return false;
}

bool PHPEditorContextMenu::IsLineMarked()
{
    GET_EDITOR_SCI_BOOL();

    int nPos = sci->GetCurrentPos();
    int nLine = sci->LineFromPosition(nPos);
    int nBits = sci->MarkerGet(nLine);

    // 128 is the mask representing a bookmark (refer to enum marker_mask_type in cl_editor.h)
    return (nBits & 128 ? true : false);
}

bool PHPEditorContextMenu::IsIncludeOrRequireStatement(wxString& includeWhat)
{
    // Do a basic check to see whether this line is include statement or not.
    // Don't bother in full parsing the file since it can be a quite an expensive operation
    // (include|require_once|require|include_once)[ \t\\(]*(.*?)[\\) \t)]*;
    static wxRegEx reInclude(wxT("(include|require_once|require|include_once)[ \t\\(]*(.*?)[\\) \t]*;"), wxRE_ADVANCED);

    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) return false;

    wxString line = editor->GetCtrl()->GetLine(editor->GetCurrentLine());
    if(reInclude.IsValid() && reInclude.Matches(line)) {
        includeWhat = reInclude.GetMatch(line, 2);
        return true;
    }
    return false;
}

bool PHPEditorContextMenu::GetIncludeOrRequireFileName(wxString& fn)
{
    GET_EDITOR_SCI_BOOL();

    // Get the current line text
    int lineStart = editor->PosFromLine(editor->GetCurrentLine());
    int lineEnd = editor->LineEnd(editor->GetCurrentLine());
    wxString lineText = editor->GetTextRange(lineStart, lineEnd);
    fn.swap(lineText);
    return true;
}

void PHPEditorContextMenu::DoOpenPHPFile()
{
    wxString fn;
    if(!GetIncludeOrRequireFileName(fn)) return; // no editor...
    wxString outFile = PHPCodeCompletion::Instance()->ExpandRequire(m_manager->GetActiveEditor()->GetFileName(), fn);
    if(!outFile.IsEmpty()) {
        m_manager->OpenFile(outFile);
    }
}

void PHPEditorContextMenu::DoGotoBeginningOfScope()
{
    GET_EDITOR_SCI_VOID();

    int caret_pos = sci->GetCurrentPos();
    wxArrayString tokensBlackList;     // there isn't a black list for '}'
    tokensBlackList.Add(wxT("{$"));    // T_CURLY_OPEN: complex variable parsed syntax
    tokensBlackList.Add(wxT("${"));    // T_DOLLAR_OPEN_CURLY_BRACES: complex variable parsed syntax
    tokensBlackList.Add(wxT("\"${a")); // T_STRING_VARNAME: complex variable parsed syntax
    int startOfScopePos = GetTokenPosInScope(sci, wxT("{"), 0, caret_pos, false, tokensBlackList);
    if(startOfScopePos == wxSTC_INVALID_POSITION) startOfScopePos = caret_pos;
    SET_CARET_POS(startOfScopePos);
}
void PHPEditorContextMenu::DoGotoEndOfScope()
{
    GET_EDITOR_SCI_VOID();

    int caret_pos = sci->GetCurrentPos();
    int end_of_file_pos = sci->GetLineEndPosition(sci->GetLineCount() - 1); // get the file last sel pos
    wxArrayString tokensBlackList;                                          // there isn't a black list for '}'
    int endOfScopePos = GetTokenPosInScope(sci, wxT("}"), caret_pos, end_of_file_pos, true, tokensBlackList);
    if(endOfScopePos == wxSTC_INVALID_POSITION) endOfScopePos = caret_pos;
    SET_CARET_POS(endOfScopePos);
}

void PHPEditorContextMenu::DoGotoDefinition()
{
    CHECK_PTR_RET(m_manager->GetActiveEditor());
    PHPCodeCompletion::Instance()->GotoDefinition(m_manager->GetActiveEditor(),
                                                  m_manager->GetActiveEditor()->GetCtrl()->GetCurrentPos());
}

int PHPEditorContextMenu::GetTokenPosInScope(wxStyledTextCtrl* sci,
                                             const wxString& token,
                                             int start_pos,
                                             int end_pos,
                                             bool direction,
                                             const wxArrayString& tokensBlackList)
{
    sci->SetTargetStart(start_pos);
    sci->SetTargetEnd(end_pos);
    int token_pos = wxSTC_INVALID_POSITION;

    if(direction) { // search down
        sci->SetCurrentPos(start_pos);
        sci->SearchAnchor();
        token_pos = sci->SearchNext(sci->GetSearchFlags(), token);
        while(token_pos != wxSTC_INVALID_POSITION && IsTokenInBlackList(sci, token, token_pos, tokensBlackList)) {
            sci->SetCurrentPos(token_pos + 1);
            sci->SearchAnchor();
            token_pos = sci->SearchNext(sci->GetSearchFlags(), token);
        }
    } else { // search up
        sci->SetCurrentPos(end_pos);
        sci->SearchAnchor();
        token_pos = sci->SearchPrev(sci->GetSearchFlags(), token);
        while(token_pos != wxSTC_INVALID_POSITION && IsTokenInBlackList(sci, token, token_pos, tokensBlackList)) {
            sci->SetCurrentPos(token_pos - 1);
            sci->SearchAnchor();
            token_pos = sci->SearchPrev(sci->GetSearchFlags(), token);
        }
    }

    return token_pos;
}

bool PHPEditorContextMenu::IsTokenInBlackList(wxStyledTextCtrl* sci,
                                              const wxString& token,
                                              int token_pos,
                                              const wxArrayString& tokensBlackList)
{
    for(int i = 0; i < (int)tokensBlackList.size(); i++) {
        sci->SetTargetStart(token_pos - (int)tokensBlackList[i].length());
        sci->SetTargetEnd(token_pos + (int)tokensBlackList[i].length());
        if(sci->SearchInTarget(tokensBlackList[i]) != wxSTC_INVALID_POSITION) return true;
    }
    return false;
}
void PHPEditorContextMenu::DoContextMenu(IEditor* editor, wxCommandEvent& e)
{
    long closePos = editor->GetCurrentPosition();
    if(closePos != wxNOT_FOUND) {
        if(!editor->GetSelection().IsEmpty()) {
            // If the selection text is placed under the cursor,
            // keep it selected, else, unselect the text
            // and place the caret to be under cursor
            int selStart = editor->GetSelectionStart();
            int selEnd = editor->GetSelectionEnd();
            if(closePos < selStart || closePos > selEnd) {
                // Cursor is not over the selected text, unselect and re-position caret
                editor->SetCaretAt(closePos);
            }
        } else {
            // No selection, just place the caret
            editor->SetCaretAt(closePos);
        }
    }

    // Create the context menu.
    // Menu can be allocated on the stack otherwise we need to delete it later
    wxMenu menu;
    DoBuildMenu(&menu, editor);
    editor->GetCtrl()->PopupMenu(&menu);
}

void PHPEditorContextMenu::OnContextMenu(clContextMenuEvent& e)
{
    e.Skip();
    IEditor* editor = m_manager->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(editor && IsPHPFile(editor)) {
        DoBuildMenu(e.GetMenu(), editor);
    }
}

void PHPEditorContextMenu::OnMarginContextMenu(clContextMenuEvent& e)
{
    e.Skip();
    IEditor* editor = m_manager->GetActiveEditor();
    if(editor && IsPHPFile(editor)) {
        wxMenu* menu = e.GetMenu();
        // Remove non-PHP related entries from the menu
        if(menu->FindItem(XRCID("insert_temp_breakpoint"))) {
            menu->Remove(XRCID("insert_temp_breakpoint"));
        }

        if(menu->FindItem(XRCID("insert_disabled_breakpoint"))) {
            menu->Remove(XRCID("insert_disabled_breakpoint"));
        }

        if(menu->FindItem(XRCID("insert_cond_breakpoint"))) {
            menu->Remove(XRCID("insert_cond_breakpoint"));
        }

        if(menu->FindItem(XRCID("ignore_breakpoint"))) {
            menu->Remove(XRCID("ignore_breakpoint"));
        }

        if(menu->FindItem(XRCID("toggle_breakpoint_enabled_status"))) {
            menu->Remove(XRCID("toggle_breakpoint_enabled_status"));
        }

        if(menu->FindItem(XRCID("edit_breakpoint"))) {
            menu->Remove(XRCID("edit_breakpoint"));
        }
    }
}

void PHPEditorContextMenu::OnContextOpenDocument(wxCommandEvent& event) { wxUnusedVar(event); }

bool PHPEditorContextMenu::RemoveTokenFirstIteration(wxStyledTextCtrl* sci,
                                                     const wxString& token,
                                                     bool direction,
                                                     int& caret_pos)
{
    // initialization of start_pos & end_pos
    int line_number = sci->LineFromPosition(sci->GetCurrentPos());
    int start_pos, end_pos;
    if(direction) {
        start_pos = sci->GetCurrentPos() - token.length();
        end_pos = sci->GetLineEndPosition(line_number);
    } else {
        start_pos = sci->PositionFromLine(line_number);
        end_pos = sci->GetCurrentPos() + token.length();
    }

    // loop until the token is found
    int token_pos = wxSTC_INVALID_POSITION;
    while(token_pos == wxSTC_INVALID_POSITION) {
        sci->SetTargetStart(start_pos);
        sci->SetTargetEnd(end_pos);
        token_pos = sci->SearchInTarget(token);
        if(token_pos != wxSTC_INVALID_POSITION) { // match
            int res = RemoveComment(sci, token_pos, token);
            if(!direction) {
                caret_pos -= res;
                if(caret_pos < token_pos) caret_pos = token_pos;
            } else {
                if(caret_pos > token_pos) caret_pos = token_pos;
            }
            return true;
        }

        if(direction)
            line_number++;
        else
            line_number--;

        start_pos = sci->PositionFromLine(line_number);
        end_pos = sci->GetLineEndPosition(line_number);
    }

    return false;
}
// this function search and remove single lines comments: '//' and '#'
// the currentPos is required in order to avoid cases in which the comment start after the caret
bool PHPEditorContextMenu::RemoveSingleLineComment(wxStyledTextCtrl* sci, int& caret_pos)
{
    int currentPos = sci->GetCurrentPos();
    int line_number = sci->LineFromPosition(currentPos);
    int line_start_pos = sci->PositionFromLine(line_number);

    // search for a single line comment
    sci->SetTargetStart(line_start_pos);
    sci->SetTargetEnd(currentPos);
    int start_comment_pos = sci->SearchInTarget(m_comment_line_1);
    if(start_comment_pos != wxSTC_INVALID_POSITION) {
        caret_pos -= RemoveComment(sci, start_comment_pos, m_comment_line_1);
        return true;
    }

    start_comment_pos = sci->SearchInTarget(m_comment_line_2);
    if(start_comment_pos != wxSTC_INVALID_POSITION) {
        caret_pos -= RemoveComment(sci, start_comment_pos, m_comment_line_2);
        return true;
    }

    return false;
}

int PHPEditorContextMenu::RemoveComment(wxStyledTextCtrl* sci, int posFrom, const wxString& value)
{
    sci->SetAnchor(posFrom);
    int posTo = posFrom;
    for(int i = 0; i < (int)value.Length(); i++) posTo = sci->PositionAfter(posTo);

    sci->SetSelection(posFrom, posTo);
    sci->DeleteBack();
    return posTo - posFrom;
}

void PHPEditorContextMenu::OnPopupClicked(wxCommandEvent& event)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(editor && IsPHPFile(editor)) {
        switch(event.GetId()) {
        case wxID_OPEN_PHP_FILE:
            DoOpenPHPFile();
            break;
        case wxID_GOTO_DEFINITION:
            DoGotoDefinition();
            break;
        case wxID_FIND_REFERENCES:
            // DoFindReferences();
            break;
        default:
            event.Skip();
            break;
        }
    } else {
        event.Skip();
    }
}

wxStyledTextCtrl* PHPEditorContextMenu::DoGetActiveScintila()
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(editor) {
        return editor->GetCtrl();
    }
    return NULL;
}

void PHPEditorContextMenu::OnInsertDoxyComment(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(editor) {
        PHPEntityBase::Ptr_t entry =
            PHPCodeCompletion::Instance()->GetPHPEntityAtPos(editor, editor->GetCurrentPosition());
        if(entry) {
            wxStyledTextCtrl* ctrl = editor->GetCtrl();
            ctrl->BeginUndoAction();

            CommentConfigData data;
            EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

            wxString comment = entry->FormatPhpDoc(data);

            // Create the whitespace buffer
            int lineStartPos = ctrl->PositionFromLine(ctrl->GetCurrentLine());
            int lineEndPos = lineStartPos + ctrl->LineLength(ctrl->GetCurrentLine());

            // Collect all whitespace from the begining of the line until the first non whitespace
            // character we find
            wxString whitespace;
            for(int i = lineStartPos; lineStartPos < lineEndPos; ++i) {
                if(ctrl->GetCharAt(i) == ' ' || ctrl->GetCharAt(i) == '\t') {
                    whitespace << (wxChar)ctrl->GetCharAt(i);
                } else {
                    break;
                }
            }

            // Prepare the comment block
            wxArrayString lines = ::wxStringTokenize(comment, "\n", wxTOKEN_STRTOK);
            for(size_t i = 0; i < lines.size(); ++i) {
                lines.Item(i).Prepend(whitespace);
            }

            // Glue the lines back together
            wxString doxyBlock = ::clJoinLinesWithEOL(lines, ctrl->GetEOLMode());
            doxyBlock << (ctrl->GetEOLMode() == wxSTC_EOL_CRLF ? "\r\n" : "\n");

            // Insert the text
            ctrl->InsertText(lineStartPos, doxyBlock);

            // Try to place the caret after the @brief
            wxRegEx reBrief("[@\\]brief[ \t]*");
            if(reBrief.IsValid() && reBrief.Matches(doxyBlock)) {
                wxString match = reBrief.GetMatch(doxyBlock);
                // Get the index
                int where = doxyBlock.Find(match);
                if(where != wxNOT_FOUND) {
                    where += match.length();
                    int caretPos = lineStartPos + where;
                    editor->SetCaretAt(caretPos);
                    // Remove the @brief as its non standard in the PHP world
                    editor->GetCtrl()->DeleteRange(caretPos - match.length(), match.length());
                }
            }
            editor->GetCtrl()->EndUndoAction();
        }
    }
}

void PHPEditorContextMenu::OnGenerateSettersGetters(wxCommandEvent& e)
{
    // CHeck the current context
    IEditor* editor = m_manager->GetActiveEditor();
    if(editor) {

        // determine the scope name at the current position
        // Parse until the current position
        wxString text = editor->GetTextRange(0, editor->GetCurrentPosition());
        PHPSourceFile sourceFile(text, NULL);
        sourceFile.SetParseFunctionBody(true);
        sourceFile.SetFilename(editor->GetFileName());
        sourceFile.Parse();

        const PHPEntityClass* scopeAtPoint = sourceFile.Class()->Cast<PHPEntityClass>();
        if(!scopeAtPoint) {
            // Could not determine the scope at the give location
            return;
        }

        // get the class name
        wxString className = scopeAtPoint->GetShortName();

        // generate the code to generate
        wxString textToAdd;
        PHPSettersGettersDialog dlg(EventNotifier::Get()->TopFrame(), editor, m_manager);
        if(dlg.ShowModal() == wxID_OK) {
            PHPSetterGetterEntry::Vec_t members = dlg.GetMembers();
            for(size_t i = 0; i < members.size(); ++i) {
                textToAdd << members.at(i).GetSetter(dlg.GetScope(), dlg.GetFlags()) << "\n";
                textToAdd << members.at(i).GetGetter(dlg.GetFlags()) << "\n";
            }

            if(!textToAdd.IsEmpty()) {
                int line = PHPCodeCompletion::Instance()->GetLocationForSettersGetters(
                    editor->GetTextRange(0, editor->GetLength()), className);

                if(!textToAdd.IsEmpty() && line != wxNOT_FOUND) {
                    editor->GetCtrl()->InsertText(editor->PosFromLine(line), textToAdd);
                }
            }
        }
    }
}

void PHPEditorContextMenu::OnCommentLine(wxCommandEvent& event)
{
    event.Skip();
    IEditor* editor = m_manager->GetActiveEditor();
    if(editor && IsPHPFile(editor)) {
        event.Skip(false);
        editor->ToggleLineComment("//", wxSTC_HPHP_COMMENTLINE);
    }
}

void PHPEditorContextMenu::OnCommentSelection(wxCommandEvent& event)
{
    event.Skip();
    IEditor* editor = m_manager->GetActiveEditor();
    if(editor && IsPHPFile(editor)) {
        event.Skip(false);
        editor->CommentBlockSelection("/*", "*/");
    }
}

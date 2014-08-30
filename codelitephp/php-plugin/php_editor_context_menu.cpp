#include <wx/regex.h>
#include "php_editor_context_menu.h"
#include "php_utils.h"
#include <wx/msgdlg.h>
#include <plugin.h>
#include <wx/app.h>
#include <wx/stc/stc.h>
#include "php_parser_api.h"
#include <wx/xrc/xmlres.h>
#include "php_code_completion.h"
#include <event_notifier.h>
#include "php_storage.h"
#include "PHPRefactoring.h"

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
    EventNotifier::Get()->Disconnect(wxEVT_CMD_EDITOR_CONTEXT_MENU,        wxCommandEventHandler(PHPEditorContextMenu::OnContextMenu),       NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_EDITOR_MARGIN_CONTEXT_MENU, wxCommandEventHandler(PHPEditorContextMenu::OnMarginContextMenu), NULL, this);
    
    wxTheApp->Disconnect(wxID_COMMENT_LINE, wxID_FIND_REFERENCES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PHPEditorContextMenu::OnPopupClicked), NULL, this);
    wxTheApp->Disconnect(wxID_ADD_DOXY_COMMENT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PHPEditorContextMenu::OnInsertDoxyComment), NULL, this);
    wxTheApp->Disconnect(wxID_GENERATE_GETTERS_SETTERS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PHPEditorContextMenu::OnGenerateSettersGetters), NULL, this);
    
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_CUT);
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_COPY);
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_PASTE);
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_SELECTALL);
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_DELETE);
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_UNDO);
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_REDO);
}

void PHPEditorContextMenu::ConnectEvents()
{
    EventNotifier::Get()->Connect(wxEVT_CMD_EDITOR_CONTEXT_MENU,        wxCommandEventHandler(PHPEditorContextMenu::OnContextMenu),       NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_EDITOR_MARGIN_CONTEXT_MENU, wxCommandEventHandler(PHPEditorContextMenu::OnMarginContextMenu), NULL, this);
    // The below Connect catches *all* the menu events there is no need to
    // call it per menu entry
    wxTheApp->Connect(wxID_COMMENT_LINE, wxID_FIND_REFERENCES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PHPEditorContextMenu::OnPopupClicked), NULL, this);
    wxTheApp->Connect(wxID_ADD_DOXY_COMMENT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PHPEditorContextMenu::OnInsertDoxyComment), NULL, this);
    wxTheApp->Connect(wxID_GENERATE_GETTERS_SETTERS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PHPEditorContextMenu::OnGenerateSettersGetters), NULL, this);
    
    // The below should cover wxID_CUT, wxID_COPY, wxID_PASTE, wxID_CLEAR, wxID_FIND, wxID_DUPLICATE, wxID_SELECTALL, wxID_DELETE
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_CUT);
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_COPY);
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_PASTE);
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_SELECTALL);
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_DELETE);
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_UNDO);
    wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &PHPEditorContextMenu::OnPopupClicked, this, wxID_REDO);
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

void PHPEditorContextMenu::DoBuildMenu(wxMenu *menu, IEditor* editor)
{
    // Add the default actions:
    // If we are placed over an include/include_once/require/require_once statement,
    // add an option in the menu to open it
    wxString includeWhat;

    menu->Append(wxID_COPY,      _("&Copy"),       _("Copy"));
    menu->Append(wxID_PASTE,     _("&Paste"),      _("Paste"));
    menu->Append(wxID_UNDO,      _("&Undo"),       _("Undo"));
    menu->Append(wxID_REDO,      _("&Redo"),       _("Redo"));
    menu->AppendSeparator();
    menu->Append(wxID_CUT,       _("&Cut"),        _("Cut"));
    menu->Append(wxID_DELETE,    _("&Delete"),     _("Delete"));
    menu->AppendSeparator();
    menu->Append(wxID_SELECTALL, _("&Select All"), _("Select All"));

    // if this is not a PHP section than the above menu items are all we can offer
    int styleAtPos = editor->GetStyleAtPos(editor->GetSelectionStart());
    if(!IsPHPSection(styleAtPos))
        return;
    
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

    // this is not an include/require line.
    // check other options.
    // add an option to remove a comment line
    if ((styleAtPos == wxSTC_HPHP_COMMENTLINE)||
        (styleAtPos == wxSTC_HPHP_COMMENT)) {
        menu->AppendSeparator();
        
        if(!editor->GetTextRange(editor->GetSelectionStart(), editor->GetSelectionEnd()).IsEmpty()) {
            menu->Append(wxID_UNCOMMENT_SELECTION, _("Uncomment selection"));
        }
        
        menu->Append(wxID_UNCOMMENT_LINE, _("Uncomment line"));
        menu->Append(wxID_UNCOMMENT, _("Uncomment"));

    } else { // add an option to comment a complete line
        menu->AppendSeparator();
        if(!editor->GetTextRange(editor->GetSelectionStart(), editor->GetSelectionEnd()).IsEmpty())
            menu->Append(wxID_COMMENT_SELECTION, _("Comment selection"));
        menu->Append(wxID_COMMENT_LINE, _("Comment line"));

    }
}

void PHPEditorContextMenu::DoBuildMarginMenu(wxMenu *menu, IEditor* editor)
{
    menu->Append(XRCID("toggle_bookmark"), IsLineMarked() ? wxString(_("Remove Bookmark")) : wxString(_("Add Bookmark")) );

    // The below Connect catches *all* the menu events there is no need to
    // call it per menu entry
    //menu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PHPEditorContextMenu::OnPopupClicked), NULL, this);
}

bool PHPEditorContextMenu::IsPHPCommentOrString(int styleAtPos) const
{
    if( (styleAtPos == wxSTC_HPHP_HSTRING)          ||
        (styleAtPos == wxSTC_HPHP_SIMPLESTRING)     ||
        (styleAtPos == wxSTC_HPHP_COMMENT)          ||
        (styleAtPos == wxSTC_HPHP_COMMENTLINE)
        )
        return true;
    return false;
}

bool PHPEditorContextMenu::IsPHPSection(int styleAtPos) const
{
    if( (styleAtPos == wxSTC_HPHP_DEFAULT)          ||
        (styleAtPos == wxSTC_HPHP_HSTRING)          ||
        (styleAtPos == wxSTC_HPHP_SIMPLESTRING)     ||
        (styleAtPos == wxSTC_HPHP_WORD)             ||
        (styleAtPos == wxSTC_HPHP_NUMBER)           ||
        (styleAtPos == wxSTC_HPHP_VARIABLE)         ||
        (styleAtPos == wxSTC_HPHP_COMMENT)          ||
        (styleAtPos == wxSTC_HPHP_COMMENTLINE)      ||
        (styleAtPos == wxSTC_HPHP_HSTRING_VARIABLE) ||
        (styleAtPos == wxSTC_HPHP_OPERATOR))
        return true;
    return false;
}

bool PHPEditorContextMenu::IsLineMarked()
{
    GET_EDITOR_SCI_BOOL();

    int nPos  = sci->GetCurrentPos();
    int nLine = sci->LineFromPosition(nPos);
    int nBits = sci->MarkerGet(nLine);

    // 128 is the mask representing a bookmark (refer to enum marker_mask_type in cl_editor.h)
    return (nBits & 128 ? true : false);
}

bool PHPEditorContextMenu::IsIncludeOrRequireStatement(wxString &includeWhat)
{
    // Do a basic check to see whether this line is include statement or not.
    // Don't bother in full parsing the file since it can be a quite an expensive operation
    // (include|require_once|require|include_once)[ \t\\(]*(.*?)[\\) \t)]*;
    static wxRegEx reInclude(wxT("(include|require_once|require|include_once)[ \\t\\(]*(.*?)[\\) \\t]*;"), wxRE_ADVANCED);

    IEditor * editor = m_manager->GetActiveEditor();
    if(!editor)
        return false;

    wxString line = editor->GetSTC()->GetLine(editor->GetCurrentLine());
    if(reInclude.IsValid() && reInclude.Matches(line)) {
        includeWhat = reInclude.GetMatch(line, 2);
        return true;
    }
    return false;
}

bool PHPEditorContextMenu::GetIncludeOrRequireFileName(wxFileName &fn)
{
    GET_EDITOR_SCI_BOOL();

    // Get the current line text
    int lineStart = editor->PosFromLine(editor->GetCurrentLine());
    int lineEnd   = editor->LineEnd(editor->GetCurrentLine());
    wxString lineText = editor->GetTextRange(lineStart, lineEnd);

    fn = PHPParser.parseIncludeStatement(lineText, editor->GetFileName().GetFullPath());

    return true;
}

void PHPEditorContextMenu::DoOpenPHPFile()
{
    wxFileName fn;

    if(!GetIncludeOrRequireFileName(fn))
        return; // no editor...

    m_manager->OpenFile(fn.GetFullPath());
}

void PHPEditorContextMenu::DoGotoBeginningOfScope()
{
    GET_EDITOR_SCI_VOID();

    int caret_pos = sci->GetCurrentPos();
    wxArrayString tokensBlackList; // there isn't a black list for '}'
    tokensBlackList.Add(wxT("{$")); // T_CURLY_OPEN: complex variable parsed syntax
    tokensBlackList.Add(wxT("${")); // T_DOLLAR_OPEN_CURLY_BRACES: complex variable parsed syntax
    tokensBlackList.Add(wxT("\"${a")); // T_STRING_VARNAME: complex variable parsed syntax
    int startOfScopePos = GetTokenPosInScope(sci, wxT("{"), 0, caret_pos, false, tokensBlackList);
    if(startOfScopePos == wxSTC_INVALID_POSITION)
        startOfScopePos = caret_pos;
    SET_CARET_POS(startOfScopePos);
}
void PHPEditorContextMenu::DoGotoEndOfScope()
{
    GET_EDITOR_SCI_VOID();

    int caret_pos = sci->GetCurrentPos();
    int end_of_file_pos = sci->GetLineEndPosition(sci->GetLineCount()-1); // get the file last sel pos
    wxArrayString tokensBlackList; // there isn't a black list for '}'
    int endOfScopePos = GetTokenPosInScope(sci, wxT("}"), caret_pos, end_of_file_pos, true, tokensBlackList);
    if(endOfScopePos == wxSTC_INVALID_POSITION)
        endOfScopePos = caret_pos;
    SET_CARET_POS(endOfScopePos);
}

void PHPEditorContextMenu::DoGotoDefinition()
{
    wxStyledTextCtrl *sci = DoGetActiveScintila();
    if(!sci) return;

    PHPLocationPtr definitionLocation = PHPCodeCompletion::Instance()->FindDefinition(m_manager->GetActiveEditor(), sci->GetCurrentPos());

    if(!definitionLocation)
        return;

    if(!definitionLocation->filename)
        return;

    // Open the file (make sure we use the 'OpenFile' so we will get a browsing record)
    if(m_manager->OpenFile(definitionLocation->filename, wxEmptyString, definitionLocation->linenumber)) {
        // Select the word in the editor (its a new one)
        IEditor *activeEditor = m_manager->GetActiveEditor();
        if(activeEditor) {
            int selectFromPos = activeEditor->GetSTC()->PositionFromLine(definitionLocation->linenumber);
            activeEditor->FindAndSelect(definitionLocation->what, definitionLocation->what, selectFromPos, NULL);
        }
    }
}

/*
void PHPEditorContextMenu::DoFindReferences()
{
	GET_EDITOR_SCI_VOID();

	int caret_pos = sci->GetCurrentPos();
	int word_start = sci->WordStartPosition(caret_pos, true);
	int word_end   = sci->WordEndPosition(caret_pos, true);

	// Read the word that we want to refactor
	wxString word = sci->GetTextRange(word_start, word_end);
	if(word.IsEmpty())
		return;

	// Save all files before 'find usage'
	if (!m_manager->SaveAll())
		return;

	// Get list of files to search in
//	wxFileList files;
	//ManagerST::Get()->GetWorkspaceFiles(files, true);
//	m_manager->GetWorkspace()->GetWorkspaceFiles(files, true);

	// Invoke the RefactorEngine - this is not relevant here since it is cpp oriented.
	// TODO: discuss with Eran
	// RefactoringEngine::Instance()->FindReferences(word, rCtrl.GetFileName(), rCtrl.LineFromPosition(pos+1), word_start, files);

	// Show the results
	// m_manager->GetOutputPane()->GetShowUsageTab()->ShowUsage( RefactoringEngine::Instance()->GetCandidates(), word);
}*/
int PHPEditorContextMenu::GetTokenPosInScope(wxStyledTextCtrl *sci, const wxString &token, int start_pos, int end_pos, bool direction, const wxArrayString &tokensBlackList)
{
    sci->SetTargetStart(start_pos);
    sci->SetTargetEnd(end_pos);
    int token_pos = wxSTC_INVALID_POSITION;

    if(direction) { // search down
        /*token_pos = sci->SearchInTarget(token);
        while(token_pos != wxSTC_INVALID_POSITION && IsTokenInBlackList(sci, token, token_pos, tokensBlackList)) {
        	sci->SetTargetStart(token_pos+1);
        	sci->SetTargetEnd(end_pos);
        	token_pos = sci->SearchInTarget(token);
        }*/
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
bool PHPEditorContextMenu::IsTokenInBlackList(wxStyledTextCtrl *sci, const wxString &token, int token_pos, const wxArrayString &tokensBlackList)
{
    for(int i=0; i<(int)tokensBlackList.size(); i++) {
        sci->SetTargetStart(token_pos - (int)tokensBlackList[i].length());
        sci->SetTargetEnd(token_pos + (int)tokensBlackList[i].length());
        if(sci->SearchInTarget(tokensBlackList[i]) != wxSTC_INVALID_POSITION)
            return true;
    }
    return false;
}
void PHPEditorContextMenu::DoContextMenu(IEditor* editor, wxCommandEvent& e)
{
    long closePos = editor->GetCurrentPosition();
    if (closePos != wxNOT_FOUND) {
        if (!editor->GetSelection().IsEmpty()) {
            // If the selection text is placed under the cursor,
            // keep it selected, else, unselect the text
            // and place the caret to be under cursor
            int selStart = editor->GetSelectionStart();
            int selEnd   = editor->GetSelectionEnd();
            if (closePos < selStart || closePos > selEnd) {
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
    editor->GetSTC()->PopupMenu(&menu);
}

void PHPEditorContextMenu::DoMarginContextMenu(IEditor* editor)
{
    long closePos = editor->GetCurrentPosition();
    if (closePos != wxNOT_FOUND) {
        if (!editor->GetSelection().IsEmpty()) {
            // If the selection text is placed under the cursor,
            // keep it selected, else, unselect the text
            // and place the caret to be under cursor
            int selStart = editor->GetSelectionStart();
            int selEnd   = editor->GetSelectionEnd();
            if (closePos < selStart || closePos > selEnd) {
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
    DoBuildMarginMenu(&menu, editor);
    editor->GetSTC()->PopupMenu(&menu);
}

void PHPEditorContextMenu::OnContextMenu(wxCommandEvent& e)
{
    IEditor *editor = dynamic_cast<IEditor*>(e.GetEventObject());
    if(editor && IsPHPFile(editor)) {
        // get the position
        DoContextMenu(editor, e);
        return;
    }
    e.Skip();
}

void PHPEditorContextMenu::OnMarginContextMenu(wxCommandEvent& e)
{
    IEditor *editor = dynamic_cast<IEditor*>(e.GetEventObject());
    if(editor && IsPHPFile(editor)) {
        // get the position
        DoMarginContextMenu(editor);
        return;
    }
    e.Skip();
}

void PHPEditorContextMenu::OnContextOpenDocument(wxCommandEvent &event)
{
    wxUnusedVar(event);
}

void PHPEditorContextMenu::DoUncomment()
{
    GET_EDITOR_SCI_VOID();

    int caret_pos = sci->GetCurrentPos();

    // the style is defined once and the comment / uncomment behavior is constant per operation
    int style = sci->GetStyleAt(caret_pos);
    // if this is not a comment area - return
    if ((style != wxSTC_HPHP_COMMENTLINE) && (style != wxSTC_HPHP_COMMENT))
        return;

    sci->BeginUndoAction();

    if(!RemoveSingleLineComment(sci, caret_pos)) {
        // search for the comment start mark
        if(RemoveTokenFirstIteration(sci, m_start_comment, false, caret_pos))
            RemoveTokenFirstIteration(sci, m_close_comment, true, caret_pos);
    }

    sci->EndUndoAction();
    SET_CARET_POS(caret_pos);
}
bool PHPEditorContextMenu::RemoveTokenFirstIteration(wxStyledTextCtrl *sci, const wxString &token, bool direction, int &caret_pos)
{
    // initialization of start_pos & end_pos
    int line_number = sci->LineFromPosition(sci->GetCurrentPos());
    int start_pos, end_pos;
    if(direction) {
        start_pos = sci->GetCurrentPos()-token.length();
        end_pos = sci->GetLineEndPosition(line_number);
    } else {
        start_pos = sci->PositionFromLine(line_number);
        end_pos = sci->GetCurrentPos()+token.length();
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
                if(caret_pos < token_pos)
                    caret_pos = token_pos;
            } else {
                if(caret_pos > token_pos)
                    caret_pos = token_pos;
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
bool PHPEditorContextMenu::RemoveSingleLineComment(wxStyledTextCtrl *sci, int &caret_pos)
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
void PHPEditorContextMenu::DoCommentSelection()
{
    GET_EDITOR_SCI_VOID();

    int start = sci->GetSelectionStart();
    int end   = sci->GetSelectionEnd();

    if (sci->LineFromPosition(sci->PositionBefore(end)) != sci->LineFromPosition(end)) {
        end = std::max(start, sci->PositionBefore(end));
    }

    int caretPos = sci->GetCurrentPos(); // will be used to place the caret after the commenting staff is done

    // the style is defined once and the comment / uncomment behavior is constant per operation
    int style = sci->GetStyleAt(start);

    sci->BeginUndoAction();
    if ((style != wxSTC_HPHP_COMMENTLINE) && (style != wxSTC_HPHP_COMMENT)) {
        // Note: incase a comment exist inside the line - all the line will be commented
        sci->SetTargetStart(start);
        sci->SetTargetEnd(end);
        int endCommnetPos = sci->SearchInTarget(m_close_comment);
        while(endCommnetPos != wxSTC_INVALID_POSITION) {
            sci->SetSelection(endCommnetPos, sci->PositionAfter(sci->PositionAfter(endCommnetPos)));
            sci->DeleteBack();
            end-=m_close_comment.Length();

            sci->SetTargetStart(endCommnetPos);
            sci->SetTargetEnd(end);
            endCommnetPos = sci->SearchInTarget(m_close_comment);
        }
        sci->InsertText(end, m_close_comment);
        sci->InsertText(start, m_start_comment);
        if(caretPos >= end)
            caretPos += m_close_comment.Length();
        if(caretPos >= start)
            caretPos += 2;

    } else { // handle only the naive case for commented lines - otherwise there are too many scenarios
        if((sci->GetTextRange(start, sci->PositionAfter(sci->PositionAfter(start))) == m_start_comment) &&
           (sci->GetTextRange(end, sci->PositionBefore(sci->PositionBefore(end))) == m_close_comment)) {

            // remove m_start_comment
            sci->SetSelection(sci->PositionBefore(sci->PositionBefore(end)), end);
            sci->DeleteBack();
            if(caretPos >= end)
                caretPos -= 2;

            // remove m_start_comment
            sci->SetSelection(start, sci->PositionAfter(sci->PositionAfter(start)));
            sci->DeleteBack();
            if(caretPos >= start)
                caretPos -= 2;
        }
    }
    sci->EndUndoAction();
    SET_CARET_POS(caretPos);
}
void PHPEditorContextMenu::DoCommentLine()
{
    wxStyledTextCtrl *sci = DoGetActiveScintila();
    if(!sci) return;

    int end   = sci->GetSelectionEnd();
    if (sci->LineFromPosition(sci->PositionBefore(end)) != sci->LineFromPosition(end)) {
        end = std::max(sci->GetSelectionStart(), sci->PositionBefore(end));
    }
    int  line_start   = sci->LineFromPosition(sci->GetSelectionStart());
    int  line_end     = sci->LineFromPosition(end);

    int caret_pos = sci->GetCurrentPos();
    // the style is defined once and the comment / uncomment behavior is constant per operation
    int style = sci->GetStyleAt(caret_pos);

    sci->BeginUndoAction();
    if(line_start < line_end) {
        // comment the exact selection
        for (; line_start <= line_end; line_start++) {

            if ((style != wxSTC_HPHP_COMMENTLINE) && (style != wxSTC_HPHP_COMMENT)) {
                // Note: incase a comment exist inside the line - all the line will be commented
                sci->InsertText(sci->PositionFromLine(line_start), m_comment_line_1);

            } else { // handle only the naive case for commented lines - otherwise there are too many scenarios
                RemoveSingleLineComment(sci, caret_pos);
            }
        }
    } else {
        CommentSingleLine(sci, style, line_start, caret_pos); // in a single line scope all cases are handled
    }

    sci->EndUndoAction();
    SET_CARET_POS(caret_pos);
    //int newPosition = sci->PositionFromLine(line_end+1);
}

void PHPEditorContextMenu::CommentSingleLine(wxStyledTextCtrl *sci, int style, int line_number, int &caret_pos)
{
    if ((style != wxSTC_HPHP_COMMENTLINE) && (style != wxSTC_HPHP_COMMENT)) {
        // Note: incase a comment exist inside the line - all the line will be commented
        sci->InsertText(sci->PositionFromLine(line_number), m_comment_line_1);
        caret_pos += m_comment_line_1.Length();
        return;
    }

    // uncomment line
    // there are 3 options:
    // 1. the current line is commented with '//'
    // 2. the current line is commented with '#'
    // 3. the current line is commented with '/* */'

    // Handle cases:	1. the current line is commented with '//'
    //  				2. the current line is commented with '#'
    if(RemoveSingleLineComment(sci, caret_pos))
        return;

    // Handle case: 3. the current line is commented with '/* */'

    // search for '/*'
    sci->SetTargetStart(sci->PositionFromLine(line_number));
    sci->SetTargetEnd(sci->GetCurrentPos());
    int startCommentPos = sci->SearchInTarget(m_start_comment);
    if(startCommentPos != wxSTC_INVALID_POSITION) {
        int closeCommentPos = sci->FindText(sci->GetCurrentPos(), sci->GetLineEndPosition(line_number), m_close_comment);
        if(closeCommentPos != wxSTC_INVALID_POSITION) {
            // verify the current position in not above the close comment mark
            if(caret_pos >= closeCommentPos)
                caret_pos -= m_start_comment.Length();
            // entering this scope means that both the comment start & end marks are in the current line
            // both should be removed
            RemoveComment(sci, closeCommentPos, m_close_comment);
            caret_pos -= RemoveComment(sci, startCommentPos, m_start_comment);
        } else {
            // only the start comment mark is in this line:
            // remove it & add a start comment mark in the next line
            caret_pos -= RemoveComment(sci, startCommentPos, m_start_comment);
            sci->InsertText(sci->PositionFromLine(line_number+1), m_start_comment);
        }
    } else {
        int closeCommentPos = sci->FindText(sci->GetCurrentPos(), sci->GetLineEndPosition(line_number), m_close_comment);
        if(closeCommentPos != wxSTC_INVALID_POSITION) {
            // verify the current position in not above the close comment mark
            if(caret_pos >= closeCommentPos)
                caret_pos -= m_start_comment.Length();
            // entering this scope means that this is the comment last line.
            // remove the close comment mark & add it to the above line
            RemoveComment(sci, closeCommentPos, m_close_comment);
            sci->InsertText(sci->GetLineEndPosition(line_number-1), m_close_comment);
            caret_pos += m_close_comment.Length();
        } else {
            // entering this scope means that none of the start comment mark and the close comment mark are in this line
            // add a close comment mark in the above line & add an open comment line in the line below
            sci->InsertText(sci->GetLineEndPosition(line_number-1), m_close_comment);
            caret_pos += m_close_comment.Length();
            sci->InsertText(sci->PositionFromLine(line_number+1), m_start_comment);
        }

    }
}
int PHPEditorContextMenu::RemoveComment(wxStyledTextCtrl *sci, int posFrom, const wxString &value)
{
    sci->SetAnchor(posFrom);
    int posTo = posFrom;
    for(int i=0; i<(int)value.Length(); i++)
        posTo = sci->PositionAfter(posTo);

    sci->SetSelection(posFrom, posTo);
    sci->DeleteBack();
    return posTo - posFrom;
}

void PHPEditorContextMenu::OnPopupClicked(wxCommandEvent& event)
{
    IEditor *editor = m_manager->GetActiveEditor();
    if ( editor ) {
        switch(event.GetId()) {
        case wxID_COMMENT_LINE:
        case wxID_UNCOMMENT_LINE:
            DoCommentLine();
            break;
        case wxID_COMMENT_SELECTION:
        case wxID_UNCOMMENT_SELECTION:
            DoCommentSelection();
            break;
        case wxID_UNCOMMENT:
            DoUncomment();
            break;
        case wxID_OPEN_PHP_FILE:
            DoOpenPHPFile();
            break;
        case wxID_GOTO_DEFINITION:
            DoGotoDefinition();
            break;
            
        case wxID_FIND_REFERENCES:
            // DoFindReferences();
            break;
        case wxID_COPY:
        case wxID_CUT:
        case wxID_PASTE:
        case wxID_SELECTALL:
        case wxID_DELETE:
        case wxID_UNDO:
        case wxID_REDO:
            m_manager->ProcessEditEvent(event, editor);
            break;
            
//            if ( editor->GetSTC()->CanUndo() ) 
//                editor->GetSTC()->Undo();
//            break;
//            if ( editor->GetSTC()->CanRedo() ) 
//                editor->GetSTC()->Redo();
//            break;
        default:
            event.Skip();
            break;
        }
    }
}

void PHPEditorContextMenu::DoNotifyCommonCommand(int cmdId)
{
    // Let codelite do the default action for the cmdId
    wxStyledTextCtrl *sci = dynamic_cast<wxStyledTextCtrl*>(m_manager->GetActiveEditor());
    if(sci) {
        wxCommandEvent eventClose(wxEVT_COMMAND_MENU_SELECTED, cmdId);
        eventClose.SetEventObject(sci);
        wxTheApp->GetTopWindow()->GetEventHandler()->ProcessEvent(eventClose);
    }
}
wxStyledTextCtrl* PHPEditorContextMenu::DoGetActiveScintila()
{
    IEditor *editor = m_manager->GetActiveEditor();
    if(editor) {
        return editor->GetSTC();
    }
    return NULL;
}

void PHPEditorContextMenu::OnInsertDoxyComment(wxCommandEvent& e)
{
    IEditor *editor = m_manager->GetActiveEditor();
    if ( editor ) {
        PHPEntry entry = PHPCodeCompletion::Instance()->GetPHPEntryUnderTheAtPos(editor, editor->GetCurrentPosition());
        if ( entry.isOk() ) {
            wxString comment = entry.FormatDoxygenComment();
            comment = editor->FormatTextKeepIndent(comment, editor->GetCurrentPosition());
            
            int curline = editor->GetCurrentLine();
            // insert the comment above the current line
            int pos = editor->PosFromLine(curline);
            editor->InsertText(pos, comment);
        }
    }
}

void PHPEditorContextMenu::OnGenerateSettersGetters(wxCommandEvent& e)
{
    // CHeck the current context
    IEditor *editor = m_manager->GetActiveEditor();
    if ( editor ) {
        wxString textToAdd;
        PHPSetterGetterEntry::Vec_t setters = PHPRefactoring::Get().GetSetters( editor );
        PHPSetterGetterEntry::Vec_t getters = PHPRefactoring::Get().GetGetters( editor );
        
        for(size_t i=0; i<setters.size(); ++i) {
            textToAdd << setters.at(i).GetSetter() << "\n";
        }
        for(size_t i=0; i<getters.size(); ++i) {
            textToAdd << getters.at(i).GetGetter() << "\n";
        }
        
        if ( !textToAdd.IsEmpty() ) {
            textToAdd = editor->FormatTextKeepIndent( textToAdd, editor->GetCurrentPosition(), Format_Text_Save_Empty_Lines );
            editor->InsertText(editor->GetCurrentPosition(), textToAdd);
        }
    }
}

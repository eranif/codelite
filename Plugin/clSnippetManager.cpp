#include "clSnippetManager.hpp"

#include <algorithm>

clSnippetManager::clSnippetManager() {}

clSnippetManager::~clSnippetManager() {}

clSnippetManager& clSnippetManager::Get()
{
    static clSnippetManager manager;
    return manager;
}

void clSnippetManager::Insert(wxStyledTextCtrl* ctrl, const wxString& snippet)
{
    if(!ctrl) {
        return;
    }
    // Support | for indicating where the caret should be placed
    int caret = snippet.Index('|');
    if(caret != wxNOT_FOUND) {
        wxString before = snippet.Mid(0, caret);
        wxString after = snippet.Mid(caret + 1); // Skip the |
        caret = InsertTextSimple(ctrl, before);
        SetCaretAt(ctrl, caret);
        InsertTextSimple(ctrl, after);
    } else {
        // No caret, just insert the text
        InsertTextSimple(ctrl, snippet);
    }
}

int clSnippetManager::InsertTextSimple(wxStyledTextCtrl* ctrl, const wxString& text)
{
    int curpos = wxNOT_FOUND;
    if(!ctrl->HasSelection()) {
        curpos = ctrl->GetCurrentPos();
    } else {
        curpos = std::min(ctrl->GetSelectionStart(), ctrl->GetSelectionEnd());
    }
    if(ctrl->HasSelection()) {
        ctrl->ReplaceSelection(text);
    } else {
        ctrl->InsertText(curpos, text);
    }
    curpos += text.length();
    return curpos;
}

void clSnippetManager::SetCaretAt(wxStyledTextCtrl* ctrl, int pos)
{
    ctrl->SetCurrentPos(pos);
    ctrl->SetSelectionStart(pos);
    ctrl->SetSelectionEnd(pos);
}

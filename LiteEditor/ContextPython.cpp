#include "ContextPython.hpp"

#include "ColoursAndFontsManager.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "lexer_configuration.h"

#include <wx/msgdlg.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

const wxString COMMENT_BLOCK = "\"\"\"";

ContextPython::ContextPython()
    : ContextGeneric("python")
{
}

ContextPython::ContextPython(clEditor* container)
    : ContextGeneric(container, "python")
{
    container->SetWordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"));
    m_completionTriggerStrings.insert(".");
    SetName("python");
    Bind(wxEVT_MENU, &ContextPython::OnCommentSelection, this, XRCID("comment_selection"));
    Bind(wxEVT_MENU, &ContextPython::OnCommentLine, this, XRCID("comment_line"));
    m_eventsBound = true;
}

ContextPython::~ContextPython()
{
    if(m_eventsBound) {
        Unbind(wxEVT_MENU, &ContextPython::OnCommentSelection, this, XRCID("comment_selection"));
        Unbind(wxEVT_MENU, &ContextPython::OnCommentLine, this, XRCID("comment_line"));
    }
}

void ContextPython::ApplySettings()
{
    SetName("python");
    clEditor& rCtrl = GetCtrl();
    LexerConf::Ptr_t lexPtr = ColoursAndFontsManager::Get().GetLexer(GetName());
    if(lexPtr) {
        rCtrl.SetLexer(lexPtr->GetLexerId());
        for(int i = 0; i <= 4; ++i) {
            wxString keyWords = lexPtr->GetKeyWords(i);
            keyWords.Replace(wxT("\n"), wxT(" "));
            keyWords.Replace(wxT("\r"), wxT(" "));
            rCtrl.SetKeyWords(i, keyWords);
        }
    } else {
        rCtrl.SetLexer(wxSTC_LEX_NULL);
    }
    DoApplySettings(lexPtr);
}

ContextBase* ContextPython::NewInstance(clEditor* container) { return new ContextPython(container); }

void ContextPython::OnCommentSelection(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetCtrl().CommentBlockSelection(COMMENT_BLOCK, COMMENT_BLOCK);
}

void ContextPython::OnCommentLine(wxCommandEvent& event)
{
    wxUnusedVar(event);
    GetCtrl().ToggleLineComment("#", wxSTC_P_COMMENTLINE);
}

void ContextPython::AutoIndent(const wxChar& ch)
{
    ContextGeneric::AutoIndent(ch);
    clEditor& rCtrl = GetCtrl();
    int curpos = rCtrl.GetCurrentPos();
    int col = rCtrl.GetColumn(curpos);
    if(ch == '\n') {
        // if in comment block, check if we are on top of a function
        // if we are, prepare a comment block
        int curline = rCtrl.LineFromPos(curpos);
        int nPrevline = curline - 1;
        wxString prevline = rCtrl.GetLine(nPrevline);
        prevline.Trim().Trim(false);
        if(prevline == COMMENT_BLOCK) {
            // Check if the previous line is a function definition
            curline -= 2;
            int nextLinePos = rCtrl.PositionFromLine(curline);
            if(nextLinePos != wxNOT_FOUND) {
                wxString defline = rCtrl.GetLine(curline);
                static wxRegEx reDef("def[ \t]+[a-z0-9_]+[ \t]*(\\(.*?\\))[ \t]*\\:", wxRE_ADVANCED | wxRE_ICASE);
                if(reDef.IsValid() && reDef.Matches(defline)) {
                    int caretPos = curpos;
                    wxString signature = reDef.GetMatch(defline, 1);
                    signature = signature.AfterFirst('(').BeforeLast(')');
                    wxArrayString params = ::wxStringTokenize(signature, ",", wxTOKEN_STRTOK);
                    wxString doc;
                    wxString NEWLINE = rCtrl.GetEolString();
                    wxString indent(rCtrl.GetUseTabs() ? '\t' : ' ', col);
                    if(params.empty()) {
                        doc << indent << NEWLINE;
                        doc << indent << COMMENT_BLOCK;
                    } else {
                        doc << indent << NEWLINE;
                        doc << indent << "Parameters" << NEWLINE;
                        doc << indent << "----------" << NEWLINE;
                        for(wxString& param : params) {
                            param.Trim().Trim(false);
                            doc << indent << param << " : " << NEWLINE;
                            doc << indent << "    "
                                << "Description of " << param << NEWLINE;
                        }
                        doc << indent << NEWLINE;
                        doc << indent << "Returns" << NEWLINE;
                        doc << indent << "-------" << NEWLINE;
                        doc << NEWLINE;
                        doc << indent << COMMENT_BLOCK;
                    }
                    rCtrl.InsertText(curpos, doc);
                    rCtrl.SetCaretAt(caretPos);
                }
            }
        }
    }
}

bool ContextPython::IsComment(int pos) const
{
    int style = GetCtrl().GetStyleAtPos(pos);
    return style == wxSTC_P_COMMENTLINE || style == wxSTC_P_COMMENTBLOCK;
}

bool ContextPython::IsCommentBlock(int pos) const
{
    int style = GetCtrl().GetStyleAtPos(pos);
    return style == wxSTC_P_COMMENTBLOCK;
}

bool ContextPython::IsAtBlockComment() const
{
    int pos = PositionBeforeCurrent();
    return IsCommentBlock(pos);
}

bool ContextPython::IsAtLineComment() const
{
    int pos = PositionBeforeCurrent();
    return IsComment(pos);
}
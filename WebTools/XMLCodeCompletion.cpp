#include "XMLCodeCompletion.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include "XMLBuffer.h"

XMLCodeCompletion::XMLCodeCompletion()
{
}

XMLCodeCompletion::~XMLCodeCompletion()
{
}

void XMLCodeCompletion::CodeComplete(IEditor* editor)
{
    // Perform XML code completion
    wxStyledTextCtrl *ctrl = editor->GetCtrl();
    
    wxChar ch = ctrl->GetCharAt(ctrl->PositionBefore(ctrl->GetCurrentPos()));
    if(ch == '/') {
        // CC was triggered by "</"
        // Read backward until we find the matching open tag
        XMLBuffer buffer(ctrl->GetTextRange(0, ctrl->GetCurrentPos()));
        buffer.Parse();
        if(buffer.InCData() || buffer.InComment()) {
            // dont offer code completion when inside CDATA or COMMENT blocks
            return;
        }
        
        wxString currentScope = buffer.GetCurrentScope();
        if(currentScope.IsEmpty()) return;
        
        wxString textToInsert;
        textToInsert << currentScope << ">";
        ctrl->InsertText(ctrl->GetCurrentPos(), textToInsert);
        editor->SetCaretAt(ctrl->GetCurrentPos() + textToInsert.length());
        
    } else if(ch == '<') {
        
        // CC was triggered by "<"
        // In this case, we simply trigger the word completion
        wxCommandEvent event(wxEVT_MENU, XRCID("word_complete_no_single_insert"));
        wxTheApp->AddPendingEvent(event);
    }
}

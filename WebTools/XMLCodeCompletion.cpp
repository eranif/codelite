#include "XMLCodeCompletion.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>
#include <wx/app.h>

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
        
    } else if(ch == '<') {
        
        // CC was triggered by "<"
        // In this case, we simply trigger the word completion
        wxCommandEvent event(wxEVT_MENU, XRCID("word_complete_no_single_insert"));
        wxTheApp->AddPendingEvent(event);
    }
}

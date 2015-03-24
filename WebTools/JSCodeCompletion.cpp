#include "JSCodeCompletion.h"
#include <wx/stc/stc.h>
#include "JSExpressionParser.h"
#include "JSObject.h"
#include "macros.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include "wxCodeCompletionBoxEntry.h"
#include <algorithm>
#include "JSFunction.h"
#include "wxCodeCompletionBoxManager.h"

JSCodeCompletion::JSCodeCompletion() { m_lookup.Reset(new JSLookUpTable()); }

JSCodeCompletion::~JSCodeCompletion() {}

void JSCodeCompletion::CodeComplete(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    wxStyledTextCtrl* ctrl = editor->GetSTC();
    wxString buffer = ctrl->GetText();
    CHECK_COND_RET(!buffer.IsEmpty());

    JSExpressionParser parser(buffer);
    JSObject::Ptr_t resolved = parser.Resolve(m_lookup, editor->GetFileName().GetFullPath());
    if(!resolved) {
        // poor mans code completion: use the word completion
        // We emulate CC for JS by triggering the word-completion plugin
        wxCommandEvent ccWordComplete(wxEVT_MENU, XRCID("word_complete_no_single_insert"));
        wxTheApp->AddPendingEvent(ccWordComplete);
        return;
    }

    // Code complete succeeded!
    wxCodeCompletionBoxEntry::Vec_t entries;
    const JSObject::Map_t& properties = resolved->GetProperties();
    std::for_each(properties.begin(), properties.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) {
        JSObject::Ptr_t obj = p.second;
        
        wxString displayText;
        displayText << obj->GetName();
        if(obj->As<JSFunction>() && !obj->As<JSFunction>()->GetSignature().IsEmpty()) {
            displayText << obj->As<JSFunction>()->GetSignature();
        }
        
        wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New("");
        entry->SetImgIndex(obj->IsFunction() ? 9 : 6);
        entry->SetComment(obj->GetComment());
        entry->SetText(displayText);
        entries.push_back(entry);
    });
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(ctrl, entries, 0, this);
}

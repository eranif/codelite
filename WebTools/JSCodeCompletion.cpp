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
#include "cl_standard_paths.h"
#include "clZipReader.h"
#include <wx/filename.h>
#include "entry.h"
#include "cl_calltip.h"
#include <wx/log.h>

JSCodeCompletion::JSCodeCompletion()
    : m_thread(NULL)
{
    m_lookup.Reset(new JSLookUpTable());
    // Extract all CC files from PHP.zip into the folder ~/.codelite/webtools/js
    wxFileName jsResources(clStandardPaths::Get().GetDataDir(), "javascript.zip");
    if(jsResources.Exists()) {

        clZipReader zipReader(jsResources);
        wxFileName targetDir(clStandardPaths::Get().GetUserDataDir(), "");
        targetDir.AppendDir("webtools");
        targetDir.AppendDir("js");

        targetDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        zipReader.Extract("*", targetDir.GetPath());

        m_thread = new JSParserThread(this);
        m_thread->Start();
        m_thread->ParseFiles(targetDir.GetPath());
    }
}

JSCodeCompletion::~JSCodeCompletion()
{
    if(m_thread) {
        m_thread->Stop();
        wxDELETE(m_thread);
    }
    m_lookup.Reset(NULL);
}

void JSCodeCompletion::CodeComplete(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    wxStyledTextCtrl* ctrl = editor->GetSTC();

    // work until the current buffer
    wxString buffer = ctrl->GetTextRange(0, ctrl->GetCurrentPos());
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
    wxString wordCompleteFilter = parser.GetWordCompleteFilter().Lower();
    wxCodeCompletionBoxEntry::Vec_t entries;
    JSObject::Map_t properties;
    if(parser.IsWordComplete()) {
        // use the resolved object properties
        properties = m_lookup->GetObjectProperties(resolved);

        // add the visible variables
        JSObject::Map_t locals = m_lookup->GetVisibleVariables();
        properties.insert(locals.begin(), locals.end());
    } else if(parser.IsFunctionTipComplete()) {
        if(!resolved->IsFunction()) return;

        // Convert the function arguments into TagEntryPtrVector_t
        TagEntryPtrVector_t tags;
        TagEntryPtr tag(new TagEntry());
        tag->SetName(resolved->GetName());
        tag->SetKind("function");
        tag->SetFlags(TagEntry::Tag_No_Signature_Format);
        tag->SetSignature(resolved->As<JSFunction>()->GetSignature());
        tags.push_back(tag);

        clCallTipPtr callTip(new clCallTip(tags));
        editor->ShowCalltip(callTip);
        return;

    } else {
        properties = m_lookup->GetObjectProperties(resolved);
    }

    std::for_each(properties.begin(), properties.end(), [&](const std::pair<wxString, JSObject::Ptr_t>& p) {
        JSObject::Ptr_t obj = p.second;

        wxString displayText;
        displayText << obj->GetName();
        wxString displayTextLower = displayText.Lower();
        if(obj->IsFunction() && !obj->IsClass() && obj->As<JSFunction>() &&
           !obj->As<JSFunction>()->GetSignature().IsEmpty()) {
            displayText << obj->As<JSFunction>()->GetSignature();
        }

        // If word complete is requsted, suggest only words that starts with the filter
        // if no filter is provided, display all entries
        if(parser.IsWordComplete() && !wordCompleteFilter.IsEmpty() &&
           displayTextLower.StartsWith(wordCompleteFilter)) {
            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New("");
            entry->SetImgIndex(GetImgIndex(obj));
            entry->SetComment(obj->GetComment());
            entry->SetText(displayText);
            entries.push_back(entry);

        } else {
            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New("");
            entry->SetImgIndex(GetImgIndex(obj));
            entry->SetComment(obj->GetComment());
            entry->SetText(displayText);
            entries.push_back(entry);
        }
    });
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(ctrl, entries, 0, this);
}

void JSCodeCompletion::PraserThreadCompleted(JSParserThread::Reply* reply)
{
    if(reply && reply->lookup) {
        JSLookUpTable::Ptr_t other(reply->lookup);
        m_lookup->CopyClassTable(other);
        // Fill the lookup with the common global objects
        m_lookup->PopulateWithGlobals();
    }
    wxLogMessage("JavaScript: parsing of Core API completed. %d files parsed (time elapsed: %ld seconds)",
                 reply->num_files,
                 reply->total_time);
    wxDELETE(reply);
}

int JSCodeCompletion::GetImgIndex(JSObject::Ptr_t obj)
{
    if(obj->IsClass())
        return 0;
    else if(obj->IsFunction())
        return 9;
    return 6; // Variable
}

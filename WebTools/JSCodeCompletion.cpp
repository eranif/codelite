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
}

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

void JSCodeCompletion::PraserThreadCompleted(JSParserThread::Reply* reply)
{
    if(reply && reply->lookup) {
        m_lookup->GetObjects().insert(reply->lookup->GetObjects().begin(), reply->lookup->GetObjects().end());
        reply->lookup->Clear();
    }
    wxDELETE(reply);
}

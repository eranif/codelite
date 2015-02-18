#include "wxCodeCompletionBoxManager.h"
#include <wx/app.h>
#include "globals.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "cl_command_event.h"
#include "codelite_events.h"
#include "imanager.h"
#include "file_logger.h"
#include <wx/app.h>

wxCodeCompletionBoxManager::wxCodeCompletionBoxManager()
    : m_box(NULL)
    , m_stc(NULL)
{
    if(wxTheApp) {
        wxTheApp->Bind(wxEVT_STC_MODIFIED, &wxCodeCompletionBoxManager::OnStcModified, this);
        wxTheApp->Bind(wxEVT_STC_CHARADDED, &wxCodeCompletionBoxManager::OnStcCharAdded, this);
    }
}

wxCodeCompletionBoxManager::~wxCodeCompletionBoxManager()
{
    if(wxTheApp) {
        wxTheApp->Unbind(wxEVT_STC_MODIFIED, &wxCodeCompletionBoxManager::OnStcModified, this);
        wxTheApp->Unbind(wxEVT_STC_CHARADDED, &wxCodeCompletionBoxManager::OnStcCharAdded, this);
    }
}

wxCodeCompletionBoxManager& wxCodeCompletionBoxManager::Get()
{
    static wxCodeCompletionBoxManager manager;
    return manager;
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const TagEntryPtrVector_t& tags,
                                                   size_t flags,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    CHECK_PTR_RET(ctrl);
    CHECK_COND_RET(!tags.empty());

    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetFlags(flags);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxTags, tags);
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   size_t flags,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    CHECK_PTR_RET(ctrl);
    CHECK_COND_RET(!entries.empty());

    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetFlags(flags);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxEntries, entries);
}

void wxCodeCompletionBoxManager::DestroyCCBox()
{
    if(m_box) {
        if(m_box->IsShown()) m_box->Hide();
        m_box->Destroy();
    }

    m_box = NULL;
    m_stc = NULL;
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   const wxCodeCompletionBox::BmpVec_t& bitmaps,
                                                   size_t flags,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    CHECK_PTR_RET(ctrl);
    CHECK_COND_RET(!entries.empty());

    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetBitmaps(bitmaps);
    m_box->SetFlags(flags);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxEntries, entries);
}

void wxCodeCompletionBoxManager::DestroyCurrent()
{
    DestroyCCBox();
}

void wxCodeCompletionBoxManager::InsertSelection(const wxString& selection)
{
    IManager* manager = ::clGetManager();
    IEditor* editor = manager->GetActiveEditor();
    if(editor) {
        wxStyledTextCtrl* ctrl = editor->GetSTC();
        // Default behviour: remove the partial text from teh editor and replace it
        // with the selection
        int start = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
        int end = ctrl->GetCurrentPos();
        ctrl->SetSelection(start, end);

        wxString entryText = selection;
        if(entryText.Find("(") != wxNOT_FOUND) {
            // a function like
            wxString textToInsert = entryText.BeforeFirst('(');

            // Build the function signature
            wxString funcSig = entryText.AfterFirst('(');
            funcSig = funcSig.BeforeLast(')');
            funcSig.Trim().Trim(false);

            CL_DEBUG("Inserting selection: %s", textToInsert);
            CL_DEBUG("Signature is: %s", funcSig);

            textToInsert << "()";
            ctrl->ReplaceSelection(textToInsert);
            if(!funcSig.IsEmpty()) {
                // Place the caret between the parenthesis
                int caretPos = start + textToInsert.Len() - 1;
                ctrl->SetCurrentPos(caretPos);
                ctrl->SetSelection(caretPos, caretPos);

                // trigger a code complete for function calltip.
                // We do this by simply mimicing the user action of going to the menubar:
                // Edit->Display Function Calltip
                wxCommandEvent event(wxEVT_MENU, XRCID("function_call_tip"));
                wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
            }
        } else {
            ctrl->ReplaceSelection(entryText);
        }
    }
}

void wxCodeCompletionBoxManager::OnStcCharAdded(wxStyledTextEvent& event)
{
    event.Skip();
    if(m_box && m_box->IsShown() && m_box->m_stc == event.GetEventObject()) {
        m_box->StcCharAdded(event);
    }
}

void wxCodeCompletionBoxManager::OnStcModified(wxStyledTextEvent& event)
{
    event.Skip();
    if(m_box && m_box->IsShown() && m_box->m_stc == event.GetEventObject()) {
        m_box->StcModified(event);
    }
}

void wxCodeCompletionBoxManager::DoShowCCBoxEntries(const wxCodeCompletionBoxEntry::Vec_t& entries)
{
    if(m_box && m_stc) {
        m_box->ShowCompletionBox(m_stc, entries);
    }
}

void wxCodeCompletionBoxManager::DoShowCCBoxTags(const TagEntryPtrVector_t& tags)
{
    if(m_box && m_stc) {
        m_box->ShowCompletionBox(m_stc, tags);
    }
}

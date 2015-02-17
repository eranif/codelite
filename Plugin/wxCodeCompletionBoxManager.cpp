#include "wxCodeCompletionBoxManager.h"
#include <wx/app.h>
#include "globals.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "cl_command_event.h"
#include "codelite_events.h"
#include "imanager.h"
#include "file_logger.h"

wxCodeCompletionBoxManager::wxCodeCompletionBoxManager()
    : m_box(NULL)
{
}

wxCodeCompletionBoxManager::~wxCodeCompletionBoxManager() {}

wxCodeCompletionBoxManager& wxCodeCompletionBoxManager::Get()
{
    static wxCodeCompletionBoxManager manager;
    return manager;
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const TagEntryPtrVector_t& tags,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    if(tags.empty()) return;
    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->ShowCompletionBox(ctrl, tags);
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    if(entries.empty()) return;
    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->ShowCompletionBox(ctrl, entries);
}

void wxCodeCompletionBoxManager::WindowDestroyed() { m_box = NULL; }

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   const wxCodeCompletionBox::BmpVec_t& bitmaps,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    if(entries.empty()) return;
    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetBitmaps(bitmaps);
    m_box->ShowCompletionBox(ctrl, entries);
}

void wxCodeCompletionBoxManager::DestroyCurrent()
{
    if(m_box) {
        m_box->Hide();
        m_box->Destroy();
        m_box = NULL;
    }
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
        int end = ctrl->WordEndPosition(ctrl->GetCurrentPos(), true);
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

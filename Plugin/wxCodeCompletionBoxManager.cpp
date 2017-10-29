#include "wxCodeCompletionBoxManager.h"

#include <algorithm>
#include <wx/app.h>
#include "globals.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "cl_command_event.h"
#include "codelite_events.h"
#include "imanager.h"
#include "file_logger.h"
#include <wx/app.h>
#include "event_notifier.h"

struct wxCodeCompletionClientData : public wxClientData {
    bool m_connected;
    wxCodeCompletionClientData()
        : m_connected(false)
    {
    }
    virtual ~wxCodeCompletionClientData() {}
};

wxCodeCompletionBoxManager::wxCodeCompletionBoxManager()
    : m_box(NULL)
    , m_stc(NULL)
{

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &wxCodeCompletionBoxManager::OnDismissBox, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &wxCodeCompletionBoxManager::OnDismissBox, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSING, &wxCodeCompletionBoxManager::OnDismissBox, this);

    wxTheApp->Bind(wxEVT_STC_MODIFIED, &wxCodeCompletionBoxManager::OnStcModified, this);
    wxTheApp->Bind(wxEVT_STC_CHARADDED, &wxCodeCompletionBoxManager::OnStcCharAdded, this);
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &wxCodeCompletionBoxManager::OnAppActivate, this);
}

wxCodeCompletionBoxManager::~wxCodeCompletionBoxManager()
{
    DestroyCurrent();
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &wxCodeCompletionBoxManager::OnDismissBox, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &wxCodeCompletionBoxManager::OnDismissBox, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSING, &wxCodeCompletionBoxManager::OnDismissBox, this);

    wxTheApp->Unbind(wxEVT_STC_MODIFIED, &wxCodeCompletionBoxManager::OnStcModified, this);
    wxTheApp->Unbind(wxEVT_STC_CHARADDED, &wxCodeCompletionBoxManager::OnStcCharAdded, this);
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &wxCodeCompletionBoxManager::OnAppActivate, this);
}

static wxCodeCompletionBoxManager* manager = NULL;
wxCodeCompletionBoxManager& wxCodeCompletionBoxManager::Get()
{
    if(!manager) {
        manager = new wxCodeCompletionBoxManager();
    }
    return *manager;
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const TagEntryPtrVector_t& tags,
                                                   size_t flags,
                                                   int startPos,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    CHECK_PTR_RET(ctrl);
    CHECK_COND_RET(!tags.empty());

    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetFlags(flags);
    m_box->SetStartPos(startPos);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxTags, tags);
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   size_t flags,
                                                   int startPos,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    CHECK_PTR_RET(ctrl);
    CHECK_COND_RET(!entries.empty());

    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetFlags(flags);
    m_box->SetStartPos(startPos);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxEntries, entries);
}

void wxCodeCompletionBoxManager::DestroyCCBox()
{
    if(m_box) {
        if(m_box->IsShown()) {
            m_box->Hide();
        }
        m_box->Destroy();
    }
    m_box = NULL;
    m_stc = NULL;
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   const wxCodeCompletionBox::BmpVec_t& bitmaps,
                                                   size_t flags,
                                                   int startPos,
                                                   wxEvtHandler* eventObject)
{
    DestroyCurrent();
    CHECK_PTR_RET(ctrl);
    //CHECK_COND_RET(!entries.empty());

    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetBitmaps(bitmaps);
    m_box->SetFlags(flags);
    m_box->SetStartPos(startPos);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxEntries, entries);
}

void wxCodeCompletionBoxManager::DestroyCurrent() { DestroyCCBox(); }

void wxCodeCompletionBoxManager::InsertSelection(const wxString& selection)
{
    IManager* manager = ::clGetManager();
    IEditor* editor = manager->GetActiveEditor();
    if(editor) {
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        bool addParens(false);
        int start = wxNOT_FOUND, end = wxNOT_FOUND;
        std::vector<std::pair<int, int> > ranges;
        if(ctrl->GetSelections() > 1) {
            for(int i = 0; i < ctrl->GetSelections(); ++i) {
                int nStart = ctrl->WordStartPosition(ctrl->GetSelectionNCaret(i), true);
                int nEnd = ctrl->GetSelectionNCaret(i);
                ranges.push_back(std::make_pair(nStart, nEnd));
            }
            std::sort(ranges.begin(), ranges.end(), [&](const std::pair<int, int>& e1, const std::pair<int, int>& e2) {
                return e1.first < e2.first;
            });
        } else {
            // Default behviour: remove the partial text from teh editor and replace it
            // with the selection
            start = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
            end = ctrl->GetCurrentPos();
            ctrl->SetSelection(start, end);
            if(ctrl->GetCharAt(end) != '(') {
                addParens = true;
            }
        }

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

            // Check if already have an open paren, don't add another
            if(addParens) {
                textToInsert << "()";
            }

            if(!ranges.empty()) {
                // Multiple carets
                int offset = 0;
                for(size_t i = 0; i < ranges.size(); ++i) {
                    int from = ranges.at(i).first;
                    int to = ranges.at(i).second;
                    from += offset;
                    to += offset;
                    // Once we enter that text into the editor, it will change the original
                    // offsets (in most cases the entered text is larger than that typed text)
                    offset += textToInsert.length() - (to - from);
                    ctrl->Replace(from, to, textToInsert);
                    ctrl->SetSelectionNStart(i, from + textToInsert.length());
                    ctrl->SetSelectionNEnd(i, from + textToInsert.length());
                }
            } else {
                ctrl->ReplaceSelection(textToInsert);
                if(!funcSig.IsEmpty()) {

                    // Place the caret between the parenthesis
                    int caretPos(wxNOT_FOUND);
                    if(addParens) {
                        caretPos = start + textToInsert.length() - 1;
                    } else {
                        // Move the caret one char to the right
                        caretPos = start + textToInsert.length() + 1;
                    }
                    ctrl->SetCurrentPos(caretPos);
                    ctrl->SetSelection(caretPos, caretPos);

                    // trigger a code complete for function calltip.
                    // We do this by simply mimicing the user action of going to the menubar:
                    // Edit->Display Function Calltip
                    wxCommandEvent event(wxEVT_MENU, XRCID("function_call_tip"));
                    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(event);
                }
            }
        } else {
            if(!ranges.empty()) {
                // Multiple carets
                int offset = 0;
                for(size_t i = 0; i < ranges.size(); ++i) {
                    int from = ranges.at(i).first;
                    int to = ranges.at(i).second;
                    from += offset;
                    to += offset;
                    // Once we enter that text into the editor, it will change the original
                    // offsets (in most cases the entered text is larger than that typed text)
                    offset += entryText.length() - (to - from);
                    ctrl->Replace(from, to, entryText);
                    ctrl->SetSelectionNStart(i, from + entryText.length());
                    ctrl->SetSelectionNEnd(i, from + entryText.length());
                }
            } else {
                // Default
                ctrl->ReplaceSelection(entryText);
            }
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
        DoConnectStcEventHandlers(m_stc);
    }
}

void wxCodeCompletionBoxManager::DoShowCCBoxTags(const TagEntryPtrVector_t& tags)
{
    if(m_box && m_stc) {
        m_box->ShowCompletionBox(m_stc, tags);
        DoConnectStcEventHandlers(m_stc);
    }
}

void wxCodeCompletionBoxManager::OnStcKeyDown(wxKeyEvent& event)
{
    if(m_box && m_stc && m_box->IsShown() && event.GetEventObject() == m_stc) {
        m_box->StcKeyDown(event);
    } else {
        event.Skip();
    }
}

void wxCodeCompletionBoxManager::OnStcLeftDown(wxMouseEvent& event)
{
    if(m_box && m_stc && m_box->IsShown() && event.GetEventObject() == m_stc) {
        m_box->StcLeftDown(event);
    } else {
        event.Skip();
    }
}

void wxCodeCompletionBoxManager::OnDismissBox(wxCommandEvent& event)
{
    event.Skip();
    DestroyCCBox();
}

void wxCodeCompletionBoxManager::OnAppActivate(wxActivateEvent& event)
{
    event.Skip();
    CallAfter(&wxCodeCompletionBox::DoDestroy);
}

void wxCodeCompletionBoxManager::Free()
{
    // Destroy the manager, Unbinding all events
    if(manager) {
        delete manager;
        manager = NULL;
    }
}

void wxCodeCompletionBoxManager::DoConnectStcEventHandlers(wxStyledTextCtrl* ctrl)
{
    if(ctrl) {
        // Connect the event handlers only once. We ensure that we do that only once by attaching
        // a client data to the stc control with a single member "m_connected"
        wxCodeCompletionClientData* cd = dynamic_cast<wxCodeCompletionClientData*>(ctrl->GetClientObject());
        if(cd && cd->m_connected) {
            return;
        }
        cd = new wxCodeCompletionClientData();
        cd->m_connected = true;
        ctrl->SetClientObject(cd);
        ctrl->Bind(wxEVT_KEY_DOWN, &wxCodeCompletionBoxManager::OnStcKeyDown, this);
        ctrl->Bind(wxEVT_LEFT_DOWN, &wxCodeCompletionBoxManager::OnStcLeftDown, this);
    }
}

void wxCodeCompletionBoxManager::InsertSelectionTemplateFunction(const wxString& selection)
{
    IManager* manager = ::clGetManager();
    IEditor* editor = manager->GetActiveEditor();
    if(editor) {
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        // Default behviour: remove the partial text from teh editor and replace it
        // with the selection
        int start = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
        int end = ctrl->GetCurrentPos();
        ctrl->SetSelection(start, end);

        wxString entryText = selection;
        if(entryText.Find("(") != wxNOT_FOUND) {
            // a function like
            wxString textToInsert = entryText.BeforeFirst('(');
            textToInsert << "<>()";
            ctrl->ReplaceSelection(textToInsert);
            // Place the caret between the angle brackets
            int caretPos = start + textToInsert.Len() - 3;
            ctrl->SetCurrentPos(caretPos);
            ctrl->SetSelection(caretPos, caretPos);

        } else {
            ctrl->ReplaceSelection(entryText);
        }
    }
}

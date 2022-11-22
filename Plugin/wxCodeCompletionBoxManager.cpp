#include "wxCodeCompletionBoxManager.h"

#include "addincludefiledlg.h"
#include "clSnippetManager.hpp"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"

#include <algorithm>
#include <wx/app.h>
#include <wx/stc/stc.h>

namespace
{
std::unordered_set<wxChar> delimiters = { ':', '@', '.',  '!', ' ', '\t', '.', '\\', '+', '*', '-',
                                          '<', '>', '[',  ']', '(', ')',  '{', '}',  '=', '%', '#',
                                          '^', '&', '\'', '"', '/', '|',  ',', '~',  ';', '`' };

int GetWordStartPos(wxStyledTextCtrl* ctrl, int from, bool includeNekudotaiim)
{
    int lineNumber = ctrl->LineFromPosition(from);
    int lineStartPos = ctrl->PositionFromLine(lineNumber);
    if(from == lineStartPos) {
        return from;
    }

    // when we start the loop from is ALWAYS  greater than lineStartPos
    while(from >= lineStartPos) {
        --from;
        if(from < lineStartPos) {
            ++from;
            break;
        }
        wxChar ch = ctrl->GetCharAt(from);
        if(delimiters.count(ch)) {
            from++;
            break;
        }
    }
    return from;
}

struct wxCodeCompletionClientData : public wxClientData {
    bool m_connected;
    wxCodeCompletionClientData()
        : m_connected(false)
    {
    }
    virtual ~wxCodeCompletionClientData() {}
};
} // namespace

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

namespace
{
const wxString valid_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_.>@$:";

bool CheckCtrlPosition(wxStyledTextCtrl* ctrl, int startPos)
{
    // if the box is about to be shown on a different line or near a whitespace
    // return false
    int start_pos = startPos == wxNOT_FOUND ? ctrl->GetCurrentPos() : startPos;
    if(start_pos <= 0 || start_pos > ctrl->GetLastPosition()) {
        return false;
    }
    int prev_char = ctrl->GetCharAt(ctrl->PositionBefore(start_pos));
    wxString prev_char_str;
    prev_char_str << (char)prev_char;
    if(prev_char_str.find_first_of(valid_chars) != std::string::npos) {
        return true;
    }
    return false;
}
} // namespace

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const LSP::CompletionItem::Vec_t& completions, size_t flags,
                                                   int startPos, wxEvtHandler* eventObject)
{
    if(!ctrl || completions.empty() || !CheckCtrlPosition(ctrl, startPos)) {
        DestroyCurrent();
        return;
    }

    if(m_box) {
        m_box->Reset(eventObject);
    } else {
        m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    }
    m_box->SetFlags(flags);
    m_box->SetStartPos(startPos);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxLSPItems, completions);
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrVector_t& tags,
                                                   size_t flags, int startPos, wxEvtHandler* eventObject)
{
    if(!ctrl || tags.empty() || !CheckCtrlPosition(ctrl, startPos)) {
        DestroyCurrent();
        return;
    }

    if(m_box) {
        m_box->Reset(eventObject);
    } else {
        m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    }

    m_box->SetFlags(flags);
    m_box->SetStartPos(startPos);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxTags, tags);
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries, size_t flags,
                                                   int startPos, wxEvtHandler* eventObject)
{
    if(!ctrl || entries.empty() || !CheckCtrlPosition(ctrl, startPos)) {
        DestroyCurrent();
        return;
    }

    if(m_box) {
        m_box->Reset(eventObject);
    } else {
        m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    }

    m_box->SetFlags(flags);
    m_box->SetStartPos(startPos);
    m_stc = ctrl;
    CallAfter(&wxCodeCompletionBoxManager::DoShowCCBoxEntries, entries);
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   const wxCodeCompletionBox::BmpVec_t& bitmaps, size_t flags,
                                                   int startPos, wxEvtHandler* eventObject)
{
    if(!ctrl || entries.empty() || !CheckCtrlPosition(ctrl, startPos)) {
        DestroyCurrent();
        return;
    }

    if(m_box) {
        m_box->Reset(eventObject);
    } else {
        m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    }
    m_box->SetBitmaps(bitmaps);
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

void wxCodeCompletionBoxManager::DestroyCurrent()
{
    clDEBUG() << "DestroyCurrent() is called" << endl;
    DestroyCCBox();
}

void wxCodeCompletionBoxManager::InsertSelection(wxCodeCompletionBoxEntry::Ptr_t match, bool userTriggered)
{
    IManager* manager = ::clGetManager();
    IEditor* editor = manager->GetActiveEditor();
    wxString entryText = match->GetInsertText();
    wxString entryLabel = match->GetText();
    if(!editor) {
        return;
    }

    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    bool addParens(false);
    bool moveCaretRight = false;
    bool moveCaretLeft = false;
    int start = wxNOT_FOUND, end = wxNOT_FOUND;
    std::vector<std::pair<int, int>> ranges;
    if(ctrl->GetSelections() > 1) {
        for(int i = 0; i < ctrl->GetSelections(); ++i) {
            int nStart = GetWordStartPos(ctrl, ctrl->GetSelectionNCaret(i), entryText.Contains(":"));
            int nEnd = ctrl->GetSelectionNCaret(i);
            ranges.push_back(std::make_pair(nStart, nEnd));
        }
        std::sort(ranges.begin(), ranges.end(),
                  [&](const std::pair<int, int>& e1, const std::pair<int, int>& e2) { return e1.first < e2.first; });
    } else {
        // Default behviour: remove the partial text from the editor and replace it
        // with the selection
        start = GetWordStartPos(ctrl, ctrl->GetCurrentPos(), entryText.Contains(":"));
        end = ctrl->GetCurrentPos();
        ctrl->SetSelection(start, end);
        wxChar endChar = ctrl->GetCharAt(end);
        if((ctrl->GetCharAt(end) != '(')) {
            addParens = true;
            moveCaretLeft = true;
        } else if(endChar == '(') {
            moveCaretRight = true;
        }
    }
    if(match->IsSnippet()) {
        clSnippetManager::Get().Insert(editor->GetCtrl(), match->GetInsertText());

    } else if(match->IsFunction()) {
        // If the user triggerd this insertion, invoke the function auto complete
        if(userTriggered) {
            // a function like
            wxString textToInsert = entryText.BeforeFirst('(');

            // Build the function signature
            wxString funcSig = match->GetSignature();
            bool userProvidedSignature = (match->GetText().Find("(") != wxNOT_FOUND);
            clDEBUG() << "Inserting selection:" << textToInsert;
            clDEBUG() << "Signature is:" << funcSig;

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
                if(!userProvidedSignature || (!funcSig.IsEmpty() && (funcSig != "()"))) {

                    // Place the caret between the parenthesis
                    int caretPos(wxNOT_FOUND);
                    if(moveCaretLeft) {
                        caretPos = start + textToInsert.length() - 1;
                    } else if(moveCaretRight) {
                        // Move the caret one char to the right
                        caretPos = start + textToInsert.length() + 1;
                    } else {
                        caretPos = start + textToInsert.length();
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
            ctrl->SetSelectionStart(ctrl->GetCurrentPos());
            ctrl->SetSelectionEnd(ctrl->GetCurrentPos());
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

    if(match->IsTriggerInclude()) {
        // The comment for this entry is the name of the include file that we want to add
        // Tell CodeLite to launch the AddIncludeFile dialog
        CallAfter(&wxCodeCompletionBoxManager::ShowAddIncludeDialog, match->GetComment());
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

void wxCodeCompletionBoxManager::DoShowCCBoxLSPItems(const LSP::CompletionItem::Vec_t& items)
{
    if(m_box && m_stc) {
        m_box->ShowCompletionBox(m_stc, items);
        DoConnectStcEventHandlers(m_stc);
    }
}

void wxCodeCompletionBoxManager::ShowAddIncludeDialog(const wxString& include)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(!editor) {
        return;
    }
    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    AddIncludeFileDlg dlg(EventNotifier::Get()->TopFrame(), include, ctrl->GetText(), 0);
    if(dlg.ShowModal() == wxID_OK) {
        // add the line to the current document
        wxString lineToAdd = dlg.GetLineToAdd();
        int line = dlg.GetLine();

        long pos = ctrl->PositionFromLine(line);
        ctrl->InsertText(pos, lineToAdd + (editor->GetEOL() == wxSTC_EOL_CRLF ? "\r\n" : "\n"));
    }
}

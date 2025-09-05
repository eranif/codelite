#include "WordCompletionDictionary.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include <algorithm>
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include <wx/stc/stc.h>

WordCompletionDictionary::WordCompletionDictionary()
{
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &WordCompletionDictionary::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &WordCompletionDictionary::OnAllEditorsClosed, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &WordCompletionDictionary::OnFileSaved, this);

    m_thread = new WordCompletionThread(this);
    m_thread->Start();
}

WordCompletionDictionary::~WordCompletionDictionary()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &WordCompletionDictionary::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &WordCompletionDictionary::OnAllEditorsClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &WordCompletionDictionary::OnFileSaved, this);

    m_thread->Stop();   // Stop the thread
    wxDELETE(m_thread); // Delete it
}

void WordCompletionDictionary::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();

    // 1) Get a list of all open editors and set the compare it to the current cached files
    //    and delete all "closed" editors
    // 2) Request to cache the newly opened file's words
    IEditor::List_t allEditors;
    wxArrayString openEditors, cachedEditors, closedEditors;
    ::clGetManager()->GetAllEditors(allEditors);

    for (IEditor* editor : allEditors) {
        openEditors.Add(editor->GetFileName().GetFullPath());
    }

    for (const auto& p : m_files) {
        cachedEditors.Add(p.first);
    }

    // std::set_difference requires that both arrays will be sorted
    openEditors.Sort();
    cachedEditors.Sort();

    // Create a new array "closedEditors" which contains list of files that exists
    // in the cachedEditors but not in openEditors list
    std::set_difference(cachedEditors.begin(),
                        cachedEditors.end(),
                        openEditors.begin(),
                        openEditors.end(),
                        std::back_inserter(closedEditors));

    for (const auto& closedEditorName : closedEditors) {
        m_files.erase(closedEditorName);
    }

    // 2: cache the active editor
    DoCacheActiveEditor(false);
}

void WordCompletionDictionary::OnSuggestThread(const WordCompletionThreadReply& reply)
{
    // Remove the current file's dictionary
    std::map<wxString, wxStringSet_t>::iterator iter = m_files.find(reply.filename.GetFullPath());
    if(iter != m_files.end()) m_files.erase(iter);

    // Keep the words
    m_files.insert(std::make_pair(reply.filename.GetFullPath(), reply.suggest));
}

void WordCompletionDictionary::OnAllEditorsClosed(wxCommandEvent& event)
{
    event.Skip();
    m_files.clear();
}

void WordCompletionDictionary::DoCacheActiveEditor(bool overwrite)
{
    // Step 2: cache the active editor (if not already cached)
    IEditor* activeEditor = ::clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(activeEditor);

    if(!overwrite && m_files.count(activeEditor->GetFileName().GetFullPath()))
        return; // we already have this file in the cache
        
    m_files.erase(activeEditor->GetFileName().GetFullPath());
    
    // Insert a dummy entry, so we won't queue this file if not needed
    m_files.insert(std::make_pair(activeEditor->GetFileName().GetFullPath(), wxStringSet_t()));
    
    // Queue this file
    wxStyledTextCtrl* stc = activeEditor->GetCtrl();
    
    // Invoke the thread to parse and suggest words for this file
    WordCompletionThreadRequest* req = new WordCompletionThreadRequest;
    req->buffer = stc->GetText();
    req->filename = activeEditor->GetFileName();
    req->filter = "filter";
    m_thread->Add(req);
}

void WordCompletionDictionary::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    DoCacheActiveEditor(true);
}

wxStringSet_t WordCompletionDictionary::GetWords() const
{
    wxStringSet_t words;
    for (const auto& p : m_files) {
        words.insert(p.second.begin(), p.second.end());
    }
    return words;
}

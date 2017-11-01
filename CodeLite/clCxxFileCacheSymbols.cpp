#include "clCxxFileCacheSymbols.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "file_logger.h"
#include "parse_thread.h"
#include <wx/tokenzr.h>
#include <algorithm>

wxDEFINE_EVENT(wxEVT_CXX_SYMBOLS_CACHE_UPDATED, clCommandEvent);

clCxxFileCacheSymbols::clCxxFileCacheSymbols()
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clCxxFileCacheSymbols::OnFileSave, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
    Bind(wxEVT_PARSE_THREAD_SOURCE_TAGS, &clCxxFileCacheSymbols::OnPraseCompleted, this);
}

clCxxFileCacheSymbols::~clCxxFileCacheSymbols()
{
    Clear();
    Unbind(wxEVT_PARSE_THREAD_SOURCE_TAGS, &clCxxFileCacheSymbols::OnPraseCompleted, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clCxxFileCacheSymbols::OnFileSave, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
}

bool clCxxFileCacheSymbols::Contains(const wxFileName& filename) { return Contains(filename.GetFullPath()); }

bool clCxxFileCacheSymbols::Contains(const wxString& filename)
{
    wxCriticalSectionLocker locker(m_cs);
    return (m_cache.count(filename) > 0);
}

void clCxxFileCacheSymbols::Clear()
{
    wxCriticalSectionLocker locker(m_cs);
    m_cache.clear();
    clDEBUG1() << "Symbols cache cleared" << clEndl;
}

void clCxxFileCacheSymbols::Update(const wxFileName& filename, const TagEntryPtrVector_t& tags)
{
    wxCriticalSectionLocker locker(m_cs);
    if(m_cache.count(filename.GetFullPath())) {
        m_cache.erase(filename.GetFullPath());
    }
    m_cache[filename.GetFullPath()] = tags;
    clDEBUG1() << "Updating Symbols cache for file:" << filename << clEndl;
}

void clCxxFileCacheSymbols::Delete(const wxFileName& filename)
{
    wxCriticalSectionLocker locker(m_cs);
    m_cache.erase(filename.GetFullPath());
    clDEBUG1() << "Deleting Symbols cache for file:" << filename << clEndl;
}

bool clCxxFileCacheSymbols::Find(const wxFileName& filename, TagEntryPtrVector_t& tags, size_t flags)
{
    {
        wxCriticalSectionLocker locker(m_cs);
        if(m_cache.count(filename.GetFullPath())) {
            tags = m_cache[filename.GetFullPath()];
            clDEBUG1() << "Symbols fetched from cache for file:" << filename << clEndl;
        } else {
            clDEBUG1() << "Symbols for file:" << filename << "do not exist in the cache" << clEndl;
            return false;
        }
    }

    // Apply the flags
    if(flags & kFunctions) {
        // Return only functions
        TagEntryPtrVector_t functions;
        std::for_each(tags.begin(), tags.end(), [&](TagEntryPtr tag) {
            if(tag->IsMethod()) {
                functions.push_back(tag);
            }
        });
        tags.swap(functions);
    }
    return true;
}

void clCxxFileCacheSymbols::OnFileSave(clCommandEvent& e)
{
    e.Skip();
    Delete(e.GetFileName());
}

void clCxxFileCacheSymbols::OnWorkspaceAction(wxCommandEvent& e)
{
    e.Skip();
    Clear();
}

void clCxxFileCacheSymbols::RequestSymbols(const wxFileName& filename)
{
    ParseRequest* req = new ParseRequest(this);
    req->setFile(filename.GetFullPath());
    req->setType(ParseRequest::PR_SOURCE_TO_TAGS);
    ParseThreadST::Get()->Add(req);
}

void clCxxFileCacheSymbols::OnPraseCompleted(clCommandEvent& e)
{
    TagEntryPtrVector_t tags;
    // Convert the string into array of tags
    wxArrayString lines = ::wxStringTokenize(e.GetString(), "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.size(); ++i) {
        wxString& strLine = lines.Item(i);
        strLine.Trim().Trim(false);
        if(strLine.IsEmpty()) continue;

        TagEntryPtr tag(new TagEntry());
        tag->FromLine(strLine);
        tags.push_back(tag);
    }
    // Update the cache
    Update(e.GetFileName(), tags);

    clCommandEvent event(wxEVT_CXX_SYMBOLS_CACHE_UPDATED);
    event.SetFileName(e.GetFileName());
    EventNotifier::Get()->AddPendingEvent(event);
}

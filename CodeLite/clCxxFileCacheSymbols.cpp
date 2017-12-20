#include "clCxxFileCacheSymbols.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "parse_thread.h"
#include "worker_thread.h"
#include <algorithm>
#include <wx/tokenzr.h>

wxDEFINE_EVENT(wxEVT_CXX_SYMBOLS_CACHE_UPDATED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CXX_SYMBOLS_CACHE_INVALIDATED, clCommandEvent);

class SourceToTagsThread : public wxThread
{
    clCxxFileCacheSymbols* m_cache;
    wxMessageQueue<wxString> m_queue;

public:
    SourceToTagsThread(clCxxFileCacheSymbols* cache)
        : wxThread(wxTHREAD_JOINABLE)
        , m_cache(cache)
    {
    }

    virtual ~SourceToTagsThread() { clDEBUG1() << "SourceToTagsThread helper thread is going down" << clEndl; }
    void Stop()
    {
        // Notify the thread to exit and
        // wait for it
        if(IsAlive()) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);

        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
    void Start()
    {
        clDEBUG1() << "SourceToTagsThread helper thread started" << clEndl;
        Create();
        SetPriority(wxPRIORITY_DEFAULT);
        Run();
    }

    virtual void* Entry()
    {
        while(true) {
            wxString filename;
            if(m_queue.ReceiveTimeout(50, filename) == wxMSGQUEUE_NO_ERROR) {
                if(TagsManagerST::Get()->IsBinaryFile(filename)) { continue; }

                wxString strTags;
                TagsManagerST::Get()->SourceToTags(filename, strTags);

                // Fire the event
                m_cache->CallAfter(&clCxxFileCacheSymbols::OnPraseCompleted, filename, strTags);
            }
            if(TestDestroy()) break;
        }
        return NULL;
    }

    void ParseFile(const wxString& filename) { m_queue.Post(filename); }
};

clCxxFileCacheSymbols::clCxxFileCacheSymbols()
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clCxxFileCacheSymbols::OnFileSave, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
    m_helperThread = new SourceToTagsThread(this);
    m_helperThread->Start();
}

clCxxFileCacheSymbols::~clCxxFileCacheSymbols()
{
    Clear();
    m_helperThread->Stop();
    wxDELETE(m_helperThread);

    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clCxxFileCacheSymbols::OnFileSave, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
}

bool clCxxFileCacheSymbols::Contains(const wxFileName& filename) const { return Contains(filename.GetFullPath()); }

bool clCxxFileCacheSymbols::Contains(const wxString& filename) const
{
    // wxCriticalSectionLocker locker(m_cs);
    return (m_cache.count(filename) > 0);
}

void clCxxFileCacheSymbols::Clear()
{
    // wxCriticalSectionLocker locker(m_cs);
    m_cache.clear();
    m_pendingFiles.clear();
    clDEBUG1() << "Symbols cache cleared" << clEndl;
}

void clCxxFileCacheSymbols::Update(const wxFileName& filename, const TagEntryPtrVector_t& tags)
{
    // wxCriticalSectionLocker locker(m_cs);
    if(m_cache.count(filename.GetFullPath())) { m_cache.erase(filename.GetFullPath()); }
    m_cache[filename.GetFullPath()] = tags;
    clDEBUG1() << "Updating Symbols cache for file:" << filename << clEndl;
}

void clCxxFileCacheSymbols::Delete(const wxFileName& filename)
{
    // wxCriticalSectionLocker locker(m_cs);
    m_cache.erase(filename.GetFullPath());
    clDEBUG1() << "Deleting Symbols cache for file:" << filename << clEndl;

    // Notify that the symbols for this file were invalidated
    clCommandEvent event(wxEVT_CXX_SYMBOLS_CACHE_INVALIDATED);
    event.SetFileName(filename.GetFullPath());
    EventNotifier::Get()->AddPendingEvent(event);
}

bool clCxxFileCacheSymbols::Find(const wxFileName& filename, TagEntryPtrVector_t& tags, size_t flags)
{
    {
        // wxCriticalSectionLocker locker(m_cs);
        if(m_cache.count(filename.GetFullPath())) {
            tags = m_cache[filename.GetFullPath()];
            clDEBUG1() << "Symbols fetched from cache for file:" << filename << clEndl;
        } else {
            clDEBUG1() << "Symbols for file:" << filename << "do not exist in the cache" << clEndl;
            return false;
        }
    }

    // Apply the flags
    if((flags & kFunctions) && !tags.empty()) {
        // Remove all non functions
        TagEntryPtrVector_t::iterator iter = tags.begin();
        for(; iter != tags.end();) {
            if(!(*iter)->IsMethod()) {
                iter = tags.erase(iter);
            } else {
                ++iter;
            }
        }
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
    // If we are waiting for this file parse to complete, dont ask for it again
    if(m_pendingFiles.count(filename.GetFullPath())) {
        clDEBUG1() << "Ignoring duplicate parse request for file:" << filename.GetFullPath() << clEndl;
        return;
    }

    m_helperThread->ParseFile(filename.GetFullPath());
    m_pendingFiles.insert(filename.GetFullPath());
}

void clCxxFileCacheSymbols::OnPraseCompleted(const wxString& filename, const wxString& strTags)
{
    TagEntryPtrVector_t tags;
    // Convert the string into array of tags
    wxArrayString lines = ::wxStringTokenize(strTags, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.size(); ++i) {
        wxString& strLine = lines.Item(i);
        strLine.Trim().Trim(false);
        if(strLine.IsEmpty()) continue;

        TagEntryPtr tag(new TagEntry());
        tag->FromLine(strLine);
        tags.push_back(tag);
    }
    // Update the cache
    Update(filename, tags);
    clCommandEvent event(wxEVT_CXX_SYMBOLS_CACHE_UPDATED);
    event.SetFileName(filename);
    EventNotifier::Get()->AddPendingEvent(event);

    m_pendingFiles.erase(filename);
}

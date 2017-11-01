#include "clCxxFileCacheSymbols.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "file_logger.h"

clCxxFileCacheSymbols::clCxxFileCacheSymbols()
{
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clCxxFileCacheSymbols::OnFileSave, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clCxxFileCacheSymbols::OnWorkspaceAction, this);
}

clCxxFileCacheSymbols::~clCxxFileCacheSymbols()
{
    Clear();
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

bool clCxxFileCacheSymbols::Find(const wxFileName& filename, TagEntryPtrVector_t& tags)
{
    wxCriticalSectionLocker locker(m_cs);
    if(m_cache.count(filename.GetFullPath())) {
        tags = m_cache[filename.GetFullPath()];
        clDEBUG1() << "Symbols fetched from cache for file:" << filename << clEndl;
        return true;
    }
    clDEBUG1() << "Symbols for file:" << filename << "do not exist in the cache" << clEndl;
    return false;
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

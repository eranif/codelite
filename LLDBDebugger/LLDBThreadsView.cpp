#include "LLDBThreadsView.h"
#include "LLDBPlugin.h"

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

LLDBThreadsView::LLDBThreadsView(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBThreadsViewBase(parent)
    , m_plugin(plugin)
    , m_selectedThread(wxNOT_FOUND)
{
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_RUNNING, &LLDBThreadsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STOPPED, &LLDBThreadsView::OnLLDBStopped, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_EXITED,  &LLDBThreadsView::OnLLDBExited,  this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STARTED, &LLDBThreadsView::OnLLDBStarted, this);
    
    m_model.reset( new ThreadsModel( m_dvListCtrlThreads ) );
    m_dvListCtrlThreads->AssociateModel( m_model.get() );
}

LLDBThreadsView::~LLDBThreadsView()
{
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_RUNNING, &LLDBThreadsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STOPPED, &LLDBThreadsView::OnLLDBStopped, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_EXITED,  &LLDBThreadsView::OnLLDBExited,  this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STARTED, &LLDBThreadsView::OnLLDBStarted, this);
}

void LLDBThreadsView::OnItemActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET( event.GetItem() );
    LLDBThreadViewClientData *cd = reinterpret_cast<LLDBThreadViewClientData*>(m_dvListCtrlThreads->GetItemData( event.GetItem() ) );
    if ( cd && !cd->GetThread().IsActive() ) {
        m_plugin->GetLLDB()->SelectThread( cd->GetThread().GetId() );
    }
}

void LLDBThreadsView::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
}

void LLDBThreadsView::OnLLDBRunning(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
}

void LLDBThreadsView::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
}

void LLDBThreadsView::OnLLDBStopped(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
    // Update the thread view
    const LLDBThread::Vect_t &threads = event.GetThreads();
    for(size_t i=0; i<threads.size(); ++i) {
        const LLDBThread& thr = threads.at(i);
        if ( thr.IsActive() ) {
            m_selectedThread = i;
        }
        wxVector<wxVariant> cols;
        cols.push_back( thr.GetId() == wxNOT_FOUND ? wxString() : wxString() << thr.GetId() );
        cols.push_back( thr.GetStopReasonString() );
        cols.push_back( thr.GetFunc() );
        cols.push_back( thr.GetFile() );
        cols.push_back( thr.GetLine() == wxNOT_FOUND ? wxString() : wxString() << thr.GetLine() );
        m_dvListCtrlThreads->AppendItem( cols, (wxUIntPtr) new LLDBThreadViewClientData(thr) );
    }
}

void LLDBThreadsView::DoCleanup()
{
    for(int i=0; i<m_dvListCtrlThreads->GetItemCount(); ++i) {
        LLDBThreadViewClientData *cd = reinterpret_cast<LLDBThreadViewClientData*>(m_dvListCtrlThreads->GetItemData( m_dvListCtrlThreads->RowToItem(i) ) );
        wxDELETE( cd );
    }
    m_dvListCtrlThreads->DeleteAllItems();
    m_selectedThread = wxNOT_FOUND;
}

#include "LLDBCallStack.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBConnector.h"
#include <wx/wupdlock.h>
#include "macros.h"

LLDBCallStackPane::LLDBCallStackPane(wxWindow* parent, LLDBConnector* connector)
    : LLDBCallStackBase(parent)
    , m_connector(connector)
{
    m_connector->Bind(wxEVT_LLDB_STOPPED, &LLDBCallStackPane::OnBacktrace, this);
    m_connector->Bind(wxEVT_LLDB_RUNNING, &LLDBCallStackPane::OnRunning, this);
}

LLDBCallStackPane::~LLDBCallStackPane()
{
    m_connector->Unbind(wxEVT_LLDB_STOPPED, &LLDBCallStackPane::OnBacktrace, this);
    m_connector->Unbind(wxEVT_LLDB_RUNNING, &LLDBCallStackPane::OnRunning, this);
}

void LLDBCallStackPane::OnBacktrace(LLDBEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker( m_dvListCtrlBacktrace );
    m_dvListCtrlBacktrace->DeleteAllItems();
    const LLDBBacktrace& bt = event.GetBacktrace();
    const LLDBBacktrace::EntryVec_t& entries =  bt.GetCallstack();
    for(size_t i=0; i<entries.size(); ++i) {
        wxVector<wxVariant> cols;
        const LLDBBacktrace::Entry& entry = entries.at(i);
        cols.push_back( wxString::Format("%d", entry.id) );
        cols.push_back( entry.functionName );
        cols.push_back( entry.filename );
        cols.push_back( wxString::Format("%d", (int)(entry.line + 1) ));
        m_dvListCtrlBacktrace->AppendItem(cols);
    }
}

void LLDBCallStackPane::OnRunning(LLDBEvent& event)
{
    event.Skip();
    m_dvListCtrlBacktrace->DeleteAllItems();
}

void LLDBCallStackPane::OnItemActivated(wxDataViewEvent& event)
{
    // Activate the selected frame
    CHECK_ITEM_RET(event.GetItem());
    int rowNum = m_dvListCtrlBacktrace->ItemToRow( event.GetItem() );
    m_connector->SelectFrame( rowNum );
}

#include "LLDBCallStack.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBConnector.h"
#include <wx/wupdlock.h>
#include "macros.h"


LLDBCallStackPane::LLDBCallStackPane(wxWindow* parent, LLDBConnector* connector)
    : LLDBCallStackBase(parent)
    , m_connector(connector)
    , m_selectedFrame(0)
{
    m_connector->Bind(wxEVT_LLDB_STOPPED, &LLDBCallStackPane::OnBacktrace, this);
    m_connector->Bind(wxEVT_LLDB_RUNNING, &LLDBCallStackPane::OnRunning, this);

    m_model.reset( new CallstackModel(this, m_dvListCtrlBacktrace) );
    m_dvListCtrlBacktrace->AssociateModel( m_model.get() );
}

LLDBCallStackPane::~LLDBCallStackPane()
{
    m_connector->Unbind(wxEVT_LLDB_STOPPED, &LLDBCallStackPane::OnBacktrace, this);
    m_connector->Unbind(wxEVT_LLDB_RUNNING, &LLDBCallStackPane::OnRunning, this);
}

void LLDBCallStackPane::OnBacktrace(LLDBEvent& event)
{
    event.Skip();

    SetSelectedFrame(0); // Clear the selected frame
    wxWindowUpdateLocker locker( m_dvListCtrlBacktrace );
    m_dvListCtrlBacktrace->DeleteAllItems();
    const LLDBBacktrace& bt = event.GetBacktrace();
    SetSelectedFrame( bt.GetSelectedFrameId() );

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
    SetSelectedFrame(0);
}

void LLDBCallStackPane::OnItemActivated(wxDataViewEvent& event)
{
    // Activate the selected frame
    CHECK_ITEM_RET(event.GetItem());
    int rowNum = m_dvListCtrlBacktrace->ItemToRow( event.GetItem() );
    m_connector->SelectFrame( rowNum );
}

bool CallstackModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
{
    int row = m_view->ItemToRow(item);
    if ( row == m_ctrl->GetSelectedFrame() ) {
        attr.SetBold(true);
        return true;
    }
    return false;
}

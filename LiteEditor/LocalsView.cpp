#include "LocalsView.h"
#include "event_notifier.h"
#include "debuggermanager.h"
#include "debuggerobserver.h"
#include "manager.h"
#include "localstable.h"

#define CHECK_DEBUGGER(d) if ( !(d && ManagerST::Get()->DbgCanInteract()) ) return;

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------

class LocalItemData : public wxClientData
{
    wxString m_name;
    wxString m_gdbId;
public:
    LocalItemData(const wxString &name, const wxString &gdb_id) : m_name(name), m_gdbId(gdb_id) {}
    virtual ~LocalItemData() {}


    void SetGdbId(const wxString& gdbId) {
        this->m_gdbId = gdbId;
    }
    void SetName(const wxString& name) {
        this->m_name = name;
    }
    const wxString& GetGdbId() const {
        return m_gdbId;
    }
    const wxString& GetName() const {
        return m_name;
    }
};

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------

LocalsView::LocalsView(wxWindow* parent)
    : LocalsViewBase(parent)
{
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_QUERY_LOCALS,      wxCommandEventHandler(LocalsView::OnLocals), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_UPDATE_VIEWS,      wxCommandEventHandler(LocalsView::OnRefreshView), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_VAROBJECT_CREATED, wxCommandEventHandler(LocalsView::OnCreateVariableObject), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_LIST_CHILDREN, wxCommandEventHandler(LocalsView::OnListChildren), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_VAROBJ_EVALUATED, wxCommandEventHandler(LocalsView::OnEvaluate), NULL, this);

}

LocalsView::~LocalsView()
{
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_QUERY_LOCALS, wxCommandEventHandler(LocalsView::OnLocals), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_UPDATE_VIEWS, wxCommandEventHandler(LocalsView::OnRefreshView), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_VAROBJECT_CREATED, wxCommandEventHandler(LocalsView::OnCreateVariableObject), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_LIST_CHILDREN, wxCommandEventHandler(LocalsView::OnListChildren), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_VAROBJ_EVALUATED, wxCommandEventHandler(LocalsView::OnEvaluate), NULL, this);
}

void LocalsView::OnLocals(wxCommandEvent& e)
{
    e.Skip();
    DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>( e.GetClientObject() );
    if ( !ded ) {
        return;
    }

    LocalVariables locals = ded->m_locals;
    std::set<wxString> excludeList;

    // Step 1:
    // Delete old locals

    // Loop over the dataview, and get the name of all of the top level items
    // if an item has a gdbId (which is stored as the LocalItemData) add this
    // name to the exclude list
    wxDataViewItemArray toplevels;
    wxDataViewItemArray items;
    m_dataviewModel->GetChildren(wxDataViewItem(0), toplevels);
    for(size_t i=0; i<toplevels.GetCount(); ++i) {
        LocalItemData *cd = dynamic_cast<LocalItemData*>( m_dataviewModel->GetClientObject( toplevels.Item(i) )) ;
        if( cd ) {
            excludeList.insert( cd->GetName() );
        } else {
            items.Add(toplevels.Item(i));
        }
    }

    m_dataviewModel->DeleteItems(wxDataViewItem(0), items);
    m_locals.clear();

    // Step 2:
    // Add the new locals
    for(size_t i=0; i<locals.size(); ++i) {

        // Add back only the locals which were not expanded by the user
        if ( excludeList.count(locals.at(i).name) ) {
            continue;
        }

        wxVector<wxVariant> cols;
        cols.push_back( locals.at(i).name  );
        cols.push_back( locals.at(i).value );
        cols.push_back( locals.at(i).type  );
        wxDataViewItem item = m_dataviewModel->AppendItem(wxDataViewItem(0), cols);
        // append a dummy item
        cols.clear();
        cols.push_back( "<dummy>"  );
        cols.push_back( "<dummy>" );
        cols.push_back( "<dummy>"  );
        m_dataviewModel->AppendItem(item, cols);
        m_locals.insert( item );
    }
}

void LocalsView::OnRefreshView(wxCommandEvent& e)
{
    e.Skip();
    if ( IsShown() ) {
        // To refresh this view:
        // 1) Update all variable objects
        // 2) Delete all locals with no variable objects associated to them
        // 3) Call to QueryLocals()

        // Delete all non variable objects
        std::set<wxDataViewItem>::iterator iter = m_locals.begin();
        for(; iter != m_locals.end(); ++iter) {
            m_dataviewModel->DeleteItem( *iter );
        }
        m_locals.clear();

        IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
        CHECK_DEBUGGER(dbgr);

        // refresh all variable objects
        std::map<wxString, wxDataViewItem>::iterator it = m_gdbIdToDVItem.begin();
        for( ; it != m_gdbIdToDVItem.end(); ++it ) {
            dbgr->UpdateVariableObject(it->first, DBG_USERR_LOCALS);
            dbgr->EvaluateVariableObject(it->first, DBG_UR_EVALVARIABLEOBJ);
        }
        dbgr->QueryLocals();
    }
}

void LocalsView::OnItemExpanding(wxDataViewEvent& event)
{
    IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    CHECK_DEBUGGER(dbgr);

    if ( m_locals.count( event.GetItem() ) ) {
        event.Veto();
        m_locals.erase( event.GetItem() );

        // this is a dummy item
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData( event.GetItem() );
        dbgr->CreateVariableObject( cols.at(0).GetString(), false, DBG_USERR_LOCALS );

        // delete this item dummy node
        wxDataViewItemArray children;
        if ( m_dataviewModel->GetChildren( event.GetItem(), children ) ) {
            m_dataviewModel->DeleteItems(event.GetItem(), children );
        }

        // place the item in the list of items that are waiting for variable object creation
        m_waitingForCreateObj.insert( std::make_pair(cols.at(0).GetString(), event.GetItem()) );

    } else {
        // this is valid item with a gdbId
        event.Skip();
        LocalItemData* data = dynamic_cast<LocalItemData*>(m_dataviewModel->GetClientObject( event.GetItem()) );
        if ( data ) {
            dbgr->UpdateVariableObject(data->GetGdbId(), DBG_USERR_LOCALS);
            dbgr->ListChildren(data->GetGdbId(), QUERY_LOCALS_CHILDS);
        }
    }
}

void LocalsView::OnCreateVariableObject(wxCommandEvent& e)
{
    e.Skip();
    // variable object creation is done
    DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>( e.GetClientObject() );
    if ( !ded ) {
        return;
    }

    if ( !m_waitingForCreateObj.count(ded->m_expression) )
        return;

    wxDataViewItem item = m_waitingForCreateObj.find(ded->m_expression)->second;
    m_waitingForCreateObj.erase(ded->m_expression);

    if ( !ded->m_evaluated.IsEmpty() ) {
        // we got a value here, update it
        wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData( item );
        cols.at(1) = ded->m_evaluated;
        m_dataviewModel->UpdateItem( item, cols );
    }

    // keep the gdbid for this item
    m_dataviewModel->SetClientObject(item, new LocalItemData(ded->m_expression, ded->m_variableObject.gdbId));

    IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    CHECK_DEBUGGER(dbgr);

    // Place this item in the "list children"
    m_gdbIdToDVItem.insert(std::make_pair(ded->m_variableObject.gdbId, item));

    // and finally call "ListChildren" for this variable object
    dbgr->UpdateVariableObject(ded->m_variableObject.gdbId, DBG_USERR_LOCALS);
    dbgr->ListChildren(ded->m_variableObject.gdbId, QUERY_LOCALS_CHILDS);
}

void LocalsView::OnListChildren(wxCommandEvent& e)
{
    e.Skip();
    DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>( e.GetClientObject() );
    if ( !ded ) {
        return;
    }

    wxString gdbId = ded->m_expression;
    if ( !m_gdbIdToDVItem.count( gdbId ) )
        return;

    if ( ded->m_varObjChildren.empty() )
        return;

    IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
    CHECK_DEBUGGER(dbgr);

    wxDataViewItem item = m_gdbIdToDVItem.find(gdbId)->second;
    if ( ded->m_userReason == QUERY_LOCALS_CHILDS ) {
        // delete old children
        wxDataViewItemArray children;
        if ( m_dataviewModel->GetChildren(item, children) ) {
            m_dataviewModel->DeleteItems(item, children);
        }
    }
    
    for(size_t i=0; i<ded->m_varObjChildren.size(); ++i) {
        const VariableObjChild &child = ded->m_varObjChildren.at(i);
        if( child.isAFake ) {
            // not really a node...
            // ask for information about this node children
            dbgr->ListChildren(child.gdbId, QUERY_LOCALS_CHILDS_FAKE_NODE);
            m_gdbIdToDVItem.insert(std::make_pair(child.gdbId, item));

        } else {
            wxVector<wxVariant> cols;
            cols.push_back( child.varName );
            cols.push_back( child.value );
            cols.push_back( child.type );

            wxDataViewItem childItem = m_dataviewModel->AppendItem(item, cols);
            m_gdbIdToDVItem.insert(std::make_pair(child.gdbId, childItem));

            dbgr->EvaluateVariableObject(child.gdbId, DBG_UR_EVALVARIABLEOBJ);
        }
    }
}

void LocalsView::OnEvaluate(wxCommandEvent& e)
{
    e.Skip();
    e.Skip();
    DebuggerEventData *ded = dynamic_cast<DebuggerEventData*>( e.GetClientObject() );
    if ( !ded ) {
        return;
    }

    wxString gdbId = ded->m_expression;
    wxString value = ded->m_evaluated;

    if ( !m_gdbIdToDVItem.count( gdbId ) )
        return;

    wxDataViewItem item = m_gdbIdToDVItem.find(gdbId)->second;
    wxVector<wxVariant> cols = m_dataviewModel->GetItemColumnsData( item );
    if ( cols.size() == 3 ) {
        cols.at(1) = value;
        m_dataviewModel->UpdateItem( item, cols );
    }
}

void LocalsView::Clear()
{
    m_gdbIdToDVItem.clear();
    //m_waitingForEvaluate.clear();
    m_locals.clear();
    m_waitingForCreateObj.clear();
    m_dataviewModel->Clear();
}

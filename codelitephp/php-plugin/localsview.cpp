#include "localsview.h"
#include <lexer_configuration.h>
#include <editor_config.h>
#include <event_notifier.h>
#include <file_logger.h>
#include "XDebugManager.h"

LocalsView::LocalsView(wxWindow* parent)
    : LocalsViewBase(parent)
{
    LexerConf::Ptr_t lex =  EditorConfigST::Get()->GetLexer("html");
    if ( lex ) {
        m_dataview->SetFont( lex->GetFontForSyle(wxSTC_HPHP_DEFAULT) );
    }
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_LOCALS_UPDATED, &LocalsView::OnLocalsUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_ENDED, &LocalsView::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTED, &LocalsView::OnXDebugSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_PROPERTY_GET, &LocalsView::OnProperytGet, this);
}

LocalsView::~LocalsView()
{
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_LOCALS_UPDATED, &LocalsView::OnLocalsUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_ENDED, &LocalsView::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTED, &LocalsView::OnXDebugSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_PROPERTY_GET, &LocalsView::OnProperytGet, this);
}

void LocalsView::OnLocalCollapsed(wxDataViewEvent& event)
{
    /// We keep track on expanded and collapsed items by their fullname
    CHECK_ITEM_RET(event.GetItem());
    wxStringClientData* cd = dynamic_cast<wxStringClientData*>(m_dataviewModel->GetClientObject(event.GetItem()));
    CHECK_PTR_RET(cd);
    if( m_localsExpandedItemsFullname.count( cd->GetData() ) ) {
        m_localsExpandedItemsFullname.erase( cd->GetData() );
    }
}

void LocalsView::OnXDebugSessionEnded(XDebugEvent &e)
{
    e.Skip();
    CL_DEBUG("LocalsView::OnXDebugSessionEnded(): Debug sessions started - cleaning all locals view");
    // Clear the variables view
    m_dataviewModel->Clear();
    m_localsExpandedItemsFullname.clear();
    m_localsExpandedItems.Clear();
    m_waitingExpand.clear();
}

void LocalsView::OnLocalExpanded(wxDataViewEvent& event)
{
    /// We keep track on expanded and collapsed items by their fullname
    CHECK_ITEM_RET(event.GetItem());
    wxStringClientData* cd = dynamic_cast<wxStringClientData*>(m_dataviewModel->GetClientObject(event.GetItem()));
    CHECK_PTR_RET(cd);
    m_localsExpandedItemsFullname.insert( cd->GetData() );
}

void LocalsView::OnLocalsUpdated(XDebugEvent& e)
{
    e.Skip();
    CL_DEBUG("Inside OnLocalsUpdated");

    m_dataviewModel->Clear();
    m_localsExpandedItems.Clear();

    const XVariable::List_t& vars = e.GetVariables();
    AppendVariablesToTree( wxDataViewItem(NULL), vars );

    // Expand the items that were expanded before the view refresh
    for(size_t i=0; i<m_localsExpandedItems.GetCount(); ++i) {
        // Ensure it is visible
        m_dataview->EnsureVisible( m_localsExpandedItems.Item(i) );
        // Ensure its expanded
        m_dataview->Expand( m_localsExpandedItems.Item(i) );
    }
    m_localsExpandedItems.Clear();
}

void LocalsView::AppendVariablesToTree(const wxDataViewItem& parent, const XVariable::List_t& children)
{
    XVariable::List_t::const_iterator iter = children.begin();
    for(; iter != children.end(); ++iter ) {
        const XVariable& var = *iter;
        wxVector<wxVariant> cols;
        cols.push_back( var.name );
        cols.push_back( var.type );
        cols.push_back( var.classname );
        cols.push_back( var.value );
        wxDataViewItem item = m_dataviewModel->AppendItem(parent, cols, new wxStringClientData(var.fullname) );

        if ( var.GetCreateFakeNode() ) {
            // create dummy node in the tree view so we can expand it later
            cols.clear();
            cols.push_back( "<dummy>" );
            cols.push_back( wxString() );
            cols.push_back( wxString() );
            cols.push_back( wxString() );
            m_dataviewModel->AppendItem(item, cols, new wxStringClientData(var.fullname) );

        } else if ( var.HasChildren() ) {
            AppendVariablesToTree( item, var.children );
            if ( m_localsExpandedItemsFullname.count(var.fullname) ) {
                // this item should be expanded
                m_localsExpandedItems.Add( item );
            }
        }
    }
}

void LocalsView::OnXDebugSessionStarted(XDebugEvent& e)
{
    e.Skip();
    CL_DEBUG("LocalsView::OnXDebugSessionStarted(): Debug sessions started - cleaning all locals view");
    // Clear the variables view
    m_dataviewModel->Clear();
    m_localsExpandedItemsFullname.clear();
    m_localsExpandedItems.Clear();
    m_waitingExpand.clear();
}

void LocalsView::OnLocalExpanding(wxDataViewEvent& event)
{
    event.Skip();
    CHECK_ITEM_RET( event.GetItem() );
    
    wxDataViewItem item = event.GetItem();
    wxDataViewItemArray children;
    if ( m_dataviewModel->GetChildren( item , children ) && children.GetCount() == 1 ) {
        wxDataViewItem child = children.Item(0);
        wxVariant v;
        m_dataviewModel->GetValue(v, child, 0);
        
        if ( v.GetString() == "<dummy>" ) {
            // a dummy node has been found
            // Delete this node and request from XDebug to expand it
            m_dataviewModel->SetValue( wxString("Loading..."), child, 0 );
            wxString propertyName = DoGetItemClientData( event.GetItem() );
            XDebugManager::Get().SendGetProperty( propertyName );
            m_waitingExpand.insert( std::make_pair(propertyName, item) );
        }
    }
}

wxString LocalsView::DoGetItemClientData(const wxDataViewItem& item) const
{
    wxStringClientData* scd = dynamic_cast<wxStringClientData*>(m_dataviewModel->GetClientObject( item ) );
    if ( scd ) {
        return scd->GetData();
    }
    return wxEmptyString;
}

void LocalsView::OnProperytGet(XDebugEvent& e)
{
    e.Skip();
    // An item was evaluated using property_get
    std::map<wxString, wxDataViewItem>::iterator iter = m_waitingExpand.find( e.GetEvaluted() );
    if ( iter == m_waitingExpand.end() ) {
        return;
    }
    
    wxDataViewItem item = iter->second;
    m_waitingExpand.erase( iter );
    
    // Delete the fake node
    wxDataViewItemArray children;
    m_dataviewModel->GetChildren( item, children );
    if ( !children.IsEmpty() ) {
        m_dataviewModel->DeleteItems( item, children );
    }
    
    XVariable::List_t vars = e.GetVariables();
    if ( vars.empty() ) 
        return;
    
    // Since we got here from property_get, XDebug will reply with the specific property (e.g. $myclass->secondClass)
    // and all its children. Howeverr, $myclass->secondClass already exist in the tree
    // so we are only interested with its children. so we use here vars.begin()->children (vars is always list of size == 1)
    wxASSERT_MSG(vars.size() == 1, "property_get returned list of size != 1");
    XVariable::List_t childs;
    childs = vars.begin()->children;

    if ( !childs.empty() ) {
        AppendVariablesToTree( item, childs );
        m_dataview->Expand( item );
    }
}

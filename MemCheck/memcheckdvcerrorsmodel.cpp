//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "memcheckdvcerrorsmodel.h"
#include <wx/dvrenderers.h>
#include <wx/variant.h>

//////////////////////////////////////////
// Implementation
//////////////////////////////////////////

// -------------------------------------------------
// Help method
// -------------------------------------------------
wxVariant MemCheckDVCErrorsModel::CreateIconTextVariant(const wxString &text, const wxBitmap& bmp)
{
    wxIcon icn;
    icn.CopyFromBitmap( bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}

// -------------------------------------------------
// The model class
// -------------------------------------------------

MemCheckDVCErrorsModel::MemCheckDVCErrorsModel()
    : m_colCount(0)
{
}

MemCheckDVCErrorsModel::~MemCheckDVCErrorsModel()
{
    for(size_t i=0; i<m_data.size(); ++i) {
        wxDELETE(m_data.at(i));
    }
    m_data.clear();
}

unsigned int MemCheckDVCErrorsModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
    if(item.GetID() == NULL) {
        // Root
        for(size_t i=0; i<m_data.size(); ++i) {
            children.Add( wxDataViewItem( m_data.at(i) ) );
        }
        return children.size();
    }

    children.Clear();
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.m_pItem);
    if ( node ) {
        for(size_t i=0; i<node->GetChildren().size(); ++i) {
            children.Add( wxDataViewItem( node->GetChildren().at(i) ) );
        }
    }
    return children.GetCount();
}

unsigned int MemCheckDVCErrorsModel::GetColumnCount() const
{
    return m_colCount;
}

wxString MemCheckDVCErrorsModel::GetColumnType(unsigned int col) const
{
    if ( !m_data.empty() && m_data.at(0)->GetData().size() > col ) {
        return m_data.at(0)->GetData().at(col).GetType();
    }
    return "string";
}

wxDataViewItem MemCheckDVCErrorsModel::GetParent(const wxDataViewItem& item) const
{
    if ( IsEmpty() ) {
        return wxDataViewItem(NULL);
    }
    
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.m_pItem);
    if ( node ) {
        return wxDataViewItem(node->GetParent());
    }
    return wxDataViewItem(NULL);
}

bool MemCheckDVCErrorsModel::IsContainer(const wxDataViewItem& item) const
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.m_pItem);
    if ( node ) {
        return node->IsContainer();
    }
    return false;
}

void MemCheckDVCErrorsModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.m_pItem);
    if ( node && node->GetData().size() > col ) {
        variant = node->GetData().at(col);
    }
}
wxDataViewItem MemCheckDVCErrorsModel::DoAppendItem(const wxDataViewItem& parent, const wxVector<wxVariant>& data, bool isContainer, wxClientData *clientData)
{
    MemCheckDVCErrorsModel_Item* parentNode = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(parent.m_pItem);
    DoChangeItemType(parent, true);
    
    MemCheckDVCErrorsModel_Item* child = new MemCheckDVCErrorsModel_Item();
    child->SetIsContainer(isContainer);
    child->SetClientObject( clientData );
    child->SetData( data );
    if ( parentNode ) {
        parentNode->AddChild( child );

    } else {
        m_data.push_back( child );
    }

    return wxDataViewItem(child);
}

wxDataViewItem MemCheckDVCErrorsModel::DoInsertItem(const wxDataViewItem& insertBeforeMe, const wxVector<wxVariant>& data, bool isContainer, wxClientData *clientData)
{
    MemCheckDVCErrorsModel_Item* child = new MemCheckDVCErrorsModel_Item();
    child->SetIsContainer(isContainer);
    child->SetClientObject( clientData );
    child->SetData( data );

    // find the location where to insert the new item
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(insertBeforeMe.m_pItem);
    if ( !node )
        return wxDataViewItem();

    wxVector<MemCheckDVCErrorsModel_Item*>::iterator where = std::find(m_data.begin(), m_data.end(), node);

    if ( where !=  m_data.end() ) {
        // top level item
        m_data.insert( where, child );

    } else {

        if ( !node->GetParent() )
            return wxDataViewItem();

        child->SetParent(node->GetParent());
        where = std::find(node->GetParent()->GetChildren().begin(), node->GetParent()->GetChildren().end(), node);
        if ( where == node->GetParent()->GetChildren().end() ) {
            node->GetParent()->GetChildren().push_back( child );

        } else {
            node->GetParent()->GetChildren().insert(where, child);

        }
    }

    return wxDataViewItem(child);
}

wxDataViewItem MemCheckDVCErrorsModel::AppendItem(const wxDataViewItem &parent, const wxVector<wxVariant>& data, wxClientData *clientData)
{
    wxDataViewItem ch = DoAppendItem(parent, data, false, clientData);
    ItemAdded(parent, ch);
    return ch;
}

wxDataViewItemArray MemCheckDVCErrorsModel::AppendItems(const wxDataViewItem &parent, const wxVector<wxVector<wxVariant> >& data)
{
    wxDataViewItemArray items;
    for(size_t i=0; i<data.size(); ++i) {
        items.push_back( DoAppendItem(parent, data.at(i), false, NULL) );
    }
    ItemsAdded(parent, items);
    return items;
}

bool MemCheckDVCErrorsModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.m_pItem);
    if ( node && node->GetData().size() > col ) {
        node->GetData().at(col) = variant;
    }
    return true;
}

void MemCheckDVCErrorsModel::DeleteItem(const wxDataViewItem& item)
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.m_pItem);
    if ( node ) {
        
        MemCheckDVCErrorsModel_Item* parent = node->GetParent();
        wxDataViewItem parentItem(parent);
        ItemDeleted(parentItem, item);
        
        // this will also remove it from its model parent children list
        if ( parent == NULL ) {
            // root item, remove it from the roots array
            wxVector<MemCheckDVCErrorsModel_Item*>::iterator where = std::find(m_data.begin(), m_data.end(), node);
            if ( where != m_data.end() ) {
                m_data.erase(where);
            }
        }
        
        // If there are no more children, change the item back to 'normal'
        if ( parent && parent->GetChildren().empty() )
            DoChangeItemType(parentItem, false);
            
        wxDELETE(node);
    }
    
    if ( IsEmpty() )
        Cleared();
}

void MemCheckDVCErrorsModel::DeleteItems(const wxDataViewItem& parent, const wxDataViewItemArray& items)
{
    // sanity
    for(size_t i=0; i<items.GetCount(); ++i) {
        MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(items.Item(i).m_pItem);
        wxUnusedVar(node);
        wxASSERT(node && node->GetParent() == parent.m_pItem);
        DeleteItem(items.Item(i));
    }
}

void MemCheckDVCErrorsModel::Clear()
{
    wxVector<MemCheckDVCErrorsModel_Item*> roots = m_data;
    for (auto item : roots) {
        DeleteItem(wxDataViewItem(item));
    }
    m_data.clear();
    Cleared();
}

bool MemCheckDVCErrorsModel::IsEmpty() const
{
    return m_data.empty();
}

wxClientData* MemCheckDVCErrorsModel::GetClientObject(const wxDataViewItem& item) const
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.GetID());
    if ( node ) {
        return node->GetClientObject();
    }
    return NULL;
}

void MemCheckDVCErrorsModel::SetClientObject(const wxDataViewItem& item, wxClientData *data)
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.GetID());
    if ( node ) {
        node->SetClientObject(data);
    }
}

void MemCheckDVCErrorsModel::UpdateItem(const wxDataViewItem& item, const wxVector<wxVariant>& data)
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.GetID());
    if ( node ) {
        node->SetData( data );
        ItemChanged( item );
    }
}

wxDataViewItem MemCheckDVCErrorsModel::InsertItem(const wxDataViewItem& insertBeforeMe, const wxVector<wxVariant>& data, wxClientData *clientData)
{
    wxDataViewItem ch = DoInsertItem(insertBeforeMe, data, false, clientData);
    if ( ch.IsOk() ) {
        MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(ch.GetID());
        ItemAdded(wxDataViewItem(node->GetParent()), ch);
    }
    return ch;
}

wxVector<wxVariant> MemCheckDVCErrorsModel::GetItemColumnsData(const wxDataViewItem& item) const
{
    if ( !item.IsOk() )
        return wxVector<wxVariant>();

    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.GetID());
    if ( !node ) {
        return wxVector<wxVariant>();
    }
    return node->GetData();
}

bool MemCheckDVCErrorsModel::HasChildren(const wxDataViewItem& item) const
{
    if ( !item.IsOk() )
        return false;

    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.GetID());
    if ( !node ) {
        return false;
    }
    return !node->GetChildren().empty();
}

void MemCheckDVCErrorsModel::DoChangeItemType(const wxDataViewItem& item, bool changeToContainer)
{
    MemCheckDVCErrorsModel_Item* node = reinterpret_cast<MemCheckDVCErrorsModel_Item*>(item.GetID());
    if ( !node )
        return;
    
    if ( ( changeToContainer && !node->IsContainer())  || // change an item from non-container to container type
         ( !changeToContainer && node->IsContainer()) ) { // change an item from container to non-container type
#if defined(__WXGTK__) || defined(__WXMAC__)
        // change the item to container type:
        // 1st we need to delete it
        ItemDeleted(wxDataViewItem(node->GetParent()), item);
        
        // update the node type
        node->SetIsContainer(changeToContainer);
        ItemAdded(wxDataViewItem(node->GetParent()), item);
#else
        node->SetIsContainer(changeToContainer);
#endif
    }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : TableSettings.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "TableSettings.h"
#include <wx/dataview.h>
#include <wx/vector.h>
#include "constraint.h"
#include "column.h"

TableSettings::TableSettings(wxWindow* parent, IDbAdapter* pDbAdapter, Table *pTable, wxSFDiagramManager *pManager)
    : _TableSettings(parent)
{
    m_pDbAdapter = pDbAdapter;
    m_pTable = pTable;
    m_pDiagramManager = pManager;
    m_pEditedConstraint = NULL;

    for( SerializableList::iterator it = pTable->GetChildrenList().begin();
         it != pTable->GetChildrenList().end(); ++it ) {

        if( (*it)->IsKindOf(CLASSINFO(Column)) ) m_lstColumns.Append( (Column*)(*it)->Clone());
        else if( (*it)->IsKindOf(CLASSINFO(Constraint)) ) m_lstKeys.Append((Constraint*)(*it)->Clone());
    }
}

TableSettings::~TableSettings()
{
    m_lstColumns.DeleteContents( true );
    m_lstColumns.Clear();

    m_lstKeys.DeleteContents( true );
    m_lstKeys.Clear();
}

void TableSettings::OnInit(wxInitDialogEvent& event)
{
    m_textName->SetValue( m_pTable->GetName() );

    // fill database data types
    wxArrayString* pDbTypes = m_pDbAdapter->GetDbTypes();
    if( pDbTypes ) {
        wxArrayString choices;
        for (unsigned int i = 0; i < pDbTypes->GetCount(); ++i) {
            choices.Add( pDbTypes->Item(i) );
        }

        m_dvColumns->DeleteColumn( m_dvColumns->GetColumn(1) );
        m_dvColumns->InsertColumn( 1, new wxDataViewColumn( _("Type"), new wxDataViewChoiceRenderer( choices, wxDATAVIEW_CELL_EDITABLE, wxDVR_DEFAULT_ALIGNMENT), 1, -2, wxALIGN_LEFT));

        pDbTypes->Clear();
        delete pDbTypes;
    }

    // fill referenced tables
    ShapeList tables;
    m_choiceRefTable->Append( wxT("") );
    m_pDiagramManager->GetShapes( CLASSINFO(ErdTable), tables );
    for( ShapeList::iterator it = tables.begin(); it != tables.end(); ++it ) {
        Table *t = (Table*) (*it)->GetUserData();
        if( t && t->GetName() != m_pTable->GetName() ) m_choiceRefTable->Append( t->GetName() );
    }

    UpdateView();

    event.Skip();
}

void TableSettings::OnAddColumnClick(wxCommandEvent& event)
{
    Column* col = new Column( MakeUniqueColumnName(_("column") ),
                              m_pTable->GetName(),
                              m_pDbAdapter->GetDbTypeByName( m_pDbAdapter->GetDbTypes()->Last() ) );

    if (col) {
        m_lstColumns.Append( col );
        UpdateView();
    }
}

void TableSettings::OnRemoveColumnClick(wxCommandEvent& event)
{
    Column *col = GetColumn( GetSelectedColumnName() );
    if( col ) {
		// delete associated keys
		SerializableList keys;
		GetConstraints( keys, col->GetName() );
		for(SerializableList::iterator it = keys.begin(); it != keys.end(); ++it ) {
			Constraint *key = (Constraint*) *it;
			m_lstKeys.DeleteObject( key );
			delete key;
		}
		// delete the column
        m_lstColumns.DeleteObject( col );
        delete col;
        UpdateView();
    }
}

void TableSettings::OnMoveDownClick(wxCommandEvent& event)
{
    Column *col = GetColumn( GetSelectedColumnName() );
    if( col ) {
        int i = m_lstColumns.IndexOf( col );
        if( i != wxNOT_FOUND && i < (int)m_lstColumns.GetCount() - 1 ) {
            m_lstColumns.DeleteObject( col );
            m_lstColumns.Insert( i + 1, col );
            UpdateView();
            m_dvColumns->SelectRow( i + 1 );
        }
    }
}

void TableSettings::OnMoveUpClick(wxCommandEvent& event)
{
    Column *col = GetColumn( GetSelectedColumnName() );
    if( col ) {
        int i = m_lstColumns.IndexOf( col );
        if( i != wxNOT_FOUND && i > 0 ) {
            m_lstColumns.DeleteObject( col );
            m_lstColumns.Insert( i - 1, col );
            UpdateView();
            m_dvColumns->SelectRow( i - 1 );
        }
    }
}

void TableSettings::OnColumnChanged(wxDataViewEvent& event)
{
    Column *col = reinterpret_cast<Column*>(m_dvColumns->GetItemData( event.GetItem() ) );
    if( col ) {
        wxVariant val;
        event.GetModel()->GetValue( val, event.GetItem(), event.GetColumn() );
        if( ! val.IsNull() ) {
            switch( event.GetColumn() ) {
            case 0: {
                // rename local columns in keys
                SerializableList keys;
                GetConstraints( keys, col->GetName() );
                for(SerializableList::iterator it = keys.begin(); it != keys.end(); ++it ) {
                    Constraint *key = (Constraint*) *it;
                    if( key->GetType() == Constraint::primaryKey ) key->SetName( wxT("PK_") + val.GetString() );
                    key->SetLocalColumn( val.GetString() );
                }
                // rename table column
                col->SetName( val.GetString() );
                break;
            }
            case 1: {
                col->SetType( m_pDbAdapter->GetDbTypeByName( val.GetString() ) );
                break;
            }
            case 2: {
                long s1, s2;
                s1 = s2 = 0;
                wxSscanf( val.GetString(), wxT("%ld,%ld"), &s1, &s2 );
                IDbType *type = col->GetType();
                if( type->HaveSize() ) type->SetSize( s1 );
                else {
                    m_infobar->ShowMessage( wxT("This data type doesn't support size definition."), wxICON_WARNING );
                    Refresh();
                }
                if( type->HaveSize2() ) type->SetSize2( s1 );
                else { 
                    m_infobar->ShowMessage( wxT("This data type doesn't support size definition."), wxICON_WARNING );
                    Refresh();
                }
                break;
            }
            case 3: {
                IDbType *type = col->GetType();
                if( type->HaveNotNull() ) type->SetNotNull( val.GetBool() );
                else {
                    m_infobar->ShowMessage( wxT("This data type doesn't support NOT NULL feature."), wxICON_WARNING );
                    Refresh();
                }
                break;
            }
            case 4: {
                IDbType *type = col->GetType();
                if( type->HaveAutoIncrement() ) type->SetAutoIncrement( val.GetBool() );
                else { 
                    m_infobar->ShowMessage( wxT("This data type doesn't support AUTOINCREMENT feature."), wxICON_WARNING );
                    Refresh();
                }
                break;
            }
            case 5: {
                Constraint *key = GetConstraint( Constraint::primaryKey, col->GetName() );
                if( key ) {
                    // remove primary key if exists
                    m_lstKeys.DeleteObject( key );
                    delete key;
                } else {
                    // create new primary key
                    key = new Constraint( wxT("PK_") + col->GetName(),
                                          col->GetName(),
                                          Constraint::primaryKey,
                                          Constraint::noAction,
                                          Constraint::noAction );

                    m_lstKeys.Append( key );
                }
                break;
            }
            }
        }
    }

    event.Skip();

    UpdateView();
}

void TableSettings::OnKeyChanged(wxDataViewEvent& event)
{
    Constraint *key = reinterpret_cast<Constraint*>( m_dvKeys->GetItemData( event.GetItem() ) );
    if( key ) {
        wxVariant val;
        event.GetModel()->GetValue( val, event.GetItem(), event.GetColumn() );
        if( ! val.IsNull() ) {
            key->SetName( val.GetString() );
        }
    }

    event.Skip();
}

void TableSettings::OnAddKeyClick(wxCommandEvent& event)
{
    Constraint *key = new Constraint( wxT("FK_") +
                                      m_pTable->GetName() +
                                      wxString::Format( wxT("_%d"), rand() % 100 ),
                                      wxT(""),
                                      Constraint::foreignKey,
                                      Constraint::noAction,
                                      Constraint::noAction );

    m_lstKeys.Append( key );

    UpdateView();
}

void TableSettings::OnRemoveKeyClick(wxCommandEvent& event)
{
    if( m_pEditedConstraint ) {
        m_lstKeys.DeleteObject( m_pEditedConstraint );
        delete m_pEditedConstraint;
        m_pEditedConstraint = NULL;

        UpdateView();
    }
}

void TableSettings::OnLocalColSelected(wxCommandEvent& event)
{
    if( m_pEditedConstraint ) {
        m_pEditedConstraint->SetLocalColumn( m_choiceLocalCol->GetStringSelection() );
    }
}

void TableSettings::OnRadioDeleteSelected(wxCommandEvent& event)
{
    if( m_pEditedConstraint ) {
        m_pEditedConstraint->SetOnDelete( (Constraint::constraintAction) m_radioOnDelete->GetSelection() );
    }
}

void TableSettings::OnRadioUpdateSelected(wxCommandEvent& event)
{
    if( m_pEditedConstraint ) {
        m_pEditedConstraint->SetOnUpdate( (Constraint::constraintAction) m_radioOnUpdate->GetSelection() );
    }
}

void TableSettings::OnRefColSelected(wxCommandEvent& event)
{
    if( m_pEditedConstraint ) {
        m_pEditedConstraint->SetRefCol( m_choiceRefCol->GetStringSelection() );
    }
}

void TableSettings::OnRefTableSelected(wxCommandEvent& event)
{
    FillRefTableColums( GetRefTable( m_choiceRefTable->GetStringSelection() ) );
    m_choiceRefCol->SetStringSelection( wxT("") );

    if( m_pEditedConstraint ) {
        m_pEditedConstraint->SetRefTable( m_choiceRefTable->GetStringSelection() );
    }
}

void TableSettings::OnKeySelected(wxDataViewEvent& event)
{
    if( m_dvKeys->GetItemCount() > 0 ) {
        m_pEditedConstraint = reinterpret_cast<Constraint*>( m_dvKeys->GetItemData( event.GetItem() ) );

        if( m_pEditedConstraint ) {
            m_choiceLocalCol->SetStringSelection( m_pEditedConstraint->GetLocalColumn() );
            m_choiceRefTable->SetStringSelection( m_pEditedConstraint->GetRefTable() );
            FillRefTableColums( GetRefTable( m_pEditedConstraint->GetRefTable() ) );
            m_choiceRefCol->SetStringSelection( m_pEditedConstraint->GetRefCol() );
            m_radioOnUpdate->SetSelection( (int) m_pEditedConstraint->GetOnUpdate() );
            m_radioOnDelete->SetSelection( (int) m_pEditedConstraint->GetOnDelete() );
        }
    } else
        m_pEditedConstraint = NULL;
}

void TableSettings::OnUpdateKeys(wxUpdateUIEvent& event)
{
    event.Enable( m_dvKeys->GetSelectedRow() != wxNOT_FOUND );
}

void TableSettings::OnUpdateColumns(wxUpdateUIEvent& event)
{
    event.Enable( m_dvColumns->GetSelectedRow() != wxNOT_FOUND );
}

void TableSettings::OnUpdateMoveDown(wxUpdateUIEvent& event)
{
    event.Enable( m_dvColumns->GetSelectedRow() != wxNOT_FOUND &&
                  m_dvColumns->GetSelectedRow() < m_dvColumns->GetItemCount() - 1 );
}

void TableSettings::OnUpdateMoveUp(wxUpdateUIEvent& event)
{
    event.Enable( m_dvColumns->GetSelectedRow() != wxNOT_FOUND &&
                  m_dvColumns->GetSelectedRow() > 0 );
}

void TableSettings::FillColumns()
{
    wxVector<wxVariant> line;

    m_dvColumns->DeleteAllItems();

    for( SerializableList::iterator it = m_lstColumns.begin();
         it != m_lstColumns.end(); ++it ) {

        Column *c = wxDynamicCast( *it, Column );
        if( c ) {
            IDbType *type = c->GetType();

            line.clear();
            line.push_back( wxVariant( c->GetName() ) );
            line.push_back( wxVariant( type->GetTypeName() ) );
            line.push_back( wxVariant( wxString::Format( wxT("%ld,%ld"), type->GetSize(), type->GetSize2() ) ) );
            line.push_back( wxVariant( type->GetNotNull() ) );
            line.push_back( wxVariant( type->GetAutoIncrement() ) );
            line.push_back( wxVariant( IsPrimaryKey( c->GetName() ) ) );

            m_dvColumns->AppendItem( line, reinterpret_cast<wxUIntPtr>(c) );
        }
    }
}

void TableSettings::FillKeys()
{
    wxVector<wxVariant> line;

    m_dvKeys->DeleteAllItems();

    for( SerializableList::iterator it = m_lstKeys.begin();
         it != m_lstKeys.end(); ++it ) {

        Constraint *c = wxDynamicCast( *it, Constraint );
        if( c && c->GetType() == Constraint::foreignKey ) {

            line.clear();
            line.push_back( wxVariant( c->GetName() ) );

            m_dvKeys->AppendItem( line, reinterpret_cast<wxUIntPtr>(c) );
        }
    }
}

void TableSettings::FillRefTableColums(Table* tab)
{
    if( tab ) {
        m_choiceRefCol->Clear();
        m_choiceRefCol->Append( wxT("") );

        for( SerializableList::iterator it = tab->GetChildrenList().begin();
             it != tab->GetChildrenList().end(); ++it ) {

            Column *c = wxDynamicCast( *it, Column );
            if( c ) m_choiceRefCol->Append( c->GetName() );
        }
    }
}

Table* TableSettings::GetRefTable(const wxString& name)
{
    ShapeList tables;
    m_pDiagramManager->GetShapes( CLASSINFO(ErdTable), tables );

    for( ShapeList::iterator it = tables.begin(); it != tables.end(); ++it ) {
        Table *t = (Table*) (*it)->GetUserData();
        if( t->GetName() == name ) return t;
    }

    return NULL;
}

void TableSettings::UpdateView()
{
    int crow = m_dvColumns->GetSelectedRow();
    //int krow = m_dvKeys->GetSelectedRow();

    FillColumns();
    FillKeys();

    m_choiceLocalCol->Clear();
    m_choiceLocalCol->Append( wxT("") );
    for( SerializableList::iterator it = m_lstColumns.begin(); it != m_lstColumns.end(); ++it ) {
        Column *c = (Column*) *it;
        if( c )	m_choiceLocalCol->Append( c->GetName() );
    }

    m_choiceLocalCol->SetStringSelection( wxT("") );
    m_choiceRefTable->SetStringSelection( wxT("") );
    m_choiceRefCol->SetStringSelection( wxT("") );
    m_radioOnDelete->SetSelection( 0 );
    m_radioOnUpdate->SetSelection( 0 );

    if( crow != wxNOT_FOUND && crow < m_dvColumns->GetItemCount() ) m_dvColumns->SelectRow( crow );
    //if( krow != wxNOT_FOUND && krow < m_dvKeys->GetItemCount() ) m_dvKeys->SelectRow( krow );
}

bool TableSettings::IsPrimaryKey(const wxString& colname)
{
    for( SerializableList::iterator it = m_lstKeys.begin();
         it != m_lstKeys.end(); ++it ) {

        Constraint *c = wxDynamicCast( *it, Constraint );
        if( c && ( c->GetType() == Constraint::primaryKey )
            && ( c->GetLocalColumn() == colname ) ) return true;
    }

    return false;
}

Constraint* TableSettings::GetConstraint(Constraint::constraintType type,const wxString& localcol)
{
    for( SerializableList::iterator it = m_lstKeys.begin();
         it != m_lstKeys.end(); ++it ) {

        Constraint *c = wxDynamicCast( *it, Constraint );
        if( c && c->GetType() == type && c->GetLocalColumn() == localcol ) return c;
    }

    return NULL;
}

void TableSettings::GetConstraints(SerializableList& keys, const wxString& localcol)
{
    for( SerializableList::iterator it = m_lstKeys.begin();
         it != m_lstKeys.end(); ++it ) {

        Constraint *c = wxDynamicCast( *it, Constraint );
        if( c && ( c->GetLocalColumn() == localcol ) ) keys.Append( *it );
    }
}

Column* TableSettings::GetColumn(const wxString& name)
{
    for( SerializableList::iterator it = m_lstColumns.begin();
         it != m_lstColumns.end(); ++it ) {

        Column *c = wxDynamicCast( *it, Column );
        if( c && ( c->GetName() == name ) ) return c;
    }

    return NULL;
}

wxString TableSettings::MakeUniqueColumnName(const wxString& name)
{
    wxString newname = name;

    while( GetColumn( newname ) ) {
        newname += wxString::Format( wxT("%d"), rand() % 10 );
    }

    return newname;
}

wxString TableSettings::GetSelectedColumnName()
{
    int row = m_dvColumns->GetSelectedRow();

    if( row != wxNOT_FOUND ) {
        wxVariant val;
        m_dvColumns->GetValue( val, row, 0 );
        return val.GetString();
    } else {
        return wxEmptyString;
    }
}

void TableSettings::OnCancelClick(wxCommandEvent& event)
{
    EndModal( wxID_CANCEL );
}

void TableSettings::OnOKClick(wxCommandEvent& event)
{
    m_pTable->SetName( m_textName->GetValue() );

    m_pTable->RemoveChildren();

    for( SerializableList::iterator it = m_lstColumns.begin();
         it != m_lstColumns.end(); ++it ) {
        m_pTable->AddColumn( (Column*) (*it) );
    }

    for( SerializableList::iterator it = m_lstKeys.begin();
         it != m_lstKeys.end(); ++it ) {
        m_pTable->AddConstraint ( (Constraint*) (*it) );
    }

    m_lstColumns.Clear();
    m_lstKeys.Clear();

    EndModal( wxID_OK );
}

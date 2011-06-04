#include "TableSettingsDialog.h"

TableSettings::TableSettings(wxWindow* parent,IDbAdapter* pDbAdapter, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):_TableSettings(parent,id,title,pos,size, style ) {
	m_pTable = NULL;
	m_pDiagramManager = NULL;
	m_pEditedColumn = NULL;
	m_pEditedConstraint = NULL;
	m_txSize->SetValidator(wxTextValidator(wxFILTER_NUMERIC));

	m_fUpdating = false;

	m_pDbAdapter = pDbAdapter;

	wxArrayString* pDbTypes = m_pDbAdapter->GetDbTypes();
	for (unsigned int i = 0; i < pDbTypes->Count(); ++i) {
		m_comboType->AppendString(pDbTypes->Item(i));
	}
	if (pDbTypes) {
		pDbTypes->clear();
		delete pDbTypes;
	}
}

TableSettings::~TableSettings() {
}

void TableSettings::OnListBoxClick(wxCommandEvent& event) {

	if( m_fUpdating ) return;

	wxString name = m_listColumns->GetStringSelection().substr(4);
	Column* col = NULL;
	Constraint* constr = NULL;

	m_pEditedColumn = NULL;
	m_pEditedConstraint = NULL;

	SerializableList::compatibility_iterator node = m_pTable->GetFirstChildNode();
	while( node ) {
		if( node->GetData()->IsKindOf( CLASSINFO(Column)) )  col = (Column*) node->GetData();

		if ((col)&&(col->GetName() == name)) {
			m_pEditedColumn = col;
			m_txColName->SetValue(col->GetName());

			IDbType* type = col->GetPType();
			if (type) {
				m_comboType->SetValue(type->GetTypeName());
				m_txSize->SetValue(wxString::Format(wxT("%li"),type->GetSize()));
				m_txSize2->SetValue(wxString::Format(wxT("%li"),type->GetSize2()));
				m_chAutoIncrement->SetValue(type->GetAutoIncrement());
				m_chNotNull->SetValue(type->GetNotNull());
			}
		}

		if( node->GetData()->IsKindOf( CLASSINFO(Constraint)) )  constr = (Constraint*) node->GetData();

		if ((constr)&&(constr->GetName() == name)) {
			m_comboLocalColumn->Clear();
			m_pEditedConstraint = constr;

			m_txConstraintName->SetValue(constr->GetName());
			m_comboLocalColumn->SetValue(constr->GetLocalColumn());
			if (m_pTable) {
				SerializableList::compatibility_iterator node = m_pTable->GetFirstChildNode();
				while( node ) {
					if( node->GetData()->IsKindOf( CLASSINFO(Column)) )  m_comboLocalColumn->AppendString(wxString::Format(wxT("%s"),((Column*) node->GetData())->GetName().c_str()));
					node = node->GetNext();
				}
			}
			m_radioBox1->Select(constr->GetType());
			m_comboRefTable->SetValue(constr->GetRefTable());
			m_comboRefColumn->SetValue(constr->GetRefCol());
			m_radioOnDelete->Select(constr->GetOnDelete());
			m_radioOnUpdate->Select(constr->GetOnUpdate());

		}

		node = node->GetNext();
	}

	/*
	Column* col = wxDynamicCast(m_pTable->GetFristColumn(),Column);
	while(col){

		if ((col)&&(col->getName() == name)) {
			m_pEditedColumn = col;
			m_txColName->SetValue(col->getName());

			IDbType* type = col->getPType();
			if (type) {
				m_comboType->SetValue(type->GetTypeName());
				m_txSize->SetValue(wxString::Format(wxT("%li"),type->GetSize()));
				m_chAutoIncrement->SetValue(type->GetAutoIncrement());
				m_chNotNull->SetValue(type->GetNotNull());
				m_chPrimary->SetValue(type->GetPrimaryKey());
				m_checkBox3->SetValue(type->GetUnique());
			}
		}
		col = wxDynamicCast(col->GetSibbling(CLASSINFO(Column)),Column);
	}*/
}

void TableSettings::OnNewColumnClick(wxCommandEvent& event) {
	Column* pCol = new Column(wxT("New col"),m_pTable->GetName(),m_pDbAdapter->GetDbTypeByName(m_pDbAdapter->GetDbTypes()->Last()));
	if (pCol) m_pTable->AddColumn(pCol);
	UpdateView();
}

void TableSettings::OnOKClick(wxCommandEvent& event) {
	m_pTable->SetName(m_txTableName->GetValue());
	Close();
}

void TableSettings::OnSaveColumnClick(wxCommandEvent& event) {
	if (m_pEditedColumn) {
		m_pEditedColumn->SetName(m_txColName->GetValue());
		IDbType* pType = m_pEditedColumn->GetPType();
		if (pType) {
			pType->SetNotNull(m_chNotNull->GetValue());
			pType->SetAutoIncrement(m_chAutoIncrement->GetValue());

			long s,s2;
			m_txSize->GetValue().ToLong(&s);
			m_txSize2->GetValue().ToLong(&s2);
			pType->SetSize(s);
			pType->SetSize2(s2);
		}
	}
	if (m_pEditedConstraint) {
		m_pEditedConstraint->SetName(m_txConstraintName->GetValue());
		m_pEditedConstraint->SetLocalColumn(m_comboLocalColumn->GetValue());
		if (m_radioBox1->GetSelection() == 0) m_pEditedConstraint->SetType(Constraint::primaryKey);
		if (m_radioBox1->GetSelection() == 1)  m_pEditedConstraint->SetType(Constraint::foreignKey);
		if (m_pEditedConstraint->GetType() == Constraint::foreignKey) {
			m_pEditedConstraint->SetRefTable(m_comboRefTable->GetValue());
			m_pEditedConstraint->SetRefCol(m_comboRefColumn->GetValue());
			m_pEditedConstraint->SetOnUpdate((Constraint::constraintAction) m_radioOnUpdate->GetSelection());
			m_pEditedConstraint->SetOnDelete((Constraint::constraintAction) m_radioOnDelete->GetSelection());
		}
	}
	UpdateView();
}

void TableSettings::OnTypeSelect(wxCommandEvent& event) {
	IDbType* pType = m_pDbAdapter->GetDbTypeByName(m_comboType->GetValue());
	if (pType) {
		m_pEditedColumn->SetPType(pType);
	}
}

void TableSettings::SetTable(Table* tab, wxSFDiagramManager* pManager) {
	m_pTable = tab;
	m_pDiagramManager = pManager;
	if (m_pTable) {
		m_txTableName->SetValue(tab->GetName());
		if (pManager) {
			ShapeList lstShapes;
			pManager->GetShapes( CLASSINFO(ErdTable), lstShapes );
			ShapeList::compatibility_iterator it = lstShapes.GetFirst();
			while( it ) {
				ErdTable* pTab = wxDynamicCast(it->GetData(),ErdTable);
				if (pTab) {
					if (m_pTable->GetName() != pTab->GetTable()->GetName()) {
						m_comboRefTable->AppendString(pTab->GetTable()->GetName());
					}
				}
				it = it->GetNext();
			}

		}
	}
	UpdateView();
}

void TableSettings::UpdateView() {
	m_fUpdating = true;

	m_listColumns->Clear();
	if (m_pTable) {

		SerializableList::compatibility_iterator node = m_pTable->GetFirstChildNode();
		while( node ) {
			if( node->GetData()->IsKindOf( CLASSINFO(Column)) )  m_listColumns->AppendString(wxString::Format(wxT("col:%s"),((Column*) node->GetData())->GetName().c_str()));
			node = node->GetNext();
		}
		node = m_pTable->GetFirstChildNode();
		while( node ) {
			if( node->GetData()->IsKindOf( CLASSINFO(Constraint)) )  m_listColumns->AppendString(wxString::Format(wxT("key:%s"),((Constraint*) node->GetData())->GetName().c_str()));
			node = node->GetNext();
		}

	}
	m_pEditedColumn = NULL;
	m_pEditedConstraint = NULL;
	m_txColName->Clear();
	m_txSize->Clear();
	m_txSize2->Clear();
	m_comboType->SetValue(wxT(""));
	m_chAutoIncrement->SetValue(false);
	m_chNotNull->SetValue(false);


	m_fUpdating = false;
}

void TableSettings::OnAutoIncrementUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) {
		if (m_pEditedColumn->GetPType()) event.Enable(m_pEditedColumn->GetPType()->HaveAutoIncrement());
	}
}

void TableSettings::OnColNameUI(wxUpdateUIEvent& event) {
	if (m_pEditedColumn) event.Enable(true);
	else event.Enable(false);
}

void TableSettings::OnColSizeUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) {
		if (m_pEditedColumn->GetPType()) event.Enable(m_pEditedColumn->GetPType()->HaveSize());
	}
}

void TableSettings::OnColTypeUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) {
		if (m_pEditedColumn->GetPType()) event.Enable(true);
	}
}

void TableSettings::OnNotNullUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) {
		if (m_pEditedColumn->GetPType()) event.Enable(m_pEditedColumn->GetPType()->HaveNotNull());
	}
}

void TableSettings::OnPrimaryKeyUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) {
		if (m_pEditedColumn->GetPType()) event.Enable(m_pEditedColumn->GetPType()->HavePrimaryKey());
	}
}

void TableSettings::OnUniqueUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) {
		if (m_pEditedColumn->GetPType()) event.Enable(m_pEditedColumn->GetPType()->HaveUnique());
	}
}
void TableSettings::OnDeleteColumn(wxCommandEvent& event) {
	wxString name = m_listColumns->GetStringSelection().substr(4);
	Column* col  = NULL;
	Constraint* constr = NULL;
	SerializableList::compatibility_iterator node = m_pTable->GetFirstChildNode();
	while( node ) {
		if( node->GetData()->IsKindOf( CLASSINFO(Column)) )  col = (Column*) node->GetData();
		if( node->GetData()->IsKindOf( CLASSINFO(Constraint)) )  constr = (Constraint*) node->GetData();

		if ((col)&&(col->GetName() == name)) {
			constr = NULL;
			break;
		} else col = NULL;
		if ((constr)&&(constr->GetName() == name)) break;
		else constr = NULL;

		node = node->GetNext();
	}
	if (col) m_pTable->RemoveChild(col);
	if (constr) m_pTable->RemoveChild(constr);
	UpdateView();
}

void TableSettings::OnNewConstrainClick(wxCommandEvent& event) {
	Constraint* pConst = new Constraint(wxString::Format(wxT("PK_%s"),m_pTable->GetName().c_str()), wxT(""), Constraint::primaryKey, Constraint::restrict, Constraint::restrict);
	if (pConst) m_pTable->AddConstraint(pConst);
	UpdateView();
}

void TableSettings::OnPageConstraintUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedConstraint) event.Enable(true);
}

void TableSettings::OnPageTypeUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) event.Enable(true);
}

void TableSettings::OnRefColUI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if ((m_pEditedConstraint) && (m_comboRefTable->GetValue().length() > 0 )) event.Enable(true);
}

void TableSettings::OnRefTabChange(wxCommandEvent& event) {
	ErdTable* pErdTab = NULL;
	Table* pTab = NULL;
	m_comboRefColumn->Clear();
	m_comboRefColumn->SetValue(wxT(""));
	if (m_pDiagramManager) {
		ShapeList lstShapes;
		m_pDiagramManager->GetShapes( CLASSINFO(ErdTable), lstShapes );
		ShapeList::compatibility_iterator it = lstShapes.GetFirst();
		while( it ) {
			pErdTab = wxDynamicCast(it->GetData(),ErdTable);
			if (pErdTab) {
				if (pErdTab->GetTable()->GetName() == m_comboRefTable->GetValue()) {
					pTab = pErdTab->GetTable();

				}
			}
			it = it->GetNext();
		}
	}
	if (pTab) {
		SerializableList::compatibility_iterator node = pTab->GetFirstChildNode();
		while( node ) {
			Column* col = wxDynamicCast(node->GetData(),Column);
			if (col) {
				m_comboRefColumn->AppendString(col->GetName());
			}
			node = node->GetNext();
		}
	}
}

void TableSettings::OnRefTabUI(wxUpdateUIEvent& event) {
	event.Enable(m_radioBox1->GetSelection() == 1);
}

void TableSettings::OnNotebookUI(wxUpdateUIEvent& event) {
	if (m_pEditedConstraint) {
		m_notebook3->SetSelection(1);
	} else {
		m_notebook3->SetSelection(0);
	}
}

void TableSettings::OnDeleteUI(wxUpdateUIEvent& event) {
	event.Enable(m_radioBox1->GetSelection() == 1);
}

void TableSettings::OnUpdateUI(wxUpdateUIEvent& event) {
	event.Enable(m_radioBox1->GetSelection() == 1);
}
void TableSettings::OnColSize2UI(wxUpdateUIEvent& event) {
	event.Enable(false);
	if (m_pEditedColumn) {
		if (m_pEditedColumn->GetPType()) event.Enable(m_pEditedColumn->GetPType()->HaveSize2());
	}
}

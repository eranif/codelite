#ifndef TABLESETTINGS_H
#define TABLESETTINGS_H

#include "GUI.h"
#include <wx/wxsf/DiagramManager.h>
#include "table.h"
#include "column.h"
#include "constraint.h"
#include "ErdTable.h"
#include "IDbType.h"
#include "IDbAdapter.h"

class TableSettings : public _TableSettings
{
public:
	TableSettings(wxWindow* parent, IDbAdapter* pDbAdapter, Table *pTable, wxSFDiagramManager *pManager);
	virtual ~TableSettings();

	void SetTable(Table* tab, wxSFDiagramManager* pManager);

protected:
	virtual void OnKeyChanged(wxDataViewEvent& event);
	virtual void OnKeySelected(wxDataViewEvent& event);
	virtual void OnColumnChanged(wxDataViewEvent& event);
	virtual void OnUpdateMoveDown(wxUpdateUIEvent& event);
	virtual void OnUpdateMoveUp(wxUpdateUIEvent& event);
	virtual void OnCancelClick(wxCommandEvent& event);
	virtual void OnOKClick(wxCommandEvent& event);
	virtual void OnLocalColSelected(wxCommandEvent& event);
	virtual void OnRadioDeleteSelected(wxCommandEvent& event);
	virtual void OnRadioUpdateSelected(wxCommandEvent& event);
	virtual void OnRefColSelected(wxCommandEvent& event);
	virtual void OnRefTableSelected(wxCommandEvent& event);
	virtual void OnUpdateKeys(wxUpdateUIEvent& event);
	virtual void OnAddColumnClick(wxCommandEvent& event);
	virtual void OnAddKeyClick(wxCommandEvent& event);
	virtual void OnInit(wxInitDialogEvent& event);
	virtual void OnMoveDownClick(wxCommandEvent& event);
	virtual void OnMoveUpClick(wxCommandEvent& event);
	virtual void OnRemoveColumnClick(wxCommandEvent& event);
	virtual void OnRemoveKeyClick(wxCommandEvent& event);
	virtual void OnUpdateColumns(wxUpdateUIEvent& event);

	bool m_fUpdating;
	Table* m_pTable;
	Constraint* m_pEditedConstraint;
	IDbAdapter* m_pDbAdapter;
	wxSFDiagramManager* m_pDiagramManager;
	SerializableList m_lstColumns;
	SerializableList m_lstKeys;

	Table* GetRefTable(const wxString& name);
	Column* GetColumn(const wxString& name);
	Constraint* GetConstraint( Constraint::constraintType type, const wxString& localcol);
	void GetConstraints(SerializableList &keys, const wxString& localcol);
	wxString GetSelectedColumnName();
	wxString MakeUniqueColumnName(const wxString& name);

	void FillRefTableColums(Table *tab);
	void FillColumns();
	void FillKeys();

	void UpdateView();

	bool IsPrimaryKey(const wxString& colname);
};
#endif // TABLESETTINGS_H

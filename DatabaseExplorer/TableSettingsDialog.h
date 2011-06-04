#ifndef TABLESETTINGS_H
#define TABLESETTINGS_H

#include <wx/wx.h>
#include <wx/validate.h>
#include "GUI.h" // Base class: _TableSettings
#include <wx/wxsf/DiagramManager.h>
#include "table.h"
#include "column.h"
#include "constraint.h"
#include "ErdTable.h"
#include "IDbType.h"
#include "IDbAdapter.h"


class TableSettings : public _TableSettings {

	public:
		TableSettings( wxWindow* parent,IDbAdapter* pDbAdapter, wxWindowID id = wxID_ANY, const wxString& title = wxT("Table settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		virtual ~TableSettings();
		void SetTable(Table* tab, wxSFDiagramManager* pManager);

		virtual void OnListBoxClick(wxCommandEvent& event);
		virtual void OnNewColumnClick(wxCommandEvent& event);
		virtual void OnOKClick(wxCommandEvent& event);
		virtual void OnSaveColumnClick(wxCommandEvent& event);
		virtual void OnTypeSelect(wxCommandEvent& event);

		virtual void OnAutoIncrementUI(wxUpdateUIEvent& event);
		virtual void OnColNameUI(wxUpdateUIEvent& event);
		virtual void OnColSizeUI(wxUpdateUIEvent& event);
		virtual void OnColSize2UI(wxUpdateUIEvent& event);
		virtual void OnColTypeUI(wxUpdateUIEvent& event);
		virtual void OnNotNullUI(wxUpdateUIEvent& event);
		virtual void OnPrimaryKeyUI(wxUpdateUIEvent& event);
		virtual void OnUniqueUI(wxUpdateUIEvent& event);
		virtual void OnDeleteColumn(wxCommandEvent& event);


		virtual void OnNewConstrainClick(wxCommandEvent& event);
		virtual void OnPageConstraintUI(wxUpdateUIEvent& event);
		virtual void OnPageTypeUI(wxUpdateUIEvent& event);
		virtual void OnRefColUI(wxUpdateUIEvent& event);
		virtual void OnRefTabChange(wxCommandEvent& event);
		virtual void OnRefTabUI(wxUpdateUIEvent& event);
		virtual void OnDeleteUI(wxUpdateUIEvent& event);
		virtual void OnUpdateUI(wxUpdateUIEvent& event);

		virtual void OnNotebookUI(wxUpdateUIEvent& event);

	protected:
		bool m_fUpdating;
		Table* m_pTable;
		Column* m_pEditedColumn;
		Constraint* m_pEditedConstraint;
		IDbAdapter* m_pDbAdapter;
		wxSFDiagramManager* m_pDiagramManager;
		//wxTextValidator validNum(wxFILTER_NUMERIC, NULL);

		void UpdateView();
};

#endif // TABLESETTINGS_H

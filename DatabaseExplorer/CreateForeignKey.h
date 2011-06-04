#ifndef CREATEFOREIGNKEY_H
#define CREATEFOREIGNKEY_H
#include <wx/wx.h>
#include <wx/wxsf/ShapeBase.h>

#include "ErdTable.h"
#include "table.h"
#include "column.h"
#include "constraint.h"

#include "GUI.h" // Base class: _CreateForeignKey
/*! \brief  Dialog for foreign key creating. If N:M connection type is selected, dialog create new connection table. */
class CreateForeignKey : public _CreateForeignKey {

public:
	CreateForeignKey(wxWindow* parent, ErdTable* pSourceTable, ErdTable* pDestTable, const wxString& srcColName, const wxString& dstColName);
	virtual ~CreateForeignKey();
	virtual void OnCancelClick(wxCommandEvent& event);
	virtual void OnOKClick(wxCommandEvent& event);
	virtual void OnOKUI(wxUpdateUIEvent& event);
protected:
	ErdTable* m_pSrcTable;
	ErdTable* m_pDstTable;
	wxString m_srcColName;
	wxString m_dstColName;
};

#endif // CREATEFOREIGNKEY_H

#ifndef CLASSGENERATEDIALOG_H
#define CLASSGENERATEDIALOG_H

#include "GUI.h" // Base class: _ClassGenerateDialog
#include "ErdTable.h"
#include "ErdView.h"
#include "table.h"
#include "IDbAdapter.h"
#include "wx/wxxmlserializer/XmlSerializer.h"
#include <wx/textfile.h>
#include <wx/stdpaths.h>
#include <wx/init.h>

#include "../Interfaces/imanager.h"
#include "../Plugin/virtualdirectoryselector.h"
#include "../Plugin/project.h"
#include "../Plugin/workspace.h"

/*! \brief Dialog for generating classes which are used for database acces. */
class ClassGenerateDialog : public _ClassGenerateDialog {

public:
		/*! \brief Defautl constructor.  */
		ClassGenerateDialog(wxWindow *parent, IDbAdapter* dbAdapter, xsSerializable* pItems, IManager* pMgr);

		/*! \brief Return string with type name on UNIVERSAL TYPE base */
		wxString GetTypeName(IDbType::UNIVERSAL_TYPE type);
		/*! \brief Return string with type name in result format on UNIVERSAL TYPE base */
		wxString GetResTypeName(IDbType::UNIVERSAL_TYPE type);
		/*! \brief Return row for filling table row attribute. */
		wxString GetFillData(DBEColumn* pCol, int colIndex);
		/*! \brief Return string with type name on UNIVERSAL TYPE base */
		wxString GetResultFunction(IDbType::UNIVERSAL_TYPE type);
		/*! \brief Return string with type name in function parameter format on UNIVERSAL TYPE base */
		wxString GetParamTypeName(IDbType::UNIVERSAL_TYPE type);
		/*! \brief Return string with type name in add function format on UNIVERSAL TYPE base */
		wxString GetAddParamFunction(IDbType::UNIVERSAL_TYPE type);


		/*! \brief Function for generating classes for selected table. It call GenerateFile function two times. Fist time for generate .h, second time for generate .cpp file. */
		bool GenerateClass(DBETable* pTab, const wxString& path);	

		/*! \brief Function for generating file for selected table.  */
		bool GenerateFile(DBETable* pTab,wxTextFile& htmpFile, wxTextFile& hFile, const wxString& classItemName,const wxString& classItemDef, const wxString& classColName, const wxString& classTableName, const wxString& classUtilName );

		virtual void OnCancelClick(wxCommandEvent& event);
		
		/*! \brief Function for generating classes for all tables. It call GenerateClass() function for each table. */
		virtual void OnGenerateClick(wxCommandEvent& event);
		virtual void OnBtnBrowseClick(wxCommandEvent& event);


		virtual ~ClassGenerateDialog();

protected:
		IDbAdapter* m_pDbAdapter;
		xsSerializable* m_pItems;
		IManager* m_mgr;
};

#endif // CLASSGENERATEDIALOG_H

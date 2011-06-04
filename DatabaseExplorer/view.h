#ifndef VIEW_H
#define VIEW_H

#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
#include "IDbAdapter.h"
// ------------------------------------------------
// Trida View
// -------------------------------------------------
/*! \brief Class representing database view. */
class View : public xsSerializable {

public:
	XS_DECLARE_CLONABLE_CLASS(View);
	/*! \brief Default constructors */
	View();
	View(const View& obj);
	View(IDbAdapter* dbAdapter,const wxString& name, const wxString& parentName, const wxString& select);
	/*! \brief Default destructors */
	virtual ~View();

	/*! \brief Set view name */
	void SetName(const wxString & name) {
		this->m_name = name;
	}
	/*! \brief Set parent name (Database) */
	void SetParentName(const wxString & parentName) {
		this->m_parentName = parentName;
	}
	/*! \brief Set SELECT string */
	void SetSelect(const wxString & select) {
		this->m_select = select;
	}
	/*! \brief Get view name */
	const wxString & GetName() const {
		return m_name;
	}
	/*! \brief Get paretn name (database) */
	const wxString & GetParentName() const {
		return m_parentName;
	}
	/*! \brief Get SELECT string */
	const wxString & GetSelect() const {
		return m_select;
	}
	/*! \brief Get IDbAdapter */
	IDbAdapter * GetDbAdapter() {
		return m_pDbAdapter;
	}

protected:
	wxString m_name;
	wxString m_parentName;
	wxString m_select;
	void initSerializable();
	IDbAdapter* m_pDbAdapter;
};

#endif // VIEW_H

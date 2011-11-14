#ifndef PERSPECTIVEMANAGER_H
#define PERSPECTIVEMANAGER_H

#include "precompiled_header.h"
#include <wx/string.h>
#include <wx/arrstr.h>
#include <map>

extern wxString DEBUG_LAYOUT ;
extern wxString NORMAL_LAYOUT;

#define PERSPECTIVE_FIRST_MENU_ID 17000
#define PERSPECTIVE_LAST_MENU_ID  17020

class PerspectiveManager
{
protected:
	std::map<wxString, int> m_menuIdToName;
	int                     m_nextId;
	wxString                m_active;

protected:
	wxString DoGetPathFromName(const wxString &name);

public:
	PerspectiveManager();
	virtual ~PerspectiveManager();

	void SetActive(const wxString& active) {
		this->m_active = active;
	}
	const wxString& GetActive() const {
		return m_active;
	}
	void          LoadPerspective(const wxString &name);
	void          LoadPerspectiveByMenuId(int id);
	void          SavePerspective(const wxString &name = wxT(""), bool notify = true);
	void          SavePerspectiveIfNotExists(const wxString &name);

	wxArrayString GetAllPerspectives();
	void          DeleteAllPerspectives();
	void          ClearIds();
	int           MenuIdFromName(const wxString &name);
	wxString      NameFromMenuId(int id);
	void          Rename(const wxString &old, const wxString &new_name);
	void          Delete(const wxString &name);

	int FirstMenuId() const {
		return PERSPECTIVE_FIRST_MENU_ID;
	}
	int LastMenuId()  const {
		return PERSPECTIVE_LAST_MENU_ID;
	}
};

#endif // PERSPECTIVEMANAGER_H

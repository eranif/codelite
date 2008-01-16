#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "serialized_object.h"
#include "wx/xml/xml.h"

/**
 * \class SessionEntry
 * \brief Session entry is associated per workspace
 * \author Eran
 * \date 09/25/07
 */
class SessionEntry : public SerializedObject {
	int m_selectedTab;
	wxArrayString m_tabs;
	wxString m_workspaceName;

public:
	//Setters
	void SetSelectedTab(const int& selectedTab) {this->m_selectedTab = selectedTab;}
	void SetTabs(const wxArrayString& tabs) {this->m_tabs = tabs;}
	void SetWorkspaceName(const wxString& workspaceName) {this->m_workspaceName = workspaceName;}
	
	//Getters
	const int& GetSelectedTab() const {return m_selectedTab;}
	const wxArrayString& GetTabs() const {return m_tabs;}
	const wxString& GetWorkspaceName() const {return m_workspaceName;}
	
	SessionEntry();
	virtual ~SessionEntry();

	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);

};

/**
 * \class SessionManager
 * \brief 
 * \author Eran
 * \date 09/25/07
 */
class SessionManager {
	wxXmlDocument m_doc;
	wxFileName m_fileName;
	
private:
	SessionManager();
	~SessionManager();

public:
	static SessionManager& Get();
	bool Load(const wxString &fileName);
	bool Save(const wxString &name, SessionEntry &session);
	bool FindSession(const wxString &name, SessionEntry &session);
};

#endif //SESSIONMANAGER_H


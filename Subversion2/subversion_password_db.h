#ifndef SUBVERSIONPASSWORDDB_H
#define SUBVERSIONPASSWORDDB_H

#include <wx/fileconf.h>

class SubversionPasswordDb {
	wxFileConfig *m_fileConfig;
public:
	SubversionPasswordDb();
	virtual ~SubversionPasswordDb();
	
	/**
	 * @brief find login details for given URL 
	 * @param url repository URL
	 * @param user [output]
	 * @param password [output]
	 * @return true if the URL is found in the database, false otherwise
	 */
	bool GetLogin(const wxString &url, wxString &user, wxString &password);
	/**
	 * @brief store login info for given url
	 * @param url url as key
	 * @param user user name 
	 * @param password password
	 */
	void SetLogin(const wxString &url, const wxString &user, const wxString &password);
	
	/**
	 * @brief delete login details for the given URL
	 * @param url
	 */
	void DeleteLogin(const wxString &url);
};

#endif // SUBVERSIONPASSWORDDB_H

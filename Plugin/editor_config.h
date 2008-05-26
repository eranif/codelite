//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editor_config.h              
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
 #ifndef LITEEDITOR_EDITOR_CONFIG_H
#define LITEEDITOR_EDITOR_CONFIG_H

#include "singleton.h"
#include <wx/filename.h>
#include <vector>
#include "wx/xml/xml.h"
#include "lexer_configuration.h"
#include "optionsconfig.h"
#include "map"
#include "serialized_object.h"
#include "plugin.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

class SimpleLongValue : public SerializedObject 
{
	long m_value;
public:
	SimpleLongValue();
	~SimpleLongValue();
	
	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);
	
	//Setters
	void SetValue(const long& value) {this->m_value = value;}
	//Getters
	const long& GetValue() const {return m_value;}
};

class SimpleStringValue : public SerializedObject 
{
	wxString m_value;
public:
	SimpleStringValue();
	~SimpleStringValue();
	
	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);
	
	void SetValue(const wxString& value) {this->m_value = value;}
	const wxString& GetValue() const {return m_value;}
};

/**
 * \ingroup LiteEditor
 * \brief EditorConfig a singleton class that manages the liteeditor.xml configuration file
 *
 * \version 1.0
 * first version
 *
 * \date 04-19-2007
 *
 * \author Eran
 */
class EditorConfig : public IConfigTool
{
	friend class Singleton<EditorConfig>;
	wxXmlDocument* m_doc;
	wxFileName m_fileName; 
	std::map<wxString, LexerConfPtr> m_lexers;
	static wxString m_svnRevision;
	
public:

	//load lexers again, based on the active theme
	void LoadLexers();
	static void Init(const wxChar *revision) { m_svnRevision = revision; }
	
public:
	typedef std::map<wxString, LexerConfPtr>::const_iterator ConstIterator;

	/**
	 * Load the configuration file
	 * \param fileName configuration file name
	 * \return true on success false otherwise
	 */
	bool Load();

	/**
	 * Find lexer configuration and return a pointer to a LexerConf object
	 * \param lexer lexer name (e.g. Cpp, Java, Default etc..)
	 * \return LexerConfPtr 
	 */
	LexerConfPtr GetLexer(const wxString& lexer);

	/**
	 * Return iterator to the begin of the undelying lexer mapping	 
 	 */
	ConstIterator LexerBegin();

	/**
	 * Return iterator to the end of the undelying lexer mapping
	 */
	ConstIterator LexerEnd();

	/**
	 * Test if this configuration is loaded properly
	 * \return true of a file is loaded into the configuration manager false otherwise
	 */
	bool IsOk() const { return m_doc->IsOk(); }

//	/**
//	 * Load notebook style from configuration
//	 * \param nbName notebook name
//	 * \return wxNOT_FOUND when notebook not found or style
//	 */
//	long LoadNotebookStyle(const wxString &nbName);
//
//	/**
//	 * Save notebook style to configuration
//	 * \param nbName notebook name
//	 * \param style style value to save
//	 */
//	void SaveNotebookStyle(const wxString &nbName, long style);

	/**
	 * Load perspective from configuration file
	 * \param name perspective name
 	 * \return perspective
	 */
	wxString LoadPerspective(const wxString &name) const ;

	/**
	 * Read the editor options from the configuration file
	 * and return them as an object
	 */
	OptionsConfigPtr GetOptions() const;

	/**
	 * Set options to the configuration file, override them if they does not exist
	 */
	void SetOptions(OptionsConfigPtr opts);

	/**
	 * Return the database that should be used by the editor
	 * \return 
	 */
	wxString GetTagsDatabase() const;

	/**
	 * Set tags database to be use by editor (in addition to the workspace one)
	 * \param &dbName 
	 */
	void SetTagsDatabase(const wxString &dbName);

	/**
	 * save lexers settings
	 */
	void SaveLexers();

	/**
	 * get an array of recently opened files
	 * \param files  [output] a place holder for the output
	 */
	void GetRecentlyOpenedFies(wxArrayString &files);

	/**
	 * get an array of recently opened files
	 * \param files  list of files to save
	 */
	void SetRecentlyOpenedFies(const wxArrayString &files);

	/**
	 * get an array of recently opened workspaces
	 * \param files  [output] a place holder for the output
	 */
	void GetRecentlyOpenedWorkspaces(wxArrayString &files);

	/**
	 * get an array of recently opened workspaces
	 * \param files  list of files to save
	 */
	void SetRecentlyOpenedWorkspaces(const wxArrayString &files);

	/**
	 * \brief write an archived object to the xml configuration
	 * \param name object name
	 * \param arch the archived object container
	 */
	virtual bool WriteObject(const wxString &name, SerializedObject *obj);
	
	/**
	 * \brief read an archived object from the configuration
	 * \param name object to read
	 * \param arch [output]
	 */
	virtual bool ReadObject(const wxString &name, SerializedObject *obj);
	
	/**
	 * Return the configuration version
	 */
	wxString GetRevision() const;

	/**
	 * Set the current configuration revision
	 */
	void SetRevision(const wxString &rev);
	
	/**
	 * \brief convinience methods to store a single long value
	 * \param name variable name
	 * \param value value to store
	 */
	void SaveLongValue(const wxString &name, long value);
	
	/**
	 * \brief convinience methods to retrieve a single long value stored using
	 * the 'SaveLongValue()' method 
	 * \param name variable name
	 * \param value value
	 * \return return true on success, false otherwise
	 */
	bool GetLongValue(const wxString &name, long &value);
	
	/**
	 * \brief get string from the configuration identified by key
	 * \param key key identifiying the string
	 * \return wxEmptyString or the value
	 */
	wxString GetStringValue(const wxString &key);
	
	/**
	 * \brief 
	 * \param key
	 * \param value
	 */
	void SaveStringValue(const wxString &key, const wxString &value);
	
private:
	EditorConfig();
	virtual ~EditorConfig();
	wxXmlNode *GetLexerNode(const wxString& lexer);
};

typedef Singleton<EditorConfig> EditorConfigST;
#endif // LITEEDITOR_EDITOR_CONFIG_H

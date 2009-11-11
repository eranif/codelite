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

class SimpleLongValue : public SerializedObject
{
	long m_value;
public:
	SimpleLongValue();
	~SimpleLongValue();

	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);

	//Setters
	void SetValue(const long& value) {
		this->m_value = value;
	}
	//Getters
	const long& GetValue() const {
		return m_value;
	}
};

class SimpleStringValue : public SerializedObject
{
	wxString m_value;
public:
	SimpleStringValue();
	~SimpleStringValue();

	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);

	void SetValue(const wxString& value) {
		this->m_value = value;
	}
	const wxString& GetValue() const {
		return m_value;
	}
};

class SimpleRectValue : public SerializedObject
{
	wxRect m_rect;

public:
	SimpleRectValue();
	~SimpleRectValue();

	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);

	void SetRect(const wxRect& rect) {
		this->m_rect = rect;
	}
	const wxRect& GetRect() const {
		return m_rect;
	}
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
	wxXmlDocument*                   m_doc;
	wxFileName                       m_fileName;
	std::map<wxString, LexerConfPtr> m_lexers;
	bool                             m_transcation;

	static wxString m_svnRevision;
	static wxString m_version;

	bool DoSave() const;
	bool DoLoadDefaultSettings();

public:

	//load lexers again, based on the active theme
	void LoadLexers(bool loadDefault);
	static void Init(const wxChar *revision, const wxChar* version) {
		m_svnRevision  = revision;
		m_version      = version;
	}

public:
	typedef std::map<wxString, LexerConfPtr>::const_iterator ConstIterator;

	void Begin();
	void Save();

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
	bool IsOk() const {
		return m_doc->IsOk();
	}

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
	 * get an array of recently opened items e.g. workspaces
	 * \param files  [output] a place holder for the output
	 * \param nodename  the type of item to get
	 */
	void GetRecentItems(wxArrayString &files, const wxString nodeName);

	/**
	 * Set an array of recently opened items e.g. workspaces
	 * \param files  list of files to save
	 * \param nodename  the type of item to set
	 */
	void SetRecentItems(const wxArrayString &files, const wxString nodeName);

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

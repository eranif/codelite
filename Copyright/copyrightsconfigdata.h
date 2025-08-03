//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : copyrightsconfigdata.h
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
#ifndef __copyrightsconfigdata__
#define __copyrightsconfigdata__

#include "serialized_object.h"

class CopyrightsConfigData : public SerializedObject
{
	wxString m_templateFilename;
	wxString m_fileMasking;
	bool m_backupFiles;
	wxString m_ignoreString;

public:
	CopyrightsConfigData();
	virtual ~CopyrightsConfigData() = default;

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);


	//Setters
	void SetFileMasking(const wxString& fileMasking) {
		this->m_fileMasking = fileMasking;
	}
	void SetTemplateFilename(const wxString& templateFilename) {
		this->m_templateFilename = templateFilename;
	}
	void SetBackupFiles(bool backupFiles) { this->m_backupFiles = backupFiles; }

	//Getters
	const wxString& GetFileMasking() const {
		return m_fileMasking;
	}
	const wxString& GetTemplateFilename() const {
		return m_templateFilename;
	}
	bool GetBackupFiles() const { return m_backupFiles; }
	void SetIgnoreString(const wxString& ignoreString) {
		this->m_ignoreString = ignoreString;
	}
	const wxString& GetIgnoreString() const {
		return m_ignoreString;
	}
};
#endif // __copyrightsconfigdata__

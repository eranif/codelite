#ifndef __copyrightsconfigdata__
#define __copyrightsconfigdata__

#include "serialized_object.h"

class CopyrightsConfigData : public SerializedObject {
	wxString m_templateFilename;
	wxString m_fileMasking;
	bool m_backupFiles;
	
public:
	CopyrightsConfigData();
	virtual ~CopyrightsConfigData();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	
	
	//Setters
	void SetFileMasking(const wxString& fileMasking) {this->m_fileMasking = fileMasking;}
	void SetTemplateFilename(const wxString& templateFilename) {this->m_templateFilename = templateFilename;}
	void SetBackupFiles(const bool& backupFiles) {this->m_backupFiles = backupFiles;}
	
	//Getters
	const wxString& GetFileMasking() const {return m_fileMasking;}
	const wxString& GetTemplateFilename() const {return m_templateFilename;}
	const bool& GetBackupFiles() const {return m_backupFiles;}
	
};
#endif // __copyrightsconfigdata__

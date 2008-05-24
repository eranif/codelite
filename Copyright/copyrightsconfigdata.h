#ifndef __copyrightsconfigdata__
#define __copyrightsconfigdata__

#include "serialized_object.h"

class CopyrightsConfigData : public SerializedObject {
	wxString m_templateFilename;
	wxString m_fileMasking;
public:
	CopyrightsConfigData();
	virtual ~CopyrightsConfigData();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	
	
	//Setters
	void SetFileMasking(const wxString& fileMasking) {this->m_fileMasking = fileMasking;}
	void SetTemplateFilename(const wxString& templateFilename) {this->m_templateFilename = templateFilename;}
	//Getters
	const wxString& GetFileMasking() const {return m_fileMasking;}
	const wxString& GetTemplateFilename() const {return m_templateFilename;}
	
};
#endif // __copyrightsconfigdata__

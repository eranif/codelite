#ifndef __tagsmanagementconf__
#define __tagsmanagementconf__

#include "serialized_object.h"

class TagsManagementConf : public SerializedObject
{
	wxArrayString m_files;
public:
	TagsManagementConf();
	virtual ~TagsManagementConf();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	void SetFiles(const wxArrayString& files) {
		this->m_files = files;
	}
	const wxArrayString& GetFiles() const {
		return m_files;
	}
};
#endif // __tagsmanagementconf__

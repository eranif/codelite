#ifndef __importfilessettings__
#define __importfilessettings__
#include "serialized_object.h"


enum 
{
	IFS_INCLUDE_FILES_WO_EXT = 0x00000001
};

class ImportFilesSettings : public SerializedObject 
{
	wxString m_fileMask;
	size_t m_flags;
	
public:
	ImportFilesSettings();
	virtual ~ImportFilesSettings();

	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	
	//Setters
	void SetFileMask(const wxString& fileMask) {this->m_fileMask = fileMask;}
	void SetFlags(const size_t& flags) {this->m_flags = flags;}
	//Getters
	const wxString& GetFileMask() const {return m_fileMask;}
	const size_t& GetFlags() const {return m_flags;}
	
};
#endif // __importfilessettings__

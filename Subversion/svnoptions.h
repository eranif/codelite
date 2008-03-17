#ifndef SVNOPTIONS_H
#define SVNOPTIONS_H

#include "serialized_object.h"

enum SvnOptionsFlags {
	SvnUseIcons 			= 0x00000001,
	SvnKeepIconsUpdated  	= 0x00000002,
	SvnAutoAddFiles 		= 0x00000004,
	SvnUpdateAfterSave 		= 0x00000008,
};

class SvnOptions : public SerializedObject {
	size_t m_flags;
	wxString m_exePath;
	wxString m_pattern;
	
public:
	SvnOptions();
	virtual ~SvnOptions();

	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);

	const size_t& GetFlags() const {return m_flags;}
	void SetFlags(const size_t& flags){m_flags = flags;}

	const wxString &GetExePath() const {return m_exePath;}
	void SetExePath(const wxString &path) {m_exePath = path;}
	
	void SetPattern(const wxString& pattern) {this->m_pattern = pattern;}
	const wxString& GetPattern() const {return m_pattern;}
	
};

#endif //SVNOPTIONS_H

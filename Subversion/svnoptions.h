#ifndef SVNOPTIONS_H
#define SVNOPTIONS_H

#include "serialized_object.h"

#define SVN_UPDATE_ON_SAVE 0x00000001
#define SVN_AUTO_ADD_FILE  0x00000002

#define SVN_DEFAULT_FLAGS SVN_AUTO_ADD_FILE

class SvnOptions : public SerializedObject {
	size_t m_flags;
	size_t m_refreshInterval;
	wxString m_exePath;
public:
	SvnOptions();
	virtual ~SvnOptions();

	void DeSerialize(Archive &arch);
	void Serialize(Archive &arch);

	const size_t& GetFlags() const {return m_flags;}
	void SetFlags(const size_t& flags){m_flags = flags;}

	const wxString &GetExePath() const {return m_exePath;}
	void SetExePath(const wxString &path) {m_exePath = path;}

	const size_t& GetRefreshInterval() const {return m_refreshInterval;}
	void SetRefreshInterval(const size_t& interval){m_refreshInterval = interval;}
};

#endif //SVNOPTIONS_H

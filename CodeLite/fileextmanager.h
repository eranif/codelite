#ifndef __fileextmanager__
#define __fileextmanager__

#include <wx/string.h>
#include <map>

class FileExtManager {

public:
	enum FileType {
		TypeSourceC,
		TypeSourceCpp,
		TypeHeader,
		TypeResource,
		TypeYacc,
		TypeLex,
		TypeQtForm,
		TypeQtResource,
		TypeWorkspace,
		TypeProject,
		TypeFormbuilder,
		TypeOther
	};

private:
	static std::map<wxString, FileType> m_map;

public:
	static FileType GetType(const wxString &filename);
	static void     Init();
};
#endif // __fileextmanager__

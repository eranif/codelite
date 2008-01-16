#ifndef TAGS_OPTIONS_DATA_H
#define TAGS_OPTIONS_DATA_H

#include "serialized_object.h"
#include "wx/filename.h"

#ifndef WXDLLIMPEXP_CL
#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE
#endif // WXDLLIMPEXP_CL

enum CodeCompletionOpts
{
	CC_PARSE_COMMENTS			= 1,
	CC_DISP_COMMENTS			= 2,
	CC_DISP_TYPE_INFO			= 4,
	CC_DISP_FUNC_CALLTIP		= 8,
	CC_LOAD_EXT_DB				= 16,
	CC_LOAD_EXT_DB_TO_MEMORY	= 32,
	CC_PARSE_EXT_LESS_FILES		= 64
};

class WXDLLIMPEXP_CL TagsOptionsData : public SerializedObject
{
	size_t m_ccFlags;
	wxArrayString m_prep;
	wxString m_fileSpec;
	wxArrayString m_languages;
	
public:	
	TagsOptionsData();
	virtual ~TagsOptionsData();

	//setters/getters
	void SetFlags(const size_t &flags) {m_ccFlags = flags;}
	void SetFileSpec(const wxString &filespec){m_fileSpec = filespec;}
	void SetPreprocessor(const wxArrayString& prep){m_prep = prep;}
	void SetLanguages(const wxArrayString &langs){m_languages = langs;}
	void SetLanguageSelection(const wxString &lang);

	const wxArrayString &GetLanguages() const {return m_languages;}
	const wxArrayString& GetPreprocessor() const {return m_prep;}
	const size_t& GetFlags() const {return m_ccFlags;}
	const wxString &GetFileSpec() const{return m_fileSpec;}
	
	//Serialization API
	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);

	wxString ToString() const;
};

#endif //TAGS_OPTIONS_DATA_H

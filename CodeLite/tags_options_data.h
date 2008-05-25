//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : tags_options_data.h              
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
	CC_PARSE_COMMENTS			= 0x00000001,
	CC_DISP_COMMENTS			= 0x00000002,
	CC_DISP_TYPE_INFO			= 0x00000004,
	CC_DISP_FUNC_CALLTIP		= 0x00000008,
	CC_LOAD_EXT_DB				= 0x00000010,
	CC_LOAD_EXT_DB_TO_MEMORY	= 0x00000020,
	CC_PARSE_EXT_LESS_FILES		= 0x00000040,
	CC_COLOUR_VARS				= 0x00000080,
	CC_COLOUR_PROJ_TAGS			= 0x00000100,
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

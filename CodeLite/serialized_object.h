//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : serialized_object.h              
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
 #ifndef SERIALIZED_OBJECT_H
#define SERIALIZED_OBJECT_H

#include "archive.h"

#ifndef WXDLLIMPEXP_CL
#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE
#endif // WXDLLIMPEXP_CL

/**
 * \class SerializedObject
 * \brief an interface to the serialized object, every class who whishes to be 
 * serialized and to be read/write from an xml file, must implement this interface
 * \author Eran
 * \date 07/20/07
 */
class WXDLLIMPEXP_CL SerializedObject
{
public:
	SerializedObject(){}
	virtual ~SerializedObject(){}
	virtual void Serialize(Archive &arch) = 0;
	virtual void DeSerialize(Archive &arch) = 0;
};


class WXDLLIMPEXP_CL TabInfo : public SerializedObject
{
	wxString m_fileName;
	int m_firstVisibleLine;
	int m_currentLine;
	wxArrayString m_bookmarks;
public:
	// setters
	void SetFileName(const wxString& _fileName) {this->m_fileName = _fileName;}
	void SetFirstVisibleLine(const int& _firstVisibleLine) {this->m_firstVisibleLine = _firstVisibleLine;}
	void SetCurrentLine(const int& _currentLine) {this->m_currentLine = _currentLine;}
	void SetBookmarks(const wxArrayString& _bookmarks) {this->m_bookmarks = _bookmarks;}
	//getters
	const wxString& GetFileName() {return this->m_fileName;}
	const int& GetFirstVisibleLine() {return this->m_firstVisibleLine;}
	const int& GetCurrentLine() {return this->m_currentLine;}
	const wxArrayString& GetBookmarks() {return this->m_bookmarks;}
	
	TabInfo();
	virtual ~TabInfo();
	
	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);
};


#endif //SERIALIZED_OBJECT_H

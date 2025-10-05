//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : confformbuilder.h              
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

#ifndef __confformbuilder__
#define __confformbuilder__

#include "serialized_object.h"

class ConfFormBuilder : public SerializedObject
{

	wxString m_fbPath;
	wxString m_command;

public:
	ConfFormBuilder();
	virtual ~ConfFormBuilder() = default;

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);


	void SetCommand(const wxString& command) {
		this->m_command = command;
	}
	void SetFbPath(const wxString& fbPath) {
		this->m_fbPath = fbPath;
	}
	const wxString& GetCommand() const {
		return m_command;
	}
	const wxString& GetFbPath() const {
		return m_fbPath;
	}
};
#endif // __confformbuilder__

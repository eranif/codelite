//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : evnvarlist.h              
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
 #ifndef __evnvarlist__
#define __evnvarlist__
#include "serialized_object.h"

class EvnVarList : public SerializedObject {
	StringMap m_variables;

public:
	EvnVarList();
	virtual ~EvnVarList();
	
	void SetVariables(const StringMap& variables) {this->m_variables = variables;}
	const StringMap& GetVariables() const {return m_variables;}
	
public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
};
#endif // __evnvarlist__

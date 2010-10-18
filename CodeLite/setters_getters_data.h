//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : setters_getters_data.h              
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
 #ifndef SETTERS_GETTERS_DATA_H
#define SETTERS_GETTERS_DATA_H

#include "serialized_object.h"

enum SettersGetters {
	SG_KEEP_UNDERSCORES,
	SG_LOWER_CASE_LETTER,
	SG_REMOVE_M_PREFIX
};

class WXDLLIMPEXP_CL SettersGettersData : public SerializedObject
{
	size_t m_flags;
public:
	SettersGettersData();
	virtual ~SettersGettersData();

	const size_t &GetFlags() const {return m_flags;}
	void SetFlags(const size_t &flags){m_flags = flags;}

	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);
};

#endif // SETTERS_GETTERS_DATA_H



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : comment_creator.h              
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
 #ifndef COMMENT_CREATOT_H
#define COMMENT_CREATOT_H

#include <wx/string.h>

/**
 * \ingroup CodeLite
 * an interface class to the comment creator. 
 *
 * \version 1.0
 * first version
 *
 * \date 07-20-2007
 *
 * \author Eran
 *
 */
class CommentCreator
{
protected:
	wxChar m_keyPrefix;
	
public:
	CommentCreator(wxChar keyPrefix = wxT('\\')) : m_keyPrefix(keyPrefix) {}
	virtual ~CommentCreator(){}
	virtual wxString CreateComment() = 0;
};
#endif //COMMENT_CREATOT_H

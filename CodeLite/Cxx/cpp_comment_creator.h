//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : cpp_comment_creator.h              
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
#ifndef CPP_COMMENT_CREATOR_H
#define CPP_COMMENT_CREATOR_H

#include "codelite_exports.h"
#include "comment_creator.h"
#include "database/entry.h"

#include <wx/string.h>

class WXDLLIMPEXP_CL CppCommentCreator : public CommentCreator
{
	TagEntryPtr m_tag;
public:
	CppCommentCreator(TagEntryPtr tag, wxChar keyPrefix);
	virtual ~CppCommentCreator();
	virtual wxString CreateComment();

private:
	wxString FunctionComment();
};

#endif //CPP_COMMENT_CREATOR_H

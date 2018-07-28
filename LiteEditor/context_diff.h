//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : context_diff.h              
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
#ifndef __contextdiff__
#define __contextdiff__

class clEditor;


class ContextDiff : public ContextBase 
{
public:
	ContextDiff();
    ContextDiff(clEditor *container);
	~ContextDiff();

	virtual void ApplySettings();
	virtual ContextBase* NewInstance(clEditor *container);
    
    // ctrl-click style navigation support
	virtual int  GetHyperlinkRange(int pos, int &start, int &end);
    virtual void GoHyperlink(int start, int end, int type, bool alt);
};

#endif // __contextdiff__

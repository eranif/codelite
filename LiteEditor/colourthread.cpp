//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : colourthread.cpp              
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
 #include "precompiled_header.h"
#include "colourthread.h"
#include "colourrequest.h"

#if 0
ColourThread* ColourThread::ms_instance = 0;

ColourThread* ColourThread::Instance()
{
	if(ms_instance == 0){
		ms_instance = new ColourThread();
	}
	return ms_instance;
}

void ColourThread::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

void ColourThread::ProcessRequest(ThreadRequest *request)
{
}
#endif

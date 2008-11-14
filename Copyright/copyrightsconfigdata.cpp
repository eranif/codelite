//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : copyrightsconfigdata.cpp              
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
 #include "copyrightsconfigdata.h"
CopyrightsConfigData::CopyrightsConfigData()
: m_templateFilename(wxEmptyString)
, m_fileMasking(wxT("*.h;*.hpp;*.h++"))
, m_backupFiles(true)
{
}

CopyrightsConfigData::~CopyrightsConfigData()
{
}

void CopyrightsConfigData::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_templateFilename"), m_templateFilename);
	arch.Read(wxT("m_fileMasking"), m_fileMasking);
	arch.Read(wxT("m_backupFiles"), m_backupFiles);
	arch.Read(wxT("m_ignoreString"), m_ignoreString);
}

void CopyrightsConfigData::Serialize(Archive& arch)
{
	arch.Write(wxT("m_templateFilename"), m_templateFilename);
	arch.Write(wxT("m_fileMasking"), m_fileMasking);
	arch.Write(wxT("m_backupFiles"), m_backupFiles);
	arch.Write(wxT("m_ignoreString"), m_ignoreString);
}

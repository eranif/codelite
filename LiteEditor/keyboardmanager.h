//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : keyboardmanager.h              
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

#ifndef __keyboardmanager__
#define __keyboardmanager__

#include "ikeyboard.h"

class KeyboardManager : public IKeyboard
{

public:
	KeyboardManager();
	virtual ~KeyboardManager();

protected:
	bool Compare(const wxString& accel1, const wxString& accel2) ;
	
public:
	virtual bool AddAccelerator(MenuItemDataMap &accelMap, const MenuItemData& menuItemData);
	virtual void GetAccelerators(MenuItemDataMap &accelMap);
	virtual bool IsDuplicate(const MenuItemDataMap &accelMap, const wxString &accelerator);
	virtual int PopupNewKeyboardShortcutDlg(wxWindow *parent, MenuItemData& menuItemData);
	virtual void Update(const MenuItemDataMap &accelMap);
};
#endif // __keyboardmanager__

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

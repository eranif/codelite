#include "my_dialog.h"
#include "wxc_aui_manager.h"

MyDialog::MyDialog() {}

MyDialog::~MyDialog() { wxcAuiManager::Get().UnInit(this); }

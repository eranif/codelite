#include "my_frame.h"

#include "wxc_aui_manager.h"

MyFrame::MyFrame() {}

MyFrame::~MyFrame() { wxcAuiManager::Get().UnInit(this); }

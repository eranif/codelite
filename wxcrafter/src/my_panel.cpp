#include "my_panel.h"

#include "wxc_aui_manager.h"

MyPanel::MyPanel() {}

MyPanel::~MyPanel() { wxcAuiManager::Get().UnInit(this); }

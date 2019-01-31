#include "my_panel.h"

MyPanel::MyPanel() {}

MyPanel::~MyPanel() { wxcAuiManager::Get().UnInit(this); }

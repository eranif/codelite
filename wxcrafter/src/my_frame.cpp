#include "my_frame.h"

MyFrame::MyFrame() {}

MyFrame::~MyFrame() { wxcAuiManager::Get().UnInit(this); }

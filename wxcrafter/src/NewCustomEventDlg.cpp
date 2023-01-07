#include "NewCustomEventDlg.h"
#include <windowattrmanager.h>

NewCustomEventDlg::NewCustomEventDlg(wxWindow* parent)
    : NewCustomEventBaseDlg(parent)
{
    SetName("NewCustomEventDlg");
    WindowAttrManager::Load(this);
}

NewCustomEventDlg::~NewCustomEventDlg() {}

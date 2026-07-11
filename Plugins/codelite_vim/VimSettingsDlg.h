#ifndef VIMSETTINGSDLG_H
#define VIMSETTINGSDLG_H
#include "VIM.hpp"

class VimSettingsDlg : public VimSettingsDlgBase
{
public:
    VimSettingsDlg(wxWindow* parent);
    virtual ~VimSettingsDlg() = default;
};
#endif // VIMSETTINGSDLG_H

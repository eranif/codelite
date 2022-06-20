#ifndef DAPDEBUGGERSETTINGSDLG_H
#define DAPDEBUGGERSETTINGSDLG_H
#include "UI.h"
#include "clDapSettingsStore.hpp"

class clPropertiesPage;
class DapDebuggerSettingsDlg : public DapDebuggerSettingsDlgBase
{
    clDapSettingsStore& m_store;

public:
    DapDebuggerSettingsDlg(wxWindow* parent, clDapSettingsStore& store);
    virtual ~DapDebuggerSettingsDlg();
};
#endif // DAPDEBUGGERSETTINGSDLG_H

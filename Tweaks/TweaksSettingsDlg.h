#ifndef TWEAKSSETTINGSDLG_H
#define TWEAKSSETTINGSDLG_H
#include "wxcrafter.h"
#include "tweaks_settings.h"

class TweaksSettingsDlg : public TweaksSettingsDlgBase
{
    TweaksSettings m_settings;
public:
    TweaksSettingsDlg(wxWindow* parent);
    virtual ~TweaksSettingsDlg();
    TweaksSettings& GetSettings() {
        return m_settings;
    }

protected:
    virtual void OnImageSelected(wxPropertyGridEvent& event);
    virtual void OnColourChanged(wxPropertyGridEvent& event);
    virtual void OnEnableTweaksCheckboxUI(wxUpdateUIEvent& event);
    virtual void OnEnableTweaks(wxCommandEvent& event);
    virtual void OnEnableTweaksUI(wxUpdateUIEvent& event);
    virtual void OnWorkspaceOpenUI(wxUpdateUIEvent& event);

protected:
    void DoPopulateList();
};
#endif // TWEAKSSETTINGSDLG_H

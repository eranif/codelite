#ifndef TWEAKSSETTINGSDLG_H
#define TWEAKSSETTINGSDLG_H
#include "wxcrafter.h"
#include "tweaks_settings.h"
#include <list>

class wxPGProperty;
class TweaksSettingsDlg : public TweaksSettingsDlgBase
{
    typedef std::list<wxPGProperty*> PropPtrList_t;
    TweaksSettings m_settings;
    PropPtrList_t  m_colourProperties;

public:
    TweaksSettingsDlg(wxWindow* parent);
    virtual ~TweaksSettingsDlg();
    TweaksSettings& GetSettings() {
        return m_settings;
    }

protected:
    virtual void OnEnableColoursTableUI(wxUpdateUIEvent& event);
    virtual void OnResetColours(wxCommandEvent& event);
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

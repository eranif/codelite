#ifndef BUILDTABTOPPANEL_H
#define BUILDTABTOPPANEL_H
#include "wxcrafter.h"

class NewBuildTab;
class BuildTabTopPanel : public BuildTabTopPanelBaseClass
{
private:
    NewBuildTab* m_buildTab;
    
public:
    BuildTabTopPanel(wxWindow* parent);
    virtual ~BuildTabTopPanel();
protected:
    virtual void OnClearBuildOutput(wxCommandEvent& event);
    virtual void OnClearBuildOutputUI(wxUpdateUIEvent& event);
    virtual void OnSaveBuildOutput(wxCommandEvent& event);
    virtual void OnSaveBuildOutputUI(wxUpdateUIEvent& event);
};
#endif // BUILDTABTOPPANEL_H

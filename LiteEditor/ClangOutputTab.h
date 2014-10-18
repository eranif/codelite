#ifndef CLANGOUTPUTTAB_H
#define CLANGOUTPUTTAB_H
#include "wxcrafter.h"
#include "cl_command_event.h"

class ClangOutputTab : public ClangOutputTabBase
{
protected:
    virtual void OnEnableClang(wxCommandEvent& event);
    virtual void OnClearCache(wxCommandEvent& event);
    virtual void OnClearCacheUI(wxUpdateUIEvent& event);
    virtual void OnClearText(wxCommandEvent& event);
    virtual void OnClearTextUI(wxUpdateUIEvent& event);
    virtual void OnPolicy(wxCommandEvent& event);
    virtual void OnPolicyUI(wxUpdateUIEvent& event);
    
    // Event handlers
    void OnBuildStarted(clBuildEvent& event);
    void OnClangOutput(clCommandEvent& event);
    void OnInitDone(wxCommandEvent &event);
    
private:
    void DoClear();
    void DoAppendText(const wxString &text);
    
public:
    ClangOutputTab(wxWindow* parent);
    virtual ~ClangOutputTab();
};
#endif // CLANGOUTPUTTAB_H

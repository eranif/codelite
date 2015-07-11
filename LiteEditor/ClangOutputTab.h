#ifndef CLANGOUTPUTTAB_H
#define CLANGOUTPUTTAB_H
#include "wxcrafter.h"
#include "cl_command_event.h"

class ClangOutputTab : public ClangOutputTabBase
{
protected:
    virtual void OnShowAnnotations(wxCommandEvent& event);
    virtual void OnShowAnnotationsUI(wxUpdateUIEvent& event);
    virtual void OnEnableClangUI(wxUpdateUIEvent& event);
    virtual void OnEnableClang(wxCommandEvent& event);
    virtual void OnClearCache(wxCommandEvent& event);
    virtual void OnClearCacheUI(wxUpdateUIEvent& event);
    virtual void OnClearText(wxCommandEvent& event);
    virtual void OnClearTextUI(wxUpdateUIEvent& event);

    // Event handlers
    void OnBuildStarted(clBuildEvent& event);
    void OnClangOutput(clCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);

private:
    void DoClear();
    void DoAppendText(const wxString& text);

public:
    ClangOutputTab(wxWindow* parent);
    virtual ~ClangOutputTab();
};
#endif // CLANGOUTPUTTAB_H

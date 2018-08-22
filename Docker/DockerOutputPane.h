#ifndef DOCKEROUTPUTPANE_H
#define DOCKEROUTPUTPANE_H

#include "UI.h"
#include "clGenericSTCStyler.h"

class DockerOutputPane : public DockerOutputPaneBase
{
    clGenericSTCStyler::Ptr_t m_styler;

protected:
    void OnKillContainer(wxCommandEvent& event);
    void OnKillContainerUI(wxUpdateUIEvent& event);
    void OnKillAllContainers(wxCommandEvent& event);
    void OnKillAllContainersUI(wxUpdateUIEvent& event);

public:
    DockerOutputPane(wxWindow* parent);
    virtual ~DockerOutputPane();

    void Clear();
    void AddOutputTextWithEOL(const wxString& msg);
    void AddOutputTextRaw(const wxString& msg);
};
#endif // DOCKEROUTPUTPANE_H

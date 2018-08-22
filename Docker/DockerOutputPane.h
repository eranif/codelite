#ifndef DOCKEROUTPUTPANE_H
#define DOCKEROUTPUTPANE_H

#include "UI.h"
#include "clDockerContainer.h"
#include "clGenericSTCStyler.h"

class DockerOutputPane : public DockerOutputPaneBase
{
    clGenericSTCStyler::Ptr_t m_styler;
    clDockerContainer::Vect_t m_containers;

protected:
    void OnKillContainer(wxCommandEvent& event);
    void OnKillContainerUI(wxUpdateUIEvent& event);
    void OnKillAllContainers(wxCommandEvent& event);
    void OnKillAllContainersUI(wxUpdateUIEvent& event);
    void OnRefreshContainersView(wxCommandEvent& event);

public:
    DockerOutputPane(wxWindow* parent);
    virtual ~DockerOutputPane();

    void Clear();
    void AddOutputTextWithEOL(const wxString& msg);
    void AddOutputTextRaw(const wxString& msg);
    void SetContainers(const clDockerContainer::Vect_t& containers);
};
#endif // DOCKEROUTPUTPANE_H

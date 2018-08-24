#ifndef DOCKEROUTPUTPANE_H
#define DOCKEROUTPUTPANE_H

#include "UI.h"
#include "clDockerContainer.h"
#include "clDockerImage.h"
#include "clGenericSTCStyler.h"

class DockerOutputPane : public DockerOutputPaneBase
{
    clGenericSTCStyler::Ptr_t m_styler;
    clDockerContainer::Vect_t m_containers;
    clDockerImage::Vect_t m_images;

protected:
    void OnKillContainer(wxCommandEvent& event);
    void OnKillContainerUI(wxUpdateUIEvent& event);
    void OnKillAllContainers(wxCommandEvent& event);
    void OnAttachTerminal(wxCommandEvent& event);
    void OnKillAllContainersUI(wxUpdateUIEvent& event);
    void OnAttachTerminalUI(wxUpdateUIEvent& event);
    void OnRefreshContainersView(wxCommandEvent& event);
    void OnRefreshImagesView(wxCommandEvent& event);
    void OnClearUnusedImages(wxCommandEvent& event);
    void OnClearUnusedImagesUI(wxUpdateUIEvent& event);
    void OnClearUnusedImagesMenu(wxCommandEvent& event);
    void OnRestartContainer(wxCommandEvent& event);
    void OnStopContainer(wxCommandEvent& event);
    void OnPauseContainer(wxCommandEvent& event);

protected:
    size_t GetSelectedContainers(clDockerContainer::Vect_t& containers);
    void DoContainerCommand(const wxString& command);
    
public:
    DockerOutputPane(wxWindow* parent);
    virtual ~DockerOutputPane();

    void Clear();
    void SelectTab(const wxString& label);
    void AddOutputTextWithEOL(const wxString& msg);
    void AddOutputTextRaw(const wxString& msg);
    void SetContainers(const clDockerContainer::Vect_t& containers);
    void SetImages(const clDockerImage::Vect_t& images);
};
#endif // DOCKEROUTPUTPANE_H

#include "ColoursAndFontsManager.h"
#include "DockerOutputPane.h"
#include "bitmap_loader.h"
#include "clDockerContainer.h"
#include "clDockerEvents.h"
#include "clDockerSettings.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

DockerOutputPane::DockerOutputPane(wxWindow* parent, clDockerDriver::Ptr_t driver)
    : DockerOutputPaneBase(parent)
    , m_driver(driver)
{
    m_stc->SetReadOnly(true);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_stc);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &DockerOutputPane::OnWorkspaceClosed, this);

    m_styler.reset(new clGenericSTCStyler(m_stc));
    {
        wxArrayString words;
        words.Add("successfully");
        m_styler->AddStyle(words, clGenericSTCStyler::kInfo);
    }
    {
        wxArrayString words;
        words.Add("abort ");
        words.Add("Error response from daemon");
        m_styler->AddStyle(words, clGenericSTCStyler::kError);
    }
    {
        wxArrayString words;
        words.Add("SECURITY WARNING");
        m_styler->AddStyle(words, clGenericSTCStyler::kWarning);
    }

    BitmapLoader* bmps = clGetManager()->GetStdIcons();

    // ===-------------------------
    // Output tab
    // ===-------------------------
    m_toolbar->AddTool(wxID_CLEAR, _("Clear"), clGetManager()->GetStdIcons()->LoadBitmap("clear"));
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL,
                    [&](wxCommandEvent& e) {
                        wxUnusedVar(e);
                        Clear();
                    },
                    wxID_CLEAR);
    m_toolbar->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& e) { e.Enable(!m_stc->IsEmpty()); }, wxID_CLEAR);

    // ===-------------------------
    // Containers toolbar
    // ===-------------------------
    m_toolbarContainers->AddTool(wxID_REFRESH, _("Refresh"), bmps->LoadBitmap("debugger_restart"));
    m_toolbarContainers->AddSeparator();
    m_toolbarContainers->AddTool(wxID_CLOSE_ALL, _("Remove all containers"), bmps->LoadBitmap("clean"));

    m_toolbarContainers->Realize();
    m_toolbarContainers->Bind(wxEVT_TOOL, &DockerOutputPane::OnKillAllContainers, this, wxID_CLOSE_ALL);
    m_toolbarContainers->Bind(wxEVT_UPDATE_UI, &DockerOutputPane::OnKillAllContainersUI, this, wxID_CLOSE_ALL);
    m_toolbarContainers->Bind(wxEVT_TOOL, &DockerOutputPane::OnRefreshContainersView, this, wxID_REFRESH);

    // ===-------------------------
    // Images toolbar
    // ===-------------------------
    m_toolbarImages->AddTool(XRCID("refresh_images"), _("Refresh"), bmps->LoadBitmap("debugger_restart"));
    m_toolbarImages->AddTool(XRCID("remove_unused_images"), _("Remove unused images"), bmps->LoadBitmap("clean"), "",
                             wxITEM_DROPDOWN);
    m_toolbarImages->Realize();

    m_toolbarImages->Bind(wxEVT_TOOL, &DockerOutputPane::OnRefreshImagesView, this, XRCID("refresh_images"));
    m_toolbarImages->Bind(wxEVT_TOOL, &DockerOutputPane::OnClearUnusedImages, this, XRCID("remove_unused_images"));
    m_toolbarImages->Bind(wxEVT_UPDATE_UI, &DockerOutputPane::OnClearUnusedImagesUI, this,
                          XRCID("remove_unused_images"));

    m_toolbarImages->Bind(wxEVT_TOOL_DROPDOWN, &DockerOutputPane::OnClearUnusedImagesMenu, this,
                          XRCID("remove_unused_images"));

    m_notebook->Bind(wxEVT_BOOK_PAGE_CHANGED, [&](wxBookCtrlEvent& event) {
        wxString selectedPage = m_notebook->GetPageText(m_notebook->GetSelection());
        if(selectedPage == _("Containers")) {
            m_driver->ListContainers();
        } else if(selectedPage == _("Images")) {
            m_driver->ListImages();
        }
    });
}

DockerOutputPane::~DockerOutputPane()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &DockerOutputPane::OnWorkspaceClosed, this);
}

void DockerOutputPane::Clear()
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetReadOnly(true);
}

void DockerOutputPane::AddOutputTextWithEOL(const wxString& msg)
{
    wxString message = msg;
    if(!message.EndsWith("\n")) { message << "\n"; }
    AddOutputTextRaw(message);
}

void DockerOutputPane::AddOutputTextRaw(const wxString& msg)
{
    m_stc->SetReadOnly(false);
    m_stc->SetInsertionPointEnd();
    m_stc->AddText(msg);
    m_stc->SetReadOnly(true);
    m_stc->ScrollToEnd();
}

void DockerOutputPane::OnKillAllContainers(wxCommandEvent& event)
{
    // Get the list of ids to kill
    wxArrayString ids;
    for(int i = 0; i < m_dvListCtrlContainers->GetItemCount(); ++i) {
        clDockerContainer* cd = reinterpret_cast<clDockerContainer*>(
            m_dvListCtrlContainers->GetItemData(m_dvListCtrlContainers->RowToItem(i)));
        ids.Add(cd->GetId());
    }
    m_driver->RemoveContainers(ids);
}

void DockerOutputPane::OnKillAllContainersUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlContainers->GetItemCount());
}

void DockerOutputPane::OnRefreshContainersView(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_driver->ListContainers();
}

void DockerOutputPane::SetContainers(const clDockerContainer::Vect_t& containers)
{
    m_containers = containers;
    m_dvListCtrlContainers->DeleteAllItems();
    for(size_t i = 0; i < m_containers.size(); ++i) {
        clDockerContainer& container = m_containers[i];
        wxVector<wxVariant> cols;
        cols.push_back(container.GetId());
        cols.push_back(container.GetImage());
        cols.push_back(container.GetCommand());
        cols.push_back(container.GetCreated());
        cols.push_back(container.GetStatus());
        cols.push_back(container.GetPorts());
        cols.push_back(container.GetName());
        m_dvListCtrlContainers->AppendItem(cols, (wxUIntPtr)&container);
    }
}

void DockerOutputPane::SetImages(const clDockerImage::Vect_t& images)
{
    m_images = images;
    m_dvListCtrlImages->DeleteAllItems();
    for(size_t i = 0; i < m_images.size(); ++i) {
        clDockerImage& image = m_images[i];
        wxVector<wxVariant> cols;
        cols.push_back(image.GetId());
        cols.push_back(image.GetRepository());
        cols.push_back(image.GetTag());
        cols.push_back(image.GetCreated());
        cols.push_back(image.GetSize());
        m_dvListCtrlImages->AppendItem(cols, (wxUIntPtr)&image);
    }
}

void DockerOutputPane::SelectTab(const wxString& label) { m_notebook->SetSelection(m_notebook->GetPageIndex(label)); }

void DockerOutputPane::OnRefreshImagesView(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_driver->ListImages();
}

void DockerOutputPane::OnClearUnusedImages(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_driver->ClearUnusedImages();
}

void DockerOutputPane::OnClearUnusedImagesUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlImages->GetItemCount());
}

void DockerOutputPane::OnClearUnusedImagesMenu(wxCommandEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("remove_all_images"), _("Remove all unused images, not just dangling ones"), "", wxITEM_CHECK);
    clDockerSettings s;
    s.Load();
    menu.Check(XRCID("remove_all_images"), s.IsRemoveAllImages());
    menu.Bind(wxEVT_MENU,
              [&](wxCommandEvent& e) {
                  s.SetRemoveAllImages(e.IsChecked());
                  s.Save();
              },
              XRCID("remove_all_images"));
    m_toolbarImages->ShowMenuForButton(event.GetId(), &menu);
}

size_t DockerOutputPane::GetSelectedContainers(clDockerContainer::Vect_t& containers)
{
    containers.clear();
    wxDataViewItemArray items;
    m_dvListCtrlContainers->GetSelections(items);
    for(int i = 0; i < items.GetCount(); ++i) {
        clDockerContainer* cd =
            reinterpret_cast<clDockerContainer*>(m_dvListCtrlContainers->GetItemData(items.Item(i)));
        containers.push_back(*cd);
    }
    return containers.size();
}

void DockerOutputPane::DoContainerCommand(const wxString& command)
{
    clDockerContainer::Vect_t containers;
    if(GetSelectedContainers(containers) != 1) return;
    m_driver->ExecContainerCommand(containers[0].GetName(), command);
    m_driver->ListContainers();
}

void DockerOutputPane::OnContainerContextMenu(wxDataViewEvent& event)
{
    clDockerContainer::Vect_t containers;
    if(GetSelectedContainers(containers) == 0) { return; }

    wxMenu menu;
    menu.Append(wxID_EXECUTE, _("Restart"));
    menu.Bind(wxEVT_MENU,
              [&](wxCommandEvent& event) {
                  for(size_t i = 0; i < containers.size(); ++i) {
                      m_driver->StartContainer(containers[i].GetName());
                  }
              },
              wxID_EXECUTE);
    menu.Append(wxID_STOP, _("Stop"));
    menu.Bind(wxEVT_MENU,
              [&](wxCommandEvent& event) {
                  for(size_t i = 0; i < containers.size(); ++i) {
                      m_driver->StopContainer(containers[i].GetName());
                  }
              },
              wxID_STOP);
    menu.Append(XRCID("pause_container"), _("Pause"));
    menu.Bind(wxEVT_MENU,
              [&](wxCommandEvent& event) {
                  for(size_t i = 0; i < containers.size(); ++i) {
                      m_driver->ExecContainerCommand(containers[i].GetName(), "pause");
                  }
                  m_driver->ListContainers();
              },
              XRCID("pause_container"));
    menu.AppendSeparator();
    menu.Append(XRCID("attach_terminal"), _("Attach Terminal"));
    menu.Bind(wxEVT_MENU,
              [&](wxCommandEvent& event) {
                  wxArrayString names;
                  for(size_t i = 0; i < containers.size(); ++i) {
                      names.Add(containers[i].GetName());
                  }
                  m_driver->AttachTerminal(names);
                  m_driver->ListContainers();
              },
              XRCID("attach_terminal"));
    menu.AppendSeparator();
    menu.Append(XRCID("delete_container"), _("Delete Container"));
    menu.Bind(wxEVT_MENU,
              [&](wxCommandEvent& event) {
                  wxArrayString ids;
                  for(size_t i = 0; i < containers.size(); ++i) {
                      ids.Add(containers[i].GetId());
                  }
                  m_driver->RemoveContainers(ids);
                  m_driver->ListContainers();
              },
              XRCID("delete_container"));
    m_dvListCtrlContainers->PopupMenu(&menu);
}

void DockerOutputPane::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}

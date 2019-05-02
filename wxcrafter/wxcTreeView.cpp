#include "wxcTreeView.h"
#include "EventsEditorDlg.h"
#include "allocator_mgr.h"
#include "wxcrafter.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_defs.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>
#include <globals.h>
#include <plugin.h>
#include <workspace.h>
#include <wx/aui/auibar.h>
#include "drawingutils.h"

const wxEventType wxEVT_SHOW_WXCRAFTER_DESIGNER = wxNewEventType();

BEGIN_EVENT_TABLE(wxcTreeView, wxcTreeViewBaseClass)
EVT_TOOL(ID_OPEN_WXGUI_PROJECT, wxcTreeView::OnOpen)
EVT_UPDATE_UI(ID_OPEN_WXGUI_PROJECT, wxcTreeView::OnOpenUI)
END_EVENT_TABLE()

wxcTreeView::wxcTreeView(wxWindow* parent, wxCrafterPlugin* plugin)
    : wxcTreeViewBaseClass(parent)
    , m_plugin(plugin)
    , m_loadingProject(false)
{
    m_treeControls->SetImageList(Allocator::Instance()->GetImageList());
    m_treeControls->SetSortFunction(nullptr);
    m_treeControls->AddRoot(wxT("wxCrafter Project"), 0, 0);
    m_eventsPane = new EventsEditorPane(m_splitterPageEvents, NULL, plugin);
    m_splitterPageEvents->GetSizer()->Add(m_eventsPane, 1, wxEXPAND | wxALL, 2);

    int treeviewSashPos = wxcSettings::Get().GetTreeviewSashPos();
    if(treeviewSashPos != wxNOT_FOUND) m_splitter347->SetSashPosition(treeviewSashPos);

    EventNotifier::Get()->Connect(wxEVT_WXGUI_PROJECT_SAVED, wxCommandEventHandler(wxcTreeView::OnProjectSaved), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WXGUI_PROJECT_CLOSED, wxCommandEventHandler(wxcTreeView::OnProjectClosed), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(wxcTreeView::OnWorkspaceLoaded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(wxcTreeView::OnWorkspaceClosed), NULL,
                                  this);
}

wxcTreeView::~wxcTreeView()
{
    wxcSettings::Get().SetTreeviewSashPos(m_splitter347->GetSashPosition());
    wxcSettings::Get().Save();

    EventNotifier::Get()->Disconnect(wxEVT_WXGUI_PROJECT_SAVED, wxCommandEventHandler(wxcTreeView::OnProjectSaved),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXGUI_PROJECT_CLOSED, wxCommandEventHandler(wxcTreeView::OnProjectClosed),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(wxcTreeView::OnWorkspaceLoaded),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(wxcTreeView::OnWorkspaceClosed),
                                     NULL, this);
}

void wxcTreeView::OnItemSelected(wxTreeEvent& event) { event.Skip(); }

void wxcTreeView::OnOpen(wxCommandEvent& e)
{
    wxUnusedVar(e);
    LoadProject(wxFileName());
}

void wxcTreeView::OnOpenUI(wxUpdateUIEvent& e) { e.Enable(true); }

void wxcTreeView::LoadProject(const wxFileName& filname)
{
    if(m_loadingProject) return;
    m_loadingProject = true;

    wxCommandEvent showDesignerEvent(wxEVT_SHOW_WXCRAFTER_DESIGNER);
    EventNotifier::Get()->ProcessEvent(showDesignerEvent);

    wxCommandEvent evtOpen(wxEVT_WXC_OPEN_PROJECT);
    if(filname.IsOk()) { evtOpen.SetString(filname.GetFullPath()); }
    EventNotifier::Get()->ProcessEvent(evtOpen);
    m_loadingProject = false;
}

void wxcTreeView::AddForm(const NewFormDetails& fd)
{
    // Top level window
    int imgId = Allocator::Instance()->GetImageId(fd.formType);

    wxcWidget* toplevel = Allocator::Instance()->Create(fd.formType);
    toplevel->SetName(fd.className);
    toplevel->SetFilename(fd.filename);
    toplevel->SetVirtualFolder(fd.virtualFolder);
    toplevel->SetTitle(fd.formTitle);
    toplevel->SetPropertyString(PROP_INHERITED_CLASS, fd.inheritedClassName);

    wxTreeItemId item = m_treeControls->AppendItem(m_treeControls->GetRootItem(), toplevel->GetName(), imgId, imgId,
                                                   new GUICraftItemData(toplevel));
    m_treeControls->SelectItem(item);

    wxCommandEvent evt(wxEVT_REFRESH_DESIGNER);
    EventNotifier::Get()->AddPendingEvent(evt);
}

void wxcTreeView::SaveProject()
{
    wxCommandEvent evtSave(wxEVT_WXC_SAVE_PROJECT);
    EventNotifier::Get()->ProcessEvent(evtSave);
}

void wxcTreeView::CloseProject(bool saveBeforeClose)
{
    if(wxcProjectMetadata::Get().GetProjectFile().IsEmpty()) return;

    if(saveBeforeClose) { SaveProject(); }

    wxCommandEvent closeEvent(wxEVT_WXC_CLOSE_PROJECT);
    EventNotifier::Get()->ProcessEvent(closeEvent);
}

void wxcTreeView::OnProjectSaved(wxCommandEvent& e)
{
    e.Skip();
    m_treeControls->SetItemText(m_treeControls->GetRootItem(), wxcProjectMetadata::Get().GetProjectFile());
}

void wxcTreeView::OnWxcpFileSelected(wxCommandEvent& event)
{
    if(!clCxxWorkspaceST::Get()->IsOpen()) return;

    if(m_comboBoxFiles->GetSelection() == wxNOT_FOUND) return;

    wxFileName wspfile = clCxxWorkspaceST::Get()->GetWorkspaceFileName();
    wxFileName wxcpfile(m_comboBoxFiles->GetStringSelection());
    wxcpfile.MakeAbsolute(wspfile.GetPath());

    LoadProject(wxcpfile.GetFullPath());
    // And restore the file list
    CallAfter(&wxcTreeView::DoRefreshFileList, true);
}

void wxcTreeView::OnRefreshWxcpFiles(wxCommandEvent& event) { DoRefreshFileList(); }

void wxcTreeView::DoRefreshFileList(bool reloadFileList)
{
    wxBusyCursor bc;
    if(!clCxxWorkspaceST::Get()->IsOpen()) return;
    wxString filter = m_comboBoxFiles->GetValue().Lower();
    wxString cbValue = m_comboBoxFiles->GetValue();

    filter.Trim().Trim(false);

    if(m_fileList.IsEmpty() || reloadFileList) {
        wxArrayString allFiles;
        clCxxWorkspaceST::Get()->GetWorkspaceFiles(allFiles);

        wxArrayString options;
        wxFileName wspfile = clCxxWorkspaceST::Get()->GetWorkspaceFileName();
        for(size_t i = 0; i < allFiles.GetCount(); ++i) {
            if(FileExtManager::GetType(allFiles.Item(i)) == FileExtManager::TypeWxCrafter) {
                wxFileName f(allFiles.Item(i));
                f.MakeRelativeTo(wspfile.GetPath());
                options.Add(f.GetFullPath());
            }
        }
        m_fileList.swap(options);
    }

    // Filter non intersting fields
    wxArrayString filteredValues;
    for(size_t i = 0; i < m_fileList.size(); ++i) {
        const wxString lcValue = m_fileList.Item(i).Lower();
        if(reloadFileList || filter.IsEmpty() || lcValue.Contains(filter)) { filteredValues.Add(m_fileList.Item(i)); }
    }

    m_comboBoxFiles->Clear();
    m_comboBoxFiles->Append(filteredValues);
}

void wxcTreeView::OnWorkspaceLoaded(wxCommandEvent& e)
{
    e.Skip();
    DoRefreshFileList();
}

void wxcTreeView::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    m_comboBoxFiles->Clear();
}

void wxcTreeView::OnWorkspaceOpenUI(wxUpdateUIEvent& event) { event.Enable(clCxxWorkspaceST::Get()->IsOpen()); }

void wxcTreeView::OnItemLabelEditEnd(wxTreeEvent& event) // NB I don't think this method is ever called...
{
    wxString new_name = event.GetLabel();
    if(new_name.IsEmpty()) {
        event.Veto();
        return;
    }

    event.Skip();
    // set the new name
    GUICraftItemData* itemData = dynamic_cast<GUICraftItemData*>(m_treeControls->GetItemData(event.GetItem()));
    CHECK_POINTER(itemData);
    CHECK_POINTER(itemData->m_wxcWidget);

    itemData->m_wxcWidget->SetName(new_name);
    m_treeControls->SetItemText(event.GetItem(), new_name);

    wxCommandEvent e(wxEVT_REFRESH_PROPERTIES_VIEW);
    EventNotifier::Get()->AddPendingEvent(e);
}

void wxcTreeView::OnProjectClosed(wxCommandEvent& event)
{
    event.Skip();
    if(!wxcProjectMetadata::Get().IsLoaded()) { m_comboBoxFiles->SetSelection(wxNOT_FOUND); }
}

void wxcTreeView::OnSashPositionChanged(wxSplitterEvent& event)
{
    event.Skip();
    m_eventsPane->SplitterPositionChanged();
}

void wxcTreeView::OnChar(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_DELETE) {
        // Fake the corresponding menu event and throw it at the tree, which is Connect()ed to GUICraftMainPanel
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, ID_DELETE_NODE);
        wxPostEvent(m_treeControls, e);
    }
}

void wxcTreeView::OnWxcpComboxTextEnter(wxCommandEvent& event)
{
    DoRefreshFileList(false);
    m_comboBoxFiles->CallAfter(&wxComboBox::Popup);
}

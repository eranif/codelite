#include "DapDebuggerSettingsDlg.h"

#include "DapLocator.hpp"
#include "DapSettingsPage.hpp"
#include "bitmap_loader.h"
#include "globals.h"
#include "imanager.h"
#include "windowattrmanager.h"

#include <wx/msgdlg.h>

DapDebuggerSettingsDlg::DapDebuggerSettingsDlg(wxWindow* parent, clDapSettingsStore& store)
    : DapDebuggerSettingsDlgBase(parent)
    , m_store(store)
{
    auto image_list = new clBitmapList;

    m_toolbar->AddTool(wxID_NEW, _("New"), image_list->Add("file_new"));
    m_toolbar->AddTool(wxID_DELETE, _("Delete"), image_list->Add("clean"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_FIND, _("Scan"), image_list->Add("find"));
    m_toolbar->AddTool(wxID_HELP, _("Help"), image_list->Add("help"));
    m_toolbar->AssignBitmaps(image_list);

    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_TOOL, &DapDebuggerSettingsDlg::OnNew, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &DapDebuggerSettingsDlg::OnScan, this, wxID_FIND);
    m_toolbar->Bind(wxEVT_TOOL, &DapDebuggerSettingsDlg::OnDelete, this, wxID_DELETE);
    m_toolbar->Bind(wxEVT_TOOL, &DapDebuggerSettingsDlg::OnOpenHelpDocs, this, wxID_HELP);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DapDebuggerSettingsDlg::OnItemsUI, this, wxID_DELETE);

    Initialise();

    ::clSetDialogSizeAndPosition(this, 0.25);
    WindowAttrManager::Load(this);
}

DapDebuggerSettingsDlg::~DapDebuggerSettingsDlg() {}

void DapDebuggerSettingsDlg::Initialise()
{
    m_notebook->DeleteAllPages();
    const auto& entries = m_store.GetEntries();
    for(const auto& vt : entries) {
        m_notebook->AddPage(new DapSettingsPage(m_notebook, m_store, vt.second), vt.first);
    }
}

void DapDebuggerSettingsDlg::OnItemsUI(wxUpdateUIEvent& event) { event.Enable(m_notebook->GetPageCount() > 0); }

void DapDebuggerSettingsDlg::OnOpenHelpDocs(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("https://docs.codelite.org/plugins/dap");
}

void DapDebuggerSettingsDlg::OnDelete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int sel = m_notebook->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }

    wxString name = m_notebook->GetPageText(sel);
    if(::wxMessageBox(_("Delete '") + name + "'", "CodeLite", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING,
                      this) != wxYES) {
        return;
    }

    m_notebook->DeletePage(sel);
    m_store.Delete(name);
}

void DapDebuggerSettingsDlg::OnScan(wxCommandEvent& event)
{
    if(::wxMessageBox(_("Are you sure you want to scan for dap servers? (this will override your current settings)"),
                      "CodeLite", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING) != wxYES) {
        return;
    }

    wxBusyCursor bc;
    wxUnusedVar(event);
    DapLocator locator;
    std::vector<DapEntry> entries;
    if(locator.Locate(&entries) == 0) {
        return;
    }
    m_store.Clear();
    for(const auto& entry : entries) {
        m_store.Set(entry);
    }
    Initialise();
}

void DapDebuggerSettingsDlg::OnNew(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // FIXME
    wxString name = clGetTextFromUser(_("Enter name"), _("New dap server name"), "", wxNOT_FOUND, this);
    if(name.empty()) {
        return;
    }

    DapEntry new_entry;
    new_entry.SetName(name);
    m_store.Set(new_entry);

    m_notebook->AddPage(new DapSettingsPage(m_notebook, m_store, new_entry), new_entry.GetName(), true);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : macbundler.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "macbundler.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include "wx/msgdlg.h"
#include "wx/textfile.h"
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"

static MacBundler* thePlugin = NULL;

// TODO: accept file drops
class IconPicker : public wxPanel
{
    wxBitmap m_image;
    wxString m_image_path;

public:
    IconPicker(wxWindow* parent)
        : wxPanel(parent, wxID_ANY)
    {
        // wxStaticBoxSizer* sizer = new wxStaticBoxSizer(wxHORIZONTAL, this);
        SetMinSize(wxSize(50, 50));
        // SetSizer(sizer);
    }

    void setImage(wxString path)
    {
        m_image_path = path;

        if(path.EndsWith(wxT(".icns"))) {
            wxExecute(wxT("sips -s format png '") + path + wxT("' --out /tmp/tmpicon.png"), wxEXEC_SYNC);
            path = wxT("/tmp/tmpicon.png");
        }

        m_image.LoadFile(path, wxBITMAP_TYPE_ANY);

        if(m_image.IsOk()) {
            if(m_image.GetWidth() > 50 or m_image.GetHeight() > 50) {
                wxImage tmp = m_image.ConvertToImage();
                tmp.Rescale(50, 50, wxIMAGE_QUALITY_HIGH);
                m_image = wxBitmap(tmp);
            }

            const int w = m_image.GetWidth();
            const int h = m_image.GetHeight();
            SetMinSize(wxSize(w + 20, h + 20));
            SetMaxSize(wxSize(w + 20, h + 20));
            Refresh(); // repaint needed to see change
        } else {
            wxMessageBox(_("Failed to load image"));
        }
    }

    void paintEvent(wxPaintEvent& evt)
    {
        wxPaintDC dc(this);

        if(m_image.IsOk()) { dc.DrawBitmap(m_image, 10, 10, false); }
    }

    void doubleClick(wxMouseEvent& evt)
    {
        wxString filename = wxFileSelector(_("Choose the icon file"));
        if(not filename.IsEmpty()) { setImage(filename); }
    }

    wxString getImagePath() { return m_image_path; }

    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(IconPicker, wxPanel)
// catch paint events
EVT_PAINT(IconPicker::paintEvent)
EVT_LEFT_DCLICK(IconPicker::doubleClick)
END_EVENT_TABLE()

class BundleConfigDialog : public wxDialog
{
    bool m_accepted;
    wxCheckListBox* m_choices_widget;
    wxCheckBox* m_info_plist_cb;
    wxCheckBox* m_icon_cb;
    IconPicker* m_icon_picker;

    wxTextCtrl* m_get_info_string;
    wxTextCtrl* m_version;
    wxTextCtrl* m_icon_file;
    wxTextCtrl* m_identifier;
    wxTextCtrl* m_signature;
    IManager* m_pluginManager;

    wxString m_project_name;

public:
    BundleConfigDialog(ProjectPtr project, wxWindow* parent, const wxArrayString& choices, IManager* manager)
        : wxDialog(parent, wxID_ANY, _("Mac Bundler Configuration"), wxDefaultPosition, wxDefaultSize,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
        , m_pluginManager(manager)
    {
        m_accepted = false;

        m_project_name = project->GetName();

        wxStaticText* titleLabel = new wxStaticText(this, wxID_ANY, _("Choose which target(s) to \"bundle-ize\""));
        m_choices_widget = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
        m_info_plist_cb = new wxCheckBox(this, wxID_ANY, _("Generate Info.plist file"));
        m_get_info_string = new wxTextCtrl(this, wxID_ANY, m_project_name + wxT(", version 1.0, copyright myself"));
        m_version = new wxTextCtrl(this, wxID_ANY, wxT("1.0"));
        m_icon_file = new wxTextCtrl(this, wxID_ANY, m_project_name + wxT(".icns"));
        m_identifier = new wxTextCtrl(this, wxID_ANY, wxT("com.mycompany.") + m_project_name.Lower());
        m_signature = new wxTextCtrl(this, wxID_ANY, (m_project_name + wxT("????")).Left(4).Upper());
        m_icon_cb = new wxCheckBox(this, wxID_ANY, _("Copy the following icon into the project"));
        wxStaticText* warning =
            new wxStaticText(this, wxID_ANY, _("Warning : applying these changes cannot be undone automatically"));
        wxButton* okBtn = new wxButton(this, wxID_OK, _("Apply changes"));
        wxButton* cancelBtn = new wxButton(this, wxID_CANCEL, _("Cancel"));

        okBtn->SetDefault();
        m_signature->SetMaxLength(4);
        m_info_plist_cb->SetValue(true);
        m_icon_cb->SetValue(true);

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* subsizer = new wxBoxSizer(wxHORIZONTAL);
        wxFlexGridSizer* plistSizer = new wxFlexGridSizer(5, 2, 5, 5);

        plistSizer->AddGrowableCol(1, 1);
        plistSizer->Add(new wxStaticText(this, wxID_ANY, _("Get Info Version String")), 0, wxALIGN_RIGHT);
        plistSizer->Add(m_get_info_string, 1, wxEXPAND);
        plistSizer->Add(new wxStaticText(this, wxID_ANY, _("Version Number")), 0, wxALIGN_RIGHT);
        plistSizer->Add(m_version, 1, wxEXPAND);
        plistSizer->Add(new wxStaticText(this, wxID_ANY, _("Icon File")), 0, wxALIGN_RIGHT);
        plistSizer->Add(m_icon_file, 1, wxEXPAND);
        plistSizer->Add(new wxStaticText(this, wxID_ANY, _("Bundle Identifier")), 0, wxALIGN_RIGHT);
        plistSizer->Add(m_identifier, 1, wxEXPAND);
        plistSizer->Add(new wxStaticText(this, wxID_ANY, _("4-Character Signature")), 0, wxALIGN_RIGHT);
        plistSizer->Add(m_signature, 1, wxEXPAND);

        sizer->Add(titleLabel, 0, wxEXPAND | wxALL, 10);
        sizer->Add(m_choices_widget, 1, wxEXPAND | wxALL, 10);
        sizer->Add(m_info_plist_cb, 0, wxEXPAND | wxALL, 5);
        sizer->Add(plistSizer, 1, wxEXPAND | wxALL, 5);

        sizer->Add(m_icon_cb, 0, wxALL, 10);

        wxStaticBoxSizer* iconBox = new wxStaticBoxSizer(wxHORIZONTAL, this);
        m_icon_picker = new IconPicker(this);
        m_icon_picker->setImage(
            wxT("/System/Library/CoreServices/CoreTypes.bundle/Contents/Resources/GenericApplicationIcon.icns"));
        iconBox->Add(m_icon_picker, 1, wxEXPAND);
        sizer->Add(iconBox, 0, wxALL, 10);

        sizer->Add(warning, 0, wxEXPAND | wxALL, 10);
        subsizer->AddStretchSpacer();
        subsizer->Add(cancelBtn, 0, wxLEFT | wxRIGHT, 5);
        subsizer->Add(okBtn, 0, wxLEFT | wxRIGHT, 5);
        sizer->Add(subsizer, 0, wxEXPAND | wxALL, 10);

        okBtn->Connect(okBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BundleConfigDialog::onOk),
                       NULL, this);
        cancelBtn->Connect(cancelBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
                           wxCommandEventHandler(BundleConfigDialog::onCancel), NULL, this);
        m_info_plist_cb->Connect(m_info_plist_cb->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
                                 wxCommandEventHandler(BundleConfigDialog::onPlistCheckboxPressed), NULL, this);

        this->SetSizerAndFit(sizer);
        this->Centre();

        // Load last stored size & position from the configuration tool
        SetName("BundleConfigDialog");
        WindowAttrManager::Load(this);
    }

    virtual ~BundleConfigDialog() {}

    void onOk(wxCommandEvent& evt)
    {
        m_accepted = true;
        EndModal(GetReturnCode());
    }

    void onCancel(wxCommandEvent& evt)
    {
        m_accepted = false;
        EndModal(GetReturnCode());
    }

    void onPlistCheckboxPressed(wxCommandEvent& evt)
    {
        const bool on = m_info_plist_cb->IsChecked();

        m_get_info_string->Enable(on);
        m_version->Enable(on);
        m_icon_file->Enable(on);
        m_identifier->Enable(on);
        m_signature->Enable(on);
    }

    bool getResults(wxArrayString& out, bool* createInfoPlistFile, bool* createIcon)
    {
        if(not m_accepted) return false;

        *createInfoPlistFile = m_info_plist_cb->IsChecked();
        *createIcon = m_icon_cb->IsChecked();

        const wxArrayString& items = m_choices_widget->GetStrings();
        const int count = items.GetCount();
        for(int n = 0; n < count; n++) {
            if(m_choices_widget->IsChecked(n)) out.Add(items[n]);
        }

        return true;
    }

    wxString getIconDestName() { return m_icon_file->GetValue(); }

    wxString getIconSource() { return m_icon_picker->getImagePath(); }

    void writeInfoPlistFile(wxTextFile& file)
    {
        if(not file.Exists()) {
            wxMessageBox(_("Cannot access or create file!"));
            return;
        }

        file.AddLine(wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
        file.AddLine(wxT("<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
                         "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"));
        file.AddLine(wxT("<plist version=\"1.0\">"));
        file.AddLine(wxT("<dict>\n"));
        file.AddLine(wxT("    <key>CFBundleExecutable</key>"));
        file.AddLine(wxT("    <string>") + m_project_name + wxT("</string>\n"));
        file.AddLine(wxT("    <key>CFBundleGetInfoString</key>"));
        file.AddLine(wxT("    <string>") + m_get_info_string->GetValue() + wxT("</string>\n"));
        file.AddLine(wxT("    <key>CFBundleIconFile</key>"));
        file.AddLine(wxT("    <string>") + m_icon_file->GetValue() + wxT("</string>\n"));
        file.AddLine(wxT("    <key>CFBundleIdentifier</key>"));
        file.AddLine(wxT("    <string>") + m_identifier->GetValue() + wxT("</string>\n"));
        file.AddLine(wxT("    <key>CFBundleSignature</key>"));
        file.AddLine(wxT("    <string>") + m_signature->GetValue() + wxT("</string>\n"));
        file.AddLine(wxT("    <key>CFBundleVersion</key>"));
        file.AddLine(wxT("    <string>") + m_version->GetValue() + wxT("</string>\n"));
        file.AddLine(wxT("    <key>CFBundleInfoDictionaryVersion</key>"));
        file.AddLine(wxT("    <string>6.0</string>\n"));
        file.AddLine(wxT("    <key>CFBundleDevelopmentRegion</key>"));
        file.AddLine(wxT("    <string>English</string>\n"));
        file.AddLine(wxT("    <key>CFBundlePackageType</key>"));
        file.AddLine(wxT("    <string>APPL</string>\n"));
        file.AddLine(wxT("    <key>CFBundleShortVersionString</key>"));
        file.AddLine(wxT("    <string>") + m_version->GetValue() + wxT("</string>\n"));
        file.AddLine(wxT("</dict>"));
        file.AddLine(wxT("</plist>"));

        if(not file.Write()) {
            wxMessageBox(_("Failed to write Info.plist file!"));
            return;
        }
    }
};

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new MacBundler(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Auria"));
    info.SetName(wxT("MacBundler"));
    info.SetDescription(_("MacBundler : manage OS X app bundles"));
    info.SetVersion(wxT("v0.1"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

MacBundler::MacBundler(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Manage OS X app bundles");
    m_shortName = wxT("MacBundler");
}

MacBundler::~MacBundler() {}

void MacBundler::onBundleInvoked_active(wxCommandEvent& evt)
{
    wxString activeProject = m_mgr->GetWorkspace()->GetActiveProjectName();

    if(activeProject == wxEmptyString) {
        wxMessageBox(_("No project is active, cannot continue."));
    } else {
        wxString error;
        ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(activeProject, error);
        if(!project) {
            wxMessageBox(_("Cannot retrieve active project, cannot continue."));
        } else {
            showSettingsDialogFor(project);
        }
    }
}

void MacBundler::onBundleInvoked_selected(wxCommandEvent& evt)
{
    if(not m_mgr->GetWorkspace() or not m_mgr->IsWorkspaceOpen()) return;

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_itemType == ProjectItem::TypeProject) {

        wxString project_name(item.m_text);
        wxString err_msg;

        ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project_name, err_msg);
        if(not proj) return;

        showSettingsDialogFor(proj);
    } else {
        wxMessageBox(_("This menu item can only be invoked when right-clicking a project."));
    }
}

void MacBundler::showSettingsDialogFor(ProjectPtr project)
{
    // project->GetSettings()->GetGlobalSettings();
    // project->GetSettings()->GetBuildConfiguration(name);

    ProjectSettingsCookie cookie;

    ProjectSettingsPtr settings = project->GetSettings();
    if(not settings) {
        wxMessageBox(_("Cannot continue, impossible to access project settings."));
        return;
    }

    std::map<wxString, BuildConfigPtr> configs;
    wxArrayString choices;

    // TODO: allow putting the rules in the config root and not in every target
    BuildConfigPtr buildConfig = settings->GetFirstBuildConfiguration(cookie);
    while(buildConfig) {

        configs[buildConfig->GetName()] = buildConfig;
        choices.Add(buildConfig->GetName());

        buildConfig = settings->GetNextBuildConfiguration(cookie);
    }

    bool accepted = false;
    bool generateInfoPlistFile = false;
    bool generateIcon = false;

    wxArrayString targetsToSet;

    wxString iconName(wxT("icon.icns"));

    {
        BundleConfigDialog configDlg(project, m_mgr->GetTheApp()->GetTopWindow(), choices, m_mgr);
        configDlg.ShowModal();
        accepted = configDlg.getResults(targetsToSet, &generateInfoPlistFile, &generateIcon);
        iconName = configDlg.getIconDestName();

        if(accepted and generateInfoPlistFile) {
            wxFileName projPath = project->GetFileName();
            projPath.SetFullName(wxT(""));
            const wxString projectDirName = projPath.GetFullPath();
            const wxString infoPlistFile = projectDirName + wxT("/Info.plist");

            if(wxFileExists(infoPlistFile)) {
                int out = wxMessageBox(wxString::Format(_("The following file:\n%s\nalready exists, overwrite it?\n"),
                                                        infoPlistFile.c_str()),
                                       _("Warning"), wxYES_NO);
                if(out == wxYES) {
                    wxTextFile file;
                    file.Open(infoPlistFile);
                    file.Clear();
                    configDlg.writeInfoPlistFile(file);
                    file.Close();
                }
            } else {
                wxTextFile file;
                if(not file.Create(infoPlistFile)) {
                    wxMessageBox(_("Could not create Info.plist file\n") + infoPlistFile);
                } else {
                    configDlg.writeInfoPlistFile(file);
                    file.Close();
                }
            }

            if(wxFileExists(infoPlistFile)) {
                // FIXME: if the file was already present, it will be added again and appear twice in the file tree
                wxArrayString paths;
                paths.Add(infoPlistFile);
                m_mgr->CreateVirtualDirectory(project->GetName(), wxT("osx"));
                m_mgr->AddFilesToVirtualFolder(project->GetName() + wxT(":osx"), paths);
            }
        } // nend if create info.plist

        if(accepted and generateIcon) {
            wxString iconSourcePath = configDlg.getIconSource();
            if(not iconSourcePath.IsEmpty()) {
                // sips doesn't like double slashes in path names
                iconSourcePath.Replace(wxT("//"), wxT("/"));

                wxFileName projPath = project->GetFileName();
                projPath.SetFullName(wxT(""));
                const wxString projectDirName = projPath.GetFullPath();
                wxString iconFileDest = projectDirName + wxT("/") + configDlg.getIconDestName();

                // sips doesn't like double slashes in path names
                iconFileDest.Replace(wxT("//"), wxT("/"));

                std::cout << "Copying icon '" << iconSourcePath.mb_str() << "' to project\n";

                if(iconSourcePath.EndsWith(wxT(".icns"))) {
                    if(not wxCopyFile(iconSourcePath, iconFileDest)) { wxMessageBox(_("Sorry, could not copy icon")); }
                } else {
                    wxString cmd =
                        wxT("sips -s format icns '") + iconSourcePath + wxT("' --out '") + iconFileDest + wxT("'");
                    std::cout << cmd.mb_str() << std::endl;
                    wxExecute(cmd, wxEXEC_SYNC);
                    if(not wxFileExists(iconFileDest)) {
                        wxMessageBox(_("Sorry, could not convert selected icon to icns format"));
                    }
                }

                // FIXME: if the file was already present, it will be added again and appear twice in the file tree
                if(wxFileExists(iconFileDest)) {
                    wxArrayString paths;
                    paths.Add(iconFileDest);
                    m_mgr->CreateVirtualDirectory(project->GetName(), wxT("osx"));
                    m_mgr->AddFilesToVirtualFolder(project->GetName() + wxT(":osx"), paths);
                }
            } // end if icon not null
        }     // end if generate icon
    }
    if(!accepted) return;

    for(int n = 0; n < targetsToSet.GetCount(); n++) {
        BuildConfigPtr buildConfig = configs[targetsToSet[n]];

        wxString outputFileName = buildConfig->GetOutputFileName();
        wxString output = wxT("$(ProjectName).app/Contents/MacOS/$(ProjectName)");
        buildConfig->SetOutputFileName(wxT("$(IntermediateDirectory)/") + output);
        buildConfig->SetCommand(wxT("./") + output);

        if(generateInfoPlistFile or generateIcon) {
            // get existing custom makefile targets, if any
            wxString customPreBuild = buildConfig->GetPreBuildCustom();

            wxString deps, rules;
            deps = customPreBuild.BeforeFirst(wxT('\n'));
            rules = customPreBuild.AfterFirst(wxT('\n'));

            rules = rules.Trim();
            rules = rules.Trim(false);

            deps = deps.Trim();
            deps = deps.Trim(false);

            if(generateInfoPlistFile) {
                // augment existing rules with new rules to manage Info.plist file
                deps.Append(wxT(" $(IntermediateDirectory)/$(ProjectName).app/Contents/Info.plist"));
                rules.Append(wxString(wxT("\n## rule to copy the Info.plist file into the bundle\n")) +
                             wxT("$(IntermediateDirectory)/$(ProjectName).app/Contents/Info.plist: Info.plist\n") +
                             wxT("\tmkdir -p '$(IntermediateDirectory)/$(ProjectName).app/Contents' && cp -f "
                                 "Info.plist '$(IntermediateDirectory)/$(ProjectName).app/Contents/Info.plist'"));
            }
            if(generateIcon) {
                // augment existing rules with new rules to manage Info.plist file
                deps.Append(wxT(" $(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/") + iconName);
                rules.Append(
                    wxT("\n## rule to copy the icon file into the "
                        "bundle\n$(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/") +
                    iconName + wxT(": ") + iconName +
                    wxT("\n\tmkdir -p '$(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/' && cp -f ") +
                    iconName + wxT(" '$(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/") + iconName +
                    wxT("'"));
            }

            // set the new rules
            rules = rules.Trim();
            rules = rules.Trim(false);
            deps = deps.Trim();
            deps = deps.Trim(false);

            wxString prebuilstep;
            prebuilstep << deps << wxT("\n");
            prebuilstep << rules;
            prebuilstep << wxT("\n");

            // Set the content only if there is real content to add
            wxString tmpPreBuildStep(prebuilstep);
            tmpPreBuildStep.Trim().Trim(false);
            buildConfig->SetPreBuildCustom(prebuilstep);
        } // end if

        settings->SetBuildConfiguration(buildConfig);

    } // end for

    project->SetSettings(settings);
}

void MacBundler::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void MacBundler::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item =
        new wxMenuItem(menu, wxNewId(), _("Make active project output a bundle"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    // item = new wxMenuItem(menu, wxID_ANY, _("New Class Wizard..."), wxEmptyString, wxITEM_NORMAL);
    // menu->Append(item);
    // item = new wxMenuItem(menu, wxID_ANY, _("New wxWidgets Project Wizard..."), wxEmptyString, wxITEM_NORMAL);
    // menu->Append(item);

    m_mgr->GetTheApp()->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(MacBundler::onBundleInvoked_active), NULL, this);

    pluginsMenu->Append(wxID_ANY, _("MacBundler"), menu);

    /*
            // You can use the below code a snippet:
            wxMenu *menu = new wxMenu();
            wxMenuItem *item(NULL);
            item = new wxMenuItem(menu, XRCID("new_plugin"), _("New CodeLite Plugin Wizard..."), wxEmptyString,
       wxITEM_NORMAL);
            menu->Append(item);
            item = new wxMenuItem(menu, XRCID("new_class"), _("New Class Wizard..."), wxEmptyString, wxITEM_NORMAL);
            menu->Append(item);
            item = new wxMenuItem(menu, XRCID("new_wx_project"), _("New wxWidgets Project Wizard..."), wxEmptyString,
       wxITEM_NORMAL);
            menu->Append(item);
            pluginsMenu->Append(wxID_ANY, _("Gizmos"), menu);
    */
}

void MacBundler::HookPopupMenu(wxMenu* menu, MenuType type)
{
    if(type == MenuTypeFileView_Project) {
        if(!menu->FindItem(XRCID("MACBUNDLER_PROJECT_MENU"))) {
            menu->Append(XRCID("MACBUNDLER_PROJECT_MENU"), _("Make this project output a bundle"), wxEmptyString,
                         wxITEM_NORMAL);
            m_mgr->GetTheApp()->Connect(XRCID("MACBUNDLER_PROJECT_MENU"), wxEVT_COMMAND_MENU_SELECTED,
                                        wxCommandEventHandler(MacBundler::onBundleInvoked_selected), NULL, this);
        }
    }
}

void MacBundler::UnPlug() {}

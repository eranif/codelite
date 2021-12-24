#include "ImportThemesDialog.h"

#include "JSON.h"
#include "fileutils.h"

ImportThemesDialog::ImportThemesDialog(wxWindow* parent)
    : ImportThemesDialogBase(parent)
{
    wxFileName config_file(wxStandardPaths::Get().GetUserDataDir(), "theme-importer.json");
    JSON root(config_file);
    if(root.isOk()) {
        wxString output_directory;
        wxString themes_directory;
        auto json = root.toElement();
        output_directory = json["output_directory"].toString();
        themes_directory = json["themes_directory"].toString();

        m_dirPickerIn->SetPath(themes_directory);
        m_dirPickerOut->SetPath(output_directory);
    }
}

ImportThemesDialog::~ImportThemesDialog()
{
    wxFileName config_file(wxStandardPaths::Get().GetUserDataDir(), "theme-importer.json");
    JSON root(cJSON_Object);
    auto json = root.toElement();
    json.addProperty("output_directory", m_dirPickerOut->GetPath());
    json.addProperty("themes_directory", m_dirPickerIn->GetPath());
    root.save(config_file);
}

wxString ImportThemesDialog::GetInputDirectory() const { return m_dirPickerIn->GetPath(); }

wxString ImportThemesDialog::GetOutputDirectory() const { return m_dirPickerOut->GetPath(); }

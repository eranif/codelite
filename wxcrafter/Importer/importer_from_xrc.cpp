#include "importer_from_xrc.h"

#include "import_dlg.h"
#include "import_from_xrc.h"
#include "wxc_project_metadata.h"

#include <wx/msgdlg.h>
#include <wx/window.h>
#include <wx/xml/xml.h>

namespace ImportFromXrc
{

Importer::Importer(wxWindow* parent)
    : m_Parent(parent)
{
}

bool Importer::ImportProject(ImportDlg::ImportFileData& data, bool showAddToProject) const
{
    ImportDlg dlg(ImportDlg::IPD_XRC, m_Parent, "", showAddToProject);
    if (dlg.ShowModal() != wxID_OK) {
        return false;
    }

    wxString filepath = dlg.GetFilepath();

    if (filepath.empty() || !wxFileExists(filepath)) {
        return false;
    }

    wxXmlDocument doc(filepath);
    if (!doc.IsOk()) {
        wxMessageBox(_("Failed to load the file to import"), wxT("CodeLite"), wxICON_ERROR | wxOK, m_Parent);
        return false;
    }

    wxcWidget::List_t toplevels;
    if (ParseFile(doc, toplevels) && !toplevels.empty()) {
        wxcProjectMetadata::Get().Serialize(toplevels, wxFileName(dlg.GetOutputFilepath()));
        data = dlg.GetData();
        return true;
    }
    return false;
}

bool Importer::ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const
{
    wxXmlNode* toplevelnode = doc.GetRoot()->GetChildren();
    while (toplevelnode) {
        wxString tag = toplevelnode->GetName();
        if (tag != wxT("object")) {
            wxMessageBox(_("This doesn't seem to be a valid XRC file. Aborting."),
                         wxT("CodeLite"),
                         wxICON_ERROR | wxOK,
                         m_Parent);
            return false;
        }

        bool alreadyParented(false);
        wxcWidget* wrapper = ParseNode(toplevelnode, NULL, alreadyParented);
        if (wrapper) {
            toplevels.push_back(wrapper);
        }

        toplevelnode = toplevelnode->GetNext();
    }

    return true;
}

} // namespace ImportFromXrc

#include "importer_from_wxFB.h"

#include "UI/import_dlg.h"
#include "allocator_mgr.h"
#include "controls/Containers/notebook_base_wrapper.h"
#include "controls/Containers/notebook_page_wrapper.h"
#include "import_from_wxFB.h"
#include "wxc_project_metadata.h"
#include "wxgui_helpers.h"
#include "xml/xmlutils.h"

#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/window.h>
#include <wx/xml/xml.h>

namespace ImportFromwxFB
{

Importer::Importer(wxWindow* parent)
    : m_Parent(parent)
{
}

bool Importer::ImportProject(ImportDlg::ImportFileData& data, const wxString& sourceFile, bool showAddToProject) const
{
    ImportDlg dlg(ImportDlg::IPD_FB, m_Parent, sourceFile, showAddToProject);

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
    wxString abortmsg(_("This doesn't seem to be a valid wxFormBuilder project file. Aborting."));
    wxXmlNode* wxFBProj = XmlUtils::FindFirstByTagName(doc.GetRoot(), wxT("object"));
    if (!wxFBProj) {
        wxMessageBox(abortmsg, wxT("CodeLite"), wxICON_ERROR | wxOK, m_Parent);
        return false;
    }

    if (XmlUtils::ReadString(wxFBProj, wxT("class")) != "Project") {
        wxMessageBox(abortmsg, wxT("CodeLite"), wxICON_ERROR | wxOK, m_Parent);
        return false;
    }

    // The file starts with cruft, which we ignore. e.g.
    //    <wxFormBuilder_Project>
    //        <FileVersion major="1" minor="11" />
    //        <object class="Project" expanded="1">
    //            <property name="class_decoration"></property>
    //            <property name="code_generation">C++</property>
    //            <property name="disconnect_events">1</property>
    //     etc etc
    //  The only ones that sound relevant are 'name', 'path' and possibly 'file'. Maybe a ToDo
    // Everything is parented by <object class="Project"...> though, so we need to get that

    // The first interesting thing is a child node of Project e.g. <object class="Dialog" expanded="1">
    wxXmlNode* toplevelnode = XmlUtils::FindFirstByTagName(wxFBProj, wxT("object"));
    if (!toplevelnode) {
        wxMessageBox(abortmsg, wxT("CodeLite"), wxICON_ERROR | wxOK, m_Parent);
        return false;
    }

    while (toplevelnode) {
        wxString tag = toplevelnode->GetName();
        if (tag != wxT("object")) {
            wxMessageBox(abortmsg, wxT("CodeLite"), wxICON_ERROR | wxOK, m_Parent);
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

wxcWidget* Importer::ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const
{
    wxcWidget* wrapper = NULL;

    wxString tag = node->GetName();
    wxCHECK_MSG(tag == wxT("object"), NULL, wxT("Passed a node that isn't an object"));

    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    wxCHECK_MSG(!classname.empty(), NULL, wxT("Object node doesn't have a 'class' attribute"));

    wxXmlNode* sizeritemnode = node;
    wxXmlNode* gbsizeritemnode = node;
    wxXmlNode* booknode = NULL;

    if (classname == wxT("sizeritem")) {
        // See the comment in import_from_xrc. Replace node with the contained object node
        // We'll process sizeritemnode node separately later
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if (!node) {
            // I don't think this can happen, but...
            return NULL;
        }

        // Read the class of the new node
        classname = XmlUtils::ReadString(node, wxT("class"));
    }

    else if (classname == "gbsizeritem") {
        // wxFB deals with this separately
        // We'll process gbsizeritem node separately later
        gbsizeritemnode = node;
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if (!node) {
            // I don't think this can happen, but...
            return NULL;
        }

        // Read the class of the new node
        classname = XmlUtils::ReadString(node, wxT("class"));
    }

    else if (classname.Contains(wxT("bookpage"))) {
        // See the above comment. Replace node with the contained object node
        // We'll process booknode node separately later
        booknode = node;
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if (!node) {
            // I don't think this can happen, but...
            return NULL;
        }

        // Read the class of the new node
        classname = XmlUtils::ReadString(node, wxT("class"));
    }

    else if (classname == "splitteritem") {
        // wxFB uses this for a splitterwindow page, but it holds no extra info :/
        // so just substitute the contained object
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if (!node) {
            // I don't think this can happen, but...
            return NULL;
        }
        classname = XmlUtils::ReadString(node, wxT("class"));
    }

    int Id = wxcWidget::StringToId(classname);
    if (Id == wxNOT_FOUND) {
        wxLogWarning(wxString::Format(_("Can't import unknown class %s from wxFB"), classname));
        return NULL;
    }

    if ((Id == ID_WXPANEL) && (parentwrapper == NULL)) {
        Id = ID_WXPANEL_TOPLEVEL;

    } else if (Id == ID_WXPANEL && parentwrapper && parentwrapper->GetType() == ID_WXSPLITTERWINDOW) {
        Id = ID_WXSPLITTERWINDOW_PAGE;
    }

    if (booknode) {
        // Correctly label a wxPanel that's really a notebookpage
        Id = ID_WXPANEL_NOTEBOOK_PAGE;
    }

    wrapper = wxcWidget::Create(Id);
    wxCHECK_MSG(wrapper, NULL, wxT("Failed to create a wrapper"));

    wrapper->LoadPropertiesFromwxFB(node);

    GetSizeritemContents(sizeritemnode, wrapper);  // Now we have a valid wrapper, we can store the sizeritem info in it
    GetGridBagSizerItem(gbsizeritemnode, wrapper); // Similarly with these, which wxFB stores separately

    int depth = 0;
    if (booknode) {
        // For book pages, extract any info from the <foobookpage> node
        NotebookPageWrapper* nbwrapper = dynamic_cast<NotebookPageWrapper*>(wrapper);
        wxCHECK_MSG(nbwrapper, NULL, wxT("A booknode which has no NotebookPageWrapper"));
        GetBookitemContents(booknode, nbwrapper, depth);

        // If this is a treebook subpage, it needs to be parented by a page, not the book
        // depth will only be >0 in that situation
        if (depth) {
            NotebookBaseWrapper* nb = dynamic_cast<NotebookBaseWrapper*>(parentwrapper);
            wxCHECK_MSG(nb, NULL, wxT("treebookpage 'parent' not a book"));

            wxcWidget* item = nb->GetChildPageAtDepth(depth - 1);
            if (item) {
                item->AddChild(wrapper);
                alreadyParented = true;
            } else {
                // If the XRC held an invalid depth (i.e. there was no suitable parent found)
                // just mention the fact, and add the page anyway at depth 0
                wxLogWarning("Trying to add to a wxTreebook a page with invalid depth");
            }
        }
    }

    wxXmlNode* child = node->GetChildren();
    while (child) {
        wxString childname(child->GetName());
        if (childname == wxT("object")) {
            bool alreadyParented(false);
            wxcWidget* childwrapper = ParseNode(child, wrapper, alreadyParented);
            if (childwrapper && !alreadyParented) {
                wrapper->AddChild(childwrapper);
            }
        }

        child = child->GetNext();
    }

    return wrapper;
}

void Importer::GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const
{
    wrapper->ClearSizerAll(); // otherwise the default ones will remain

    // Unlike XRC, wxFB stores everything in <property name=foo>value</property> nodes
    wxXmlNode* child = node->GetChildren();
    while (child) {
        wxString childname(child->GetName());
        if (childname == wxT("property")) {

            if (XmlUtils::ReadString(child, wxT("name")) == "flag") {
                wxString flags = child->GetNodeContent();
                // We must cope with wxC's American misspellings...
                flags.Replace("wxALIGN_CENTRE", "wxALIGN_CENTER");

                // if left|right|top|bottom add wxALL. Duplication does no harm here
                if (flags.Contains("wxLEFT") && flags.Contains("wxRIGHT") && flags.Contains("wxTOP") &&
                    flags.Contains("wxBOTTOM")) {
                    flags << "|wxALL";
                }

                wxArrayString flagsarray = wxCrafter::Split(flags, "|");

                // Eran: If flagsarray contains 'wxALL' - make sure all the other four stars are also there...
                if (flagsarray.Index("wxALL") != wxNOT_FOUND) {
                    flagsarray.Add("wxLEFT");
                    flagsarray.Add("wxRIGHT");
                    flagsarray.Add("wxTOP");
                    flagsarray.Add("wxBOTTOM");
                    flagsarray = wxCrafter::MakeUnique(flagsarray);
                }

                for (size_t n = 0; n < flagsarray.GetCount(); ++n) {
                    wrapper->EnableSizerFlag(flagsarray.Item(n), true);
                }
            }

            if (XmlUtils::ReadString(child, wxT("name")) == "proportion") {
                wxString proportion = child->GetNodeContent();
                wrapper->SizerItem().SetProportion(wxCrafter::ToNumber(proportion, 0));
            }

            if (XmlUtils::ReadString(child, wxT("name")) == "border") {
                wxString border = child->GetNodeContent();
                wrapper->SizerItem().SetBorder(wxCrafter::ToNumber(border, 0));
            }
        }

        child = child->GetNext();
    }
}

void Importer::GetGridBagSizerItem(const wxXmlNode* node, wxcWidget* wrapper) const
{
    // The gbsizeritem node contains 'flag' and 'border' (though not 'proportion') so process them with
    // GetSizeritemContents()
    GetSizeritemContents(node, wrapper);

    wxString row, column, rowspan, colspan; // wxFB holds these individually; wxC needs pairs

    wxXmlNode* child = node->GetChildren();
    while (child) {
        wxString childname(child->GetName());
        if (childname == wxT("property")) {
            if (XmlUtils::ReadString(child, wxT("name")) == "row") {
                row = child->GetNodeContent();
            }

            if (XmlUtils::ReadString(child, wxT("name")) == "column") {
                column = child->GetNodeContent();
            }
            if (XmlUtils::ReadString(child, wxT("name")) == "rowspan") {
                rowspan = child->GetNodeContent();
            }

            if (XmlUtils::ReadString(child, wxT("name")) == "colspan") {
                colspan = child->GetNodeContent();
            }
        }

        child = child->GetNext();
    }
    wrapper->SetGbPos(row + ',' + column);
    wrapper->SetGbSpan(rowspan + ',' + colspan);
}

void Importer::GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const
{
    wxString classname = XmlUtils::ReadString(node, wxT("class"));

    wxXmlNode* propertynode = XmlUtils::FindNodeByName(node, "property", "select");
    if (propertynode) {
        wxString selected = propertynode->GetNodeContent();
        if (selected == "1") {
            wrapper->SetSelected(true);
        }
    }

    propertynode = XmlUtils::FindNodeByName(node, "property", "label");
    if (propertynode) {
        wxString label = propertynode->GetNodeContent();
        PropertyBase* prop = wrapper->GetProperty(PROP_LABEL);
        if (prop) {
            prop->SetValue(label);
        }
    }

    if (classname != "choicebookpage") { // which don't have bitmaps
        propertynode = XmlUtils::FindNodeByName(node, "property", "bitmap");
        if (propertynode) {
            wxString bitmap = propertynode->GetNodeContent();
            ProcessBitmapProperty(bitmap, wrapper);
        }
    }
    /* wxFB doesn't seem to do treebooks
        if (classname == "treebookpage") {
            propertynode = XmlUtils::FindNodeByName(node, "property", "depth");
            if (propertynode) {
                depth = wxCrafter::ToNumber(propertynode->GetNodeContent(), 0);
            }
        }*/
}

} // namespace ImportFromwxFB

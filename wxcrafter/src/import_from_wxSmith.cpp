#include "import_from_wxSmith.h"
#include "allocator_mgr.h"
#include "file_logger.h"
#include "import_dlg.h"
#include "import_from_xrc.h"
#include "notebook_base_wrapper.h"
#include "notebook_page_wrapper.h"
#include "wxc_project_metadata.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/window.h>
#include <wx/xml/xml.h>

std::map<wxString, wxString> ImportFromwxSmith::sm_eventMap;

ImportFromwxSmith::ImportFromwxSmith(wxWindow* parent)
    : m_Parent(parent)
{
}

ImportFromwxSmith::~ImportFromwxSmith() {}

bool ImportFromwxSmith::ImportProject(ImportDlg::ImportFileData& data, const wxString& sourceFile) const
{
    ImportDlg dlg(ImportDlg::IPD_Smith, m_Parent, sourceFile);

    if(dlg.ShowModal() != wxID_OK) { return false; }

    wxString filepath = dlg.GetFilepath();
    if(filepath.empty() || !wxFileExists(filepath)) { return false; }

    wxXmlDocument doc(filepath);
    if(!doc.IsOk()) {
        wxMessageBox(_("Failed to load the file to import"), _("CodeLite"), wxICON_ERROR | wxOK, m_Parent);
        return false;
    }

    wxcWidget::List_t toplevels;
    if(ParseFile(doc, toplevels) && !toplevels.empty()) {
        wxcProjectMetadata::Get().Serialize(toplevels, wxFileName(dlg.GetOutputFilepath()));
        data = dlg.GetData();
        return true;
    }
    return false;
}

bool ImportFromwxSmith::ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const
{
    // <?xml version="1.0" encoding="utf-8" ?>
    // <wxsmith>
    //   <object class="wxFrame" name="Foo">
    wxXmlNode* toplevelnode = doc.GetRoot()->GetChildren();
    while(toplevelnode) {
        wxString tag = toplevelnode->GetName();
        if(tag != wxT("object")) {
            wxMessageBox(_("This doesn't seem to be a valid wxSmith file. Aborting."), _("CodeLite"),
                         wxICON_ERROR | wxOK, m_Parent);
            return false;
        }

        bool alreadyParented(false);
        wxcWidget* wrapper = ParseNode(toplevelnode, NULL, alreadyParented);
        if(wrapper) { toplevels.push_back(wrapper); }

        toplevelnode = toplevelnode->GetNext();
    }

    return true;
}

wxcWidget* ImportFromwxSmith::ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const
{
    wxcWidget* wrapper = NULL;
    wxXmlNode* sizeritemnode = node;
    wxXmlNode* buttonnode = NULL;
    wxXmlNode* booknode = NULL;

    wxString tag = node->GetName();
    wxCHECK_MSG(tag == wxT("object"), NULL, wxT("Passed a node that isn't an object"));

    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    wxCHECK_MSG(!classname.empty(), NULL, wxT("Object node doesn't have a 'class' attribute"));

    if(classname == wxT("sizeritem")) {
        // See the ImportFromXRC comment. Replace node with the contained object node
        // We'll process sizeritemnode node separately later
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if(!node) {
            // I don't think this can happen, but...
            return NULL;
        }

        // Read the class of the new node
        classname = XmlUtils::ReadString(node, wxT("class"));
    }

    else if(classname.Contains(wxT("bookpage"))) {
        // See the above comment. Replace node with the contained object node
        // We'll process booknode node separately later
        booknode = node;
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if(!node) {
            // I don't think this can happen, but...
            return NULL;
        }

        // Read the class of the new node
        classname = XmlUtils::ReadString(node, wxT("class"));
    }

    else if(classname == wxT("button")) {
        buttonnode = node;
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if(!node) {
            return NULL;
        }

        classname = XmlUtils::ReadString(node, wxT("class"));
        if(classname == "wxButton") { // and it certainly should be
            classname = "stdbutton";
        }
    }

    int Id = Allocator::StringToId(classname);
    if(Id == wxNOT_FOUND) {
        wxLogWarning(wxString::Format(_("Can't import unknown class %s in from wxSmith"), classname));
        return NULL;
    }

    if((Id == ID_WXPANEL) && (parentwrapper == NULL)) { Id = ID_WXPANEL_TOPLEVEL; }

    // classname == "separator", which gives and Id of ID_WXMENUITEM, could be a menu or toolbar one
    if((classname == "separator") && (parentwrapper && parentwrapper->GetWidgetType() == TYPE_TOOLBAR)) {
        Id = ID_WXTOOLBARITEM;
    }

    if(booknode) {
        // Correctly label a wxPanel that's really a notebookpage
        Id = ID_WXPANEL_NOTEBOOK_PAGE;
    }

    wrapper = Allocator::Instance()->Create(Id);
    wxCHECK_MSG(wrapper, NULL, wxT("Failed to create a wrapper"));

    wrapper->LoadPropertiesFromwxSmith(node);

    GetSizeritemContents(sizeritemnode, wrapper); // Now we have a valid wrapper, we can store the sizeritem info in it

    int depth = 0;
    if(booknode) {
        // For book pages, extract any info from the <foobookpage> node
        NotebookPageWrapper* nbwrapper = dynamic_cast<NotebookPageWrapper*>(wrapper);
        wxCHECK_MSG(nbwrapper, NULL, wxT("A booknode which has no NotebookPageWrapper"));
        GetBookitemContents(booknode, nbwrapper, depth);

        // If this is a treebook subpage, it needs to be parented by a page, not the book
        // depth will only be >0 in that situation
        // NB I can't find a way to create a subpage in wxSmith, but just maybe...
        if(depth) {
            NotebookBaseWrapper* nb = dynamic_cast<NotebookBaseWrapper*>(parentwrapper);
            wxCHECK_MSG(nb, NULL, wxT("treebookpage 'parent' not a book"));

            wxcWidget* item = nb->GetChildPageAtDepth(depth - 1);
            if(item) {
                item->AddChild(wrapper);
                alreadyParented = true;
            } else {
                // If the XRC held an invalid depth (i.e. there was no suitable parent found)
                // just mention the fact, and add the page anyway at depth 0
                wxLogWarning("Trying to add to a wxTreebook a page with invalid depth");
            }
        }
    }

    if(buttonnode) {
        // For wxStdDialogButtonSizer buttons, extract any info from the <buttons> node
        ImportFromXrc::DoProcessButtonNode(buttonnode, wrapper);
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        wxString childname(child->GetName());
        if(childname == wxT("object")) {
            bool alreadyParented(false);
            wxcWidget* childwrapper = ParseNode(child, wrapper, alreadyParented);
            if(childwrapper && !alreadyParented) { wrapper->AddChild(childwrapper); }
        }

        child = child->GetNext();
    }

    return wrapper;
}

void ImportFromwxSmith::GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const
{
    wrapper->ClearSizerAll(); // otherwise the default ones will remain

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("flag"));
    if(propertynode) {
        wxString flags = propertynode->GetNodeContent();
        // We must cope with wxC's American misspellings...
        flags.Replace("wxALIGN_CENTRE", "wxALIGN_CENTER");

        // if left|right|top|bottom add wxALL. Duplication does no harm here
        if(flags.Contains("wxLEFT") && flags.Contains("wxRIGHT") && flags.Contains("wxTOP") &&
           flags.Contains("wxBOTTOM")) {
            flags << "|wxALL";
        }

        wxArrayString flagsarray = wxCrafter::Split(flags, "|");

        // Eran: If flagsarray contains 'wxALL' - make sure all the other four stars are also there...
        if(flagsarray.Index("wxALL") != wxNOT_FOUND) {
            flagsarray.Add("wxLEFT");
            flagsarray.Add("wxRIGHT");
            flagsarray.Add("wxTOP");
            flagsarray.Add("wxBOTTOM");
            flagsarray = wxCrafter::MakeUnique(flagsarray);
        }

        for(size_t n = 0; n < flagsarray.GetCount(); ++n) {
            wrapper->EnableSizerFlag(flagsarray.Item(n), true);
        }
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("option")); // 'option' is XRC's term for proportion :/
    if(propertynode) {
        wxString proportion = propertynode->GetNodeContent();
        wrapper->SizerItem().SetProportion(wxCrafter::ToNumber(proportion, 0));
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("border"));
    if(propertynode) {
        wxString border = propertynode->GetNodeContent();
        wrapper->SizerItem().SetBorder(wxCrafter::ToNumber(border, 0));
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("minsize"));
    if(propertynode) {
        wxString minsize = propertynode->GetNodeContent();
        PropertyBase* prop = wrapper->GetProperty(PROP_MINSIZE);
        if(prop) { prop->SetValue(minsize); }
    }

    // wxSmith stores these individually (like wxFB), but not in a separate node like wxFB
    GetGridBagSizerData(node, wrapper);
}

void ImportFromwxSmith::GetGridBagSizerData(const wxXmlNode* node, wxcWidget* wrapper) const
{
    wxString row, column, rowspan("1"), colspan("1"); // wxSmith, like wxFB, holds these individually; wxC needs pairs
    bool hasPos(false), hasSpan(false);

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("row"));
    if(propertynode) {
        row = propertynode->GetNodeContent();
        hasPos = true;
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("col"));
    if(propertynode) {
        column = propertynode->GetNodeContent();
        hasPos = true;
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("rowspan"));
    if(propertynode) {
        rowspan = propertynode->GetNodeContent();
        hasSpan = true;
    }
    propertynode = XmlUtils::FindFirstByTagName(node, wxT("colspan"));
    if(propertynode) {
        colspan = propertynode->GetNodeContent();
        hasSpan = true;
    }

    if(hasPos) { wrapper->SetGbPos(row + ',' + column); }
    if(hasSpan) { wrapper->SetGbSpan(rowspan + ',' + colspan); }
}

void ImportFromwxSmith::GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const
{
    wxString classname = XmlUtils::ReadString(node, wxT("class"));

    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("selected"));
    if(propertynode) {
        wxString selected = propertynode->GetNodeContent();
        if(selected == "1") { wrapper->SetSelected(true); }
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("label"));
    if(propertynode) {
        wxString label = propertynode->GetNodeContent();
        PropertyBase* prop = wrapper->GetProperty(PROP_LABEL);
        if(prop) { prop->SetValue(label); }
    }

    if(classname != "choicebookpage") { // which don't have bitmaps
        propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
        if(propertynode) {
            ImportFromXrc::ProcessBitmapProperty(propertynode, wrapper, "PROP_BITMAP_PATH", "wxART_OTHER");
        }
    }

    if(classname == "treebookpage") {
        propertynode = XmlUtils::FindFirstByTagName(node, wxT("depth"));
        if(propertynode) { depth = wxCrafter::ToNumber(propertynode->GetNodeContent(), 0); }
    }
}

bool ImportFromwxSmith::GetProject() const { return false; }

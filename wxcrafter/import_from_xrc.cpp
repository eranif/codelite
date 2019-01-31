#include "import_from_xrc.h"
#include "allocator_mgr.h"
#include "file_logger.h"
#include "import_dlg.h"
#include "notebook_base_wrapper.h"
#include "notebook_page_wrapper.h"
#include "wxc_project_metadata.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/window.h>
#include <wx/xml/xml.h>

ImportFromXrc::ImportFromXrc(wxWindow* parent)
    : m_Parent(parent)
{
}

ImportFromXrc::~ImportFromXrc() {}

bool ImportFromXrc::ImportProject(ImportDlg::ImportFileData& data) const
{
    ImportDlg dlg(ImportDlg::IPD_XRC, m_Parent);
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

bool ImportFromXrc::ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const
{
    wxXmlNode* toplevelnode = doc.GetRoot()->GetChildren();
    while(toplevelnode) {
        wxString tag = toplevelnode->GetName();
        if(tag != wxT("object")) {
            wxMessageBox(_("This doesn't seem to be a valid XRC file. Aborting."), _("CodeLite"), wxICON_ERROR | wxOK,
                         m_Parent);
            return false;
        }

        bool alreadyParented(false);
        wxcWidget* wrapper = ParseNode(toplevelnode, NULL, alreadyParented);
        if(wrapper) { toplevels.push_back(wrapper); }

        toplevelnode = toplevelnode->GetNext();
    }

    return true;
}

wxcWidget* ImportFromXrc::ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const
{
    wxcWidget* wrapper = NULL;

    // XRC for some reason treats sizeritems as an object! We'll deal with that below, but cache node first
    // BTW, wxSpacer is an exception: it's called <spacer> but looks like a sizeritem with no contained object
    // BTW2, the buttons in a wxStdButtonSizer are also an exception: each is wrapped in a <button> node that behaves
    // like sizeritem, so treat it as such
    wxXmlNode* sizeritemnode = node;
    wxXmlNode* buttonnode = NULL;

    // For notebooks, both XRC and JSON store their pages as sibling children of the book
    // XRC stores them in a <notebookpage> 'wrapper', similar to the sizeritem ones
    // If you add a notebookpage in e.g. XRCed, it invents a wxPanel and adds it as a notebookpage child object
    // (If you add a normal control, that takes the place of the wxPanel. wxCrafter doesn't let you do that, at least
    // atm) For treebook this is still true for level 0 pages, but subpages are different: In XRC each is a sibling of
    // all the other tbk pages, but with a <depth> of 1 (or 2 or...) JSON stores subpages as children of the last
    // page-of-less-depth; so the hierarchy might be:
    //  Treebook
    //      Subpage 1 depth 0
    //      Subpage 2 depth 0
    //          Subpage 3 depth 1
    //          Subpage 4 depth 1
    //              Subpage 5 depth 2
    //          Subpage 6 depth 1
    //      Subpage 7 depth 0
    //  SomeOtherControl
    wxXmlNode* booknode = NULL;

    wxString tag = node->GetName();
    wxCHECK_MSG(tag == wxT("object"), NULL, wxT("Passed a node that isn't an object"));

    wxString classname = XmlUtils::ReadString(node, wxT("class"));
    wxCHECK_MSG(!classname.empty(), NULL, wxT("Object node doesn't have a 'class' attribute"));

    if(classname == wxT("sizeritem")) {
        // See the above comment. Replace node with the contained object node
        // We'll process sizeritemnode node separately later
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if(!node) {
            // I don't think this can happen, but...
            CL_WARNING(wxT("No object found in a sizeritem"));
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
            CL_WARNING(wxString::Format(wxT("No object found in a %s"), classname.c_str()));
            return NULL;
        }

        // Read the class of the new node
        classname = XmlUtils::ReadString(node, wxT("class"));
    }

    else if(classname == wxT("button")) {
        buttonnode = node;
        node = XmlUtils::FindFirstByTagName(node, wxT("object"));
        if(!node) {
            CL_WARNING(wxT("No object found in a <button> node"));
            return NULL;
        }

        classname = XmlUtils::ReadString(node, wxT("class"));
        if(classname == "wxButton") { // and it certainly should be
            classname = "stdbutton";
        }
    }

    int Id = Allocator::StringToId(classname);
    if(Id == wxNOT_FOUND) {
        wxLogWarning(wxString::Format(_("Can't import unknown class %s from XRC"), classname));
        return NULL;
    }

    if((Id == ID_WXPANEL) && (parentwrapper == NULL)) { Id = ID_WXPANEL_TOPLEVEL; }

    if(parentwrapper && parentwrapper->GetWidgetType() == TYPE_TOOLBAR) {
        // In XRC, classname == "separator", which gives and Id of ID_WXMENUITEM, could be a menu or toolbar one
        // "space" is a stretchspacer
        if(classname == "separator") {
            Id = ID_WXTOOLBARITEM_SEPARATOR;
        } else if(classname == "space") {
            Id = ID_WXTOOLBARITEM_STRETCHSPACE;
        }
    }

    if(parentwrapper && parentwrapper->GetWidgetType() == TYPE_AUITOOLBAR) {
        // This time it could be separator, space, or label
        // "space" is a stretchspacer
        if(classname == "separator") {
            Id = ID_WXTOOLBARITEM_SEPARATOR; // No need for a specific one for aui
        } else if(classname == "space") {
            // There were briefly 2 different words for 'space': space itself, and nonstretchspace
            // Now the official auitoolbar handler uses 'space' for both, distinguishing by the presence/absence of a
            // 'width' attribute, so do the same
            if(XmlUtils::FindFirstByTagName(node, "width")) {
                Id = ID_WXAUITOOLBARITEM_SPACE;
            } else {
                Id = ID_WXAUITOOLBARITEM_STRETCHSPACE;
            }
        }
    }

    if(booknode) {
        // Correctly label a wxPanel that's really a notebookpage
        Id = ID_WXPANEL_NOTEBOOK_PAGE;
    }

    wrapper = Allocator::Instance()->Create(Id);
    wxCHECK_MSG(wrapper, NULL, wxT("Failed to create a wrapper"));

    wrapper->LoadPropertiesFromXRC(node);

    GetSizeritemContents(sizeritemnode, wrapper); // Now we have a valid wrapper, we can store the sizeritem info in it

    int depth = 0;
    if(booknode) {
        // For book pages, extract any info from the <foobookpage> node
        NotebookPageWrapper* nbwrapper = dynamic_cast<NotebookPageWrapper*>(wrapper);
        wxCHECK_MSG(nbwrapper, NULL, wxT("A booknode which has no NotebookPageWrapper"));
        GetBookitemContents(booknode, nbwrapper, depth);

        // If this is a treebook subpage, it needs to be parented by a page, not the book
        // depth will only be >0 in that situation
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
        ProcessButtonNode(buttonnode, wrapper);
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

void ImportFromXrc::GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const
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

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("cellpos"));
    if(propertynode) {
        wxString cellpos = propertynode->GetNodeContent();
        if(!cellpos.empty()) { wrapper->SetGbPos(cellpos); }
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("cellspan"));
    if(propertynode) {
        wxString cellspan = propertynode->GetNodeContent();
        if(!cellspan.empty()) { wrapper->SetGbSpan(cellspan); }
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("minsize"));
    if(propertynode) {
        wxString minsize = propertynode->GetNodeContent();
        PropertyBase* prop = wrapper->GetProperty(PROP_MINSIZE);
        if(prop) { prop->SetValue(minsize); }
    }
}

void ImportFromXrc::GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const
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

// static  NB This is also used to import from wxSmith
void ImportFromXrc::ProcessBitmapProperty(const wxXmlNode* node, wxcWidget* wrapper,
                                          const wxString& property /*= PROP_BITMAP_PATH*/,
                                          const wxString& client_hint /*=""*/)
{
    // Node may be e.g. <bitmap stock_id="wxART_GO_DOWN" stock_client="wxART_BUTTON">some.png</bitmap>
    // (the some.png is optional here, but is used as a fallback if stock_id fails. wxC doesn't use this atm, so ignore
    // it here)
    wxString artstring = XmlUtils::ReadString(node, "stock_id");
    if(!artstring.empty()) {
        wxString clientId = XmlUtils::ReadString(node, "stock_client");
        if(clientId.empty()) {
            clientId = client_hint; // Use any hint we were given
        }
        if(!clientId.empty()) { artstring << "," << clientId; }
        wrapper->SetPropertyString(property, artstring);
    } else {
        // or e.g. <bitmap>/path/to/some.png</bitmap>
        wrapper->SetPropertyString(property, node->GetNodeContent());
    }
}

void ImportFromXrc::ProcessButtonNode(const wxXmlNode* node, wxcWidget* wrapper) const
{
    // First deal with any sizer stuff (flag, option...) in here; wxFB does this, XRCed and wxSmith don't
    GetSizeritemContents(node, wrapper);

    DoProcessButtonNode(node, wrapper); // Shared with ImportFromwxSmith
}

void ImportFromXrc::DoProcessButtonNode(const wxXmlNode* node, wxcWidget* wrapper)
{
    // Deduce a sensible ID. There isn't an id field in the xrc, so work it out from the name or label
    wxString name = wrapper->GetName().MakeLower();
    wxString label = wrapper->Label().MakeLower();
    if(name.Contains("ok") || label.Contains("ok")) {
        wrapper->SetId("wxID_OK");
    } else if(name.Contains("cancel") || label.Contains("cancel")) {
        wrapper->SetId("wxID_CANCEL");
    } else if(name.Contains("yes") || label.Contains("yes")) {
        wrapper->SetId("wxID_YES");
    } else if(name.Contains("save") || label.Contains("save")) {
        wrapper->SetId("wxID_SAVE");
    } else if(name.Contains("apply") || label.Contains("apply")) {
        wrapper->SetId("wxID_APPLY");
    } else if(name.Contains("close") || label.Contains("close")) {
        wrapper->SetId("wxID_CLOSE");
    } else if(name.Contains("no") || label.Contains("no")) {
        wrapper->SetId("wxID_NO");
    } else if(name.Contains("context") || label.Contains("context")) {
        wrapper->SetId("wxID_CONTEXT_HELP");
    } else if(name.Contains("help") || label.Contains("help")) {
        wrapper->SetId("wxID_HELP");
    }
}

void ImportFromXrc::ProcessNamedNode(wxXmlNode* node, wxcWidget* parentwrapper, const wxString& name) const
{
    // This is for processing the child menu of an auitoolbar dropdownitem (but might be useful elsewhere in the future)
    // The original node was <dropdown>, not <object>, so normal parsing would ignore any children
    wxXmlNode* childnode = XmlUtils::FindFirstByTagName(node, "object");
    if(childnode && (XmlUtils::ReadString(childnode, "class") == name)) {
        bool alreadyParented(false);
        wxcWidget* childwrapper = ParseNode(childnode, parentwrapper, alreadyParented);
        if(childwrapper) { parentwrapper->AddChild(childwrapper); }
    }
}

bool ImportFromXrc::GetProject() const { return false; }

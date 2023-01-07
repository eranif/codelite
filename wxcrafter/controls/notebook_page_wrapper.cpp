#include "notebook_page_wrapper.h"
#include "DirectoryChanger.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "choice_book_wrapper.h"
#include "file_ficker_property.h"
#include "notebook_base_wrapper.h"
#include "string_property.h"
#include "tree_book_wrapper.h"
#include "wxc_bitmap_code_generator.h"
#include "wxc_notebook_code_helper.h"
#include "wxc_project_metadata.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include "xmlutils.h"
#include <wx/panel.h>

NotebookPageWrapper::NotebookPageWrapper()
    : wxcWidget(ID_WXPANEL_NOTEBOOK_PAGE)
    , m_selected(false)
{

    SetPropertyString(_("Common Settings"), "wxNotebookPage");
    AddProperty(new StringProperty(PROP_LABEL, _("Page"), _("The tab's label")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, wxT(""), _("Tab Image")));
    AddProperty(new BoolProperty(PROP_SELECTED, false, _("Select this page")));
    AddProperty(
        new BoolProperty(PROP_NULL_BOOK_PAGE, false,
                         _("Add a NULL page. This is only makes sense when the notebook is of type wxTreebook")));

    // Enable the 'wxTAB_TRAVERSAL' style
    EnableStyle(wxT("wxTAB_TRAVERSAL"), true);
    m_namePattern = wxT("m_panel");
    SetName(GenerateName());
}

NotebookPageWrapper::~NotebookPageWrapper() {}

wxcWidget* NotebookPageWrapper::Clone() const { return new NotebookPageWrapper(); }

wxString NotebookPageWrapper::CppCtorCode() const
{
    if(IsTreebookPage()) { return DoTreebookCppCtorCode(); }

    wxString code;
    code << CPPStandardWxCtor(wxT("wxTAB_TRAVERSAL"));

    // If our parent is not a notebook, return the code
    NotebookBaseWrapper* book = GetNotebook();
    if(!book) { return code; }

    bool isAuiPage = (book->GetType() == ID_WXAUINOTEBOOK);
    bool isChoicePage = (book->GetType() == ID_WXCHOICEBOOK);

    if(!isChoicePage) {
        // wxChoicebook does not support images (yet)
        wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    }

    bool bHasBitmap = (GetImageSize() != wxSize(-1, -1));
    wxString imgListName, imgIndex;
    if(!isChoicePage && !isAuiPage && bHasBitmap) {
        imgListName << GetParent()->GetName() << wxT("_il");
        imgIndex << GetName() << wxT("ImgIndex");
        code << wxT("int ") << imgIndex << wxT(";\n");
        code << imgIndex << wxT(" = ") << imgListName << wxT("->Add(")
             << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << wxT(");\n");
    }

    if(isAuiPage && bHasBitmap) {
        // Aui passes the bitmap in the AddPage(...) call
        // Add this page to the notebook
        code << GetParent()->GetName() << wxT("->AddPage(") << GetName() << wxT(", ") << Label() << wxT(", ")
             << PropertyBool(PROP_SELECTED) << wxT(", ")
             << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << wxT(");\n");

    } else if(bHasBitmap && !imgIndex.IsEmpty()) {
        // Add this page to the notebook
        code << GetParent()->GetName() << wxT("->AddPage(") << GetName() << wxT(", ") << Label() << wxT(", ")
             << PropertyBool(PROP_SELECTED) << wxT(", ") << imgIndex << wxT(");\n");
    } else {

        // Add this page to the notebook
        code << GetParent()->GetName() << wxT("->AddPage(") << GetName() << wxT(", ") << Label() << wxT(", ")
             << PropertyBool(PROP_SELECTED) << wxT(");\n");
    }
    return code;
}

void NotebookPageWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/panel.h>"));
    headers.Add(wxT("#include <wx/imaglist.h>"));
}

wxString NotebookPageWrapper::GetWxClassName() const { return wxT("wxPanel"); }

void NotebookPageWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    NotebookBaseWrapper* book = dynamic_cast<NotebookBaseWrapper*>(GetParent());
    if(!book) { return; }

    if(IsTreebookPage()) {
        int depth(0);
        DoTreebookXRC(text, type, depth);
        return;
    }

    text << wxT("<object class=\"") << book->GetXRCPageClass() << wxT("\">") << XRCLabel();

    if(!IsChoicebookPage()) {
        wxString bmp = PropertyFile(PROP_BITMAP_PATH);
        if(bmp.IsEmpty() == false) { text << XRCBitmap(); }
    }

    if(type == XRC_LIVE) {
        text << wxT("<selected>") << wxCrafter::XMLEncode(PropertyString(PROP_SELECTED)) << wxT("</selected>");

    } else {
        // For the preview we use the fake selection
        text << wxT("<selected>") << (IsSelected() ? wxT("1") : wxT("0")) << wxT("</selected>");
    }

    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes();

    ChildrenXRC(text, type);

    text << XRCSuffix() << wxT("</object>");
}

void NotebookPageWrapper::LoadPropertiesFromXRC(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromXRC(node);

    // There are 3 possible properties. Not all will apply to all bookpage types, but the XRC should only supply
    // relevant ones...
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("bitmap"));
    if(propertynode) {
        wxString value = propertynode->GetNodeContent();
        PropertyBase* prop = GetProperty(PROP_BITMAP_PATH);
        if(prop) { prop->SetValue(value); }
    }

    propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) {
        wxString value = propertynode->GetNodeContent();
        PropertyBase* prop = GetProperty(PROP_SELECTION);
        if(prop) { prop->SetValue(value); }
    }
    /*
        propertynode = XmlUtils::FindFirstByTagName(node, wxT("depth"));
        if (propertynode) {
            wxString value = propertynode->GetNodeContent();
            PropertyBase* prop = GetProperty(.....);    // No corresponding property-id (yet)?
            if (prop) {
                prop->SetValue(value);
            }
        }*/
}

void NotebookPageWrapper::LoadPropertiesFromwxSmith(const wxXmlNode* node)
{
    // First call the base-class for the standard things
    wxcWidget::LoadPropertiesFromwxSmith(node);

    // There are 3 possible properties. Not all will apply to all bookpage types, but the XRC should only supply
    // relevant ones...
    wxXmlNode* propertynode = XmlUtils::FindFirstByTagName(node, wxT("selection"));
    if(propertynode) {
        wxString value = propertynode->GetNodeContent();
        PropertyBase* prop = GetProperty(PROP_SELECTION);
        if(prop) { prop->SetValue(value); }
    }
}

wxSize NotebookPageWrapper::GetImageSize() const
{
    DirectoryChanger dc(wxcProjectMetadata::Get().GetProjectPath());
    wxString bmp = PropertyFile(PROP_BITMAP_PATH);
    wxFileName fn(bmp);
    if(fn.FileExists()) {
        wxBitmap bmpFile(fn.GetFullPath(), wxBITMAP_TYPE_ANY);
        if(bmpFile.IsOk()) {
            wxSize bmpSize;
            bmpSize.x = bmpFile.GetWidth();
            bmpSize.y = bmpFile.GetHeight();
            return bmpSize;
        }
    }
    return wxSize(-1, -1);
}

void NotebookPageWrapper::SetParent(wxcWidget* parent)
{
    wxcWidget::SetParent(parent);
    if(IsChoicebookPage()) {
        DelProperty(PROP_BITMAP_PATH);

    } else if(IsTreebookPage()) {
        AddProperty(new BoolProperty(PROP_EXPANDED, true, _("Expand this node")));
    }
}

bool NotebookPageWrapper::IsChoicebookPage() const { return (dynamic_cast<ChoiceBookWrapper*>(GetParent()) != NULL); }

bool NotebookPageWrapper::IsTreebookPage() const
{
    bool isTreebookPage = false;
    // Find the first parent of type Notebook
    NotebookBaseWrapper* book = GetNotebook();
    if(book) { isTreebookPage = (dynamic_cast<TreeBookWrapper*>(book) != NULL); }

    if(!isTreebookPage) {
        NotebookPageWrapper* parent = dynamic_cast<NotebookPageWrapper*>(GetParent());
        isTreebookPage = (parent != NULL);
    }
    return isTreebookPage;
}

void NotebookPageWrapper::DoTreebookXRC(wxString& text, XRC_TYPE type, int depth) const
{
    text << wxT("<object class=\"treebookpage\">") << XRCLabel();

    if(!IsChoicebookPage()) {
        wxString bmp = PropertyFile(PROP_BITMAP_PATH);
        if(bmp.IsEmpty() == false) { text << XRCBitmap(); }
    }

    if(type == XRC_LIVE) {
        text << wxT("<selected>") << wxCrafter::XMLEncode(PropertyString(PROP_SELECTED)) << wxT("</selected>");

    } else {
        // For the preview we use the fake selection
        text << wxT("<selected>") << (IsSelected() ? wxT("1") : wxT("0")) << wxT("</selected>");
    }

    text << wxT("<depth>") << depth << wxT("</depth>");
    text << wxT("<expanded>") << 1 << wxT("</expanded>");
    text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes();

    // Loop over the children
    wxString childPageXRC;
    wxcWidget::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); iter++) {
        wxString xrc;
        // If it is another page, recurse
        NotebookPageWrapper* page = dynamic_cast<NotebookPageWrapper*>(*iter);
        if(page) {
            page->DoTreebookXRC(xrc, type, depth + 1);
            childPageXRC << xrc;
            xrc.Clear();

        } else {
            (*iter)->ToXRC(xrc, type);
            if((*iter)->IsSizerItem()) { xrc = (*iter)->WrapInSizerXRC(xrc); }
        }
        text << xrc;
    }

    text << XRCSuffix() << wxT("</object>");

    text << childPageXRC;
}

NotebookBaseWrapper* NotebookPageWrapper::GetNotebook() const
{
    // Find the first parent of type Notebook
    wxcWidget* parent = GetParent();
    while(parent) {
        if(dynamic_cast<NotebookBaseWrapper*>(parent)) { break; }
        parent = parent->GetParent();
    }

    if(!parent) return NULL;

    return dynamic_cast<NotebookBaseWrapper*>(parent);
}

int NotebookPageWrapper::GetPageIndex() const
{
    NotebookBaseWrapper* book = GetNotebook();
    if(!book) { return wxNOT_FOUND; }

    return book->GetPageIndex((const NotebookPageWrapper*)this);
}

wxString NotebookPageWrapper::DoTreebookCppCtorCode() const
{
    if(!GetNotebook()) return wxT("");

    wxString code;
    bool isNullPage = (PropertyBool(PROP_NULL_BOOK_PAGE) == "true");

    if(!isNullPage) { code << CPPStandardWxCtor(wxT("wxTAB_TRAVERSAL")); }

    bool bHasBitmap = false;
    wxString imgListName, imgIndex;

    NotebookPageWrapper* parentPage = dynamic_cast<NotebookPageWrapper*>(GetParent());
    NotebookBaseWrapper* book = GetNotebook();

    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));

    wxSize sz = GetImageSize();
    if(sz != wxSize(-1, -1)) {

        imgListName << book->GetName() << wxT("_il");
        imgIndex << GetName() << wxT("ImgIndex");
        code << wxT("int ") << imgIndex << wxT(";\n");
        code << imgIndex << wxT(" = ") << imgListName << wxT("->Add(")
             << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << wxT(");\n");
        bHasBitmap = true;
    }

    if(parentPage) {
        // This is a sub page
        int parentIndex = parentPage->GetPageIndex();
        if(parentIndex != wxNOT_FOUND) {
            code << book->GetName() << wxT("->InsertSubPage(") << parentIndex << wxT(", ")
                 << (isNullPage ? wxString("NULL") : GetName()) << wxT(", ") << Label() << wxT(", ")
                 << PropertyBool(PROP_SELECTED) << wxT(", ")
                 << (bHasBitmap && !imgIndex.IsEmpty() ? imgIndex : wxT("wxNOT_FOUND")) << wxT(");\n");

            if(PropertyBool(PROP_EXPANDED) == wxT("true")) {
                wxcNotebookCodeHelper::Get().Code()
                    << book->GetName() << wxT("->ExpandNode( ") << GetPageIndex() << wxT(", true );\n");
            }
        }
    } else {
        // top level page
        code << book->GetName() << wxT("->AddPage(") << (isNullPage ? wxString("NULL") : GetName()) << wxT(", ")
             << Label() << wxT(", ") << PropertyBool(PROP_SELECTED) << wxT(", ")
             << (bHasBitmap && !imgIndex.IsEmpty() ? imgIndex : wxT("wxNOT_FOUND")) << wxT(");\n");

        if(PropertyBool(PROP_EXPANDED) == wxT("true")) {
            wxcNotebookCodeHelper::Get().Code()
                << book->GetName() << wxT("->ExpandNode( ") << GetPageIndex() << wxT(", true );\n");
        }
    }
    return code;
}

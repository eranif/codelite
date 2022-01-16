#include "ribbon_gallery_item_wrapper.h"

#include "allocator_mgr.h"
#include "file_ficker_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_bitmaploader.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

RibbonGalleryItemWrapper::RibbonGalleryItemWrapper()
    : wxcWidget(ID_WXRIBBONGALLERYITME)
{
    SetPropertyString(_("Common Settings"), "wxRibbonGalleryItem");
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", _("Select the bitmap file")));

    wxCrafter::ResourceLoader bl;
    m_properties.Item(PROP_BITMAP_PATH)->SetValue(bl.GetPlaceHolderImagePath().GetFullPath());

    m_namePattern = "galleryItem";
    SetName(GenerateName());
}

RibbonGalleryItemWrapper::~RibbonGalleryItemWrapper() {}

wxcWidget* RibbonGalleryItemWrapper::Clone() const { return new RibbonGalleryItemWrapper(); }

wxString RibbonGalleryItemWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_BITMAP_PATH));
    wxString code;
    code << GetParent()->GetName() << "->Append( "
         << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH)) << ", " << GetId() << ");\n";
    return code;
}

void RibbonGalleryItemWrapper::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString RibbonGalleryItemWrapper::GetWxClassName() const { return ""; }

void RibbonGalleryItemWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    text << "<object class=\"item\" name=\"" << wxCrafter::XMLEncode(GetName()) << "\">";
    text << XRCBitmap() << XRCSuffix();
}

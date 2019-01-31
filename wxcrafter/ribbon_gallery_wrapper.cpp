#include "ribbon_gallery_wrapper.h"
#include "allocator_mgr.h"
#include <wx/ribbon/gallery.h>

RibbonGalleryWrapper::RibbonGalleryWrapper()
    : wxcWidget(ID_WXRIBBONGALLERY)
{
    SetPropertyString(_("Common Settings"), "wxRibbonGallery");
    RegisterEvent("wxEVT_COMMAND_RIBBONGALLERY_SELECTED", "wxRibbonGalleryEvent",
                  _("Triggered when the user selects an item from the gallery. Note that the ID is that of the "
                    "gallery, not of the item."));
    RegisterEvent("wxEVT_COMMAND_RIBBONGALLERY_CLICKED", "wxRibbonGalleryEvent",
                  _("Similar to EVT_RIBBONGALLERY_SELECTED but triggered every time a gallery item is clicked, even if "
                    "it is already selected. Note that the ID of the event is that of the gallery, not of the item, "
                    "just as above. This event is available since wxWidgets 2.9.2."));
    RegisterEvent("wxEVT_COMMAND_RIBBONGALLERY_HOVER_CHANGED", "wxRibbonGalleryEvent",
                  _("Triggered when the item being hovered over by the user changes. The item in the event will be the "
                    "new item being hovered, or NULL if there is no longer an item being hovered. Note that the ID is "
                    "that of the gallery, not of the item."));
    RegisterEventCommand("wxEVT_COMMAND_BUTTON_CLICKED",
                         _("Triggered when the \"extension\" button of the gallery is pressed"));
    m_namePattern = "m_ribbonGallery";
    SetName(GenerateName());
}

RibbonGalleryWrapper::~RibbonGalleryWrapper() {}

wxcWidget* RibbonGalleryWrapper::Clone() const { return new RibbonGalleryWrapper(); }

wxString RibbonGalleryWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << CPPStandardWxCtor("0");
    return cppCode;
}

void RibbonGalleryWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/ribbon/gallery.h>");
}

wxString RibbonGalleryWrapper::GetWxClassName() const { return "wxRibbonGallery"; }

void RibbonGalleryWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCCommonAttributes() << XRCStyle() << XRCSize();
    ChildrenXRC(text, type);
    text << XRCSuffix();
}

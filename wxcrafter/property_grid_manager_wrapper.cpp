#include "property_grid_manager_wrapper.h"
#include "allocator_mgr.h"
#include "wxc_settings.h"

PropertyGridManagerWrapper::PropertyGridManagerWrapper()
    : wxcWidget(ID_WXPROPERTYGRIDMANAGER)
{
    SetPropertyString(_("Common Settings"), "wxPropertyGridManager");
    PREPEND_STYLE_FALSE(wxPG_AUTO_SORT);
    PREPEND_STYLE_FALSE(wxPG_HIDE_CATEGORIES);
    PREPEND_STYLE_FALSE(wxPG_ALPHABETIC_MODE);
    PREPEND_STYLE_TRUE(wxPG_BOLD_MODIFIED);
    PREPEND_STYLE_TRUE(wxPG_SPLITTER_AUTO_CENTER);
    PREPEND_STYLE_FALSE(wxPG_TOOLTIPS);
    PREPEND_STYLE_FALSE(wxPG_HIDE_MARGIN);
    PREPEND_STYLE_FALSE(wxPG_STATIC_SPLITTER);
    PREPEND_STYLE_FALSE(wxPG_STATIC_LAYOUT);
    PREPEND_STYLE_FALSE(wxPG_LIMITED_EDITING);
    PREPEND_STYLE_FALSE(wxPG_TOOLBAR);
    PREPEND_STYLE_TRUE(wxPG_DESCRIPTION);
    PREPEND_STYLE_FALSE(wxPG_NO_INTERNAL_BORDER);

    RegisterEvent(
        "wxEVT_PG_SELECTED", "wxPropertyGridEvent",
        _("Respond to wxEVT_PG_SELECTED event, generated when a property selection has been changed, either by user "
          "action or by indirect program function.\nFor instance, collapsing a parent property programmatically causes "
          "any selected child property to become unselected, and may therefore cause this event to be generated."));
    RegisterEvent("wxEVT_PG_CHANGED", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_CHANGED event, generated when property value has been changed by the user"));
    RegisterEvent("wxEVT_PG_CHANGING", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_CHANGING event, generated when property value is about to be changed by user. "
                    "Use wxPropertyGridEvent::GetValue() to take a peek at the pending value, and "
                    "wxPropertyGridEvent::Veto() to prevent change from taking place, if necessary"));
    RegisterEvent("wxEVT_PG_HIGHLIGHTED", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_HIGHLIGHTED event, which occurs when mouse moves over a property. Event's "
                    "property is NULL if hovered area does not belong to any property."));
    RegisterEvent(
        "wxEVT_PG_RIGHT_CLICK", "wxPropertyGridEvent",
        _("Respond to wxEVT_PG_RIGHT_CLICK event, which occurs when property is clicked on with right mouse button."));
    RegisterEvent("wxEVT_PG_DOUBLE_CLICK", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_DOUBLE_CLICK event, which occurs when "
                    "property is double-clicked on with left mouse button."));
    RegisterEvent("wxEVT_PG_ITEM_COLLAPSED", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_ITEM_COLLAPSED event, generated when user collapses a property or category."));
    RegisterEvent("wxEVT_PG_ITEM_EXPANDED", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_ITEM_EXPANDED event, generated when user expands a property or category."));
    RegisterEvent("wxEVT_PG_LABEL_EDIT_BEGIN", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_LABEL_EDIT_BEGIN event, generated when user is about to begin editing a "
                    "property label. You can veto this event to prevent the action."));
    RegisterEvent("wxEVT_PG_LABEL_EDIT_ENDING", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_LABEL_EDIT_ENDING event, generated when user is about to end editing of a "
                    "property label. You can veto this event to prevent the action."));
    RegisterEvent(
        "wxEVT_PG_COL_BEGIN_DRAG", "wxPropertyGridEvent",
        _("Respond to wxEVT_PG_COL_BEGIN_DRAG event, generated when user starts resizing a column - can be vetoed."));
    RegisterEvent("wxEVT_PG_COL_DRAGGING", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_COL_DRAGGING, event, generated when a column resize by user is in progress. "
                    "This event is also generated when user double-clicks the splitter in order to recenter it."));
    RegisterEvent("wxEVT_PG_COL_END_DRAG", "wxPropertyGridEvent",
                  _("Respond to wxEVT_PG_COL_END_DRAG event, generated after column resize by user has finished."));
    RegisterEvent("wxEVT_COMMAND_BUTTON_CLICKED", "wxCommandEvent",
                  "Process a wxEVT_COMMAND_BUTTON_CLICKED event, when the custom editor button is clicked.");

    AddInteger(PROP_SASH_POS, "Sets x coordinate of the splitter", -1);
    AddBool(PROP_SPLITTER_LEFT,
            "Moves splitter as left as possible, while still allowing all labels to be shown in full", false);

    m_namePattern = "m_pgMgr";
    SetName(GenerateName());
}

PropertyGridManagerWrapper::~PropertyGridManagerWrapper() {}

wxcWidget* PropertyGridManagerWrapper::Clone() const { return new PropertyGridManagerWrapper(); }

wxString PropertyGridManagerWrapper::CppCtorCode() const
{
    wxString cppCode;

    cppCode << "wxArrayString " << GetName() << "Arr;\n";
    cppCode << "wxUnusedVar(" << GetName() << "Arr"
            << ");\n";

    cppCode << "wxArrayInt " << GetName() << "IntArr;\n";
    cppCode << "wxUnusedVar(" << GetName() << "IntArr"
            << ");\n";

    cppCode << CPPStandardWxCtor(StyleFlags("wxPGMAN_DEFAULT_STYLE"));
    return cppCode;
}

void PropertyGridManagerWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/propgrid/manager.h>");
}

wxString PropertyGridManagerWrapper::GetWxClassName() const { return "wxPropertyGridManager"; }

void PropertyGridManagerWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();

    } else {
        text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes();
        text << "<splitterpos>" << PropertyInt(PROP_SASH_POS) << "</splitterpos>";
        text << "<splitterleft>" << PropertyBool(PROP_SPLITTER_LEFT) << "</splitterleft>";
        ChildrenXRC(text, type);
        text << XRCSuffix();
    }
}

bool PropertyGridManagerWrapper::IsLicensed() const { return wxcSettings::Get().IsLicensed(); }

wxString PropertyGridManagerWrapper::DoGenerateCppCtorCode_End() const
{
    wxString cppCode;

    if(IsPropertyChecked(PROP_SPLITTER_LEFT)) { cppCode << GetName() << "->GetGrid()->SetSplitterLeft(true);\n"; }

    int splitterPos = PropertyInt(PROP_SASH_POS);
    if(splitterPos != wxNOT_FOUND) {
        cppCode << GetName() << "->GetGrid()->SetSplitterPosition(" << splitterPos << ", 0);\n";
    }
    return cppCode;
}

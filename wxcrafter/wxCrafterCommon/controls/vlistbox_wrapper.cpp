#include "vlistbox_wrapper.h"

#include "Properties/string_property.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

#include <wx/vlbox.h>

VListBoxWrapper::VListBoxWrapper(int type, const wxString& className, const wxString& namePattern)
    : wxcWidget(type)
{
    RegisterEventCommand("wxEVT_LISTBOX", _("Process a wxEVT_LISTBOX event, when an item on the list is selected"));
    RegisterEventCommand(
        "wxEVT_LISTBOX_DCLICK", _("Process a wxEVT_LISTBOX_DCLICK event, when the listbox is double-clicked"));

    SetPropertyString(_("Common Settings"), className);
    Add<StringProperty>(
        PROP_ITEM_COUNT, "5", _("The number of items in the list box. Calls SetItemCount() after the creation"));
    Add<StringProperty>(PROP_SELECTION,
                        wxT("-1"),
                        _("The zero-based position of any initially selected item, or -1 if none are to be selected"));

    m_namePattern = namePattern;
    SetName(GenerateName());
}

VListBoxWrapper::VListBoxWrapper()
    : VListBoxWrapper(ID_WXVLISTBOX, "wxVListBox", "m_vListBox")
{
    PREPEND_STYLE(wxLB_MULTIPLE, false);
}

void VListBoxWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/vlbox.h>");
    if (UsesDefaultImpl()) {
        // Used by the default implementation class
        headers.Add("#include <wx/dc.h>");
        headers.Add("#include <wx/settings.h>");
    }
}

wxString VListBoxWrapper::GetWxClassName() const { return "wxVListBox"; }

bool VListBoxWrapper::UsesDefaultImpl() const
{
    wxString subclass = PropertyString(PROP_SUBCLASS_NAME);
    return subclass.Trim().Trim(false).empty();
}

wxString VListBoxWrapper::CppCtorCode() const
{
    // wxVListBox is abstract: without a subclass, use the generated default implementation class
    const wxString className = UsesDefaultImpl() ? DefaultImplName() : GetRealClassName();

    wxString code;
    code << GetName() << " = new " << className << "(" << GetWindowParent() << ", " << WindowID()
         << ", wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("0") << ");\n";
    code << CPPCommonAttributes();

    const int count = wxCrafter::ToNumber(PropertyString(PROP_ITEM_COUNT), 0);
    if (count > 0) {
        code << GetName() << "->SetItemCount(" << count << ");\n";
    }

    const int sel = wxCrafter::ToNumber(PropertyString(PROP_SELECTION), -1);
    if (sel >= 0 && sel < count) {
        code << GetName() << "->SetSelection(" << sel << ");\n";
    }
    return code;
}

void VListBoxWrapper::DefaultImplMethods(wxString& decl, wxString& impl, const wxString& qualifiedName) const
{
    decl << "        void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const override;\n"
         << "        wxCoord OnMeasureItem(size_t n) const override;\n";

    impl << "void " << qualifiedName << "::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const\n"
         << "{\n"
         << "    dc.SetTextForeground(IsSelected(n) ? wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT)\n"
         << "                                       : GetForegroundColour());\n"
         << "    dc.DrawText(wxString::Format(\"Item %zu\", n), rect.x + 2, rect.y + 2);\n"
         << "}\n\n"
         << "wxCoord " << qualifiedName << "::OnMeasureItem(size_t n) const\n"
         << "{\n"
         << "    wxUnusedVar(n);\n"
         << "    return GetCharHeight() + 4;\n"
         << "}\n\n";
}

void VListBoxWrapper::DoGenerateExtraFunctions(wxString& decl, wxString& impl) const
{
    // With a subclass, the user is responsible for the abstract methods
    const wxString scope = DoGetScopeName();
    if (!UsesDefaultImpl() || scope.empty()) {
        return;
    }

    const wxString baseClass = GetWxClassName();
    const wxString implName = DefaultImplName();

    decl << "\n"
         << "    // The default implementation of '" << GetName() << "'. Set its 'Subclass' property to use your own\n"
         << "    // class instead\n"
         << "    class " << implName << " : public " << baseClass << "\n"
         << "    {\n"
         << "    public:\n"
         << "        " << implName
         << "(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)\n"
         << "            : " << baseClass << "(parent, id, pos, size, style)\n"
         << "        {\n"
         << "        }\n"
         << "\n"
         << "    protected:\n";
    DefaultImplMethods(decl, impl, scope + "::" + implName);
    decl << "    };\n";
}

void VListBoxWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    // The XRC handler of the abstract class works only when the derived class is known to the application, which is
    // never the case in the designer or in the preview. Show a placeholder there
    if (type != XRC_LIVE || UsesDefaultImpl() || !HasXRCHandler()) {
        text << XRCUnknown();
        return;
    }
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
}

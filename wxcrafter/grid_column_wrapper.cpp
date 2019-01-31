#include "grid_column_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

GridColumnWrapper::GridColumnWrapper()
    : wxcWidget(ID_WXGRIDCOL)
{
    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    SetPropertyString(_("Common Settings"), "wxGridColumn");
    AddProperty(new CategoryProperty(_("wxGrid Column")));
    AddText(PROP_NAME, _("The Column Label"), "My Column");
    AddInteger(PROP_WIDTH,
               _("Sets the width of the specified column.\nThe new column width in pixels, 0 to hide the column or -1 "
                 "to fit the column width to its label width"),
               -1);

    m_namePattern = "Column";
    SetName(GenerateName());
}

GridColumnWrapper::~GridColumnWrapper() {}

wxcWidget* GridColumnWrapper::Clone() const { return new GridColumnWrapper(); }

wxString GridColumnWrapper::CppCtorCode() const
{
    return ""; // The code is generated inside the wxGrid wrapper
}

void GridColumnWrapper::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString GridColumnWrapper::GetWxClassName() const { return ""; }

void GridColumnWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(text);
    text << "<col>"
         << "<name>" << wxCrafter::CDATA(GetName()) << "</name>"
         << "<colsize>" << PropertyInt(PROP_WIDTH, -1) << "</colsize>"
         << "</col>";
}

wxString GridColumnWrapper::Code(int colIndex) const
{
    wxString cppCode;
    cppCode << GetParent()->GetName() << wxT("->SetColLabelValue(") << colIndex << wxT(", ")
            << wxCrafter::UNDERSCORE(GetName()) << wxT(");\n");
    int width = PropertyInt(PROP_WIDTH, -1);
    if(width != -1) {
        cppCode << GetParent()->GetName() << wxT("->SetColSize(") << colIndex << ", " << width << ");\n";
    }
    return cppCode;
}

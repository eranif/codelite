#include "grid_row_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_helpers.h"

GridRowWrapper::GridRowWrapper()
    : wxcWidget(ID_WXGRIDROW)
{
    m_styles.Clear();
    m_sizerFlags.Clear();
    m_properties.DeleteValues();

    SetPropertyString(_("Common Settings"), "wxGridRow");
    AddProperty(new CategoryProperty(_("wxGrid Row")));
    AddText(PROP_NAME, _("The Row Label"), "My Row");
    AddInteger(PROP_HEIGHT, _("Sets the height of the row\nDefault -1 fits to the label height"), -1);

    m_namePattern = "Row";
    SetName(GenerateName());
}

GridRowWrapper::~GridRowWrapper() {}

wxcWidget* GridRowWrapper::Clone() const { return new GridRowWrapper(); }

wxString GridRowWrapper::CppCtorCode() const { return ""; }

void GridRowWrapper::GetIncludeFile(wxArrayString& headers) const { wxUnusedVar(headers); }

wxString GridRowWrapper::GetWxClassName() const { return ""; }

void GridRowWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    wxUnusedVar(type);
    text << "<row>"
         << "<name>" << wxCrafter::CDATA(GetName()) << "</name>"
         << "<rowsize>" << PropertyInt(PROP_HEIGHT, -1) << "</rowsize>"
         << "</row>";
}

wxString GridRowWrapper::Code(int rowIndex) const
{
    wxString cppCode;
    cppCode << GetParent()->GetName() << wxT("->SetRowLabelValue(") << rowIndex << wxT(", ")
            << wxCrafter::UNDERSCORE(GetName()) << wxT(");\n");
    int height = PropertyInt(PROP_HEIGHT, -1);
    if(height != -1) {
        cppCode << GetParent()->GetName() << wxT("->SetRowSize(") << rowIndex << ", " << height << ");\n";
    }
    return cppCode;
}

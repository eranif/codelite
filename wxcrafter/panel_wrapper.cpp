#include "panel_wrapper.h"
#include "allocator_mgr.h"
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

PanelWrapper::PanelWrapper()
    : wxcWidget(ID_WXPANEL)
{
    SetPropertyString(_("Common Settings"), "wxPanel");
    // Enable the 'wxTAB_TRAVERSAL' style
    EnableStyle(wxT("wxTAB_TRAVERSAL"), true);
    m_namePattern = wxT("m_panel");
    SetName(GenerateName());
}

PanelWrapper::~PanelWrapper() {}

void PanelWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/panel.h>")); }

wxString PanelWrapper::GetWxClassName() const { return wxT("wxPanel"); }

wxString PanelWrapper::CppCtorCode() const
{
    // m_panel4 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxString code;

    code << GetName() << wxT(" = new ") << GetRealClassName() << "(" << GetWindowParent() << wxT(", ") << WindowID()
         << wxT(", ") << wxT("wxDefaultPosition") << wxT(", ") << SizeAsString() << wxT(", ")
         << StyleFlags("wxTAB_TRAVERSAL") << ");\n";
    code << CPPCommonAttributes();
    return code;
}

void PanelWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes();

    ChildrenXRC(text, type);
    text << wxT("</object>");
}

bool PanelWrapper::IsTopWindow() const { return false; }

#include "AuiToolbarWrapperBase.h"

#include "controls/menu_toolbar/tool_bar_item_wrapper.h"
#include "macros.h"
#include "top_level_win_wrapper.h"
#include "wxc_widget.h"
#include "wxgui_helpers.h"

bool AuiToolbarWrapperBase::HasDropdownWithMenu(const wxcWidget* widget) const
{
    for (auto child : widget->GetChildren()) {
        ToolBarItemWrapper* item = dynamic_cast<ToolBarItemWrapper*>(child);
        if(item && wxCrafter::GetToolType(item->PropertyString(PROP_KIND)) == wxCrafter::TOOL_TYPE_DROPDOWN &&
           item->PropertyString(PROP_DROPDOWN_MENU) == "1") {
            return true;
        }
    }
    return false;
}

void AuiToolbarWrapperBase::GenerateExtraFunctions(const wxcWidget* widget, wxString& decl, wxString& impl) const
{
    TopLevelWinWrapper* tlw = dynamic_cast<TopLevelWinWrapper*>(widget->GetTopLevel());
    CHECK_PTR_RET(tlw);
    if(!tlw->IsAuiToolBarDropDownHelpersRegistered() && HasDropdownWithMenu(widget)) {
        impl << "\n"
             << "void " << tlw->GetName() << "::" << DEFAULT_AUI_DROPDOWN_FUNCTION_AND_SIG << "\n{\n"
             << "    event.Skip();\n"
             << "    if (event.IsDropDownClicked()) {\n"
             << "        wxAuiToolBar* toolbar = wxDynamicCast(event.GetEventObject(), wxAuiToolBar);\n"
             << "        if (toolbar) {\n"
             << "            wxAuiToolBarItem* item = toolbar->FindTool(event.GetId());\n"
             << "            if (item) {\n"
             << "                std::map<int, wxMenu*>::iterator iter = m_dropdownMenus.find(item->GetId());\n"
             << "                if (iter != m_dropdownMenus.end()) {\n"
             << "                    event.Skip(false);\n"
             << "                    wxPoint pt = event.GetItemRect().GetBottomLeft();\n"
             << "                    pt.y++;\n"
             << "                    toolbar->PopupMenu(iter->second, pt);\n"
             << "                }\n"
             << "            }\n"
             << "        }\n"
             << "    }\n"
             << "}\n";
        tlw->SetAuiToolBarDropDownHelpersRegistered(true);
        decl << "    virtual void " << DEFAULT_AUI_DROPDOWN_FUNCTION_AND_SIG << ";\n";
    }
}

wxString AuiToolbarWrapperBase::CppDtorCode(const wxcWidget* widget) const
{
    wxString cppCode;
    TopLevelWinWrapper* tlw = dynamic_cast<TopLevelWinWrapper*>(widget->GetTopLevel());
    if(tlw && !tlw->IsAuiToolBarDropDownHelpersRegistered()) {
        if(HasDropdownWithMenu(widget)) {
            cppCode << "    std::map<int, wxMenu*>::iterator menuIter = m_dropdownMenus.begin();\n"
                    << "    for( ; menuIter != m_dropdownMenus.end(); ++menuIter ) {\n"
                    << "        wxDELETE( menuIter->second );\n"
                    << "    }\n"
                    << "    m_dropdownMenus.clear();\n";
        }
    }
    return cppCode;
}

wxString AuiToolbarWrapperBase::GenerateClassMembers(const wxcWidget* widget) const
{
    wxString memberCode = widget->BaseDoGenerateClassMember(); // Base class stuff

    TopLevelWinWrapper* tlw = dynamic_cast<TopLevelWinWrapper*>(widget->GetTopLevel());
    if(tlw && !tlw->IsAuiToolBarDropDownHelpersRegistered() && HasDropdownWithMenu(widget)) {
        if(!memberCode.empty()) { memberCode << "\n"; }
        memberCode << "    std::map<int, wxMenu*> m_dropdownMenus;";
    }
    return memberCode;
}

void AuiToolbarWrapperBase::BaseGetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/pen.h>"));
    headers.Add(wxT("#include <wx/aui/auibar.h>"));
    headers.Add(wxT("#include <map>"));
    headers.Add(wxT("#include <wx/menu.h>"));
}

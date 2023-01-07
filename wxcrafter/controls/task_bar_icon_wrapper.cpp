#include "task_bar_icon_wrapper.h"

#include "allocator_mgr.h"
#include "bitmap_picker_property.h"
#include "choice_property.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

#include <wx/taskbar.h>

TaskBarIconWrapper::TaskBarIconWrapper()
    : wxcWidget(ID_WXTASKBARICON)
{
    m_styles.Clear();
    AddText(PROP_TOOLTIP, _("Set the wxTaskBarIcon tooltip"));
    wxArrayString types;
    types.Add("wxTBI_DEFAULT_TYPE");
    types.Add("wxTBI_DOCK");
    types.Add("wxTBI_CUSTOM_STATUSITEM");

    SetPropertyString(_("Common Settings"), "wxTaskBarIcon");
    AddProperty(
        new ChoiceProperty(PROP_TASKBAR_ICONTYPE, types, 0, _("The iconType is only applicable on wxOSX_Cocoa")));
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", _("Set the wxTaskBarIcon icon")));

    RegisterEvent("wxEVT_TASKBAR_MOVE", "wxTaskBarIconEvent", _("Process a wxEVT_TASKBAR_MOVE event"));
    RegisterEvent("wxEVT_TASKBAR_LEFT_DOWN", "wxTaskBarIconEvent", _("Process a wxEVT_TASKBAR_LEFT_DOWN event"));
    RegisterEvent("wxEVT_TASKBAR_LEFT_UP", "wxTaskBarIconEvent", _("Process a wxEVT_TASKBAR_LEFT_UP event"));
    RegisterEvent("wxEVT_TASKBAR_RIGHT_DOWN", "wxTaskBarIconEvent", _("Process a wxEVT_TASKBAR_RIGHT_DOWN event"));
    RegisterEvent("wxEVT_TASKBAR_RIGHT_UP", "wxTaskBarIconEvent", _("Process a wxEVT_TASKBAR_RIGHT_UP event"));
    RegisterEvent("wxEVT_TASKBAR_LEFT_DCLICK", "wxTaskBarIconEvent", _("Process a wxEVT_TASKBAR_LEFT_DCLICK event"));
    RegisterEvent("wxEVT_TASKBAR_RIGHT_DCLICK", "wxTaskBarIconEvent", _("Process a wxEVT_TASKBAR_RIGHT_DCLICK event"));

    m_namePattern = "m_taskBarIcon";
    SetName(GenerateName());
}

TaskBarIconWrapper::~TaskBarIconWrapper() {}

wxcWidget* TaskBarIconWrapper::Clone() const { return new TaskBarIconWrapper(); }

wxString TaskBarIconWrapper::CppCtorCode() const
{
    wxString bmp = PropertyFile(PROP_BITMAP_PATH);
    wxcCodeGeneratorHelper::Get().AddBitmap(bmp);

    wxString code;
    code << GetName() << " = new " << GetRealClassName() << "(" << PropertyString(PROP_TASKBAR_ICONTYPE) << ");\n";
    if(!bmp.IsEmpty()) {
        code << "{\n";
        code << "    wxIcon icn;\n";
        code << "    icn.CopyFromBitmap(" << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_BITMAP_PATH))
             << ");\n";
        code << "    " << GetName() << "->SetIcon(icn, " << wxCrafter::UNDERSCORE(PropertyString(PROP_TOOLTIP))
             << ");\n";
        code << "}\n";
    }
    return code;
}

wxString TaskBarIconWrapper::CppDtorCode() const
{
    wxString code;
    code << "wxDELETE(" << GetName() << ");\n";
    return code;
}

void TaskBarIconWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/taskbar.h>"); }

wxString TaskBarIconWrapper::GetWxClassName() const { return "wxTaskBarIcon"; }

void TaskBarIconWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    // No preview support for wxTaskBarIcon
    wxUnusedVar(text);
    wxUnusedVar(type);
}

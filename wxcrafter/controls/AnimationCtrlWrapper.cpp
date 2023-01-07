#include "AnimationCtrlWrapper.h"

#include "allocator_mgr.h"
#include "wxc_bitmap_code_generator.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

#include <wx/animate.h>
#include <wx/mstream.h>

AnimationCtrlWrapper::AnimationCtrlWrapper()
    : wxcWidget(ID_WXANIMATIONCTRL)
{
    PREPEND_STYLE(wxAC_DEFAULT_STYLE, true);
    PREPEND_STYLE(wxAC_NO_AUTORESIZE, false);

    SetPropertyString(_("Common Settings"), "wxAnimationCtrl");

    AddBool(PROP_ANIM_AUTO_PLAY, _("Load and play animation on creation"), false);
    AddProperty(new BitmapPickerProperty(PROP_BITMAP_PATH, "", _("Select the animation file")));
    AddProperty(new BitmapPickerProperty(
        PROP_DISABLED_BITMAP_PATH, "", _("Sets the bitmap to show on the control when it's not playing an animation")));

    m_namePattern = "m_animationCtrl";
    SetName(GenerateName());
}

AnimationCtrlWrapper::~AnimationCtrlWrapper() {}

wxcWidget* AnimationCtrlWrapper::Clone() const { return new AnimationCtrlWrapper(); }

wxString AnimationCtrlWrapper::CppCtorCode() const
{
    wxcCodeGeneratorHelper::Get().AddBitmap(PropertyFile(PROP_DISABLED_BITMAP_PATH));

    wxString code;
    // wxAnimationCtrl (wxWindow *parent, wxWindowID id, const wxAnimation &anim=wxNullAnimation, const wxPoint
    // &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxAC_DEFAULT_STYLE, const wxString
    // &name=wxAnimationCtrlNameStr)
    code << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << WindowID()
         << ", wxNullAnimation, wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("wxAC_DEFAULT_STYLE")
         << ");\n";

    // Load the image file
    if(PropertyBool(PROP_ANIM_AUTO_PLAY) == "true") {
        code << "if( wxFileName::Exists(" << wxCrafter::WXT(PropertyFile(PROP_BITMAP_PATH)) << ") && " << GetName()
             << "->LoadFile(" << wxCrafter::WXT(PropertyFile(PROP_BITMAP_PATH)) << "))" << GetName() << "->Play();\n";
    }
    code << CPPCommonAttributes();
    code << GetName() << "->SetInactiveBitmap("
         << wxcCodeGeneratorHelper::Get().BitmapCode(PropertyFile(PROP_DISABLED_BITMAP_PATH)) << ");\n";
    return code;
}

void AnimationCtrlWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add("#include <wx/mstream.h>");
    headers.Add("#include <wx/filename.h>");
    headers.Add("#include <wx/animate.h>");
}

wxString AnimationCtrlWrapper::GetWxClassName() const { return "wxAnimationCtrl"; }

void AnimationCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCStyle() << XRCCommonAttributes() << XRCBitmap("animation") // Uses PROP_BITMAP_PATH
         << "<inactive-bitmap>" << PropertyFile(PROP_DISABLED_BITMAP_PATH) << "</inactive-bitmap>" << XRCSuffix();
}

#include "media_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "wxc_settings.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"
#include <wx/mediactrl.h>

MediaCtrlWrapper::MediaCtrlWrapper()
    : wxcWidget(ID_WXMEDIACTRL)
{
    wxArrayString backendList;
    backendList.Add(""); // Default empty
    backendList.Add("wxMEDIABACKEND_DIRECTSHOW");
    backendList.Add("wxMEDIABACKEND_MCI");
    backendList.Add("wxMEDIABACKEND_QUICKTIME");
    backendList.Add("wxMEDIABACKEND_GSTREAMER");
    backendList.Add("wxMEDIABACKEND_REALPLAYER");
    backendList.Add("wxMEDIABACKEND_WMP10");

    SetPropertyString(_("Common Settings"), "wxMediaCtrl");
    AddProperty(new ChoiceProperty(PROP_MC_BACKENDNAME, backendList, 0,
                                   _("Select the media control backend, leave empty for the default")));
    wxArrayString controls;
    controls.Add(PROP_MC_NO_CONTROLS);
    controls.Add(PROP_MC_DEFAULT_CONTROLS);
    AddProperty(new ChoiceProperty(PROP_MC_CONTROLS, controls, 0, _("Show the player controls")));

    RegisterEvent("wxEVT_MEDIA_LOADED", "wxMediaEvent",
                  _("Sent when a media has loaded enough data that it can start playing"));
    RegisterEvent("wxEVT_MEDIA_STOP", "wxMediaEvent",
                  _("Sent when a media has switched to the wxMEDIASTATE_STOPPED state. You may be able to Veto this "
                    "event to prevent it from stopping, causing it to continue playing - even if it has reached that "
                    "end of the media (note that this may not have the desired effect - if you want to loop the media, "
                    "for example, catch the EVT_MEDIA_FINISHED and play there instead)."));
    RegisterEvent("wxEVT_MEDIA_FINISHED", "wxMediaEvent",
                  _("Sent when a media has finished playing in a wxMediaCtrl."));
    RegisterEvent("wxEVT_MEDIA_STATECHANGED", "wxMediaEvent",
                  _("Sent when a media has switched its state (from any media state)"));
    RegisterEvent("wxEVT_MEDIA_PLAY", "wxMediaEvent",
                  _("Sent when a media has switched to the wxMEDIASTATE_PLAYING state"));
    RegisterEvent("wxEVT_MEDIA_PAUSE", "wxMediaEvent",
                  _("Sent when a media has switched to the wxMEDIASTATE_PAUSED statee"));

    m_namePattern = "m_mediaCtrl";
    SetName(GenerateName());
}

MediaCtrlWrapper::~MediaCtrlWrapper() {}

wxcWidget* MediaCtrlWrapper::Clone() const { return new MediaCtrlWrapper(); }

wxString MediaCtrlWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << GetName() << " = new " << GetRealClassName() << "();\n";
    cppCode << GetName() << "->Create(" << GetWindowParent() << ", " << GetId() << ", "
            << wxCrafter::WXT(PropertyString(PROP_MC_BACKENDNAME)) << ", "
            << "wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("0") << ");\n";
    cppCode << GetName() << "->ShowPlayerControls(" << PropertyString(PROP_MC_CONTROLS) << ");\n";

    cppCode << CPPCommonAttributes();
    return cppCode;
}

void MediaCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/mediactrl.h>"); }

wxString MediaCtrlWrapper::GetWxClassName() const { return "wxMediaCtrl"; }

void MediaCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();

    } else {
        text << XRCPrefix() << XRCSize() << XRCCommonAttributes() << XRCStyle() << "<controlstyle>"
             << PropertyString(PROP_MC_CONTROLS) << "</controlstyle>" << XRCSuffix();
    }
}

bool MediaCtrlWrapper::IsLicensed() const { return wxcSettings::Get().IsLicensed(); }

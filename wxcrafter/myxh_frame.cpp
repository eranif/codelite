#include "myxh_frame.h"
#include "my_frame.h"
#include <wx/dialog.h>

IMPLEMENT_DYNAMIC_CLASS(wxMyFrameXmlHandler, wxXmlResourceHandler)

wxMyFrameXmlHandler::wxMyFrameXmlHandler()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxSTAY_ON_TOP);
    XRC_ADD_STYLE(wxCAPTION);
    XRC_ADD_STYLE(wxDEFAULT_DIALOG_STYLE);
    XRC_ADD_STYLE(wxDEFAULT_FRAME_STYLE);
#if WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxTHICK_FRAME);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxSYSTEM_MENU);
    XRC_ADD_STYLE(wxRESIZE_BORDER);
#if WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxRESIZE_BOX);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxCLOSE_BOX);

    XRC_ADD_STYLE(wxFRAME_NO_TASKBAR);
    XRC_ADD_STYLE(wxFRAME_SHAPED);
    XRC_ADD_STYLE(wxFRAME_TOOL_WINDOW);
    XRC_ADD_STYLE(wxFRAME_FLOAT_ON_PARENT);
    XRC_ADD_STYLE(wxMAXIMIZE_BOX);
    XRC_ADD_STYLE(wxMINIMIZE_BOX);
    XRC_ADD_STYLE(wxSTAY_ON_TOP);

#if WXWIN_COMPATIBILITY_2_6 && defined(wxNO_3D)
    XRC_ADD_STYLE(wxNO_3D);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    XRC_ADD_STYLE(wxFRAME_EX_METAL);
    XRC_ADD_STYLE(wxFRAME_EX_CONTEXTHELP);

    AddWindowStyles();
}

wxObject* wxMyFrameXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(frame, MyFrame);

    frame->Create(m_parentAsWindow, GetID(), GetText(wxT("title")), wxDefaultPosition, wxDefaultSize,
                  GetStyle(wxT("style"), wxDEFAULT_FRAME_STYLE), GetName());

    if(HasParam(wxT("size"))) frame->SetClientSize(GetSize(wxT("size"), frame));
    if(HasParam(wxT("pos"))) frame->Move(GetPosition());
    if(HasParam(wxT("icon"))) frame->SetIcons(GetIconBundle(wxT("icon"), wxART_FRAME_ICON));

    SetupWindow(frame);
    CreateChildren(frame);
    if(GetBool(wxT("centered"), false)) frame->Centre();

    return frame;
}

bool wxMyFrameXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxFrame")); }

#include "gl_canvas_wrapper.h"
#include "allocator_mgr.h"
//#include <wx/glcanvas.h>
#include "int_property.h"

GLCanvasWrapper::GLCanvasWrapper()
    : wxcWidget(ID_WXGLCANVAS)
{
    m_namePattern = "m_glCanvas";

    SetPropertyString(_("Common Settings"), "wxGLCanvas");
    DelProperty(PROP_CONTROL_SPECIFIC_SETTINGS);
    AddCategory(_("wxGLCanvas Attribute List"));

    AddProperty(new StringProperty("WX_GL_RGBA", _("Use true color palette (on if no attrs specified)")));
    m_attrs.Add("WX_GL_RGBA");

    AddProperty(new StringProperty("WX_GL_BUFFER_SIZE", _("bits for buffer if not WX_GL_RGBA")));
    m_attrs.Add("WX_GL_BUFFER_SIZE");

    AddProperty(new StringProperty("WX_GL_LEVEL", _("0 for main buffer, >0 for overlay, <0 for underlay")));
    m_attrs.Add("WX_GL_LEVEL");

    AddProperty(new StringProperty("WX_GL_DOUBLEBUFFER", _("use double buffering (on if no attrs specified)")));
    m_attrs.Add("WX_GL_DOUBLEBUFFER");

    AddProperty(new StringProperty("WX_GL_STEREO", _("use stereoscopic display")));
    m_attrs.Add("WX_GL_STEREO");

    AddProperty(new StringProperty("WX_GL_AUX_BUFFERS", _("number of auxiliary buffers")));
    m_attrs.Add("WX_GL_AUX_BUFFERS");

    AddProperty(new StringProperty("WX_GL_MIN_RED", _("use red buffer with most bits (> MIN_RED bits)")));
    m_attrs.Add("WX_GL_MIN_RED");

    AddProperty(new StringProperty("WX_GL_MIN_GREEN", _("use green buffer with most bits (> MIN_GREEN bits)")));
    m_attrs.Add("WX_GL_MIN_GREEN");

    AddProperty(new StringProperty("WX_GL_MIN_BLUE", _("use blue buffer with most bits (> MIN_BLUE bits)")));
    m_attrs.Add("WX_GL_MIN_BLUE");

    AddProperty(new StringProperty("WX_GL_MIN_ALPHA", _("use alpha buffer with most bits (> MIN_ALPHA bits)")));
    m_attrs.Add("WX_GL_MIN_ALPHA");

    AddProperty(new StringProperty("WX_GL_DEPTH_SIZE", _("bits for Z-buffer (0,16,32)")));
    m_attrs.Add("WX_GL_DEPTH_SIZE");

    AddProperty(new StringProperty("WX_GL_STENCIL_SIZE", _("bits for stencil buffer")));
    m_attrs.Add("WX_GL_STENCIL_SIZE");

    AddProperty(
        new StringProperty("WX_GL_MIN_ACCUM_RED", _("use red accum buffer with most bits (> MIN_ACCUM_RED bits)")));
    m_attrs.Add("WX_GL_MIN_ACCUM_RED");

    AddProperty(
        new StringProperty("WX_GL_MIN_ACCUM_GREEN", _("use green buffer with most bits (> MIN_ACCUM_GREEN bits)")));
    m_attrs.Add("WX_GL_MIN_ACCUM_GREEN");

    AddProperty(
        new StringProperty("WX_GL_MIN_ACCUM_BLUE", _("use blue buffer with most bits (> MIN_ACCUM_BLUE bits)")));
    m_attrs.Add("WX_GL_MIN_ACCUM_BLUE");

    AddProperty(
        new StringProperty("WX_GL_MIN_ACCUM_ALPHA", _("use alpha buffer with most bits (> MIN_ACCUM_ALPHA bits)")));
    m_attrs.Add("WX_GL_MIN_ACCUM_ALPHA");

    AddProperty(new StringProperty("WX_GL_SAMPLE_BUFFERS", _("1 for multisampling support (antialiasing)")));
    m_attrs.Add("WX_GL_SAMPLE_BUFFERS");

    AddProperty(new StringProperty("WX_GL_SAMPLES", _("4 for 2x2 antialiasing supersampling on most graphics cards")));
    m_attrs.Add("WX_GL_SAMPLES");

    SetName(GenerateName());
}

GLCanvasWrapper::~GLCanvasWrapper() {}

wxcWidget* GLCanvasWrapper::Clone() const { return new GLCanvasWrapper(); }

wxString GLCanvasWrapper::CppCtorCode() const
{
    wxString cppCode;
    wxString attrList;
    wxArrayString validAttrsName, validAttrsValue;
    // get the number of attributes provided
    int bufferSize = 0;
    for(size_t i = 0; i < m_attrs.GetCount(); ++i) {
        wxString v = PropertyString(m_attrs.Item(i));
        if(!v.Trim().Trim(false).IsEmpty()) {
            bufferSize += 2; // one for the property and one for its value
            validAttrsName.Add(m_attrs.Item(i));
            validAttrsValue.Add(v);
        }
    }

    wxString AttrName;
    AttrName << GetName() << "Attr";
    if(bufferSize) {
        bufferSize++; // terminator
        attrList << "int *" << AttrName << " = new int[ " << bufferSize << " ];\n";
    } else {
        attrList << "int *" << AttrName << " = NULL;\n";
    }

    int idx = 0;
    for(size_t i = 0; i < validAttrsName.GetCount(); ++i) {
        attrList << AttrName << "[" << idx++ << "] = " << validAttrsName.Item(i) << ";\n";
        attrList << AttrName << "[" << idx++ << "] = " << validAttrsValue.Item(i) << ";\n";
    }

    if(validAttrsName.GetCount()) { attrList << AttrName << "[" << idx << "] = 0;\n"; }

    if(!attrList.IsEmpty()) { cppCode << attrList; }

    cppCode << GetName() << " = new " << GetRealClassName() << "(" << GetWindowParent() << ", " << GetId() << ", "
            << AttrName << ", "
            << "wxDefaultPosition, " << SizeAsString() << ", " << StyleFlags("0") << ");\n";
    cppCode << CPPCommonAttributes();
    cppCode << "wxDELETEA( " << AttrName << " );\n";
    return cppCode;
}

void GLCanvasWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/glcanvas.h>"); }

wxString GLCanvasWrapper::GetWxClassName() const { return "wxGLCanvas"; }

void GLCanvasWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();

    } else {
        text << XRCPrefix() << XRCStyle() << XRCSize() << XRCCommonAttributes() << XRCSuffix();
    }
}

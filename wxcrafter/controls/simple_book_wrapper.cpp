#include "simple_book_wrapper.h"
#include "allocator_mgr.h"
#include "choice_property.h"
#include "wxgui_defs.h"

SimpleBookWrapper::SimpleBookWrapper()
    : NotebookBaseWrapper(ID_WXSIMPLEBOOK)
{
    // Remove styles added by NotebookBaseWrapper
    REMOVE_STYLE(wxBK_DEFAULT);
    REMOVE_STYLE(wxBK_LEFT);
    REMOVE_STYLE(wxBK_RIGHT);
    REMOVE_STYLE(wxBK_TOP);
    REMOVE_STYLE(wxBK_BOTTOM);

    RegisterEvent(wxT("wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED"), wxT("wxNotebookEvent"),
                  _("The page selection was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING"), wxT("wxNotebookEvent"),
                  _("The page selection is about to be changed. This event can be vetoed"));

    wxArrayString effects;
    effects.Add("wxSHOW_EFFECT_NONE");
    effects.Add("wxSHOW_EFFECT_ROLL_TO_LEFT");
    effects.Add("wxSHOW_EFFECT_ROLL_TO_RIGHT");
    effects.Add("wxSHOW_EFFECT_ROLL_TO_TOP");
    effects.Add("wxSHOW_EFFECT_ROLL_TO_BOTTOM");
    effects.Add("wxSHOW_EFFECT_SLIDE_TO_LEFT");
    effects.Add("wxSHOW_EFFECT_SLIDE_TO_RIGHT");
    effects.Add("wxSHOW_EFFECT_SLIDE_TO_TOP");
    effects.Add("wxSHOW_EFFECT_SLIDE_TO_BOTTOM");
    effects.Add("wxSHOW_EFFECT_BLEND");
    effects.Add("wxSHOW_EFFECT_EXPAND");

    SetPropertyString(_("Common Settings"), "wxSimplebook");
    AddProperty(new ChoiceProperty(PROP_EFFECT, effects, 0,
                                   _("Set the same effect to use for both showing and hiding the pages")));
    m_namePattern = wxT("m_simpleBook");
    SetName(GenerateName());
}

SimpleBookWrapper::~SimpleBookWrapper() {}

wxString SimpleBookWrapper::GetXRCPageClass() const { return wxT("simplebookpage"); }

wxcWidget* SimpleBookWrapper::Clone() const { return new SimpleBookWrapper(); }

void SimpleBookWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/simplebook.h>"); }

wxString SimpleBookWrapper::GetWxClassName() const { return "wxSimplebook"; }

void SimpleBookWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    text << XRCPrefix() << XRCSize() << XRCStyle() << "<effect>" << PropertyString(PROP_EFFECT) << "</effect>"
         << XRCCommonAttributes();

    ChildrenXRC(text, type);
    text << XRCSuffix();
}

wxString SimpleBookWrapper::CppCtorCode() const
{
    wxString code = NotebookBaseWrapper::CppCtorCode();
    code << GetName() << "->SetEffect(" << PropertyString(PROP_EFFECT) << ");\n";
    return code;
}

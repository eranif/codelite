#include "choice_book_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/choicebk.h>

ChoiceBookWrapper::ChoiceBookWrapper()
    : NotebookBaseWrapper(ID_WXCHOICEBOOK)
{
    RegisterEvent(wxT("wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED"), wxT("wxChoicebookEvent"),
                  _("The page selection was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING"), wxT("wxChoicebookEvent"),
                  _("The page selection is about to be changed. This event can be vetoed"));

    SetPropertyString(_("Common Settings"), "wxChoicebook");
    m_namePattern = wxT("m_choicebook");
    SetName(GenerateName());
}

ChoiceBookWrapper::~ChoiceBookWrapper() {}

wxcWidget* ChoiceBookWrapper::Clone() const { return new ChoiceBookWrapper(); }

void ChoiceBookWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/choicebk.h>")); }

wxString ChoiceBookWrapper::GetWxClassName() const { return wxT("wxChoicebook"); }

wxString ChoiceBookWrapper::GetXRCPageClass() const { return wxT("choicebookpage"); }

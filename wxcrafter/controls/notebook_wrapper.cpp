#include "notebook_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/notebook.h>

NotebookWrapper::NotebookWrapper()
    : NotebookBaseWrapper(ID_WXNOTEBOOK)
{
    SetPropertyString(_("Common Settings"), "wxNotebook");
    PREPEND_STYLE_FALSE(wxNB_FIXEDWIDTH);
    PREPEND_STYLE_FALSE(wxNB_MULTILINE);
    PREPEND_STYLE_FALSE(wxNB_NOPAGETHEME);

    RegisterEvent(wxT("wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED"), wxT("wxNotebookEvent"),
                  _("The page selection was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING"), wxT("wxNotebookEvent"),
                  _("The page selection is about to be changed. This event can be vetoed"));
    m_namePattern = wxT("m_notebook");
    SetName(GenerateName());
}

NotebookWrapper::~NotebookWrapper() {}

wxcWidget* NotebookWrapper::Clone() const { return new NotebookWrapper(); }

void NotebookWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/notebook.h>")); }

wxString NotebookWrapper::GetWxClassName() const { return wxT("wxNotebook"); }

wxString NotebookWrapper::GetXRCPageClass() const { return wxT("notebookpage"); }

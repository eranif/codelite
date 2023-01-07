#include "tool_book_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/notebook.h>
#include <wx/toolbook.h>

ToolBookWrapper::ToolBookWrapper()
    : NotebookBaseWrapper(ID_WXTOOLBOOK)
{
    SetPropertyString(_("Common Settings"), "wxToolbook");
#if wxVERSION_NUMBER > 2900
    PREPEND_STYLE_TRUE(wxTBK_BUTTONBAR);
    PREPEND_STYLE_FALSE(wxTBK_HORZ_LAYOUT);
#endif

    RegisterEvent(wxT("wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED"), wxT("wxToolbookEvent"),
                  _("The page selection was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING"), wxT("wxToolbookEvent"),
                  _("The page selection is about to be changed. This event can be vetoed"));
    m_namePattern = wxT("m_toolbook");
    SetName(GenerateName());
}

ToolBookWrapper::~ToolBookWrapper() {}

wxcWidget* ToolBookWrapper::Clone() const { return new ToolBookWrapper(); }

void ToolBookWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add(wxT("#include <wx/toolbook.h>")); }

wxString ToolBookWrapper::GetWxClassName() const { return wxT("wxToolbook"); }

wxString ToolBookWrapper::GetXRCPageClass() const { return wxT("toolbookpage"); }

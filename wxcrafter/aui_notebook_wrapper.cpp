#include "aui_notebook_wrapper.h"
#include "allocator_mgr.h"
#include "wxgui_defs.h"
#include <wx/aui/auibook.h>

AuiNotebookWrapper::AuiNotebookWrapper()
    : NotebookBaseWrapper(ID_WXAUINOTEBOOK)
{
    PREPEND_STYLE_FALSE(wxAUI_NB_TAB_EXTERNAL_MOVE);
    PREPEND_STYLE_FALSE(wxAUI_NB_TAB_SPLIT);
    PREPEND_STYLE_FALSE(wxAUI_NB_TAB_FIXED_WIDTH);
    PREPEND_STYLE_FALSE(wxAUI_NB_SCROLL_BUTTONS);
    PREPEND_STYLE_FALSE(wxAUI_NB_WINDOWLIST_BUTTON);
    PREPEND_STYLE_FALSE(wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    PREPEND_STYLE_FALSE(wxAUI_NB_CLOSE_ON_ALL_TABS);
    PREPEND_STYLE_FALSE(wxAUI_NB_CLOSE_BUTTON);
    PREPEND_STYLE_FALSE(wxAUI_NB_TAB_MOVE);
    PREPEND_STYLE_FALSE(wxAUI_NB_TOP);
    PREPEND_STYLE_FALSE(wxAUI_NB_BOTTOM);
    PREPEND_STYLE_TRUE(wxAUI_NB_DEFAULT_STYLE);

    SetPropertyString(_("Common Settings"), "wxAuiNotebook");
    m_properties.Item(PROP_SIZE)->SetValue(wxT("250,250"));

    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE"), wxT("wxAuiNotebookEvent"),
                  _("A page is about to be closed"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED"), wxT("wxAuiNotebookEvent"), _("A page has been closed"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED"), wxT("wxAuiNotebookEvent"),
                  _("The page selection was changed"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING"), wxT("wxAuiNotebookEvent"),
                  _("The page selection is about to be changed.\nThis event can be vetoed"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_BUTTON"), wxT("wxAuiNotebookEvent"),
                  _("The window list button has been pressed"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG"), wxT("wxAuiNotebookEvent"),
                  _("Dragging is about to begin"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_END_DRAG"), wxT("wxAuiNotebookEvent"), _("Dragging has ended"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION"), wxT("wxAuiNotebookEvent"),
                  _("Emitted during a drag and drop operation"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND"), wxT("wxAuiNotebookEvent"),
                  _("Whether to allow a tab to be dropped. This event must be specially allowed"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE"), wxT("wxAuiNotebookEvent"),
                  _("Notify that the tab has been dragged"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK"), wxT("wxAuiNotebookEvent"),
                  _("Double clicked on the tabs background area"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN"), wxT("wxAuiNotebookEvent"),
                  _("The middle mouse button is pressed on a tab"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP"), wxT("wxAuiNotebookEvent"),
                  _("The middle mouse button is released on a tab"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN"), wxT("wxAuiNotebookEvent"),
                  _("The right mouse button is pressed on a tab"));
    RegisterEvent(wxT("wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP"), wxT("wxAuiNotebookEvent"),
                  _("The right mouse button is released on a tab"));
    m_namePattern = wxT("m_auiBook");
    SetName(GenerateName());
}

AuiNotebookWrapper::~AuiNotebookWrapper() {}

wxString AuiNotebookWrapper::GetXRCPageClass() const { return wxT("notebookpage"); }

wxcWidget* AuiNotebookWrapper::Clone() const { return new AuiNotebookWrapper(); }

void AuiNotebookWrapper::GetIncludeFile(wxArrayString& headers) const
{
    headers.Add(wxT("#include <wx/aui/auibook.h>"));
}

wxString AuiNotebookWrapper::GetWxClassName() const { return wxT("wxAuiNotebook"); }

#include "ToolBoxPanel.h"
#include "allocator_mgr.h"
#include "wxc_settings.h"
#include "wxguicraft_main_view.h"

ToolBoxPanel::ToolBoxPanel(wxWindow* parent, GUICraftMainPanel* mainView)
    : ToolBoxPanelBaseClass(parent)
    , m_mainView(mainView)
{
}

ToolBoxPanel::~ToolBoxPanel() {}

void ToolBoxPanel::OnControlUI(wxUpdateUIEvent& event)
{

    GUICraftItemData* data = m_mainView->GetSelItemData();

    if(data && data->m_wxcWidget) {
        wxcWidget* widget = data->m_wxcWidget;

        if(widget->GetType() == ID_WXSCROLLEDWIN && event.GetId() == ID_WXAUIMANAGER &&
           Allocator::Instance()->GetInsertionType(event.GetId(), data->m_wxcWidget->GetType(), false) ==
               Allocator::INSERT_CHILD) {
            event.Enable(false);

        } else if(widget->IsTopWindow() && widget->HasMenuBar() && event.GetId() == ID_WXMENUBAR) {
            event.Enable(false);

        } else if(widget->IsTopWindow() && widget->HasToolBar() && event.GetId() == ID_WXTOOLBAR) {
            event.Enable(false);

        } else if(widget->IsTopWindow() && widget->HasStatusBar() && event.GetId() == ID_WXSTATUSBAR) {
            event.Enable(false);

        } else if(Allocator::Instance()->GetInsertionType(event.GetId(), data->m_wxcWidget->GetType(), false, widget) !=
                  Allocator::INSERT_NONE) {
            event.Enable(true);

        } else {
            event.Enable(false);
        }

    } else if(!data) {
        // Root item is selected
        int type = wxcWidget::GetWidgetType(event.GetId());
        event.Enable(type == TYPE_FORM || type == TYPE_WIZARD || type == TYPE_FORM_FRAME || type == TYPE_IMGLIST);

    } else {
        event.Enable(false);
    }
}
/*
void ToolBoxPanel::OnNewControl(wxRibbonToolBarEvent& event)
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, event.GetId());
    m_mainView->GetEventHandler()->AddPendingEvent( evt );
}

void ToolBoxPanel::OnCustomControl(wxRibbonToolBarEvent& event)
{
    wxMenu menu;
    const CustomControlTemplateMap_t& controls =  wxcSettings::Get().GetTemplateClasses();
    CustomControlTemplateMap_t::const_iterator iter = controls.begin();

    if ( controls.empty() == false ) {
        for( ; iter != controls.end(); ++iter ) {
            menu.Append(iter->second.GetControlId(), iter->first);
            menu.Connect(iter->second.GetControlId(), wxEVT_COMMAND_MENU_SELECTED,
wxCommandEventHandler(ToolBoxPanel::OnMenuSelection), NULL, this);
        }
        menu.AppendSeparator();
    }

    menu.Append(XRCID("define_custom_controls"), _("Define custom control..."));
    event.PopupMenu(&menu);
}

void ToolBoxPanel::OnMenuSelection(wxCommandEvent& e)
{
    m_mainView->AddCustomControl( e.GetId() );
}
*/

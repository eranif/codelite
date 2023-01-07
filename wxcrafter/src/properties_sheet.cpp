#include "properties_sheet.h"

#include "choice_property.h"
#include "color_property.h"
#include "file_picker_ctrl.h"
#include "font_property.h"
#include "multi_string_ctrl.h"
#include "properties_list_view.h"
#include "virtual_folder_picker_ctrl.h"
#include "winid_property.h"
#include "wxguicraft_main_view.h"

#include <event_notifier.h>
#include <wx/app.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/wupdlock.h>

PropertiesSheet::PropertiesSheet(wxWindow* parent)
    : PropertiesSheetBase(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxTAB_TRAVERSAL)
{
    GetSizer()->Clear(true);
    m_table = new PropertiesListView(this);
    GetSizer()->Add(m_table, 1, wxEXPAND);
    GetSizer()->Layout();
}

PropertiesSheet::~PropertiesSheet() {}

void PropertiesSheet::Construct(wxcWidget* wb) { m_table->Construct(wb); }

void PropertiesSheet::ConstructProjectSettings() { m_table->ConstructProjectSettings(); }

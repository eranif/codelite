#include "properties_sheet.h"

#include "properties_list_view.h"

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

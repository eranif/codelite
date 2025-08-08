#ifndef __properties_sheet__
#define __properties_sheet__

#include "gui.h"
#include "wxc_widget.h"

class PropertiesListView;
class PropertiesSheet : public PropertiesSheetBase
{
    PropertiesListView* m_table;

public:
    PropertiesSheet(wxWindow* parent);
    ~PropertiesSheet() override = default;
    void Construct(wxcWidget* wb);
    void ConstructProjectSettings();
};

#endif // __properties_sheet__

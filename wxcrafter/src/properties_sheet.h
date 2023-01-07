#ifndef __properties_sheet__
#define __properties_sheet__

#include "gui.h"
#include "wx_ordered_map.h"
#include "wxc_widget.h"
#include <wx/filepicker.h>

class PropertiesListView;
class PropertiesSheet : public PropertiesSheetBase
{
    PropertiesListView* m_table;

public:
    PropertiesSheet(wxWindow* parent);
    virtual ~PropertiesSheet();
    void Construct(wxcWidget* wb);
    void ConstructProjectSettings();
};

#endif // __properties_sheet__

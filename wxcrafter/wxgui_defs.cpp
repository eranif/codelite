#include "wxgui_defs.h"
#include "wxc_widget.h"
#include "wxgui_helpers.h"

void WxStyleInfo::UpdateStyleString(wxString& styleAsString) const
{
    if(style_group.IsEmpty()) return;

    bool allGroupExists = true;
    wxString newStyle;
    wxArrayString arr = wxCrafter::Split(styleAsString, ",");
    for(size_t i = 0; i < style_group.GetCount(); ++i) {
        if(arr.Index(style_group.Item(i)) == wxNOT_FOUND) {
            allGroupExists = false;
            break;
        }
    }

    if(allGroupExists && arr.Index(style_name) == wxNOT_FOUND) {
        // add the style
        arr.Add(style_name);

    } else if(!allGroupExists && arr.Index(style_name) != wxNOT_FOUND) {
        // not all the group exists and the style exists
        // remove it
        arr.Remove(style_name);
    }

    styleAsString = wxCrafter::Join(arr, ",");
}

bool WxStyleInfo::IsGroupConditionMet(wxcWidget* widget) const
{
    if(style_group.IsEmpty()) return true;

    for(size_t i = 0; i < style_group.GetCount(); ++i) {
        if(!widget->IsSizerFlagChecked(style_group.Item(i))) return false;
    }
    return true;
}

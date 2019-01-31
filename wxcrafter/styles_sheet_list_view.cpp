#include "styles_sheet_list_view.h"
#include "wxc_widget.h"
#include "wxgui_defs.h"
#include "wxguicraft_main_view.h"
#include <event_notifier.h>
#include <wx/wupdlock.h>

StylesSheetListView::StylesSheetListView()
    : m_wxcWidget(NULL)
    , m_controlFlags(NULL)
    , m_windowFlags(NULL)
{
}

StylesSheetListView::~StylesSheetListView() {}

void StylesSheetListView::Construct(wxPropertyGrid* pg, wxcWidget* wb)
{
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(pg);
#endif

    pg->Clear();
    m_wxcWidget = wb;

    CHECK_POINTER(wb);

    wxArrayString windowStyles, controlStyles;
    wxArrayInt windowStylesInt, controlStylesInt;
    size_t windowStylesValue, controlStylesValue;
    size_t count = 0;

    windowStylesValue = 0;
    controlStylesValue = 0;

    wxcWidget::MapStyles_t flags = wb->GetStyles();
    wxcWidget::MapStyles_t::ConstIterator iter = flags.Begin();

    // The control specific flags will show up first
    for(; iter != flags.End(); ++iter) {
        if(iter->first == "0") {
            iter++;
            // We found the terminator
            break;
        }
        controlStyles.Add(iter->first); // Add the style name
        // Rather than using the real bitvalue of the style flag, create our own fake one
        // This is to avoid clashes between e.g. the composite wxDEFAULT_DIALOG_STYLE and its constituents e.g.
        // wxCAPTION
        size_t flag_fake_value = (1 << count++);
        controlStylesInt.Add(flag_fake_value); // Add the value
        if(iter->second.is_set) { controlStylesValue |= flag_fake_value; }
    }

    // From this point on, everything is belonged to the
    // wxWindow styles
    for(; iter != flags.End(); ++iter) {

        windowStyles.Add(iter->first);               // Add the style name
        windowStylesInt.Add(iter->second.style_bit); // Add the value
        if(iter->second.is_set) { windowStylesValue |= iter->second.style_bit; }
    }

    m_controlFlags = NULL;
    m_windowFlags = NULL;
    if(controlStyles.IsEmpty() == false) {
        m_controlFlags = (wxFlagsProperty*)pg->Append(
            new wxFlagsProperty(wb->GetWxClassName(), wxPG_LABEL, controlStyles, controlStylesInt, controlStylesValue));
        pg->SetPropertyAttribute(m_controlFlags, wxPG_BOOL_USE_CHECKBOX, true);
        pg->Expand(m_controlFlags);
    }

    if(windowStyles.IsEmpty() == false) {
        m_windowFlags = (wxFlagsProperty*)pg->Append(
            new wxFlagsProperty(_("wxWindow"), wxPG_LABEL, windowStyles, windowStylesInt, windowStylesValue));
        pg->SetPropertyAttribute(m_windowFlags, wxPG_BOOL_USE_CHECKBOX, true);
        pg->Expand(m_windowFlags);
    }
}

void StylesSheetListView::Clear(wxPropertyGrid* pg)
{
    pg->Clear();
    m_controlFlags = NULL;
    m_windowFlags = NULL;
}

void StylesSheetListView::Changed(wxPropertyGrid* pg, wxPropertyGridEvent& e)
{
    // We used fake flags in controlStyles to avoid clashes, so we can't just retrieve the value as integer
    // Translating back into the real values won't work either: it just produces the clashes again. So use the strings
    if(m_wxcWidget) {
        wxString controlStyles;
        wxString windowStyles;
        if(m_windowFlags) { windowStyles = m_windowFlags->GetValueAsString(); }

        if(m_controlFlags) { controlStyles = m_controlFlags->GetValueAsString(); }

        // joint flags
        if(!controlStyles.empty() && !windowStyles.empty()) { controlStyles << ','; }
        wxArrayString arr = ::wxStringTokenize(controlStyles + windowStyles, ",", wxTOKEN_STRTOK);
        // Reset the styles
        m_wxcWidget->ClearStyles();
        for(size_t i = 0; i < arr.GetCount(); ++i) {
            m_wxcWidget->EnableStyle(arr.Item(i).Trim().Trim(false), true);
        }

        // Notify about modifications
        wxCommandEvent evt(wxEVT_PROPERTIES_MODIFIED);
        EventNotifier::Get()->AddPendingEvent(evt);
    }
}

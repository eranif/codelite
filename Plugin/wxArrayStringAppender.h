#ifndef WXARRAYSTRINGAPPENDER_H
#define WXARRAYSTRINGAPPENDER_H

#include <wx/arrstr.h>

struct wxArrayStringAppender {
    wxArrayString& m_arr;
    wxString       m_strToAppend;
    bool           m_prepending;
    
    wxArrayStringAppender(wxArrayString& arr, const wxString &appendThis, bool prepending = false) 
        : m_arr(arr) 
        , m_strToAppend(appendThis)
        , m_prepending(prepending)
    {}
    
    void operator()(wxString &str) {
        if ( m_prepending ) {
            str.Prepend(m_strToAppend);
        } else {
            str.Append(m_strToAppend);
        }
    }
};

#endif // WXARRAYSTRINGAPPENDER_H

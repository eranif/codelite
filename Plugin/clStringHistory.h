#ifndef CLSTRINGHISTORY_H
#define CLSTRINGHISTORY_H
#include <wx/string.h>
#include <wx/arrstr.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clStringHistory
{
    wxArrayString m_strings;
    int           m_index;

public:
    clStringHistory();
    virtual ~clStringHistory();

    /**
     * @brief can we move to the next item in the history?
     */
    bool CanNext();
    /**
     * @brief can we move to the previous item in the history?
     */
    bool CanPrev();

    /**
     * @brief initialize the history strings
     */
    void SetStrings ( const wxArrayString& strings ) {
        this->m_strings = strings;
        m_index = 0;
    }



    /**
     * @brief return the current item
     */
    bool Current( wxString &str );
    /**
     * @brief return the next item in the history
     * @param str [output]
     * @return true on success, false otherwise
     */
    bool Next( wxString &str );
    /**
     * @brief return the previous item in the history
     * @param str [output]
     * @return true on success, false otherwise
     */
    bool Previous( wxString &str );
};

#endif // CLSTRINGHISTORY_H

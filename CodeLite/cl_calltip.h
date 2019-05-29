//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cl_calltip.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef CODELITE_CALLTIP_H
#define CODELITE_CALLTIP_H

#include "tokenizer.h"
#include "smart_ptr.h"
#include "entry.h"
#include "codelite_exports.h"

struct clTipInfo {
    wxString str;
    std::vector<std::pair<int, int> > paramLen;
};

/**
 * A call tip function that wraps a tip strings for function prototypes.
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 *
 * \date 09-18-2006
 * \author Eran
 */
class WXDLLIMPEXP_CL clCallTip
{
    std::vector<clTipInfo> m_tips;
    int m_curr;
    void Initialize(const std::vector<TagEntryPtr> &tags);

public:
    
    /**
     * @brief format list of tags into calltips
     */
    static void FormatTagsToTips( const TagEntryPtrVector_t &tags,  std::vector<clTipInfo>& tips);
    
    /**
     * Constructor
     * \param tips input tips
     */
    clCallTip(const std::vector<TagEntryPtr> & tips );

    /**
     * default constructor
     */
    clCallTip();

    /**
     * Copy constructor
     */
    clCallTip(const clCallTip& rhs);

    /**
     * Assignment operator
     * \param rhs right hand side
     * \return this
     */
    clCallTip& operator=(const clCallTip& rhs);

    /**
     * Destructor
     * \return
     */
    virtual ~clCallTip() {}

    /**
     * Show next tip, if we are at last tip, return the first tip or empty string if no tips exists
     * \return next tip
     */
    wxString Next() ;

    /**
     * Show previous tip, if we are at first tip, return the last tip or empty string if no tips exists
     * \return previous tip
     */
    wxString Prev() ;

    /**
     * return the first tip
     */
    wxString First();

    /**
     * @brief return the current tip
     * @return return the current tip
     */
    wxString Current();
    
    /**
     * @brief select the first tip that has at least argcount
     * @return true if we managed to find this tip, false otherwise
     */
    bool SelectTipToMatchArgCount(size_t argcount);
    
    /**
     * Return number of tips stored in this object
     * \return number of tips
     */
    int Count() const;

    /**
     * \brief return all tips as a single string
     */
    wxString All();

    /**
     * @brief get the highlight offset & width for the current tip
     * @param index paramter index
     * @param start [output]
     * @param len [output]
     */
    void GetHighlightPos(int index, int &start, int &len);

    int GetCurr() const {
        return m_curr;
    }
    
    /**
     * @brief set the tip to a specific tag
     */
    void SelectSiganture( const wxString &signature );
    
private:
    wxString TipAt(int at);
};

typedef SmartPtr<clCallTip> clCallTipPtr;
#endif // CODELITE_CALLTIP_H

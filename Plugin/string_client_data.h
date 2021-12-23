//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : string_client_data.h
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

#ifndef MYSTRINGCLIENTDATA_H
#define MYSTRINGCLIENTDATA_H

#include <wx/clntdata.h>
#include <wx/string.h>

/**
 * @class MyStringClientData
 * @author Eran
 * @date 11/23/2009
 * @file string_client_data.h
 * @brief provide our own string client data, since compilation is broken under window
 * since wxStringClientData does not provide destructor (virtual)
 */
class MyStringClientData : public wxClientData
{
public:
    MyStringClientData()
        : m_data()
    {
    }
    MyStringClientData(const wxString& data)
        : m_data(data)
    {
    }
    virtual ~MyStringClientData() {}

    void SetData(const wxString& data) { m_data = data; }
    const wxString& GetData() const { return m_data; }

private:
    wxString m_data;
};

#endif // MYSTRINGCLIENTDATA_H

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : attribute_style.h
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
#ifndef ATTRIBUTE_STYLE_H
#define ATTRIBUTE_STYLE_H

#include "wx/string.h"
#include <list>
#include <wx/colour.h>

// Set default font size per-OS
#if defined(__WXGTK__)
#define FONT_SIZE 10
#elif defined(__WXMSW__)
#define FONT_SIZE 10
#else
#define FONT_SIZE 12
#endif

// Special attributes IDs
#define FOLD_MARGIN_ATTR_ID -1
#define SEL_TEXT_ATTR_ID -2
#define CARET_ATTR_ID -3
#define WHITE_SPACE_ATTR_ID -4

#define LINE_NUMBERS_ATTR_ID 33

#define STYLE_PROPERTY_NULL_ID -999

class StyleProperty
{
    int m_id;
    wxString m_fgColour;
    wxString m_bgColour;
    long m_fontSize;
    wxString m_name;
    wxString m_faceName;
    bool m_bold;
    bool m_italic;
    bool m_underlined;
    bool m_eolFilled;
    int m_alpha;
public:
    typedef std::list<StyleProperty> List_t;
    struct FindByName {
        wxString m_name;
        FindByName(const wxString &name) : m_name(name) {}
        bool operator()(const StyleProperty& other) const {
            return m_name == other.GetName();
        }
    };

    struct FindByID {
        int m_id;
        FindByID(int id) : m_id(id) {}
        bool operator()(const StyleProperty& other) const {
            return m_id == other.GetId();
        }
    };

public:
    StyleProperty(int id,
                  const wxString& fgColour,
                  const wxString& bgColour,
                  const int fontSize,
                  const wxString& name,
                  const wxString& face,
                  bool bold,
                  bool italic,
                  bool underline,
                  bool eolFilled,
                  int alpha)
        : m_id(id)
        , m_fgColour(fgColour)
        , m_bgColour(bgColour)
        , m_fontSize(fontSize)
        , m_name(name)
        , m_faceName(face)
        , m_bold(bold)
        , m_italic(italic)
        , m_underlined(underline)
        , m_eolFilled(eolFilled)
        , m_alpha(alpha){};

    StyleProperty()
        : m_id(0)
        , m_fgColour(_T("BLACK"))
        , m_bgColour(_T("WHITE"))
        , m_fontSize(10)
        , m_name(wxEmptyString)
        , m_faceName(_T("Courier"))
        , m_bold(false)
        , m_italic(false)
        , m_underlined(false)
        , m_eolFilled(false)
        , m_alpha(0){};

    StyleProperty(const StyleProperty& rhs)
    {
        *this = rhs;
    };

    StyleProperty& operator=(const StyleProperty& rhs)
    {
        m_fgColour = rhs.m_fgColour;
        m_bgColour = rhs.m_bgColour;
        m_faceName = rhs.m_faceName;
        m_bold = rhs.m_bold;
        m_fontSize = rhs.m_fontSize;
        m_name = rhs.m_name;
        m_id = rhs.m_id;
        m_italic = rhs.m_italic;
        m_underlined = rhs.m_underlined;
        m_eolFilled = rhs.m_eolFilled;
        m_alpha = rhs.m_alpha;
        return *this;
    }

    virtual ~StyleProperty()
    {
    }

    bool IsNull() const {
        return m_id == STYLE_PROPERTY_NULL_ID;
    }
    
    void SetAlpha(int alpha)
    {
        this->m_alpha = alpha;
    }
    int GetAlpha() const
    {
        return m_alpha;
    }

    wxString GetFgColour() const
    {
        return m_fgColour;
    }

    wxString GetBgColour() const
    {
        return m_bgColour;
    }

    void SetEolFilled(bool eolFilled)
    {
        this->m_eolFilled = eolFilled;
    }
    bool GetEolFilled() const
    {
        return m_eolFilled;
    }
    const long GetFontSize() const
    {
        if(m_fontSize <= 0)
            return FONT_SIZE;
        return m_fontSize;
    }

    const wxString& GetFaceName() const
    {
        return m_faceName;
    }
    bool IsBold() const
    {
        return m_bold;
    }
    const wxString& GetName() const
    {
        return m_name;
    }
    int GetId() const
    {
        return m_id;
    }
    void SetBgColour(const wxString& colour)
    {
        m_bgColour = colour;
    }
    void SetFgColour(const wxString& colour)
    {
        m_fgColour = colour;
    }
    void SetFontSize(long size)
    {
        m_fontSize = size;
    }
    void SetFaceName(const wxString& face)
    {
        m_faceName = face;
    }
    void SetBold(bool bold)
    {
        m_bold = bold;
    }
    void SetId(int id)
    {
        m_id = id;
    }
    void SetItalic(const bool& italic)
    {
        this->m_italic = italic;
    }
    const bool& GetItalic() const
    {
        return m_italic;
    }
    void SetUnderlined(const bool& underlined)
    {
        this->m_underlined = underlined;
    }
    const bool& GetUnderlined() const
    {
        return m_underlined;
    }
};
#endif

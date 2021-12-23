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

#include "JSON.h"
#include "codelite_exports.h"
#include "wx/string.h"

#include <list>
#include <map>
#include <wx/colour.h>

// Set default font size per-OS
#if defined(__WXGTK__)
#define FONT_SIZE 12
#elif defined(__WXMSW__)
#define FONT_SIZE 12
#else
#define FONT_SIZE 14
#endif

// Special attributes IDs
#define FOLD_MARGIN_ATTR_ID -1
#define SEL_TEXT_ATTR_ID -2
#define CARET_ATTR_ID -3
#define WHITE_SPACE_ATTR_ID -4
#define LINE_NUMBERS_ATTR_ID 33
#define STYLE_PROPERTY_NULL_ID -999

class WXDLLIMPEXP_SDK StyleProperty
{
    int m_id;
    wxString m_fgColour;
    wxString m_bgColour;
    long m_fontSize;
    wxString m_name;
    wxString m_faceName;
    size_t m_flags;
    int m_alpha;

public:
    enum eStyleFlags {
        kNone = 0,
        kItalic = (1 << 0),
        kBold = (1 << 1),
        kUnderline = (1 << 2),
        kEolFilled = (1 << 3),
    };

public:
    typedef std::map<long, StyleProperty> Map_t;
    struct FindByName {
        wxString m_name;
        FindByName(const wxString& name)
            : m_name(name)
        {
        }
        bool operator()(const std::pair<long, StyleProperty>& other) const { return m_name == other.second.GetName(); }
    };

    struct FindByID {
        int m_id;
        FindByID(int id)
            : m_id(id)
        {
        }
        bool operator()(const std::pair<long, StyleProperty>& other) const { return m_id == other.first; }
    };

protected:
    inline void EnableFlag(eStyleFlags flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    inline bool HasFlag(eStyleFlags flag) const { return m_flags & flag; }

public:
    StyleProperty(int id, const wxString& fgColour, const wxString& bgColour, const int fontSize, const wxString& name,
                  const wxString& face, bool bold, bool italic, bool underline, bool eolFilled, int alpha);
    StyleProperty();
    StyleProperty(const StyleProperty& rhs) { *this = rhs; };
    StyleProperty& operator=(const StyleProperty& rhs);
    ~StyleProperty() {}

    //----------------------------
    // Serialization
    //----------------------------

    /**
     * @brief unserialize an object from JSON
     */
    void FromJSON(JSONItem json);

    /**
     * @brief serialize this style property into a JSON object
     * @param portable set to true to avoid serializing non portables properties (e.g. face name)
     */
    JSONItem ToJSON(bool portable = false) const;

    // Accessors

    bool IsNull() const { return m_id == STYLE_PROPERTY_NULL_ID; }

    void SetAlpha(int alpha) { this->m_alpha = alpha; }
    int GetAlpha() const { return m_alpha; }

    const wxString& GetFgColour() const { return m_fgColour; }
    const wxString& GetBgColour() const { return m_bgColour; }

    void SetEolFilled(bool eolFilled) { EnableFlag(kEolFilled, eolFilled); }
    bool GetEolFilled() const { return HasFlag(kEolFilled); }
    long GetFontSize() const
    {
        if(m_fontSize <= 0)
            return FONT_SIZE;
        return m_fontSize;
    }

    const wxString& GetFaceName() const { return m_faceName; }
    bool IsBold() const { return HasFlag(kBold); }
    const wxString& GetName() const { return m_name; }
    int GetId() const { return m_id; }
    void SetBgColour(const wxString& colour) { m_bgColour = colour; }
    void SetFgColour(const wxString& colour) { m_fgColour = colour; }
    void SetFontSize(long size) { m_fontSize = size; }
    void SetFaceName(const wxString& face) { m_faceName = face; }
    void SetBold(bool bold) { EnableFlag(kBold, bold); }
    void SetId(int id) { m_id = id; }
    void SetItalic(bool italic) { EnableFlag(kItalic, italic); }
    bool GetItalic() const { return HasFlag(kItalic); }
    void SetUnderlined(bool underlined) { EnableFlag(kUnderline, underlined); }
    bool GetUnderlined() const { return HasFlag(kUnderline); }
    void SetName(const wxString& name) { this->m_name = name; }
};
#endif

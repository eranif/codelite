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
#include <vector>
#include <wx/colour.h>
#include <wx/stc/stc.h>

// Special attributes IDs
#define FOLD_MARGIN_ATTR_ID -1
#define SEL_TEXT_ATTR_ID -2
#define CARET_ATTR_ID -3
#define WHITE_SPACE_ATTR_ID -4
#define LINE_NUMBERS_ATTR_ID 33
#define STYLE_PROPERTY_NULL_ID -999

class WXDLLIMPEXP_SDK StyleProperty
{
    int m_id = 0;
    wxString m_name;
    wxString m_fontDesc;
    wxString m_fgColour;
    wxString m_bgColour;
    int m_fontSize = wxNOT_FOUND;
    size_t m_flags = 0;

public:
    enum eStyleFlags {
        kNone = 0,
        kItalic = (1 << 0),
        kBold = (1 << 1),
        kUnderline = (1 << 2),
        kEolFilled = (1 << 3),
        kIsSubStyle = (1 << 4),
    };

public:
    typedef std::vector<StyleProperty> Vec_t;

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
    StyleProperty(int id, const wxString& name, const wxString& fgColour, const wxString& bgColour, const int fontSize,
                  bool bold, bool italic, bool underline, bool eolFilled);
    StyleProperty(int id, const wxString& name, const wxString& fontDesc, const wxString& fgColour,
                  const wxString& bgColour, bool eolFilled);
    StyleProperty();
    ~StyleProperty() {}

    bool IsSubstyle() const { return m_flags & kIsSubStyle; }
    void SetSubstyle() { m_flags |= kIsSubStyle; }

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

    const wxString& GetFgColour() const { return m_fgColour; }
    const wxString& GetBgColour() const { return m_bgColour; }

    void SetEolFilled(bool eolFilled) { EnableFlag(kEolFilled, eolFilled); }
    bool GetEolFilled() const { return HasFlag(kEolFilled); }
    wxString GetFontInfoDesc() const;
    bool HasFontInfoDesc() const { return !m_fontDesc.empty(); }
    void SetFontInfoDesc(const wxString& desc);
    bool IsBold() const { return HasFlag(kBold); }
    const wxString& GetName() const { return m_name; }
    int GetId() const { return m_id; }
    void SetBgColour(const wxString& colour) { m_bgColour = colour; }
    void SetFgColour(const wxString& colour) { m_fgColour = colour; }
    void SetBold(bool bold) { EnableFlag(kBold, bold); }
    void SetId(int id) { m_id = id; }
    void SetItalic(bool italic) { EnableFlag(kItalic, italic); }
    bool GetItalic() const { return HasFlag(kItalic); }
    void SetUnderlined(bool underlined) { EnableFlag(kUnderline, underlined); }
    bool GetUnderlined() const { return HasFlag(kUnderline); }
    void SetName(const wxString& name) { this->m_name = name; }
    void FromAttributes(wxFont* font) const;
    void SetFontSize(int size) { m_fontSize = size; }
};
#endif

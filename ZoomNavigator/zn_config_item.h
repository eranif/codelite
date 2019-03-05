//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : zn_config_item.h
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

#ifndef ZNCONFIGITEM_H
#define ZNCONFIGITEM_H

#include "cl_config.h" // Base class: clConfigItem

class znConfigItem : public clConfigItem
{
    wxString m_highlightColour;
    bool m_enabled;
    int m_zoomFactor;
    bool m_useScrollbar;

public:
    znConfigItem(); 
    virtual ~znConfigItem();

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    void SetEnabled(bool enabled) { this->m_enabled = enabled; }
    void SetHighlightColour(const wxString& highlightColour) { this->m_highlightColour = highlightColour; }
    bool IsEnabled() const { return m_enabled; }
    const wxString& GetHighlightColour() const { return m_highlightColour; }
    void SetZoomFactor(int zoomFactor) { this->m_zoomFactor = zoomFactor; }
    int GetZoomFactor() const { return m_zoomFactor; }
    void SetUseScrollbar(bool useScrollbar) { this->m_useScrollbar = useScrollbar; }
    bool IsUseScrollbar() const { return m_useScrollbar; }
};

#endif // ZNCONFIGITEM_H

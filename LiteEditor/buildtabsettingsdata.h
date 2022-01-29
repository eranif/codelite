//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buildtabsettingsdata.h
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
#ifndef __buildtabsettingsdata__
#define __buildtabsettingsdata__

#include "serialized_object.h"

#include <wx/font.h>

class BuildTabSettingsData : public SerializedObject
{
public:
    enum ErrorsWarningStyle { MARKER_BOOKMARKS, MARKER_ANNOTATE, MARKER_NONE };
    enum ScrollToLine { SCROLL_TO_FIRST_ERROR, SCROLL_TO_NOWHERE };

private:
    ScrollToLine m_scroll_to = ScrollToLine::SCROLL_TO_FIRST_ERROR;
    ErrorsWarningStyle m_errorWarningStyle = BuildTabSettingsData::MARKER_BOOKMARKS;
    bool m_autoHide = false;
    bool m_autoShow = true;
    bool m_skipWarnings = false;

public:
    BuildTabSettingsData(const BuildTabSettingsData& rhs);
    BuildTabSettingsData& operator=(const BuildTabSettingsData& rhs);

    BuildTabSettingsData();
    ~BuildTabSettingsData();

    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);

    void SetSkipWarnings(const bool& skipWarnings) { this->m_skipWarnings = skipWarnings; }
    bool IsSkipWarnings() const { return m_skipWarnings; }

    // Setters
    void SetAutoHide(bool autoHide) { this->m_autoHide = autoHide; }
    bool IsAutoHide() const { return m_autoHide; }

    void SetAutoShow(bool autoShow) { this->m_autoShow = autoShow; }
    bool IsAutoShow() const { return m_autoShow; }

    // when build ends...
    void SetScrollTo(ScrollToLine where) { this->m_scroll_to = where; }
    ScrollToLine GetScrollTo() const { return m_scroll_to; }

    void SetErrorWarningStyle(ErrorsWarningStyle errorWarningStyle) { this->m_errorWarningStyle = errorWarningStyle; }
    ErrorsWarningStyle GetErrorWarningStyle() const { return m_errorWarningStyle; }
};
#endif // __buildtabsettingsdata__

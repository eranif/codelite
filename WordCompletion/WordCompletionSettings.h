//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WordCompletionSettings.h
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

#ifndef WORDCOMPLETIONSETTINGS_H
#define WORDCOMPLETIONSETTINGS_H

#include "cl_config.h" // Base class: clConfigItem

class WordCompletionSettings : public clConfigItem
{
public:
    enum {
        kComparisonStartsWith = 0,
        kComparisonContains = 1,
    };

private:
    int m_comparisonMethod;
    bool m_enabled;

public:
    WordCompletionSettings();
    virtual ~WordCompletionSettings();

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    void SetComparisonMethod(int comparisonMethod) { this->m_comparisonMethod = comparisonMethod; }
    int GetComparisonMethod() const { return m_comparisonMethod; }

    void SetEnabled(bool enabled) { this->m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
    WordCompletionSettings& Load();
    WordCompletionSettings& Save();
};

#endif // WORDCOMPLETIONSETTINGS_H

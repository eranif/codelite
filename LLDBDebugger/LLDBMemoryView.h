//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBMemoryView.h
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

#ifndef LLDBMEMORYVIEW_H
#define LLDBMEMORYVIEW_H

#include "UI.h"
#include "../LiteEditor/memoryview.h"
#include <unordered_map>
#include <set>

class LLDBPlugin;
class LLDBEvent;

class LLDBMemoryView : public MemoryView
{
public:
    LLDBMemoryView(wxWindow* parent, LLDBPlugin& plugin, const size_t initialColumns);
    ~LLDBMemoryView();

private:
    LLDBPlugin& m_plugin;
    wxULongLong_t m_targetProcessAddress;

    void Refresh();
    void OnViewExpression(LLDBEvent& lldbEvent);
    void OnMemoryViewResponse(LLDBEvent& lldbEvent);

    // MemoryView overrides.
    virtual void OnMemorySize(wxCommandEvent& event) override;
    virtual void OnNumberOfRows(wxCommandEvent& event) override;
    virtual void OnEvaluate(wxCommandEvent& event) override;
    virtual void OnUpdate(wxCommandEvent& event) override;
};
#endif // LLDBMEMORYVIEW_H

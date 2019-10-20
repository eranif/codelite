//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : EditorOptionsGeneralEdit.h
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

#ifndef EDITOROPTIONSGENERALEDIT_H
#define EDITOROPTIONSGENERALEDIT_H

#include "editoroptionsgeneralguidespanelbase.h"
#include "optionsconfig.h"
#include "treebooknodebase.h"

class EditorOptionsGeneralEdit : public EditorOptionsGeneralEditBase, public TreeBookNode<EditorOptionsGeneralEdit>
{
public:
    EditorOptionsGeneralEdit(wxWindow* parent);
    virtual ~EditorOptionsGeneralEdit();
    void Save(OptionsConfigPtr options);

protected:
    virtual void OnValueChanged(wxPropertyGridEvent& event);
};
#endif // EDITOROPTIONSGENERALEDIT_H

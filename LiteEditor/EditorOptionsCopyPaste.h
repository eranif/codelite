//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : EditorOptionsCopyPaste.h
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

#ifndef EDITOROPTIONSCOPYPASTE_H
#define EDITOROPTIONSCOPYPASTE_H

#include "editor_options_copy_paste.h"
#include "treebooknodebase.h"
#include "optionsconfig.h"

class EditorOptionsCopyPaste : public EditorOptionsCopyPasteBase, public TreeBookNode<EditorOptionsCopyPaste>
{
public:
    EditorOptionsCopyPaste(wxWindow* parent);
    virtual ~EditorOptionsCopyPaste();
    virtual void Save(OptionsConfigPtr options);
};
#endif // EDITOROPTIONSCOPYPASTE_H

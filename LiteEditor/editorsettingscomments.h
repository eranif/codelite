//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingscomments.h
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

#ifndef __editorsettingscomments__
#define __editorsettingscomments__

#include "OptionsConfigPage.hpp"
#include "commentconfigdata.h"

class EditorSettingsComments : public OptionsConfigPage
{
    CommentConfigData m_data;

public:
    EditorSettingsComments(wxWindow* parent, OptionsConfigPtr options);
    virtual ~EditorSettingsComments() {}
};

#endif // __editorsettingscomments__

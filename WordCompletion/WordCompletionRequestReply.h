//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WordCompletionRequestReply.h
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

#ifndef WordCompletionRequestReply_H__
#define WordCompletionRequestReply_H__

#include "worker_thread.h"

struct WordCompletionThreadRequest : public ThreadRequest {
    wxString buffer;
    wxString filter;
    wxFileName filename;
    bool insertSingleMatch;
};

struct WordCompletionThreadReply {
    wxStringSet_t suggest;
    wxFileName filename;
    wxString filter;
    bool insertSingleMatch;
};

#endif

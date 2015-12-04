//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_helpers.h
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

#ifndef PHP_HELPERS_H
#define PHP_HELPERS_H

#include <wx/string.h>
#define PHP_PREFIX_WITH_SPACE "<?php "
#define PHP_PREFIX            "<?php"

template <class T>
class TDeleter {
    T* m_token;
public:
    TDeleter(T* token) : m_token( token ) {}
    ~TDeleter() { wxDELETE(m_token); }
};

#endif //  PHP_HELPERS_H

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : memcheckdefs.h
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

/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 *
 * Sets some hardcoded constants
 */

#ifndef _MEMCHECKDEFS_H_
#define _MEMCHECKDEFS_H_

#define PLUGIN_PREFIX(...) "[MemCheck] %s", wxString::Format(__VA_ARGS__)
#define BUSY_MESSAGE _("Please wait, working...")
#define SUPPRESSION_NAME_PLACEHOLDER "<insert_a_suppression_name_here>"
#define FILTER_NONWORKSPACE_PLACEHOLDER "<nonworkspace_errors>"
#define WAIT_UPDATE_PER_ITEMS 1000
#define ITEMS_FOR_WAIT_DIALOG 5000

#endif

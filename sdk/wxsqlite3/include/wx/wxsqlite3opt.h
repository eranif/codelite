///////////////////////////////////////////////////////////////////////////////
// Name:        wxsqlite3opt.h
// Purpose:     wxSQLite3 optional feature selection
// Author:      Ulrich Telle
// Modified by:
// Created:     2007-02-01
//                            
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file wxsqlite3opt.h Compile time switches for the wxSQLite3 class

#ifndef _WX_SQLITE3_OPT_H_
#define _WX_SQLITE3_OPT_H_

// Conditional compilation
// -----------------------

//! If this define is set to 1, then the SQLite library will be loaded dynamically
//! otherwise a link library is required to build wxSQLite3.
#ifndef wxUSE_DYNAMIC_SQLITE3_LOAD
#define wxUSE_DYNAMIC_SQLITE3_LOAD   0
#endif

//! To enable SQLite's meta data methods define WXSQLITE3_HAVE_METADATA as 1 here.
//! Attention: SQLite needs to be compiled with SQLITE_ENABLE_COLUMN_METADATA for this to work
#ifndef WXSQLITE3_HAVE_METADATA
#define WXSQLITE3_HAVE_METADATA 0
#endif

//! To enable SQLite's database encryption support define WXSQLITE3_HAVE_CODEC as 1 here.
//! Attention: SQLite needs to be compiled with SQLITE_HAS_CODEC for this to work
#ifndef WXSQLITE3_HAVE_CODEC
#define WXSQLITE3_HAVE_CODEC 0
#endif

//! To enable SQLite's loadable extension feature define WXSQLITE3_HAVE_LOAD_EXTENSION as 1 here.
//! Attention: SQLite needs to be compiled without SQLITE_OMIT_LOAD_EXTENSION for this to work
#ifndef WXSQLITE3_HAVE_LOAD_EXTENSION
#define WXSQLITE3_HAVE_LOAD_EXTENSION 0
#endif

#endif // _WX_SQLITE3_OPT_H_

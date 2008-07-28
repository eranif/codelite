///////////////////////////////////////////////////////////////////////////////
// Name:        wxsqlite3def.h
// Purpose:     wxWidgets wrapper around the SQLite3 embedded database library.
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-07-14
// Changes:     2005-10-03  - Upgrade to SQLite3 version 3.2.7
//              2005-10-09  - Corrected error in wxSQLite3Table::FindColumnIndex
//              2005-10-30  - Added wxGTK build support
//              2005-11-01  - Corrected wxSQLite3ResultSet::GetInt64.
//                            Added wxSQLite3Table::GetInt64
//              2005-11-09  - Optionally load SQLite library dynamically
//              2006-02-01  - Upgrade to SQLite3 version 3.3.3
//              2006-02-12  - Upgrade to SQLite3 version 3.3.4 (wxMSW only)
//              2006-03-15  - Fixed a bug in wxSQLite3Database::Prepare
//                            Added wxSQLite3Database::IsOpen for convenience
//              2006-06-11  - Upgrade to SQLite3 version 3.3.6
//                            Added support for optional SQLite meta data methods
//              2007-01-11  - Upgrade to SQLite3 version 3.3.10
//                            Added support for BLOBs as wxMemoryBuffer objects
//                            Added support for loadable extensions
//                            Optional support for key based database encryption
//              2007-02-12  - Upgrade to SQLite3 version 3.3.12
//              2007-05-01  - Upgrade to SQLite3 version 3.3.17
//              2007-10-28  - Upgrade to SQLite3 version 3.5.2
//              2007-11-17  - Fixed a bug in wxSQLite3Database::Close
//                            Eliminated several compile time warnings
//              2007-12-19  - Upgrade to SQLite3 version 3.5.4
//                            Fixed a bug in wxSQLite3Database::Begin
//              2008-01-05  - Added support for shared cache mode
//                            Added support for access to original SQL statement
//                            for prepared statements (requires SQLite 3.5.3 or above)
//              2008-04-27  - Upgrade to SQLite3 version 3.5.8
//                            Fixed several minor issues in the build files
//              2008-06-28  - Upgrade to SQLite3 version 3.5.9
//              2008-07-19  - Upgrade to SQLite3 version 3.6.0
//                            
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file wxsqlite3def.h Compile time switches for the wxSQLite3 class

/** \mainpage wxSQLite3

\section intro What is wxSQLite3?

  \b wxSQLite3 is a C++ wrapper around the public domain <a href="http://www.sqlite.org">SQLite 3.x</a> database
  and is specifically designed for use in programs based on the \b wxWidgets library.

  Several solutions already exist to access SQLite databases. To name just a few:

  - <a href="http://sourceforge.net/projects/wxsqlite">wxSQLite</a> :
    This is a wxWidgets wrapper for version 2.8.x of SQLite. 
    SQLite version 3.x has a lot more features - which are not supported by this wrapper.

  - <a href="http://www.codeproject.com/database/CppSQLite.asp">CppSQLite</a> : 
    Not wxWidgets specific, but with (partial) support for the newer version 3.x of SQLite.

  - <a href="http://wxcode.sf.net">DatabaseLayer</a> :
    This is a database abstraction providing a JDBC-like interface to database I/O.
    In the current version SQLite3, PostgreSQL, MySQL, Firebird, and ODBC database backends
    are supported.

  The component \b wxSQLite3 was inspired by all three mentioned SQLite wrappers.
  \b wxSQLite3 does not try to hide the underlying database, in contrary almost all special features
  of the SQLite3 version 3.x are supported, like for example the creation of user defined
  scalar or aggregate functions.

  Since SQLite stores strings in UTF-8 encoding, the wxSQLite3 methods provide automatic conversion
  between wxStrings and UTF-8 strings. This works best for the \b Unicode builds of \b wxWidgets.
  In \b ANSI builds the current locale conversion object \b wxConvCurrent is used for conversion
  to/from UTF-8. Special care has to be taken if external administration tools are used to modify
  the database contents, since not all of these tools operate in Unicode or UTF-8 mode.

\section version Version history

<dl>
<dt><b>1.9.0</b> - <i>July 2008</i></dt>
<dd>
Upgrade to SQLite version 3.6.0<br>
The optional key based encryption support has been adapted to
support SQLite version 3.6.0.<br>
Added static methods to initialize and shutdown the SQLite library.<br>
Changed build system to support static library build against shared
wxWidgets build on Linux.<br>
Changed behaviour of wxSQLite3Database::Close method to finalize
all unfinalized prepared statements.

</dd>
<dt><b>1.8.5</b> - <i>June 2008</i></dt>
<dd>
Upgrade to SQLite version 3.5.9<br>
Integration of the optional key based encryption support into SQLite
has been made easier. Changes to original SQLite source files
are no longer necessary.

</dd>
<dt><b>1.8.4</b> - <i>April 2008</i></dt>
<dd>
Upgrade to SQLite version 3.5.8<br>
Added support for accessing database limits<br>
Changed method TableExists to check a table name case insensitive<br>
Fixed several minor issues in the build files.

</dd>
<dt><b>1.8.3</b> - <i>January 2008</i></dt>
<dd>
Added support for shared cache mode<br>
Added support for access to original SQL statement
for prepared statements (requires SQLite 3.5.3 or above)

</dd>
<dt><b>1.8.2</b> - <i>December 2007</i></dt>
<dd>
Upgrade to SQLite version 3.5.4<br>
Fixed a bug in wxSQLite3Database::Begin (wrong transaction type)

</dd>
<dt><b>1.8.1</b> - <i>November 2007</i></dt>
<dd>
Fixed a bug in in wxSQLite3Database::Close (resetting flag m_isEncrypted)<br>
Eliminated several compile time warnings (regarding unused parameters)<br>
Fixed a compile time bug in wxSQLite3Database::GetBlob (missing explicit type cast)

</dd>
<dt><b>1.8.0</b> - <i>November 2007</i></dt>
<dd>
Upgrade to SQLite version 3.5.2<br>
Support for SQLite incremental BLOBs<br>
 Changed source code in the SQLite3 encryption extension to eliminate several warnings<br>
Changed default wxWidgets version to 2.8.x<br>
Adjusted sources for SQLite encryption support are included for all SQLite version from 3.3.1 up to 3.5.2<br>
SQLite link libraries for MinGW on Windows are included<br>
Added <code>WXMAKINGLIB_WXSQLITE3</code> compile time option
to support building wxSQLite3 as a static library while
using the shared libraries of wxWidgets.

</dd>
<dt><b>1.7.3</b> - <i>May 2007</i></dt>
<dd>
Upgrade to SQLite version 3.3.17<br>

Fixed a bug in the SQLite3 encryption extension
(MD5 algorithm was not aware of endianess on
big-endian platforms, resulting in non-portable
database files)

</dd>
<dt><b>1.7.2</b> - <i>February 2007</i></dt>
<dd>
Upgrade to SQLite version 3.3.12<br>
Support for loadable extensions is now optional
Check for optional wxSQLite3 features at runtime
wxSQLite3 API independent of optional features

</dd>
<dt><b>1.7.1</b> - <i>January 2007</i></dt>
<dd>
Fixed a bug in the key based database encryption feature
(The call to <b>sqlite3_rekey</b> in wxSQLite3Database::ReKey
could cause a program crash, when used to encrypt a previously
unencrypted database.)<br>

</dd>
<dt><b>1.7.0</b> - <i>January 2007</i></dt>
<dd>
Upgrade to SQLite version 3.3.10 (<b>Attention</b>: at least SQLite version 3.3.9 is required)<br>
Added support for BLOBs as wxMemoryBuffer objects<br>
Added support for loadable extensions<br>
Optional support for key based database encryption

</dd>
<dt><b>1.6.0</b> - <i>July 2006</i></dt>
<dd>
Added support for user defined collation sequences

</dd>
<dt><b>1.5.3</b> - <i>June 2006</i></dt>
<dd>
Upgrade to SQLite version 3.3.6<br>
Added support for optional SQLite meta data methods

</dd>
<dt><b>1.5.2</b> - <i>March 2006</i></dt>
<dd>
Fixed a bug in wxSQLite3Database::Prepare<br>
Added wxSQLite3Database::IsOpen for convenience

</dd>
<dt><b>1.5.1</b> - <i>February 2006</i></dt>
<dd>
Upgrade to SQLite version 3.3.4 (wxMSW only)

</dd>
<dt><b>1.5</b> - <i>February 2006</i></dt>
<dd>
Upgrade to SQLite version 3.3.3<br>
Added support for commit, rollback and update callbacks

</dd>
<dt><b>1.4.2</b> - <i>November 2005</i></dt>
<dd>
Optimized code for wxString arguments

</dd>
<dt><b>1.4.1</b> - <i>November 2005</i></dt>
<dd>
Fixed a bug in wxSQLite3Database::TableExists,<br>
Changed the handling of Unicode string conversion,<br>
Added support for different transaction types

</dd>
<dt><b>1.4</b> - <i>November 2005</i></dt>
<dd>
Optionally load the SQLite library dynamically at run time.

</dd>
<dt><b>1.3.1</b> - <i>November 2005</i></dt>
<dd>
Corrected wxSQLite3ResultSet::GetInt64.<br>
Added wxSQLite3Table::GetInt64

</dd>
<dt><b>1.3</b> - <i>October 2005</i></dt>
<dd>
Added wxGTK build support<br>

</dd>
<dt><b>1.2</b> - <i>October 2005</i></dt>
<dd>
Corrected error in wxSQLite3Table::FindColumnIndex<br>

</dd>
<dt><b>1.1</b> - <i>October 2005</i></dt>
<dd>
Upgrade to SQLite version 3.2.7 <br>

</dd>

<dt><b>1.0</b> - <i>July 2005</i></dt>
<dd>
First public release
</dd>
</dl>

\author Ulrich Telle (<a href="&#109;&#97;&#105;&#108;&#116;&#111;:&#117;&#108;&#114;&#105;&#99;&#104;&#46;&#116;&#101;&#108;&#108;&#101;&#64;&#103;&#109;&#120;&#46;&#100;&#101;">ulrich DOT telle AT gmx DOT de</a>)

 */

#ifndef _WX_SQLITE3_DEF_H_
#define _WX_SQLITE3_DEF_H_

#if defined(WXMAKINGLIB_WXSQLITE3)
  #define WXDLLIMPEXP_SQLITE3
#elif defined(WXMAKINGDLL_WXSQLITE3)
  #define WXDLLIMPEXP_SQLITE3 WXEXPORT
#elif defined(WXUSINGDLL_WXSQLITE3)
  #define WXDLLIMPEXP_SQLITE3 WXIMPORT
#else // not making nor using DLL
  #define WXDLLIMPEXP_SQLITE3
#endif

#endif // _WX_SQLITE3_DEF_H_

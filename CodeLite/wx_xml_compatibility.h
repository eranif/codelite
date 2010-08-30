//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : wx_xml_compatibility.h
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

#ifndef WX_XML_COMPATIBILTY_H
#define WX_XML_COMPATIBILTY_H

// wxXmlNode compatibilty macros
#if wxVERSION_NUMBER < 2900
#    define AddProperty    AddProperty
#    define GetPropVal     GetPropVal
#    define GetProperties  GetProperties
#    define HasProp        HasProp
#else
#    define AddProperty    AddAttribute
#    define GetPropVal     GetAttribute
#    define GetProperties  GetAttributes
#    define HasProp        HasAttribute
#endif

#endif // WX_XML_COMPATIBILTY_H


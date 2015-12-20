//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XMLLexerTokens.h
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

#ifndef XMLLexerTokens_H
#define XMLLexerTokens_H

enum {
    kXML_T_XML_OPEN_TAG = 400, // <?
    kXML_T_XML_CLOSE_TAG,      // ?>
    kXML_T_CLOSE_TAG_PREFIX,   // </
    kXML_T_CLOSE_TAG_SUFFIX,   // />
    kXML_T_IDENTIFIER,
    kXML_T_STRING,
    kXML_T_COMMENT_START,
    kXML_T_COMMENT_END,
    kXML_T_CDATA_START,
    kXML_T_CDATA_END,
    // XML entitites
    kXML_T_ENTITY_LT,
    kXML_T_ENTITY_GT,
    kXML_T_ENTITY_AMP,
    kXML_T_ENTITY_APOS,
    kXML_T_ENTITY_QUOATATION_MARK,
    kXML_T_HTML_DOCTYPE,
};

#endif // XMLLexerTokens_H

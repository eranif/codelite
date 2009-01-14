/////////////////////////////////////////////////////////////////////////////
// Name:        propdev.h
// Purpose:     wxPropertyGrid Internal Header
// Author:      Jaakko Salli
// Modified by:
// Created:     ?-?-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PROPGRID_PROPDEV_H_
#define _WX_PROPGRID_PROPDEV_H_

// -----------------------------------------------------------------------


class WXDLLIMPEXP_PG wxArrayEditorDialog;

// -----------------------------------------------------------------------

// These are intuitive substitutes for base property classes.
typedef wxPGProperty                wxBasePropertyClass;
typedef wxPGProperty                wxBaseParentPropertyClass;

// -----------------------------------------------------------------------

#ifndef SWIG

// This is required for sharing common global variables.
// TODO: Automatic locking mechanism?
class WXDLLIMPEXP_PG wxPGGlobalVarsClass
{
public:

    wxPGGlobalVarsClass();
    ~wxPGGlobalVarsClass();

    wxString            m_pDefaultImageWildcard; // Used by advprops, but here to make things easier.

    //wxArrayPtrVoid      m_arrEditorClasses; // List of editor class instances.
    wxPGHashMapS2P      m_mapEditorClasses; // Map of editor class instances (keys are name string).

#if wxUSE_VALIDATORS
    wxArrayPtrVoid      m_arrValidators; // These wxValidators need to be freed
#endif

    //wxPGHashMapS2P      m_dictValueType; // Valuename -> Value type object instance.

/*#if wxPG_VALUETYPE_IS_STRING
    wxPGHashMapS2P      m_dictValueTypeByClass; // VTClassname -> Value type object instance.
#endif*/

    wxPGHashMapS2P      m_dictPropertyClassInfo; // PropertyName -> ClassInfo

    wxPGChoices*        m_fontFamilyChoices;

    wxPGCellRenderer*   m_defaultRenderer;  // Replace with your own to affect all properties using
                                            // default renderer.

    wxPGVariantDataClassInfo wxVariantClassInfo_long;  // These are for fast variant type comparison
    wxPGVariantDataClassInfo wxVariantClassInfo_string;
    wxPGVariantDataClassInfo wxVariantClassInfo_double;
    wxPGVariantDataClassInfo wxVariantClassInfo_bool;
    wxPGVariantDataClassInfo wxVariantClassInfo_arrstring;
    wxPGVariantDataClassInfo wxVariantClassInfo_wxobject;
    wxPGVariantDataClassInfo wxVariantClassInfo_list;
#if wxCHECK_VERSION(2,8,0)
    wxPGVariantDataClassInfo wxVariantClassInfo_wxColour;
#endif
#if wxUSE_DATETIME
    wxPGVariantDataClassInfo wxVariantClassInfo_datetime;
#endif

    wxPGChoices         m_boolChoices;

#if wxVARIANT_REFCOUNTED
    wxVariant           m_vEmptyString;
    wxVariant           m_vZero;
    wxVariant           m_vMinusOne;
    wxVariant           m_vTrue;
    wxVariant           m_vFalse;
#endif

    // Cached constant strings
    wxPGCachedString    m_strMin;
    wxPGCachedString    m_strMax;
    wxPGCachedString    m_strUnits;
    wxPGCachedString    m_strInlineHelp;

    bool                m_autoGetTranslation; // If true then some things are automatically translated

    int                 m_offline; // > 0 if errors cannot or should not be shown in statusbar etc.

    int                 m_extraStyle;  // global extra style

#ifdef __WXDEBUG__
    int                 m_warnings;
#endif

    int HasExtraStyle( int style ) const { return (m_extraStyle & style); }

/*#if wxUSE_THREADS
    wxCriticalSection   m_critSect; // To allow different property grids to be addressed from different threads
#endif*/
};

#if wxCHECK_VERSION(2,9,0)

#define wxPGIsVariantType(VARIANT, T) \
    ((VARIANT).GetData() && typeid(*(VARIANT).GetData()) == *wxPGGlobalVars->wxVariantClassInfo_##T)

#define wxPGIsVariantClassInfo(CLASSINFO, T) \
    (*CLASSINFO == *wxPGGlobalVars->wxVariantClassInfo_##T)

#else

#define wxPGIsVariantType(VARIANT, T) \
    ((VARIANT).GetData() && wxPGVariantDataGetClassInfo((VARIANT).GetData()) == wxPGGlobalVars->wxVariantClassInfo_##T)

#define wxPGIsVariantClassInfo(CLASSINFO, T) \
    (CLASSINFO == wxPGGlobalVars->wxVariantClassInfo_##T)

#endif

#if wxVARIANT_REFCOUNTED
    #define wxPGVariant_EmptyString     (wxPGGlobalVars->m_vEmptyString)
    #define wxPGVariant_Zero            (wxPGGlobalVars->m_vZero)
    #define wxPGVariant_MinusOne        (wxPGGlobalVars->m_vMinusOne)
    #define wxPGVariant_True            (wxPGGlobalVars->m_vTrue)
    #define wxPGVariant_False           (wxPGGlobalVars->m_vFalse)
#else
    #define wxPGVariant_EmptyString     wxEmptyString
    #define wxPGVariant_Zero            ((long)0)
    #define wxPGVariant_MinusOne        ((long)-1)
    #define wxPGVariant_True            true
    #define wxPGVariant_False           false
#endif

#define wxPGVariant_Bool(A)     (A?wxPGVariant_True:wxPGVariant_False)


#if wxVARIANT_REFCOUNTED
    #define wxPGVariantAssign(A, B) A = B
#else
//
// Does A = B, so that the operation should not
// fail due to dataclass mismatch with typename
// match.
    #define wxPGVariantAssign(A, B) \
        { \
            A.MakeNull(); \
            A = B; \
        }

#endif


extern WXDLLIMPEXP_PG wxPGGlobalVarsClass* wxPGGlobalVars;

#endif

// -----------------------------------------------------------------------


#ifndef SWIG

//
// Tokenizer macros.
// NOTE: I have made two versions - worse ones (performance and consistency
//   wise) use wxStringTokenizer and better ones (may have unfound bugs)
//   use custom code.
//

#include <wx/tokenzr.h>

// TOKENIZER1 can be done with wxStringTokenizer
#define WX_PG_TOKENIZER1_BEGIN(WXSTRING,DELIMITER) \
    wxStringTokenizer tkz(WXSTRING,DELIMITER,wxTOKEN_RET_EMPTY); \
    while ( tkz.HasMoreTokens() ) \
    { \
        wxString token = tkz.GetNextToken(); \
        token.Trim(true); \
        token.Trim(false);

#define WX_PG_TOKENIZER1_END() \
    }


//
// 2nd version: tokens are surrounded by DELIMITERs (for example, C-style strings).
// TOKENIZER2 must use custom code (a class) for full compliancy
// with " surrounded strings with \" inside.
//
// class implementation is in propgrid.cpp
//

class WXDLLIMPEXP_PG wxPGStringTokenizer
{
public:
    wxPGStringTokenizer( const wxString& str, wxChar delimeter );
    ~wxPGStringTokenizer();

    bool HasMoreTokens(); // not const so we can do some stuff in it
    wxString GetNextToken();

protected:
#ifndef SWIG

    const wxString*             m_str;
    wxString::const_iterator    m_curPos;
    wxString                    m_readyToken;
    wxUniChar                   m_delimeter;
#endif
};

#define WX_PG_TOKENIZER2_BEGIN(WXSTRING,DELIMITER) \
    wxPGStringTokenizer tkz(WXSTRING,DELIMITER); \
    while ( tkz.HasMoreTokens() ) \
    { \
        wxString token = tkz.GetNextToken();

#define WX_PG_TOKENIZER2_END() \
    }

#endif

#ifndef __wxPG_SOURCE_FILE__
    // Reduce compile time, but still include in user app
    #include "props.h"
#endif

// -----------------------------------------------------------------------

#if !wxCHECK_VERSION(2,8,0)

// There is no wxString::EndsWith() in wxWidgets 2.6.
bool wxPG_String_EndsWith(const wxString& str, const wxChar *suffix, wxString *rest = NULL);

#else

inline bool wxPG_String_EndsWith(const wxString& str, const wxChar *suffix, wxString *rest = NULL)
{
    return str.EndsWith(suffix, rest);
}

#endif

// -----------------------------------------------------------------------

#endif // _WX_PROPGRID_PROPDEV_H_

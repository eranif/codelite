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


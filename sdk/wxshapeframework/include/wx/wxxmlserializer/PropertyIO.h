/***************************************************************
 * Name:      PropertyIO.cpp
 * Purpose:   Declares data types I/O and conversion functions
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-10-28
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _XSPROPERTYIO_H
#define _XSPROPERTYIO_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/hashmap.h>
#include <wx/xml/xml.h>
#include <wx/tokenzr.h>
#include <wx/list.h>

#include <wx/wxxmlserializer/Defs.h>

class WXDLLIMPEXP_XS xsProperty;
class WXDLLIMPEXP_XS xsSerializable;
class WXDLLIMPEXP_XS SerializableList;

namespace wxXS
{
	WX_DECLARE_OBJARRAY_WITH_DECL(wxRealPoint, RealPointArray, class WXDLLIMPEXP_XS);
	WX_DECLARE_LIST_WITH_DECL(wxRealPoint, RealPointList, class WXDLLIMPEXP_XS);

	WX_DEFINE_USER_EXPORTED_ARRAY_CHAR(char, CharArray, class WXDLLIMPEXP_XS);
	WX_DEFINE_USER_EXPORTED_ARRAY_INT(int, IntArray, class WXDLLIMPEXP_XS);
	WX_DEFINE_USER_EXPORTED_ARRAY_LONG(long, LongArray, class WXDLLIMPEXP_XS);
	WX_DEFINE_USER_EXPORTED_ARRAY_DOUBLE(double, DoubleArray, class WXDLLIMPEXP_XS);

	WX_DECLARE_STRING_HASH_MAP_WITH_DECL(wxString, StringMap, class WXDLLIMPEXP_XS);
}

/*!
 * \brief Base class encapsulating a property I/O handler. The class is used by
 * the xsSerializable class and is responsiblefor reading and writing of an XML node
 * containing property information. Each supported property (data) type should have
 * its own I/O handler class. Moreover, all derived classes must provide public functions
 * 'static wxString classname::ToString(datatype value)' and 'static datatype classname::
 * FromString( const wxString& value )' responsible for conversion between datatype and
 * and its string representation (these functions are used internally by class virtual functions.
 */
class WXDLLIMPEXP_XS xsPropertyIO : public wxObject
{
public:
    DECLARE_DYNAMIC_CLASS(xsProperty);

    /*! \brief Constructor. */
    xsPropertyIO(){;}
    /*! \brief Destructor. */
    virtual ~xsPropertyIO(){;}

    /*!
     * \brief Read content of the property XML node and store it to given property object.
     * \param property Pointer to the target property object
     * \param source Pointer to the source XML node
     */
    virtual void Read(xsProperty *property, wxXmlNode *source){wxUnusedVar(property);wxUnusedVar(source);}
    /*!
     * \brief Write content of given property object to target XML node.
     * \param property Pointer to the source property object
     * \param target Pointer to the target XML node
     */
    virtual void Write(xsProperty *property, wxXmlNode *target){wxUnusedVar(property);wxUnusedVar(target);}
    /*!
     * \brief Get textual representation of current property value.
	 * \param property Pointer to the source property object
	 * \return Textual representation of property's value
     */
	virtual wxString GetValueStr(xsProperty *property){wxUnusedVar(property);return wxT("");}
    /*!
     * \brief Set value defined by its textual representation to given property.
	 * \param property Pointer to the target property object
	 * \param valstr Textual representation of given value
     */
	virtual void SetValueStr(xsProperty *property, const wxString& valstr){wxUnusedVar(property); wxUnusedVar(valstr);}

    /*!
     * \brief Create new XML node of given name and value and assign it to the given
     * parent XML node.
     * \param parent Pointer to parent XML node
     * \param name Name of new XML node
     * \param value Content of new XML node
     * \param type Type of new XML (content) node
     */
     static wxXmlNode* AddPropertyNode(wxXmlNode* parent, const wxString& name, const wxString& value, wxXmlNodeType type = wxXML_TEXT_NODE );

protected:

    /*!
     * \brief Append info about the source property to given XML node.
     * \param source Pointer to the source property
     * \param target Pointer to modified XML node
     */
    void AppendPropertyType(xsProperty *source, wxXmlNode *target);
};

/*!
 * \brief Macro suitable for declaration of new property I/O handler
 * \param datatype Property's data type
 * \param name Handler class name
 */
#define XS_DECLARE_IO_HANDLER(datatype, name) \
class name : public xsPropertyIO \
{ \
public: \
	DECLARE_DYNAMIC_CLASS(name); \
	name(){;} \
	virtual ~name(){;} \
\
	virtual void Read(xsProperty *property, wxXmlNode *source); \
	virtual void Write(xsProperty *property, wxXmlNode *target); \
	virtual wxString GetValueStr(xsProperty *property); \
	virtual void SetValueStr(xsProperty *property, const wxString& valstr); \
	static wxString ToString(const datatype& value); \
	static datatype FromString(const wxString& value); \
}; \

/*!
 * \brief Macro suitable for declaration of exported new property I/O handler
 * \param datatype Property's data type
 * \param name Handler class name
 * \param decoration Class decoration
 */
#define XS_DECLARE_EXPORTED_IO_HANDLER(datatype, name, decoration) \
class decoration name : public xsPropertyIO \
{ \
public: \
	DECLARE_DYNAMIC_CLASS(name); \
	name(){;} \
	virtual ~name(){;} \
\
	virtual void Read(xsProperty *property, wxXmlNode *source); \
	virtual void Write(xsProperty *property, wxXmlNode *target); \
	virtual wxString GetValueStr(xsProperty *property); \
	virtual void SetValueStr(xsProperty *property, const wxString& valstr); \
	static wxString ToString(const datatype& value); \
	static datatype FromString(const wxString& value); \
}; \

/*!
 * \brief Macro suitable for implementation of new property I/O handler
 * \param datatype Property's data type
 * \param name Handler class name
 */
#define XS_DEFINE_IO_HANDLER(datatype, name) \
IMPLEMENT_DYNAMIC_CLASS(name, xsPropertyIO); \
\
void name::Read(xsProperty *property, wxXmlNode *source) \
{ \
    *((datatype*)property->m_pSourceVariable) = FromString(source->GetNodeContent()); \
} \
\
void name::Write(xsProperty *property, wxXmlNode *target) \
{ \
    wxString val = ToString(*((datatype*)property->m_pSourceVariable)); \
\
    if(val != property->m_sDefaultValueStr) \
    { \
        wxXmlNode *newNode = AddPropertyNode(target, wxT("property"), val); \
        AppendPropertyType(property, newNode); \
    } \
} \
\
wxString name::GetValueStr(xsProperty *property) \
{ \
	return ToString(*((datatype*)property->m_pSourceVariable)); \
} \
\
void name::SetValueStr(xsProperty *property, const wxString& valstr) \
{ \
	*((datatype*)property->m_pSourceVariable) = FromString(valstr); \
} \

/*!
 * \brief Property class encapsulating I/O functions used by 'wxString' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxString, xsStringPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxChar' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxChar, xsCharPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'long' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(long, xsLongPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'int' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(int, xsIntPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'bool' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(bool, xsBoolPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'double' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(double, xsDoublePropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'float' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(float, xsFloatPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxPoint' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxPoint, xsPointPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxSize' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxSize, xsSizePropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxRealPoint' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxRealPoint, xsRealPointPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxColour' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxColour, xsColourPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxPen' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxPen, xsPenPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxBrush' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxBrush, xsBrushPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxFont' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxFont, xsFontPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'wxArrayString' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxArrayString, xsArrayStringPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'CharArray' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxXS::CharArray, xsArrayCharPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'IntArray' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxXS::IntArray, xsArrayIntPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'LongArray' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxXS::LongArray, xsArrayLongPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'DoubleArray' properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxXS::DoubleArray, xsArrayDoublePropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'RealPointArray' (array of
 * integer values) properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxXS::RealPointArray, xsArrayRealPointPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'ListRealPoint' (list of
 * wxRealPoint objects) properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxXS::RealPointList, xsListRealPointPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'SerializableList' (list of
 * xsSerializable objects) properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(SerializableList, xsListSerializablePropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'serializabledynamic' (xsSerializable
 * dynamic class objects which are created during the deserialization process) properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(xsSerializable, xsDynObjPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'serializabledynamicnocreate' (already
 * existing xsSerializable dynamic class objects) properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(xsSerializable, xsDynNCObjPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'serializablestatic' (static
 * xsSerializable class objects) properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(xsSerializable, xsStaticObjPropIO, WXDLLIMPEXP_XS);

/*!
 * \brief Property class encapsulating I/O functions used by 'mapstring' (string hash map) properties.
 */
XS_DECLARE_EXPORTED_IO_HANDLER(wxXS::StringMap, xsMapStringPropIO, WXDLLIMPEXP_XS);

WX_DECLARE_HASH_MAP( wxString, xsPropertyIO*, wxStringHash, wxStringEqual, PropertyIOMap );

#endif //_XSPROPERTYIO_H

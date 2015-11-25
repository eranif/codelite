/***************************************************************
 * Name:      XmlSerializer.h
 * Purpose:   Defines XML serializer and related classes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-08-28
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _XSXMLSERIALIZE_H
#define _XSXMLSERIALIZE_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wxxmlserializer/PropertyIO.h>

#include <wx/xml/xml.h>
#include <wx/hashmap.h>

#define xsWITH_ROOT true
#define xsWITHOUT_ROOT false

#define xsRECURSIVE true
#define xsNORECURSIVE false

/*! \brief Macro creates new serialized STRING property */
#define XS_SERIALIZE_STRING(x, name) XS_SERIALIZE_PROPERTY(x, wxT("string"), name);
/*! \brief Macro creates new serialized STRING property with defined default value */
#define XS_SERIALIZE_STRING_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("string"), name, def);
/*! \brief Macro creates new serialized STRING property */
#define XS_SERIALIZE_CHAR(x, name) XS_SERIALIZE_PROPERTY(x, wxT("char"), name);
/*! \brief Macro creates new serialized STRING property with defined default value */
#define XS_SERIALIZE_CHAR_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("char"), name, def);
/*! \brief Macro creates new serialized LONG property */
#define XS_SERIALIZE_LONG(x, name) XS_SERIALIZE_PROPERTY(x, wxT("long"), name);
/*! \brief Macro creates new serialized LONG property with defined default value */
#define XS_SERIALIZE_LONG_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("long"), name, xsLongPropIO::ToString(def));
/*! \brief Macro creates new serialized DOUBLE property */
#define XS_SERIALIZE_DOUBLE(x, name) XS_SERIALIZE_PROPERTY(x, wxT("double"), name);
/*! \brief Macro creates new serialized DOUBLE property with defined default value */
#define XS_SERIALIZE_DOUBLE_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("double"), name, xsDoublePropIO::ToString(def));
/*! \brief Macro creates new serialized INT property */
#define XS_SERIALIZE_INT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("int"), name);
/*! \brief Macro creates new serialized INT property with defined default value */
#define XS_SERIALIZE_INT_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("int"), name, xsIntPropIO::ToString(def));
/*! \brief Macro creates new serialized FLOAT property */
#define XS_SERIALIZE_FLOAT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("float"), name);
/*! \brief Macro creates new serialized FLOAT property with defined default value */
#define XS_SERIALIZE_FLOAT_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("float"), name, xsFloatPropIO::ToString(def));

/*! \brief Macro creates new serialized BOOL property */
#define XS_SERIALIZE_BOOL(x, name) XS_SERIALIZE_PROPERTY(x, wxT("bool"), name);
/*! \brief Macro creates new serialized BOOL property with defined default value */
#define XS_SERIALIZE_BOOL_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("bool"), name, xsBoolPropIO::ToString(def));

/*! \brief Macro creates new serialized wxPoint property */
#define XS_SERIALIZE_POINT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("point"), name);
/*! \brief Macro creates new serialized wxPoint property with defined default value */
#define XS_SERIALIZE_POINT_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("point"), name, xsPointPropIO::ToString(def));
/*! \brief Macro creates new serialized wxRealPoint property */
#define XS_SERIALIZE_REALPOINT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("realpoint"), name);
/*! \brief Macro creates new serialized wxRealPoint property with defined default value */
#define XS_SERIALIZE_REALPOINT_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("realpoint"), name, xsRealPointPropIO::ToString(def));
/*! \brief Macro creates new serialized wxSize property */
#define XS_SERIALIZE_SIZE(x, name) XS_SERIALIZE_PROPERTY(x, wxT("size"), name);
/*! \brief Macro creates new serialized wxSize property with defined default value */
#define XS_SERIALIZE_SIZE_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("size"), name, xsSizePropIO::ToString(def));

/*! \brief Macro creates new serialized wxColour property */
#define XS_SERIALIZE_COLOUR(x, name) XS_SERIALIZE_PROPERTY(x, wxT("colour"), name);
/*! \brief Macro creates new serialized wxColour property with defined default value */
#define XS_SERIALIZE_COLOUR_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("colour"), name, xsColourPropIO::ToString(def));
/*! \brief Macro creates new serialized wxPen property */
#define XS_SERIALIZE_PEN(x, name) XS_SERIALIZE_PROPERTY(x, wxT("pen"), name);
/*! \brief Macro creates new serialized wxPen property with defined default value */
#define XS_SERIALIZE_PEN_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("pen"), name, xsPenPropIO::ToString(def));
/*! \brief Macro creates new serialized wxBrush property */
#define XS_SERIALIZE_BRUSH(x, name) XS_SERIALIZE_PROPERTY(x, wxT("brush"), name);
/*! \brief Macro creates new serialized wxBrush property with defined default value */
#define XS_SERIALIZE_BRUSH_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("brush"), name, xsBrushPropIO::ToString(def));
/*! \brief Macro creates new serialized wxFont property */
#define XS_SERIALIZE_FONT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("font"), name);
/*! \brief Macro creates new serialized wxFont property with defined default value */
#define XS_SERIALIZE_FONT_EX(x, name, def) XS_SERIALIZE_PROPERTY_EX(x, wxT("font"), name, xsFontPropIO::ToString(def));

/*! \brief Macro creates new serialized property (type 'array of strings (wxArrayString)') */
#define XS_SERIALIZE_ARRAYSTRING(x, name) XS_SERIALIZE_PROPERTY(x, wxT("arraystring"), name);
/*! \brief Macro creates new serialized property (type 'array of chars (CharArray)') */
#define XS_SERIALIZE_ARRAYCHAR(x, name) XS_SERIALIZE_PROPERTY(x, wxT("arraychar"), name);
/*! \brief Macro creates new serialized property (type 'array of ints (IntArray)') */
#define XS_SERIALIZE_ARRAYINT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("arrayint"), name);
/*! \brief Macro creates new serialized property (type 'array of longs (LongArray)') */
#define XS_SERIALIZE_ARRAYLONG(x, name) XS_SERIALIZE_PROPERTY(x, wxT("arraylong"), name);
/*! \brief Macro creates new serialized property (type 'array of doubles (DoubleArray)') */
#define XS_SERIALIZE_ARRAYDOUBLE(x, name) XS_SERIALIZE_PROPERTY(x, wxT("arraydouble"), name);
/*! \brief Macro creates new serialized property (type 'array of wxRealPoint objects') */
#define XS_SERIALIZE_ARRAYREALPOINT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("arrayrealpoint"), name);
/*! \brief Macro creates new serialized property (type 'list of wxRealPoint objects') */
#define XS_SERIALIZE_LISTREALPOINT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("listrealpoint"), name);
/*! \brief Macro creates new serialized property (type 'list of xsSerializable objects') */
#define XS_SERIALIZE_LISTSERIALIZABLE(x, name) XS_SERIALIZE_PROPERTY(x, wxT("listserializable"), name);

/*! \brief Macro creates new serialized property (type 'string hash map (StringMap)') */
#define XS_SERIALIZE_MAPSTRING(x, name) XS_SERIALIZE_PROPERTY(x, wxT("mapstring"), name);

/*! \brief Macro creates new serialized property encapsulating a dynamic serializable object */
#define XS_SERIALIZE_DYNAMIC_OBJECT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("serializabledynamic"), name);
/*! \brief Macro creates new serialized property encapsulating a dynamic serializable object */
#define XS_SERIALIZE_DYNAMIC_OBJECT_NO_CREATE(x, name) XS_SERIALIZE_PROPERTY(x, wxT("serializabledynamicnocreate"), name);
/*! \brief Macro creates new serialized property encapsulating a static serializable object */
#define XS_SERIALIZE_STATIC_OBJECT(x, name) XS_SERIALIZE_PROPERTY(x, wxT("serializablestatic"), name);

/*! \brief Macro creates new serialized property of given type */
#define XS_SERIALIZE_PROPERTY(x, type, name) AddProperty(new xsProperty(&x, type, name));
/*! \brief Macro creates new serialized property of given type with defined dafult value */
#define XS_SERIALIZE_PROPERTY_EX(x, type, name, def) AddProperty(new xsProperty(&x, type, name, def));

/*! \brief Macro creates new serialized property and automaticaly determines its type (if supported) */
#define XS_SERIALIZE(x, name) AddProperty(new xsProperty(&x, name));
/*! \brief Macro creates new serialized property with defined dafult value and automaticaly determines its type (if supported)*/
#define XS_SERIALIZE_EX(x, name, def) AddProperty(new xsProperty(&x, name, def));

/*! \brief Macro registers new IO handler for specified data type (handler class must exist) */
#define XS_REGISTER_IO_HANDLER(type, class) wxXmlSerializer::m_mapPropertyIOHandlers[type] = new class();


/*! \brief Enable RTTI (the same as DECLARE_DYNAMIC_CLASS) and declare xsSerializable::Clone() function */
#define XS_DECLARE_CLONABLE_CLASS(name) \
public: \
	DECLARE_DYNAMIC_CLASS(name) \
	virtual wxObject* Clone(); \

/*! \brief Enable RTTI (the same as IMPLEMENT_DYNAMIC_CLASS) and implement xsSerializable::Clone() function */
#define XS_IMPLEMENT_CLONABLE_CLASS(name, base) \
	IMPLEMENT_DYNAMIC_CLASS(name, base) \
	wxObject* name::Clone() \
	{ \
		if( m_fClone ) return new name(*this); \
		else \
			return NULL; \
	} \
	
/*! \brief Enable RTTI (the same as IMPLEMENT_DYNAMIC_CLASS2) and implement xsSerializable::Clone() function */
#define XS_IMPLEMENT_CLONABLE_CLASS2(name, base1, base2) \
	IMPLEMENT_DYNAMIC_CLASS2(name, base1, base2) \
	wxObject* name::Clone() \
	{ \
		if( m_fClone ) return new name(*this); \
		else \
			return NULL; \
	} \


class WXDLLIMPEXP_XS xsProperty;
class WXDLLIMPEXP_XS xsSerializable;
class WXDLLIMPEXP_XS wxXmlSerializer;

WX_DECLARE_LIST_WITH_DECL(xsProperty, PropertyList, class WXDLLIMPEXP_XS);
WX_DECLARE_LIST_WITH_DECL(xsSerializable, SerializableList, class WXDLLIMPEXP_XS);

WX_DECLARE_HASH_MAP( long, xsSerializable*, wxIntegerHash, wxIntegerEqual, IDMap );

/*!
 * \brief Base class encapsulating object which can be serialized/deserialized to/from
 * XML file (disk file or any stream). This class acts as a data container for properties
 * (xsProperty class objects) encapsulating serialized class data members.
 *
 * Class data members which should be serialized must be marked by appropriate macro defined
 * in wxXmlSerializer.h header file (it is recommended to mark desired data members in the class constructor).
 *
 * Instances of this class can be arranged into a list/d-ary tree hierarchy so it can behave like
 * powerfull data container. All chained serializable class objects can be handled by class
 * member functions or by member functions of wxXmlSerializer class object which should be
 * used as their manager (recommended way).
 *
 * Another built-in (optional) functionality is class instaces' cloning. User can use
 * XS_DECLARE_CLONABLE_CLASS and XS_IMPLEMENT_CLONABLE_CLASS macros instead of classic
 * DECLARE_DYNAMIC_CLASS and IMPLEMENT_DYNAMIC_CLASS macros which lead to definition of
 * xsSerializable::Clone() virtual function used for cloning of current class instance
 * via its copy constructor (user must define it manually). Virtual xsSerializble::Clone()
 * function is also used by the wxXmlSerializer::CopyItems() function (used by the
 * wxXmlSerializer copy constructor).
 */
class WXDLLIMPEXP_XS xsSerializable : public wxObject
{
public:
    friend class wxXmlSerializer;

    XS_DECLARE_CLONABLE_CLASS(xsSerializable);

	enum SEARCHMODE
	{
		/*! \brief Depth-First-Search algorithm */
		searchDFS,
		/*! \brief Breadth-First-Search algorithm */
		searchBFS
	};

    /*! \brief Constructor. */
    xsSerializable();
    /*! \brief Copy constructor. */
    xsSerializable(const xsSerializable& obj);
    /*! \brief Destructor. */
    ~xsSerializable();

    // public functions

    /*!
     * \brief Get serializable parent object.
     * \return Pointer to serializable parent object if exists, otherwise NULL
     */
    inline xsSerializable* GetParent() { return m_pParentItem; }
    /*!
     * \brief Get parent data manager (instance of wxXmlSerializer).
     * \return Pointer to parent data manager if set, otherwise NULL
     */
    inline wxXmlSerializer* GetParentManager() { return m_pParentManager; }
    /*!
     * \brief Get first serializable child object.
     * \return Pointer to child object if exists, otherwise NULL
     */
    xsSerializable* GetFirstChild();
    /*!
     * \brief Get first serializable child object of given type.
	 * \param type Child object type (can be NULL for any type)
     * \return Pointer to child object if exists, otherwise NULL
     */
    xsSerializable* GetFirstChild(wxClassInfo *type);
    /*!
     * \brief Get last serializable child object.
     * \return Pointer to child object if exists, otherwise NULL
     */
    xsSerializable* GetLastChild();
    /*!
     * \brief Get last serializable child object of given type.
	 * \param type Child object type (can be NULL for any type)
     * \return Pointer to child object if exists, otherwise NULL
     */
    xsSerializable* GetLastChild(wxClassInfo *type);
    /*!
     * \brief Get next serializable sibbling object.
     * \return Pointer to sibbling object if exists, otherwise NULL
     */
    xsSerializable* GetSibbling();
    /*!
     * \brief Get next serializable sibbling object of given type.
	 * \param type Child object type (can be NULL for any type)
     * \return Pointer to sibbling object if exists, otherwise NULL
     */
    xsSerializable* GetSibbling(wxClassInfo *type);
    /*!
     * \brief Get child item with given ID if exists.
     * \param id ID of searched child item
     * \param recursive If TRUE then the child shape will be searched recursivelly
     * \return Pointer to first child with given ID if pressent, otherwise NULL
     */
	xsSerializable* GetChild(long id, bool recursive = xsNORECURSIVE);

    /*!
     * \brief Function finds out whether this serializable item has some children.
     * \return TRUE if the parent shape has children, otherwise FALSE
     */
	inline bool HasChildren() const { return !m_lstChildItems.IsEmpty(); }
    /*!
     * \brief Get list of children (serializable objects) of this object.
     * \return Reference to a list with child serializable objects (can be empty)
     */
    inline SerializableList& GetChildrenList() { return m_lstChildItems; }
    /*!
     * \brief Get children of given type.
     * \param type Child object type (if NULL then all children are returned)
     * \param list Reference to a list where all found child objects will be appended
     */
    void GetChildren(wxClassInfo *type, SerializableList& list);
    /*!
     * \brief Get all children of given type recursively (i.e. children of children of .... ).
     * \param type Get only children of given type (if NULL then all children are returned)
     * \param list Reference to a list where all found child objects will be appended
	 * \param mode Search mode. User can choose Depth-First-Search or Breadth-First-Search algorithm (BFS is default)
	 * \sa SEARCHMODE
     */
    void GetChildrenRecursively(wxClassInfo *type, SerializableList& list, SEARCHMODE mode = searchBFS);
    /*!
     * \brief Get pointer to list node containing first serializable child object.
     */
	inline SerializableList::compatibility_iterator GetFirstChildNode() const { return m_lstChildItems.GetFirst(); }
    /*!
     * \brief Get pointer to list node containing last serializable child object.
     */
    inline SerializableList::compatibility_iterator GetLastChildNode() const { return m_lstChildItems.GetLast(); }

    /*!
     * \brief Set serializable parent object.
     * \param parent Pointer to parent object
     */
    inline void SetParent(xsSerializable* parent) { m_pParentItem = parent; }
    /*!
     * \brief Set parent data manager.
     * \param parent Pointer to parent data manager
     */
    inline void SetParentManager(wxXmlSerializer* parent) { m_pParentManager = parent; }
    /*!
     * \brief Add serializable child object to this object.
     * \param child Pointer to added child object (must NOT be NULL)
     * \return Pointer to to the added child object
     */
    xsSerializable* AddChild(xsSerializable* child);
    /*!
     * \brief Insert serializable child object to this object at given position.
	 * \param pos Zero-based position
     * \param child Pointer to added child object (must NOT be NULL)
     * \return Pointer to to the added child object
     */
    xsSerializable* InsertChild(size_t pos, xsSerializable* child);
	/*!
	 * \brief Remove serializable child item from this object (the child item will be destroyed).
	 * \param child Pointer to child item which should be removed
	 */
	void RemoveChild(xsSerializable *child);
	/*!
	 * \brief Remove all child items (all items will be destroyed).
	 */
	void RemoveChildren();
	
    /*!
     * \brief Assign this object as a child to given parent object.
     * \param parent Pointer to new parent object (must NOT be NULL)
     */
    void Reparent(xsSerializable* parent);

    /*!
     * \brief Set ID of this object. Can be used for further objects' handling by
     * wxXmlSerializer class (default ID value is -1). This functions should NOT
     * be used directly; it is called by wxXmlSerializer object in the case that this
     * serializable object is attached to another one (or directly to root node of wxXmlSerializer) by
     * wxXmlSerializer::AddItem() member function.
     */
    void SetId(long id);
    /*!
     * \brief Get object ID.
     * \return ID value or -1 if the ID hasn't been set yet
     */
    inline long GetId() const { return m_nId; }

    /*!
     * \brief Create new 'object' XML node and serialize all marked class data members (properties) into it.
     * \param node Pointer to parent XML node
     * \return Pointer to modified parent XML node
     */
    wxXmlNode* SerializeObject(wxXmlNode* node);
    /*!
     * \brief Deserialize marked class data members (properties) from appropriate fields of given
     * parent 'object' XML node.
     * \param node Pointer to parent 'object' XML node
     */
    void DeserializeObject(wxXmlNode* node);

    /*!
     * \brief Add new property to the property list.
     * \param property Pointer to added property object
     * \sa xsProperty
     */
    void AddProperty(xsProperty* property);
	/**
	 * \brief Remove given property from the property list.
	 * \param property Pointer to existing property.
	 * \sa xsProperty, GetProperty()
	 */
	void RemoveProperty(xsProperty *property);
    /*!
     * \brief Get serialized property of given name.
     * \return Pointer to the property object if exists, otherwise NULL
     * \sa xsProperty
     */
    xsProperty* GetProperty(const wxString& field);
    /*!
     * \brief Get reference to properties list.
     * \sa xsProperty
     */
	inline PropertyList& GetProperties() { return m_lstProperties; }

    /*!
     * \brief Enable/disable serialization of given property.
     * \param field Property name
     * \param enab TRUE if the property should be serialized, otherwise FALSE
     */
    void EnablePropertySerialization(const wxString& field, bool enab);
    /*!
     * \brief Returns information whether the given property is serialized or not.
     * \param field Name of examined property
     */
    bool IsPropertySerialized(const wxString& field);
    /*!
     * \brief Enable/disable object serialization.
     * \param enab TRUE if the object should be serialized, otherwise FALSE
     */
    inline void EnableSerialization(bool enab) { m_fSerialize = enab; }
    /*!
     * \brief Returns information whether the object can be serialized or not.
     */
    inline bool IsSerialized() const { return m_fSerialize; }
    /*!
     * \brief Enable/disable object cloning.
     * \param enab TRUE if the object can be cloned, otherwise FALSE
     */
	inline void EnableCloning(bool enab) { m_fClone = enab; }
    /*!
     * \brief Returns information whether the object can be cloned or not.
     */
	inline bool IsCloningEnabled() const { return m_fClone; }
	
	// overloaded operators
    /*!
     * \brief Add serializable child object to this object.
     * \param child Pointer to added child object (should NOT be NULL)
	 * \return Pointer to added object
     */
	 xsSerializable* operator<<(xsSerializable *child);
	
protected:
    // protected data members
    /*! \brief List of serialized properties */
    PropertyList m_lstProperties;
    /*! \brief List of child objects */
    SerializableList m_lstChildItems;

    /*! \brief Pointer to parent serializable object */
    xsSerializable *m_pParentItem;
	/*! \brief Pointer to parent data manager */
	wxXmlSerializer *m_pParentManager;

    /*! \brief Object serialization flag */
    bool m_fSerialize;
	/*! \brief Object cloning flag */
	bool m_fClone;

	/**
	 * \brief Initialize new child object.
	 * \param child Pointer to new child object
	 */
	void InitChild(xsSerializable *child);
	
    // protected virtual functions
    /*!
     * \brief Serialize stored properties to the given XML node. The serialization
     * routine is automatically called by the framework and cares about serialization
     * of all defined properties.
     *
     * Note that default implementation automatically serializes all class data members
     * marked by appropriate macros. If some non-standard class member should be serialized as well,
     * the source code of derived function implementation can be as in following example.
     *
     * \param node Pointer to XML node where the property nodes will be appended to
     *
     * Example code:
     * \code
     * wxXmlNode* DerivedFrom_xsSerializable::Serialize(wxXmlNode* node)
     * {
     *     if(node)
     *     {
     *         // call base class's serialization routine
     *         node = xsSeralizable::Serialize(node);
     *
     *         // serialize custom property
     *         xsPropertyIO::AddPropertyNode(node, wxT("some_property_field_name"), wxT("string_repr_of_its_value"));
     *     }
     *     // return updated node
     *     return node;
     * }
     * \endcode
     */
    virtual wxXmlNode* Serialize(wxXmlNode* node);
    /*!
     * \brief Deserialize object properties from the given XML node. The
     * routine is automatically called by the framework and cares about deserialization
     * of all defined properties.
     *
     * Note that default implementation automatically deserializes all class data members
     * marked by appropriate macros. If some non-standard class member should be deserialized as well,
     * the source code of derived function implementation can be as in following example.
     *
     * \param node Pointer to a source XML node containig the property nodes
     *
     * Example code:
     * \code
     * void DerivedFrom_xsSerializable::Deserialize(wxXmlNode* node)
     * {
     *      // call base class's deserialization rountine (if necessary...)
     *      xsSerializable::Deserialize(node);
     *
     *      // iterate through all custom property nodes
     *      wxXmlNode *propNode = node->GetChildren();
     *      while(propNode)
     *      {
     *          if(propNode->GetName() == wxT("some_property_field_name"))
     *          {
     *              // read the node content and convert it to a proper data type
     *          }
     *          propNode = propNode->GetNext();
     *      }
     * }
     * \endcode
     */
    virtual void Deserialize(wxXmlNode* node);
	
private:
    /*! \brief Object ID */
    long m_nId;
};

/*!
 * \brief Class encapsulates a serializable objects' manager which is responsible
 * for handling stored serializable objects and their serialization/deserialization
 * from/to XML files or streams.
 *
 * Stored objects can be arranged into a list or d-ary tree structure so this class
 * can be used as a container for various application data. Also created XML files
 * (root node) can be marked with given version and owner name, so it is possible to
 * control a version of saved document.
 *
 * wxXmlSerializer class contains one instance of xsSerializable object created in the
 * class constructor (can be set later via member functions as well). This serializable
 * object called 'root object' holds all other inserted serializable objects (in case of
 * tree structure it is a topmost tree node, in case of list structure all list items are
 * its children). These child object can be handled via xsSerializable and wxXmlSerializer
 * classes' member functions.
 *
 * Another built-in (optional) functionality is class instaces' cloning. User can use
 * XS_DECLARE_CLONABLE_CLASS and XS_IMPLEMENT_CLONABLE_CLASS macros instead of classic
 * DECLARE_DYNAMIC_CLASS and IMPLEMENT_DYNAMIC_CLASS macros which lead to definition of
 * wxXmlSerializer::Clone() virtual function used for cloning of current class instance
 * via its copy constructor (user must define it manually).
 */
class WXDLLIMPEXP_XS wxXmlSerializer : public wxObject
{
public:
    XS_DECLARE_CLONABLE_CLASS(wxXmlSerializer);

    /*! \brief Constructor. */
    wxXmlSerializer();
    /*!
     * \brief User constructor.
     * \param owner Owner name
     * \param root Name of root node
     * \param version File version
     */
    wxXmlSerializer(const wxString& owner, const wxString& root, const wxString& version);
    /*! \brief Copy constructor. */
    wxXmlSerializer(const wxXmlSerializer &obj);
    /*! \brief Destructor. */
    virtual ~wxXmlSerializer();

    // public member data accessors
    /*!
     * \brief Set owner name.
     * \param name Owner name
     */
    inline void SetSerializerOwner(const wxString& name) { m_sOwner = name; }
    /*!
     * \brief Set root name.
     * \param name Root name
     */
    inline void SetSerializerRootName(const wxString& name) { m_sRootName = name; }
    /*!
     * \brief Set file version.
     * \param name File version
     */
    inline void SetSerializerVersion(const wxString& name) { m_sVersion = name; }
    /*! \brief Get owner name. */
    inline const wxString& GetSerializerOwner() const { return m_sOwner; }
    /*! \brief Get name of root node. */
    inline const wxString& GetSerializerRootName() const { return m_sRootName; }
    /*! \brief Get file version. */
    inline const wxString& GetSerializerVersion() const { return m_sVersion; }
	/*! \brief Get the library version. */
	inline const wxString& GetLibraryVersion() const { return m_sLibraryVersion; }

    // public functions
	/**
	 * \brief Get last occured error state/message.
	 * \return Error message
	 */
	const wxString& GetErrMessage() const { return m_sErr; }
	
    /*! \brief Get pointer to root serializable object. */
    inline xsSerializable* GetRootItem() const { return m_pRoot; }
    /*!
     * \brief Get serializable object with given ID.
     * \param id Object ID
     * \return Pointer to serializable object if exists, otherwise NULL
     */
    xsSerializable* GetItem(long id);
    /*!
     * \brief Get items of given class type.
     * \param type Class type
     * \param list List with matching serializable objects
	 * \param mode Search mode
	 * \sa xsSerializable::SEARCHMODE
     */
    void GetItems(wxClassInfo* type, SerializableList& list, xsSerializable::SEARCHMODE mode = xsSerializable::searchBFS);
    /*!
     * \brief Check whether given object is included in the serializer.
     * \param object Pointer to checked object
     * \return True if the object is included in the serializer, otherwise False
     */
    bool Contains(xsSerializable *object) const;
    /*!
     * \brief Check whether any object of given type is included in the serializer.
     * \param type Pointer to class info
     * \return True if at least one object of given type is included in the serializer, otherwise False
     */	
    bool Contains(wxClassInfo *type);

    /*!
     * \brief Set root item.
     * \param root Pointer to root item
     */
    void SetRootItem(xsSerializable* root);

    /*!
     * \brief Replace current stored data with a content stored in given source manager.
	 *
	 * For proper functionality all stored data items derived from the xsSerializable class
	 * MUST implement virtual function xsSerializable::Clone() as well as the copy
	 * constructor. For more details see the xsSerializable::Clone() function documentation.
     * \param src Reference to the source data manager
     */
	void CopyItems(const wxXmlSerializer& src);
    /*!
     * \brief Add serializable object to the serializer.
     * \param parentId ID of parent serializable object
     * \param item Added serializable object
	 * \return Pointer to added item
     */
     xsSerializable* AddItem(long parentId, xsSerializable* item);
    /*!
     * \brief Add serializable object to the serializer.
     * \param parent Pointer to parent serializable object (if NULL then the object
	 * is added directly to the root item)
     * \param item Added serializable object
	 * \return Pointer to added item
     */
     xsSerializable* AddItem(xsSerializable* parent, xsSerializable* item);
    /*!
     * \brief Remove serializable object from the serializer (object will be destroyed).
     * \param id Object ID
     */
    void RemoveItem(long id);
    /*!
     * \brief Remove serializable object from the serializer (object will be destroyed).
     * \param item Pointer to removed object
     */
    void RemoveItem(xsSerializable* item);
    /*! \brief Remove and destroy all stored serializable objects*/
    void RemoveAll();
    /*!
     * \brief Enable/disable object cloning.
     * \param enab TRUE if the object can be cloned, otherwise FALSE
     */
	inline void EnableCloning(bool enab) { m_fClone = enab; }
    /*!
     * \brief Returns information whether the object can be cloned or not.
     */
	inline bool IsCloned() const { return m_fClone; }

    /*!
     * \brief Serialize stored objects to given file.
     * \param file Full path to output file
     * \param withroot If TRUE then the root item's properties are serialized as well
	 * \return TRUE on success, otherwise FALSE
     */
    virtual bool SerializeToXml(const wxString& file, bool withroot = false);
    /*!
     * \brief Serialize stored objects to given stream.
     * \param outstream Output stream
     * \param withroot If TRUE then the root item's properties are serialized as well
	 * \return TRUE on success, otherwise FALSE
     */
    virtual bool SerializeToXml(wxOutputStream& outstream, bool withroot = false);
    /*!
     * \brief Deserialize objects from given file.
     * \param file Full path to input file
	 * \return TRUE on success, otherwise FALSE
     */
    virtual bool DeserializeFromXml(const wxString& file);
    /*!
     * \brief Deserialize objects from given stream.
     * \param instream Input stream
	 * \return TRUE on success, otherwise FALSE
     */
    virtual bool DeserializeFromXml(wxInputStream& instream);

    /*!
     * \brief Serialize child objects of given parent object (parent object can be optionaly
     * serialized as well) to given XML node. The function can be overriden if necessary.
     * \param parent Pointer to parent serializable object
     * \param node Pointer to output XML node
     * \param withparent TRUE if the parent object should be serialized as well
     */
    virtual void SerializeObjects(xsSerializable* parent, wxXmlNode* node, bool withparent);
    /*!
     * \brief Deserialize child objects of given parent object from given XML node.
     * The function can be overriden if necessary.
     * \param parent Pointer to parent serializable object
     * \param node Pointer to input XML node
     */
    virtual void DeserializeObjects(xsSerializable* parent, wxXmlNode* node);

	/*!
	 * \brief Get the lowest free object ID
	 */
	long GetNewId();
	/*!
	 * \brief Find out whether given object ID is already used.
	 * \param id Object ID
	 * \return TRUE if the object ID is used, otherwise FALSE
	 */
	bool IsIdUsed(long id);
	/*!
	 * \brief Get number of occurences of given ID.
	 * \param id Object ID
	 * \return Number of ID's occurences
	 */
	int GetIDCount(long id);
	/*!
	 * \brief Get map of used IDs.
	 * \return Reference to map where all used IDs are stored
	 */
	IDMap& GetUsedIDs() { return m_mapUsedIDs; }

	/*! \brief Initialize all standard property IO handlers */
	void InitializeAllIOHandlers();
	/*! \brief Clear all initialized property IO handlers */
	void ClearIOHandlers();
	/*!
	 * \brief Get property I/O handler for given datatype.
	 * \param datatype String ID of data type
	 * \return Pointer to I/O handler suitable for given data type if exists, otherwise NULL
	 */
	inline static xsPropertyIO* GetPropertyIOHandler(const wxString& datatype) { return m_mapPropertyIOHandlers[datatype]; }

    /*! \brief Map of property IO handlers */
    static PropertyIOMap m_mapPropertyIOHandlers;
	
	// overloaded operators
    /*!
     * \brief Add serializable object to the serializer's root node.
     * \param obj Pointer to serializable object
	 * \return Reference to the serializer
     */
	wxXmlSerializer& operator<< (xsSerializable *obj) { 
		if( obj ) this->AddItem( (xsSerializable*)NULL, obj);
		return *this;
	}
	/*!
     * \brief Add serializable objects stored in source serializable list to the serializer's root node.
     * \param src Reference to source serializable list
	 * \return Reference to the serializer
     */
	wxXmlSerializer& operator<< (SerializableList &src) { 
		for( SerializableList::iterator it = src.begin(); it != src.end(); ++it ) 
			this->AddItem( (xsSerializable*)NULL, *it); 
		return *this;
	}
	/*!
     * \brief Get all items managed by the serializer (note that the items will be stored in given
	 * target list in a row regardless their original hierarchy).
     * \param dest Reference to target serializable list
     */
	void operator>> (SerializableList &dest) {
		this->GetItems(CLASSINFO(xsSerializable), dest);
	}

protected:
    // protected data members
    /*! \brief Owner name */
    wxString m_sOwner;
    /*! \brief Root node name */
    wxString m_sRootName;
    /*! \brief File version */
    wxString m_sVersion;
	/*! \brief Error message */
	wxString m_sErr;

    /*! \brief Pointer to root object */
    xsSerializable* m_pRoot;

	/*! \brief Object cloning flag */
	bool m_fClone;
	
	/*! \brief Map storing information which ID is already used */
	IDMap m_mapUsedIDs;

private:
    // private data members
    //int m_nCounter;
	static int m_nRefCounter;
	static wxString m_sLibraryVersion;

    // private functions
    /*! \brief Auxiliary function */
    xsSerializable* _GetItem(long id, xsSerializable* parent);
	/*! \brief Auxiliary function */
    bool _Contains(xsSerializable *object, xsSerializable* parent) const;
};

/*!
 * \brief Class encapsulates a property stored in a list included inside a parent serializable
 * object (class xsSerializable) which is serialized/deserialized to/from XML file. The
 * property object type is defined by a string name and is processed by parent xsSerializable class object.
 *
 * Allowed property data types (keywords) are: 'long', 'double', 'bool', 'string', 'point', 'size',
 * 'realpoint', 'colour', 'brush', 'pen', 'font', 'arraystring', 'arrayrealpoint', 'listrealpoint',
 * 'listserializable', 'serializabledynamic' and 'serializablestatic'. Only properties of these data types are
 * recognized and processed by parent serializable object.
 */
class WXDLLIMPEXP_XS xsProperty : public wxObject
{
public:	
    DECLARE_DYNAMIC_CLASS(xsProperty);

    /*! \brief Default constructor */
    xsProperty()
    {
        m_pSourceVariable = NULL;
        m_sDataType = wxT("Undefined");
        m_sFieldName = wxT("Undefined");
        m_sDefaultValueStr = wxT("");
        m_fSerialize = false;
    }

    /*!
     * \brief Constructor
     * \param src Pointer to serialized object
     * \param type String value describing data type of serialized object
     * \param field Property name used in XML files and for property handling
     * \param def String representation of default poperty value
     */
    xsProperty(void* src, const wxString& type, const wxString& field, const wxString& def = wxT(""))
    {
        m_pSourceVariable = src;
        m_sDataType = type;
        m_sFieldName = field;
        m_sDefaultValueStr = def;
        m_fSerialize = true;
    }

    /*! \brief Constructor for BOOL property. */
    xsProperty(bool* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("bool")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for BOOL property with defined default value. */
    xsProperty(bool* src, const wxString& field, bool def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("bool")), m_sDefaultValueStr(xsBoolPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for LONG property. */
    xsProperty(long* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("long")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for LONG property with defined default value. */
    xsProperty(long* src, const wxString& field, long def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("long")), m_sDefaultValueStr(xsLongPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for INT property. */
    xsProperty(int* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("int")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for INT property with defined default value. */
    xsProperty(int* src, const wxString& field, int def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("int")), m_sDefaultValueStr(xsIntPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for DOUBLE property. */
    xsProperty(double* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("double")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for DOUBLE property with defined default value. */
    xsProperty(double* src, const wxString& field, double def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("double")), m_sDefaultValueStr(xsDoublePropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for FLOAT property. */
    xsProperty(float* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("float")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for FLOAT property with defined default value. */
    xsProperty(float* src, const wxString& field, float def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("float")), m_sDefaultValueStr(xsFloatPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxString property. */
    xsProperty(wxString* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("string")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxString property with defined default value. */
    xsProperty(wxString* src, const wxString& field, const wxString& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("string")), m_sDefaultValueStr(def), m_fSerialize(true) {;}

    /*! \brief Constructor for wxChar property. */
    xsProperty(wxChar* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("char")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxChar property with defined default value. */
	xsProperty(wxChar* src, const wxString& field, wxChar def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("char")), m_sDefaultValueStr(xsCharPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxPoint property. */
    xsProperty(wxPoint* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("point")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxPoint property with defined default value. */
    xsProperty(wxPoint* src, const wxString& field, const wxPoint& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("point")), m_sDefaultValueStr(xsPointPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxRealPoint property. */
    xsProperty(wxRealPoint* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("realpoint")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxRealPoint property with defined default value. */
    xsProperty(wxRealPoint* src, const wxString& field, const wxRealPoint& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("realpoint")), m_sDefaultValueStr(xsRealPointPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxSize property. */
    xsProperty(wxSize* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("size")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxSize property with defined default value. */
    xsProperty(wxSize* src, const wxString& field, const wxSize& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("size")), m_sDefaultValueStr(xsSizePropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxBrush property. */
    xsProperty(wxBrush* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("brush")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxBrush property with defined default value. */
    xsProperty(wxBrush* src, const wxString& field, const wxBrush& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("brush")), m_sDefaultValueStr(xsBrushPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxPen property. */
    xsProperty(wxPen* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("pen")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxPen property with defined default value. */
    xsProperty(wxPen* src, const wxString& field, const wxPen& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("pen")), m_sDefaultValueStr(xsPenPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxFont property. */
    xsProperty(wxFont* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("font")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxFont property with defined default value. */
    xsProperty(wxFont* src, const wxString& field, const wxFont& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("font")), m_sDefaultValueStr(xsFontPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxColour property. */
    xsProperty(wxColour* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("colour")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}
    /*! \brief Constructor for wxColour property with defined default value. */
    xsProperty(wxColour* src, const wxString& field, const wxColour& def) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("colour")), m_sDefaultValueStr(xsColourPropIO::ToString(def)), m_fSerialize(true) {;}

    /*! \brief Constructor for wxArrayString property. */
    xsProperty(wxArrayString* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("arraystring")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for CharArray property. */
    xsProperty(wxXS::CharArray* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("arraychar")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for IntArray property. */
    xsProperty(wxXS::IntArray* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("arrayint")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for LongArray property. */
    xsProperty(wxXS::LongArray* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("arraylong")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for DoubleArray property. */
    xsProperty(wxXS::DoubleArray* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("arraydoubles")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for RealPointArray property. */
    xsProperty(wxXS::RealPointArray* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("arrayrealpoint")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for RealPointList property. */
    xsProperty(wxXS::RealPointList* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("listrealpoint")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for SerializableList property. */
    xsProperty(SerializableList* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("listserializable")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for StringMap property. */
    xsProperty(wxXS::StringMap* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("mapstring")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for static serializable property. */
    xsProperty(xsSerializable* src, const wxString& field) : m_pSourceVariable((void*)src), m_sFieldName(field), m_sDataType(wxT("serializablestatic")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

    /*! \brief Constructor for dynamic serializable property. */
    xsProperty(xsSerializable** src, const wxString& field) : m_pSourceVariable((void**)src), m_sFieldName(field), m_sDataType(wxT("serializabledynamic")), m_sDefaultValueStr(wxT("")), m_fSerialize(true) {;}

	/*! \brief Copy constructor. */
	xsProperty(const xsProperty& obj) : wxObject( obj ), m_pSourceVariable(obj.m_pSourceVariable), m_sFieldName(obj.m_sFieldName), m_sDataType(obj.m_sDataType), m_sDefaultValueStr(obj.m_sDefaultValueStr), m_fSerialize(obj.m_fSerialize) {;}

    ~xsProperty(){;}
	
	// public functions
	/**
	 * \brief Convert managed data to wxVariant (supported data types: int, long, bool, double, float,
	 * wxChar, wxString and wxArrayString).
	 * \return wxVariant object containing the data
	 */
	wxVariant ToVariant()
	{
		if( m_sDataType == wxT("int") )	return wxVariant( *(int*) m_pSourceVariable );
		else if( m_sDataType == wxT("long") ) return wxVariant( *(long*) m_pSourceVariable );
		else if( m_sDataType == wxT("bool") ) return wxVariant( *(bool*) m_pSourceVariable );
		else if( m_sDataType == wxT("double") ) return wxVariant( *(double*) m_pSourceVariable );
		else if( m_sDataType == wxT("float") ) return wxVariant( *(float*) m_pSourceVariable );
		else if( m_sDataType == wxT("char") ) return wxVariant( *(wxChar*) m_pSourceVariable );
		else if( m_sDataType == wxT("string") ) return wxVariant( *(wxString*) m_pSourceVariable );
		else if( m_sDataType == wxT("arraystring") ) return wxVariant( *(wxArrayString*) m_pSourceVariable );
		else return wxVariant();
	}
	
	/**
	 * \brief Get textual representation of the property's value.
	 * \return Textual representation of current value
	 */
	wxString ToString()
	{
		xsPropertyIO *pIO = wxXmlSerializer::m_mapPropertyIOHandlers[m_sDataType];
		if(pIO) return pIO->GetValueStr(this);
		else
			return wxEmptyString;
	}
	
	/**
	 * \brief Set value defined by its textual representation.
	 * \param val Textual representation of given value
	 */
	void FromString(const wxString& val)
	{
		xsPropertyIO *pIO = wxXmlSerializer::m_mapPropertyIOHandlers[m_sDataType];
		if(pIO) pIO->SetValueStr(this, val);
	}
	
	/**
	 * \brief Get reference to managed data member as BOOL.
	 * \return Reference to managed data member
	 */
	inline bool& AsBool() { wxASSERT(m_sDataType == wxT("bool")); return *(bool*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as INT.
	 * \return Reference to managed data member
	 */
	inline int& AsInt() { wxASSERT(m_sDataType == wxT("int")); return *(int*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as LONG.
	 * \return Reference to managed data member
	 */
	inline long& AsLong() { wxASSERT(m_sDataType == wxT("long")); return *(long*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as FLOAT.
	 * \return Reference to managed data member
	 */
	inline float& AsFloat() { wxASSERT(m_sDataType == wxT("float")); return *(float*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as DOUBLE.
	 * \return Reference to managed data member
	 */
	inline double& AsDouble() { wxASSERT(m_sDataType == wxT("double")); return *(double*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxChar.
	 * \return Reference to managed data member
	 */
	inline wxChar& AsChar() { wxASSERT(m_sDataType == wxT("char")); return *(wxChar*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxString.
	 * \return Reference to managed data member
	 */
	inline wxString& AsString() { wxASSERT(m_sDataType == wxT("string")); return *(wxString*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxSize.
	 * \return Reference to managed data member
	 */
	inline wxSize& AsSize() { wxASSERT(m_sDataType == wxT("size")); return *(wxSize*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxPoint.
	 * \return Reference to managed data member
	 */
	inline wxPoint& AsPoint() { wxASSERT(m_sDataType == wxT("point")); return *(wxPoint*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxRealPoint.
	 * \return Reference to managed data member
	 */
	inline wxRealPoint& AsRealPoint() { wxASSERT(m_sDataType == wxT("realpoint")); return *(wxRealPoint*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxBrush.
	 * \return Reference to managed data member
	 */
	inline wxBrush& AsBrush() { wxASSERT(m_sDataType == wxT("brush")); return *(wxBrush*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxPen.
	 * \return Reference to managed data member
	 */
	inline wxPen& AsPen() { wxASSERT(m_sDataType == wxT("pen")); return *(wxPen*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxFont.
	 * \return Reference to managed data member
	 */
	inline wxFont& AsFont() { wxASSERT(m_sDataType == wxT("font")); return *(wxFont*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as wxColour.
	 * \return Reference to managed data member
	 */
	inline wxColour& AsColour() { wxASSERT(m_sDataType == wxT("colour")); return *(wxColour*)m_pSourceVariable; }
	
	/**
	 * \brief Get reference to managed data member as wxArrayString.
	 * \return Reference to managed data member
	 */
	inline wxArrayString& AsStringArray() { wxASSERT(m_sDataType == wxT("arraystring")); return *(wxArrayString*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as CharArray.
	 * \return Reference to managed data member
	 */
	inline wxXS::CharArray& AsCharArray() { wxASSERT(m_sDataType == wxT("arraychar")); return *(wxXS::CharArray*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as IntArray.
	 * \return Reference to managed data member
	 */
	inline wxXS::IntArray& AsIntArray() { wxASSERT(m_sDataType == wxT("arrayint")); return *(wxXS::IntArray*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as LongArray.
	 * \return Reference to managed data member
	 */
	inline wxXS::LongArray& AsLongArray() { wxASSERT(m_sDataType == wxT("arraylong")); return *(wxXS::LongArray*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as DoubleArray.
	 * \return Reference to managed data member
	 */
	inline wxXS::DoubleArray& AsDoubleArray() { wxASSERT(m_sDataType == wxT("arraydouble")); return *(wxXS::DoubleArray*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as RealPointArray.
	 * \return Reference to managed data member
	 */
	inline wxXS::RealPointArray& AsRealPointArray() { wxASSERT(m_sDataType == wxT("arrayrealpoint")); return *(wxXS::RealPointArray*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as RealPointList.
	 * \return Reference to managed data member
	 */
	inline wxXS::RealPointList& AsRealPointList() { wxASSERT(m_sDataType == wxT("listrealpoint")); return *(wxXS::RealPointList*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as SerializableList.
	 * \return Reference to managed data member
	 */
	inline SerializableList& AsSerializableList() { wxASSERT(m_sDataType == wxT("listserializable")); return *(SerializableList*)m_pSourceVariable; }

	/**
	 * \brief Get reference to managed data member as StringMap.
	 * \return Reference to managed data member
	 */
	inline wxXS::StringMap& AsStringMap() { wxASSERT(m_sDataType == wxT("mapstring")); return *(wxXS::StringMap*)m_pSourceVariable; }
	
	/**
	 * \brief Get reference to managed data member as serializable static object.
	 * \return Reference to managed data member
	 */
	inline xsSerializable& AsSerializableStatic() { wxASSERT(m_sDataType == wxT("serializablestatic")); return *(xsSerializable*)m_pSourceVariable; }
	/**
	 * \brief Get reference to managed data member as serializable dynamic object.
	 * \return Reference to managed data member
	 */
	inline xsSerializable& AsSerializableDynamic() { wxASSERT(m_sDataType == wxT("serializabledynamic")); return **(xsSerializable**)m_pSourceVariable; }
	
	
	// public data members
	/*! \brief General (void) pointer to serialized object encapsulated by the property */
    void* m_pSourceVariable;
    /*! \brief Field (property) name */
    wxString m_sFieldName;
    /*! \brief Data type */
    wxString m_sDataType;
    /*! \brief String representation of property's default value */
    wxString m_sDefaultValueStr;
    /*! \brief Flag used for enabling/disabling of property serialization */
    bool m_fSerialize;
};

#endif //_XSXMLSERIALIZE_H

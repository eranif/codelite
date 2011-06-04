/***************************************************************
 * Name:      XmlSerializer.cpp
 * Purpose:   Implements XML serializer and related classes
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-08-28
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxxmlserializer/XmlSerializer.h"

#include <wx/listimpl.cpp>
#include <wx/wfstream.h>
#include <wx/arrimpl.cpp>

WX_DEFINE_EXPORTED_LIST(PropertyList);
WX_DEFINE_EXPORTED_LIST(SerializableList);

// static members
PropertyIOMap wxXmlSerializer::m_mapPropertyIOHandlers;
int wxXmlSerializer::m_nRefCounter = 0;
wxString wxXmlSerializer::m_sLibraryVersion = wxT("1.3.4 beta");

/////////////////////////////////////////////////////////////////////////////////////
// xsProperty class /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsProperty, wxObject);

/////////////////////////////////////////////////////////////////////////////////////
// xsSerializable class /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_IMPLEMENT_CLONABLE_CLASS(xsSerializable, wxObject);

// constructor and destructor ///////////////////////////////////////////////////////

xsSerializable::xsSerializable()
{
	m_pParentManager = NULL;
    m_pParentItem = NULL;
    m_fSerialize = true;
	m_fClone = true;
    m_nId = -1;

    XS_SERIALIZE(m_nId, wxT("id"));
}

xsSerializable::xsSerializable(const xsSerializable& obj)
: wxObject(obj)
{
	m_pParentManager = NULL;
    m_pParentItem = NULL;
    m_fSerialize = obj.m_fSerialize;
	m_fClone = obj.m_fClone;
    m_nId = obj.m_nId;

    XS_SERIALIZE(m_nId, wxT("id"));
	
	// copy serialized children as well
	SerializableList::compatibility_iterator node = obj.GetFirstChildNode();
	while( node )
	{
		if( node->GetData()->IsSerialized() ) AddChild( (xsSerializable*)node->GetData()->Clone() );
		node = node->GetNext();
	}
}

xsSerializable::~xsSerializable()
{
	if( m_pParentManager )
	{
		m_pParentManager->GetUsedIDs().erase( m_nId );
	}
	
    m_lstProperties.DeleteContents(true);
    m_lstProperties.Clear();

    m_lstChildItems.DeleteContents(true);
    m_lstChildItems.Clear();
}

// public functions /////////////////////////////////////////////////////////////////

void xsSerializable::SetId(long id)
{
	m_nId = id;
	
	if( m_pParentManager ) m_pParentManager->GetUsedIDs()[id] = this;
}

xsSerializable* xsSerializable::AddChild(xsSerializable* child)
{
    wxASSERT(child);

    if( child )
    {
		InitChild( child );
		
        m_lstChildItems.Append(child);
    }

    return child;
}

xsSerializable* xsSerializable::InsertChild(size_t pos, xsSerializable* child)
{
    wxASSERT(child);

    if( child )
    {
		InitChild( child );
		
        m_lstChildItems.Insert(pos, child);
    }

    return child;
}

void xsSerializable::Reparent(xsSerializable* parent)
{
    if(m_pParentItem)
    {
        m_pParentItem->m_lstChildItems.DeleteObject(this);
    }

    if(parent)
    {
        parent->AddChild(this);
    }
    else
        m_pParentItem = NULL;
}

void xsSerializable::RemoveChild(xsSerializable* child)
{
	wxASSERT( child );
	
	if( child )
	{
		m_lstChildItems.DeleteObject( child );
		delete child;
	}
}

void xsSerializable::RemoveChildren()
{
	m_lstChildItems.DeleteContents( true );
	m_lstChildItems.Clear();
	m_lstChildItems.DeleteContents( false );
}

xsSerializable* xsSerializable::GetFirstChild()
{
	SerializableList::compatibility_iterator node = m_lstChildItems.GetFirst();
    if( node )return node->GetData();
    else
        return NULL;
}

xsSerializable* xsSerializable::GetFirstChild(wxClassInfo *type)
{
	SerializableList::compatibility_iterator node = m_lstChildItems.GetFirst();
    while( node )
	{
		if( node->GetData()->IsKindOf( type ) ) return node->GetData();
		node = node->GetNext();
    }
	return NULL;
}

xsSerializable* xsSerializable::GetLastChild()
{
    SerializableList::compatibility_iterator node = m_lstChildItems.GetLast();
    if( node )return node->GetData();
    else
        return NULL;
}

xsSerializable* xsSerializable::GetLastChild(wxClassInfo *type)
{
    SerializableList::compatibility_iterator node = m_lstChildItems.GetLast();
    while( node )
	{
		if( node->GetData()->IsKindOf( type ) ) return node->GetData();
		node = node->GetPrevious();
    }
	return NULL;
}

xsSerializable* xsSerializable::GetSibbling()
{
    wxASSERT( m_pParentItem );

    if( m_pParentItem )
    {
		SerializableList::compatibility_iterator node = m_pParentItem->GetChildrenList().Find( this );
		if( node )
		{
			if( node->GetNext() ) return node->GetNext()->GetData();
		}
    }

    return NULL;
}

xsSerializable* xsSerializable::GetSibbling(wxClassInfo *type)
{
    wxASSERT( m_pParentItem );

    if( m_pParentItem )
    {
		SerializableList::compatibility_iterator node = m_pParentItem->GetChildrenList().Find( this );
		while( node )
		{
			node = node->GetNext();
			
			if( node && (node->GetData()->IsKindOf( type ) ) ) return node->GetData();
		}
    }

    return NULL;
}


xsSerializable* xsSerializable::GetChild(long id, bool recursive)
{
    SerializableList lstChildren;
    SerializableList::compatibility_iterator node = NULL;

    if( recursive )
    {
        GetChildrenRecursively( CLASSINFO(xsSerializable), lstChildren );
        node = lstChildren.GetFirst();
    }
    else
        node = m_lstChildItems.GetFirst();

    while(node)
    {
		if( node->GetData()->GetId() == id) return node->GetData();
        node = node->GetNext();
    }

	return NULL;
}

void xsSerializable::GetChildren(wxClassInfo *type, SerializableList& list)
{
    xsSerializable *pChild;

    SerializableList::compatibility_iterator node = m_lstChildItems.GetFirst();
    while(node)
    {
        pChild = node->GetData();

        if( !type || pChild->IsKindOf(type) ) list.Append(pChild);

        node = node->GetNext();
    }
}

void xsSerializable::GetChildrenRecursively(wxClassInfo *type, SerializableList& list, SEARCHMODE mode)
{
    xsSerializable *pChild;

    SerializableList::compatibility_iterator node = m_lstChildItems.GetFirst();
    while(node)
    {
        pChild = node->GetData();
        if( !type || pChild->IsKindOf(type) ) list.Append(pChild);
		if( mode == searchDFS ) pChild->GetChildrenRecursively(type, list);

        node = node->GetNext();
    }
	
	if( mode == searchBFS )
	{
		node = m_lstChildItems.GetFirst();
		while(node)
		{
			node->GetData()->GetChildrenRecursively(type, list);
			node = node->GetNext();
		}
	}
}

void xsSerializable::AddProperty(xsProperty* property)
{
    if(property)
    {
        if(!GetProperty(property->m_sFieldName))
        {
            m_lstProperties.Append(property);
        }
    }
}

void xsSerializable::RemoveProperty(xsProperty* property)
{
	if( property )
	{
		m_lstProperties.DeleteObject( property );
		delete property;
	}
}

xsProperty* xsSerializable::GetProperty(const wxString& field)
{
	PropertyList::compatibility_iterator node = m_lstProperties.GetFirst();
    while(node)
    {
        if(node->GetData()->m_sFieldName == field)return node->GetData();
        node = node->GetNext();
    }
    return NULL;
}

void xsSerializable::EnablePropertySerialization(const wxString& field, bool enab)
{
    xsProperty* property = GetProperty(field);
    if(property)
    {
        property->m_fSerialize = enab;
    }
}

bool xsSerializable::IsPropertySerialized(const wxString& field)
{
    xsProperty* property = GetProperty(field);
    if(property)
    {
        return property->m_fSerialize;
    }
    return false;
}

wxXmlNode* xsSerializable::SerializeObject(wxXmlNode* node)
{
	if(!node || (node->GetName() != wxT("object")))
	{
		node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("object"));
		node->AddProperty(new wxXmlProperty(wxT("type"), this->GetClassInfo()->GetClassName()));
	}

	if(node) return this->Serialize(node);
	else
		return NULL;
}

void xsSerializable::DeserializeObject(wxXmlNode* node)
{
	if(node && (node->GetName()==wxT("object")))
	{
		this->Deserialize(node);
	}
}

// overloaded operators /////////////////////////////////////////////////////////////

xsSerializable* xsSerializable::operator<<( xsSerializable *child )
{
	if( child && (child != this ) )
	{
		return this->AddChild( child );
	}
	else
		return this;
}

// protected functions //////////////////////////////////////////////////////////////

wxXmlNode* xsSerializable::Serialize(wxXmlNode* node)
{
    xsProperty* property;
    xsPropertyIO* ioHandler;

    PropertyList::compatibility_iterator propNode = m_lstProperties.GetFirst();
    while(propNode)
    {
        property = propNode->GetData();

        if(property->m_fSerialize)
        {
			ioHandler = wxXmlSerializer::m_mapPropertyIOHandlers[property->m_sDataType];
            if(ioHandler)
            {
                ioHandler->Write(property, node);
            }
        }

        propNode = propNode->GetNext();
    }

    return node;
}

void xsSerializable::Deserialize(wxXmlNode* node)
{
    wxASSERT(node);
    if(!node)return;

    xsProperty* property;
    xsPropertyIO* ioHandler;
    wxString propName;

    wxXmlNode *xmlNode = node->GetChildren();
	while(xmlNode)
	{
	    if(xmlNode->GetName() == wxT("property"))
	    {
	        xmlNode->GetPropVal(wxT("name"), &propName);
	        property = GetProperty(propName);

	        if(property)
	        {
                ioHandler =  wxXmlSerializer::m_mapPropertyIOHandlers[property->m_sDataType];
                if(ioHandler)
                {
                    ioHandler->Read(property, xmlNode);
                }
	        }
	    }

	    xmlNode = xmlNode->GetNext();
	}
}

void xsSerializable::InitChild(xsSerializable* child)
{
	if( child )
	{
        child->m_pParentItem = this;

        if( m_pParentManager )
        {
			if( child->m_pParentManager != m_pParentManager )
			{
				child->m_pParentManager = m_pParentManager;
				
				// assign unique ids to the child object
				if( child->GetId() == -1 ) child->SetId(m_pParentManager->GetNewId());
				else
					m_pParentManager->GetUsedIDs()[child->GetId()] = child;
				
				// if the child has another children, set their parent manager and ID as well
				xsSerializable *pItem;
				SerializableList lstChildren;
				child->GetChildrenRecursively( NULL, lstChildren );
				
				SerializableList::compatibility_iterator node = lstChildren.GetFirst();
				while( node )
				{
					pItem = node->GetData();
					
					pItem->SetParentManager( m_pParentManager );
					
					if( pItem->GetId() == -1 ) pItem->SetId(m_pParentManager->GetNewId());
					else
						m_pParentManager->GetUsedIDs()[pItem->GetId()] = pItem;
					
					node = node->GetNext();
				}
			}
        }
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// wxXmlSerializer class ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_IMPLEMENT_CLONABLE_CLASS(wxXmlSerializer, wxObject);

// constructor and destructor ///////////////////////////////////////////////////////

wxXmlSerializer::wxXmlSerializer()
{
    m_sOwner = wxT("");
    m_sRootName = wxT("root");
    m_sVersion = wxT("");
	m_fClone = true;

	m_pRoot = NULL;
    SetRootItem(new xsSerializable());

	if(m_nRefCounter == 0)
	{
		InitializeAllIOHandlers();
	}
	m_nRefCounter++;
}

wxXmlSerializer::wxXmlSerializer(const wxXmlSerializer &obj)
: wxObject(obj)
{
	m_sOwner = obj.m_sOwner;
    m_sRootName = obj.m_sRootName;
	m_sVersion = obj.m_sVersion;
	m_fClone = obj.m_fClone;

	m_pRoot = NULL;
	
	SetRootItem((xsSerializable*)obj.m_pRoot->Clone());

	m_nRefCounter++;
}

wxXmlSerializer::wxXmlSerializer(const wxString& owner, const wxString& root, const wxString& version)
{
    m_sOwner = owner;
    m_sRootName = root;
    m_sVersion = version;
	m_fClone = true;

	m_pRoot = NULL;
    SetRootItem(new xsSerializable());

	if(m_nRefCounter == 0)
	{
		InitializeAllIOHandlers();
	}
	m_nRefCounter++;
}

wxXmlSerializer::~wxXmlSerializer()
{
    if( m_pRoot ) delete m_pRoot;

	m_nRefCounter--;
	if(m_nRefCounter == 0)
	{
		ClearIOHandlers();
	}
}

// public functions //////////////////////////////////////////////////////////////////

void wxXmlSerializer::InitializeAllIOHandlers()
{
	ClearIOHandlers();

    XS_REGISTER_IO_HANDLER(wxT("string"), xsStringPropIO);
	XS_REGISTER_IO_HANDLER(wxT("char"), xsCharPropIO);
	XS_REGISTER_IO_HANDLER(wxT("int"), xsIntPropIO);
    XS_REGISTER_IO_HANDLER(wxT("long"), xsLongPropIO);
	XS_REGISTER_IO_HANDLER(wxT("float"), xsFloatPropIO);
    XS_REGISTER_IO_HANDLER(wxT("double"), xsDoublePropIO);
    XS_REGISTER_IO_HANDLER(wxT("bool"), xsBoolPropIO);
    XS_REGISTER_IO_HANDLER(wxT("point"), xsPointPropIO);
    XS_REGISTER_IO_HANDLER(wxT("size"), xsSizePropIO);
    XS_REGISTER_IO_HANDLER(wxT("realpoint"), xsRealPointPropIO);
    XS_REGISTER_IO_HANDLER(wxT("colour"), xsColourPropIO);
    XS_REGISTER_IO_HANDLER(wxT("brush"), xsBrushPropIO);
    XS_REGISTER_IO_HANDLER(wxT("pen"), xsPenPropIO);
    XS_REGISTER_IO_HANDLER(wxT("font"), xsFontPropIO);
    XS_REGISTER_IO_HANDLER(wxT("arraystring"), xsArrayStringPropIO);
    XS_REGISTER_IO_HANDLER(wxT("arraychar"), xsArrayCharPropIO);
    XS_REGISTER_IO_HANDLER(wxT("arrayint"), xsArrayIntPropIO);
    XS_REGISTER_IO_HANDLER(wxT("arraylong"), xsArrayLongPropIO);
    XS_REGISTER_IO_HANDLER(wxT("arraydouble"), xsArrayDoublePropIO);
    XS_REGISTER_IO_HANDLER(wxT("arrayrealpoint"), xsArrayRealPointPropIO);
	XS_REGISTER_IO_HANDLER(wxT("mapstring"), xsMapStringPropIO);
    XS_REGISTER_IO_HANDLER(wxT("listrealpoint"), xsListRealPointPropIO);
    XS_REGISTER_IO_HANDLER(wxT("listserializable"), xsListSerializablePropIO);
    XS_REGISTER_IO_HANDLER(wxT("serializablestatic"), xsStaticObjPropIO);
    XS_REGISTER_IO_HANDLER(wxT("serializabledynamic"), xsDynObjPropIO);
    XS_REGISTER_IO_HANDLER(wxT("serializabledynamicnocreate"), xsDynNCObjPropIO);
}

void wxXmlSerializer::ClearIOHandlers()
{
    PropertyIOMap::iterator it = m_mapPropertyIOHandlers.begin();
    while(it != m_mapPropertyIOHandlers.end())
    {
		if(it->second)delete it->second;
        it++;
    }
    m_mapPropertyIOHandlers.clear();
}

xsSerializable* wxXmlSerializer::GetItem(long id)
{
    if( m_pRoot )
    {
		IDMap::iterator it = m_mapUsedIDs.find( id );
		if( it != m_mapUsedIDs.end() ) return it->second;
    }

    return NULL;
}

bool wxXmlSerializer::Contains(xsSerializable *object) const
{
    if( m_pRoot )
    {
        return _Contains(object, m_pRoot);
    }

    return false;
}

bool wxXmlSerializer::Contains(wxClassInfo *type)
{
	SerializableList lstItems;
		
	GetItems( type, lstItems );
		
	return !lstItems.IsEmpty();
}

void wxXmlSerializer::GetItems(wxClassInfo* type, SerializableList& list, xsSerializable::SEARCHMODE mode)
{
    if( m_pRoot )
    {
        m_pRoot->GetChildrenRecursively(type, list, mode);
    }
}

void wxXmlSerializer::CopyItems(const wxXmlSerializer& src)
{
	// clear current content
	m_pRoot->GetChildrenList().DeleteContents( true );
	m_pRoot->GetChildrenList().Clear();
	m_pRoot->GetChildrenList().DeleteContents( false );
	
	m_mapUsedIDs.clear();
	
	SerializableList::compatibility_iterator node = src.GetRootItem()->GetFirstChildNode();
	while( node )
	{
		AddItem( m_pRoot, (xsSerializable*)node->GetData()->Clone() );
		node = node->GetNext();
	}
}

xsSerializable*  wxXmlSerializer::AddItem(long parentId, xsSerializable* item)
{
    return AddItem(GetItem(parentId), item);
}

xsSerializable*  wxXmlSerializer::AddItem(xsSerializable* parent, xsSerializable* item)
{
    wxASSERT(m_pRoot);
    wxASSERT(item);

    if( item )
    {
        if( parent )parent->AddChild(item);
		else
			m_pRoot->AddChild(item);
    }
	
	return item;
}

void wxXmlSerializer::RemoveItem(long id)
{
    RemoveItem(GetItem(id));
}

void wxXmlSerializer::RemoveItem(xsSerializable* item)
{
    wxASSERT(item);

    if(item)
    {		
        if( item->GetParent() )
        {
            item->GetParent()->GetChildrenList().DeleteObject(item);
        }
        delete item;
    }
}

void wxXmlSerializer::RemoveAll()
{
	SetRootItem(new xsSerializable());
}

void wxXmlSerializer::SetRootItem(xsSerializable* root)
{
    wxASSERT(root);
    wxASSERT(root->IsKindOf(CLASSINFO(xsSerializable)));

	if( m_pRoot )delete m_pRoot;

    if(root && root->IsKindOf(CLASSINFO(xsSerializable)))
    {
        m_pRoot = root;
    }
	else
		m_pRoot = new xsSerializable();

	// update pointers to parent manager
	m_mapUsedIDs.clear();
	
	m_pRoot->m_pParentManager = this;
	m_mapUsedIDs[m_pRoot->GetId()] = m_pRoot;
	
	xsSerializable *pItem;
	SerializableList lstItems;
	GetItems(NULL, lstItems);
	
	SerializableList::compatibility_iterator node = lstItems.GetFirst();
	while( node )
	{
		pItem = node->GetData();
		
		pItem->m_pParentManager = this;
		m_mapUsedIDs[pItem->GetId()] = pItem;
		
		node = node->GetNext();
	}
}

bool wxXmlSerializer::SerializeToXml(const wxString& file, bool withroot)
{
	wxFileOutputStream outstream(file);

	if(outstream.IsOk())
	{
		return this->SerializeToXml(outstream, withroot);
	}
	else
		m_sErr = wxT("Unable to initialize output file stream.");

	return false;
}

bool wxXmlSerializer::SerializeToXml(wxOutputStream& outstream, bool withroot)
{
	// create root node
	wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, m_sRootName);

	if(root)
	{
	    // add version
	    root->AddProperty(wxT("owner"), m_sOwner);
	    root->AddProperty(wxT("version"), m_sVersion);

	    // serialize root item properties
	    if(withroot)
	    {
	        wxXmlNode *root_props = new wxXmlNode(wxXML_ELEMENT_NODE, m_sRootName + wxT("_properties"));
	        root_props->AddChild(m_pRoot->SerializeObject(NULL));
	        root->AddChild(root_props);
	    }

		// serialize shapes recursively
		this->SerializeObjects(m_pRoot, root, false);

		// create XML document
		try
		{
			wxXmlDocument xmlDoc;
			xmlDoc.SetRoot(root);

			xmlDoc.Save(outstream, 2);
		}
		catch (...)
		{
			m_sErr = wxT("Unable to save XML document.");
			return false;
		}
	}
	
	return true;
}

bool wxXmlSerializer::DeserializeFromXml(const wxString& file)
{
	wxFileInputStream instream(file);
	
	if(instream.IsOk())
	{
		return this->DeserializeFromXml(instream);
	}
	else
		m_sErr = wxT("Unable to initialize input stream.");
	
	return false;
}

bool wxXmlSerializer::DeserializeFromXml(wxInputStream& instream)
{
	// load an XML file
	try
	{
		wxXmlDocument xmlDoc;
		xmlDoc.Load(instream);

		wxXmlNode* root = xmlDoc.GetRoot();
		if(root && (root->GetName() == m_sRootName))
		{
		    // read project node's properties here...
		    wxString version, owner;
		    root->GetPropVal(wxT("owner"), &owner);
		    root->GetPropVal(wxT("version"), &version);

		    if( (owner == m_sOwner) && (version == m_sVersion) )
		    {
                // read shape objects from XML recursively
                this->DeserializeObjects(NULL, root);
				return true;
		    }
            else
				m_sErr = wxT("No matching file owner or version.");
		}
		else
			m_sErr = wxT("Unknown file format.");
	}
	catch (...)
	{
		m_sErr = wxT("Unable to load XML file.");
	}
	
	return false;
}

void wxXmlSerializer::SerializeObjects(xsSerializable* parent, wxXmlNode* node, bool withparent)
{
    wxASSERT(parent);
    if(!parent)return;

	wxXmlNode* projectNode = NULL;
	xsSerializable* pChild;

	// serialize parent shape
	if(withparent)
	{
	    if(parent->IsSerialized())
	    {
            projectNode = parent->SerializeObject(NULL);
            if(projectNode)
            {
                SerializeObjects(parent, projectNode, false);
                node->AddChild(projectNode);
            }
	    }
	}
	else
	{
		// serialize parent's children
		SerializableList::compatibility_iterator snode = parent->GetChildrenList().GetFirst();
		while(snode)
		{
			pChild = snode->GetData();

            if(pChild->IsSerialized())
            {
                projectNode = pChild->SerializeObject(NULL);
                if(projectNode)
                {
                    SerializeObjects(pChild, projectNode, false);
                    node->AddChild(projectNode);
                }
            }

			snode = snode->GetNext();
		}
	}
}

void wxXmlSerializer::DeserializeObjects(xsSerializable* parent, wxXmlNode* node)
{
    wxASSERT(m_pRoot);
    if(!m_pRoot)return;

	xsSerializable *pItem;

	wxXmlNode* projectNode = node->GetChildren();
	while(projectNode)
	{
		if(projectNode->GetName() == wxT("object"))
		{
		    pItem = (xsSerializable*)wxCreateDynamicObject(projectNode->GetPropVal(wxT("type"), wxT("")));
			if(pItem)
			{
			    if(parent)
			    {
			        parent->AddChild(pItem);
			    }
			    else
                    m_pRoot->AddChild(pItem);

				pItem->DeserializeObject(projectNode);
				// id could change so we must update the IDs map
				m_mapUsedIDs[pItem->GetId()] = pItem;

				// deserialize child objects
				DeserializeObjects(pItem, projectNode);
			}
		}
		else if(projectNode->GetName() == m_sRootName + wxT("_properties"))
		{
		    m_pRoot->DeserializeObject(projectNode->GetChildren());
		}

		projectNode = projectNode->GetNext();
	}
}

bool wxXmlSerializer::IsIdUsed(long id)
{
	//return (GetIDCount(id) > 0);
	return (m_mapUsedIDs.find( id ) != m_mapUsedIDs.end());
}

int wxXmlSerializer::GetIDCount(long id)
{
	int nCount = 0;

    SerializableList items;
    GetItems(CLASSINFO(xsSerializable), items);

	SerializableList::compatibility_iterator node = items.GetFirst();
	while(node)
	{
		if( node->GetData()->GetId() == id ) nCount++;
		node = node->GetNext();
	}

	if( m_pRoot->GetId() == id ) nCount++;

	return nCount;
}

long wxXmlSerializer::GetNewId()
{
/*	long nId = 1;
	
	for ( IDMap::iterator it = m_mapUsedIDs.begin(); it != m_mapUsedIDs.end(); it++, nId++ )
	{
		if (it->first != nId) break;
	}
	
	return nId;*/
	
	long nId = 1;
	
	while( m_mapUsedIDs.find( nId ) != m_mapUsedIDs.end() ) nId++;
	
	return nId;
}

// private virtual functions  ///////////////////////////////////////////////////////

xsSerializable* wxXmlSerializer::_GetItem(long id, xsSerializable* parent)
{
    wxASSERT(parent);

    if( !parent )return NULL;

    if( parent->GetId() == id )return parent;

    xsSerializable *pItem = NULL;
    SerializableList::compatibility_iterator node = parent->GetChildrenList().GetFirst();
    while(node)
    {
        pItem = _GetItem(id, node->GetData());
        if( pItem )break;
        node = node->GetNext();
    }
    return pItem;
}

bool wxXmlSerializer::_Contains(xsSerializable* object, xsSerializable* parent) const
{
    wxASSERT(parent);

    if( !parent )return false;

    if( parent == object )return true;

    bool fFound = false;
    SerializableList::compatibility_iterator node = parent->GetChildrenList().GetFirst();
    while(node)
    {
        fFound = _Contains(object, node->GetData());
        if( fFound )break;
        node = node->GetNext();
    }
    return fFound;
}

/***************************************************************
 * Name:      DiagramManager.cpp
 * Purpose:   Implements shape manager class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-25
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <wx/wfstream.h>
#include <wx/mstream.h>
#include <wx/listimpl.cpp>

#include "wx/wxsf/DiagramManager.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/ControlShape.h"
#include "wx/wxsf/LineShape.h"
#include "wx/wxsf/GridShape.h"

using namespace wxSFCommonFcn;

// TODO: implement better line X ellipse check

WX_DEFINE_LIST(IDList);

XS_IMPLEMENT_CLONABLE_CLASS(wxSFDiagramManager, wxXmlSerializer);

wxSFDiagramManager::wxSFDiagramManager()
{
	m_fIsModified = false;
    m_pShapeCanvas = NULL;
    m_lstIDPairs.DeleteContents(true);

    m_sSFVersion =  wxT("1.15.1 beta");

    SetSerializerOwner(wxT("wxShapeFramework"));
    SetSerializerVersion(wxT("1.0"));
    SetSerializerRootName(wxT("chart"));
	
	m_arrAcceptedShapes.Add( wxT("All") );
	m_arrAcceptedTopShapes.Add( wxT("All") );
}

wxSFDiagramManager::wxSFDiagramManager(const wxSFDiagramManager &obj)
: wxXmlSerializer(obj)
{
	m_fIsModified = false;
	m_pShapeCanvas = NULL;
	m_sSFVersion = obj.m_sSFVersion;
	m_arrAcceptedShapes = obj.m_arrAcceptedShapes;
	m_arrAcceptedTopShapes = obj.m_arrAcceptedTopShapes;

    m_lstIDPairs.DeleteContents(true);
}

wxSFDiagramManager::~wxSFDiagramManager()
{
    Clear();
}

//----------------------------------------------------------------------------------//
// Adding/removing shapes functions
//----------------------------------------------------------------------------------//

wxSFShapeBase* wxSFDiagramManager::AddShape(wxClassInfo* shapeInfo, bool saveState, wxSF::ERRCODE *err)
{
    wxPoint shapePos;

    if(m_pShapeCanvas)
    {
        wxRect crect = m_pShapeCanvas->GetClientRect();
        shapePos = wxPoint((crect.GetRight() - crect.GetLeft())/2,
                (crect.GetBottom() - crect.GetTop())/2);
    }

	wxSFShapeBase* pShape = AddShape(shapeInfo, shapePos, saveState, err);

	return pShape;
}

wxSFShapeBase* wxSFDiagramManager::AddShape(wxClassInfo* shapeInfo, const wxPoint& pos, bool saveState, wxSF::ERRCODE *err)
{
	wxASSERT( shapeInfo );
	
    if( shapeInfo && IsShapeAccepted(shapeInfo->GetClassName()) )
    {
        // create shape object from class info
        wxSFShapeBase *pShape = (wxSFShapeBase*)shapeInfo->CreateObject();

        wxSFShapeBase *pParentShape = NULL;
		// update given possition
		wxPoint lpos = pos;
		if( m_pShapeCanvas )
		{
			lpos = m_pShapeCanvas->FitPositionToGrid( m_pShapeCanvas->DP2LP(pos) );
		}
		// line shapes can be assigned to root only
		if( !pShape->IsKindOf(CLASSINFO(wxSFLineShape)) ) pParentShape = GetShapeAtPosition(lpos);
		
        if( pParentShape && pParentShape->IsChildAccepted(shapeInfo->GetClassName()) )
        {
            pShape = AddShape(pShape, (xsSerializable*)pParentShape, pos - Conv2Point( pParentShape->GetAbsolutePosition() ), sfINITIALIZE, saveState, err);
        }
        else
            pShape = AddShape(pShape, NULL, pos, sfINITIALIZE, saveState, err);


		if( pParentShape ) pParentShape->Update();

        return pShape;
    }
    else
	{
		if( err ) *err = wxSF::errNOT_ACCEPTED;
        return NULL;
	}
}

wxSFShapeBase* wxSFDiagramManager::AddShape(wxSFShapeBase* shape, xsSerializable* parent, const wxPoint& pos, bool initialize, bool saveState, wxSF::ERRCODE *err)
{
	if(shape)
	{
		if( shape->IsKindOf(CLASSINFO(wxSFShapeBase)) && IsShapeAccepted(shape->GetClassInfo()->GetClassName()) )
		{
		    if( m_pShapeCanvas )
		    {
                wxPoint newPos = m_pShapeCanvas->FitPositionToGrid(m_pShapeCanvas->DP2LP(pos));
                shape->SetRelativePosition( Conv2RealPoint(newPos) );
		    }
		    else
                shape->SetRelativePosition( Conv2RealPoint(pos) );

            // add parent shape to the data manager (serializer)
            if( parent && parent != GetRootItem() )
            {
                AddItem(parent, shape);
            }
            else
			{
                if( IsTopShapeAccepted( shape->GetClassInfo()->GetClassName() ) ) AddItem(GetRootItem(), shape);
				else
				{
					delete shape;
					if( err ) *err = wxSF::errNOT_ACCEPTED;
					
					return NULL;
				}
			}

            // initialize added shape
			if(initialize)
			{
				shape->CreateHandles();
				
				if( m_pShapeCanvas )
				{
                    shape->SetHoverColour(m_pShapeCanvas->GetHoverColour());
				}

                if(HasChildren(shape))
                {
                    wxSFShapeBase* pChild;
                    ShapeList lstChildren;

                    // get shape's children (if exist)
                    shape->GetChildShapes(sfANY, lstChildren, sfRECURSIVE);
                    // initialize shape's children
                    ShapeList::compatibility_iterator node = lstChildren.GetFirst();
                    while(node)
                    {
                        pChild = (wxSFShapeBase*)node->GetData();

                        pChild->CreateHandles();
                        pChild->Update();
						
                        if( m_pShapeCanvas )
                        {
                            pChild->SetHoverColour(m_pShapeCanvas->GetHoverColour());
                        }

                        node = node->GetNext();
                    }
                }
			}

			// reset scale of assigned shape canvas (if exists and it is necessary...)
			if( m_pShapeCanvas && shape->IsKindOf( CLASSINFO(wxSFControlShape) ) )
			{
				m_pShapeCanvas->SetScale( 1 );
			}
			
            if( m_pShapeCanvas )
            {
                if( saveState )
                {
                    m_pShapeCanvas->SaveCanvasState();
                }
            }
			
			if( err ) *err = wxSF::errOK;
			
			m_fIsModified = true;
		}
		else
		{
			//wxMessageBox(wxString::Format(wxT("Unable to add '%s' class object to the canvas"), shape->GetClassInfo()->GetClassName()), wxT("ShapeFramework"), wxICON_WARNING);

			delete shape;
			shape = NULL;
			
			if( err ) *err = wxSF::errNOT_ACCEPTED;
		}
	}
	else if( err ) *err = wxSF::errINVALID_INPUT;

	return shape;
}

wxSFShapeBase* wxSFDiagramManager::CreateConnection(long srcId, long trgId, bool saveState, wxSF::ERRCODE *err)
{
    return CreateConnection(srcId, trgId, CLASSINFO(wxSFLineShape), saveState, err);
}

wxSFShapeBase* wxSFDiagramManager::CreateConnection(long srcId, long trgId, wxClassInfo *lineInfo, bool saveState, wxSF::ERRCODE *err)
{
    wxSFShapeBase* pShape = AddShape(lineInfo, sfDONT_SAVE_STATE, err);
    if(pShape)
    {
        wxSFLineShape *pLine = (wxSFLineShape*)pShape;
        pLine->SetSrcShapeId(srcId);
        pLine->SetTrgShapeId(trgId);


        if( m_pShapeCanvas )
        {
            if(saveState)m_pShapeCanvas->SaveCanvasState();
            pLine->Refresh();
        }
    }
    return pShape;
}

wxSFShapeBase* wxSFDiagramManager::CreateConnection(long srcId, long trgId, wxSFLineShape *line, bool saveState, wxSF::ERRCODE *err)
{
    wxSFShapeBase* pShape = AddShape(line, NULL, wxDefaultPosition, sfINITIALIZE, sfDONT_SAVE_STATE, err);
    if(pShape)
    {
        wxSFLineShape *pLine = (wxSFLineShape*)pShape;
        pLine->SetSrcShapeId(srcId);
        pLine->SetTrgShapeId(trgId);


        if( m_pShapeCanvas )
        {
            if(saveState)m_pShapeCanvas->SaveCanvasState();
            pLine->Refresh();
        }
    }
    return pShape;
}


void wxSFDiagramManager::RemoveShape(wxSFShapeBase* shape, bool refresh)
{
	if(shape)
	{
	    wxSFShapeBase *pParent = shape->GetParentShape();

        // remove connected lines (to all children)
        ShapeList lstChildren;
        ShapeList lstConnections;
        ShapeList lstRemovedConnections;

        // get all shape's children
        shape->GetChildShapes(sfANY, lstChildren, sfRECURSIVE);
        lstChildren.Append(shape);

        // retrieve all assigned lines
        ShapeList::compatibility_iterator snode = lstChildren.GetFirst();
        while(snode)
        {
            GetAssignedConnections(snode->GetData(), CLASSINFO(wxSFLineShape), wxSFShapeBase::lineBOTH, lstConnections);
            snode = snode->GetNext();
        }

		// remove all assigne lines
		ShapeList::compatibility_iterator node = lstConnections.GetFirst();
		while(node)
		{
		    // one connection may be used by the parent and also by his child
		    if(lstRemovedConnections.IndexOf(node->GetData()) == wxNOT_FOUND)
		    {
		        lstRemovedConnections.Append(node->GetData());
                RemoveShape(node->GetData(), false);
		    }
			node = node->GetNext();
		}

		// remove the shape also from m_lstCurrentShapes list
		if( m_pShapeCanvas ) m_pShapeCanvas->RemoveFromTemporaries( shape );
		
        // remove the shape
		RemoveItem(shape);
		
		m_fIsModified = true;
		
        if( pParent ) pParent->Update();

		if( refresh && m_pShapeCanvas ) m_pShapeCanvas->Refresh(false);
	}
}

void wxSFDiagramManager::RemoveShapes(const ShapeList& selection)
{
    wxSFShapeBase* pShape;
	ShapeList::compatibility_iterator node = selection.GetFirst();
	while(node)
	{
	    pShape = node->GetData();
	    // it is important to check whether double-linked shapes already exist before
	    // they are deleted
	    if(Contains(pShape))RemoveShape(pShape, false);
		node = node->GetNext();
	}
}

void wxSFDiagramManager::Clear()
{
	RemoveAll();

    if( m_pShapeCanvas )
    {
        m_pShapeCanvas->GetMultiselectionBox().Show(false);
        m_pShapeCanvas->UpdateVirtualSize();
    }
}

//----------------------------------------------------------------------------------//
// Serialization/deserialization functions
//----------------------------------------------------------------------------------//

bool wxSFDiagramManager::SerializeToXml(const wxString& file, bool withroot)
{
	m_fIsModified = false;
    return wxXmlSerializer::SerializeToXml(file, withroot);
}

bool wxSFDiagramManager::SerializeToXml(wxOutputStream& outstream, bool withroot)
{
	m_fIsModified = false;
    return wxXmlSerializer::SerializeToXml(outstream, withroot);
}

bool wxSFDiagramManager::DeserializeFromXml(const wxString& file)
{
	bool fSuccess = false;
	
	wxFileInputStream instream(file);
	if(instream.IsOk())
	{
        if( m_pShapeCanvas) m_pShapeCanvas->ClearCanvasHistory();

		fSuccess = DeserializeFromXml(instream);

        if( m_pShapeCanvas) m_pShapeCanvas->SaveCanvasState();
	}
	else
		wxMessageBox(wxT("Unable to initialize input stream."), wxT("ShapeFramework"), wxOK | wxICON_ERROR);

	return fSuccess;
}

bool wxSFDiagramManager::DeserializeFromXml(wxInputStream& instream)
{
	// load an XML file
	try
	{
		wxXmlDocument xmlDoc;
		xmlDoc.Load(instream);

		wxXmlNode* root = xmlDoc.GetRoot();
		if(root && (root->GetName() == wxT("chart")))
		{
			// read shape objects from XML recursively
			DeserializeObjects(NULL, root);
			m_fIsModified = false;
			return true;
		}
		else
			wxMessageBox(wxT("Unknown file format."), wxT("ShapeFramework"), wxOK | wxICON_WARNING);
	}
	catch (...)
	{
		wxMessageBox(wxT("Unable to load XML file."), wxT("ShapeFramework"), wxOK | wxICON_ERROR);
	}
	
	return false;
}

void wxSFDiagramManager::DeserializeObjects(xsSerializable* parent, wxXmlNode* node)
{
    _DeserializeObjects(parent, node);

    // update IDs in connection lines and grids
    UpdateConnections();
	UpdateGrids();
	
	m_lstIDPairs.Clear();
	
    if( m_pShapeCanvas )
    {
        //m_pShapeCanvas->MoveShapesFromNegatives();
        m_pShapeCanvas->UpdateVirtualSize();
    }
}

void wxSFDiagramManager::_DeserializeObjects(xsSerializable* parent, wxXmlNode* node)
{
	wxSFShapeBase *pShape;
	
	wxXS::IntArray arrNewIDs;
	SerializableList lstForUpdate;

	wxXmlNode* shapeNode = node->GetChildren();
	while(shapeNode)
	{
		if(shapeNode->GetName() == wxT("object"))
		{
#if wxVERSION_NUMBER < 2900
			pShape = AddShape((wxSFShapeBase*)wxCreateDynamicObject(shapeNode->GetPropVal(wxT("type"), wxT(""))), parent, wxPoint(0, 0), true, sfDONT_SAVE_STATE);
#else
			pShape = AddShape((wxSFShapeBase*)wxCreateDynamicObject(shapeNode->GetAttribute(wxT("type"), wxT(""))), parent, wxPoint(0, 0), true, sfDONT_SAVE_STATE);
#endif
			if(pShape)
			{
				// store new assigned ID
				lstForUpdate.Append( pShape );
				pShape->GetChildrenRecursively( NULL, lstForUpdate );
				
				for( SerializableList::iterator it = lstForUpdate.begin(); it != lstForUpdate.end(); ++it )
				{
					arrNewIDs.Add( (*it)->GetId() );
				}
				
				// deserialize stored content
				pShape->DeserializeObject(shapeNode);

				// update handle in line shapes
				if( pShape->IsKindOf( CLASSINFO(wxSFLineShape) ) )
				{
					pShape->CreateHandles();
					m_lstLinesForUpdate.Append(pShape);
				}
				else if( pShape->IsKindOf( CLASSINFO(wxSFGridShape) ) )
				{
					m_lstGridsForUpdate.Append(pShape);
				}

				// store information about IDs' changes and re-assign shapes' IDs
				int newId, i = 0;
				for( SerializableList::iterator it = lstForUpdate.begin(); it != lstForUpdate.end(); ++it )
				{
					newId = arrNewIDs[i++];
					if( newId != (*it)->GetId() )
					{
						m_lstIDPairs.Append( new IDPair((*it)->GetId(), newId) );
						(*it)->SetId( newId );
					}
				}

				// deserialize child objects
				_DeserializeObjects(pShape, shapeNode);
				
				arrNewIDs.Clear();
				lstForUpdate.Clear();
			}
			else
			{
				// there are some unsupported shapes so the diagrams must be cleared because of possible damage
				RemoveAll();
				m_lstLinesForUpdate.Clear();
				m_lstGridsForUpdate.Clear();
				
				wxMessageBox( wxT("Deserialization couldn't be completed because not of all shapes are accepted."), wxT("wxShapeFramework"), wxOK | wxICON_WARNING );
				return;
			}
		}
		else if(shapeNode->GetName() == m_sRootName + wxT("_properties"))
		{
		    m_pRoot->DeserializeObject(shapeNode->GetChildren());
		}
		shapeNode = shapeNode->GetNext();
	}
}

//----------------------------------------------------------------------------------//
// Shape handling functions
//----------------------------------------------------------------------------------//

void wxSFDiagramManager::AcceptShape(const wxString& type)
{
    if(m_arrAcceptedShapes.Index(type) == wxNOT_FOUND)
    {
        m_arrAcceptedShapes.Add(type);
    }
}

bool wxSFDiagramManager::IsShapeAccepted(const wxString& type)
{
    if( m_arrAcceptedShapes.Index(type) != wxNOT_FOUND )return true;
    else if( m_arrAcceptedShapes.Index(wxT("All")) != wxNOT_FOUND )return true;
    else
        return false;
}

void wxSFDiagramManager::AcceptTopShape(const wxString& type)
{
    if(m_arrAcceptedTopShapes.Index(type) == wxNOT_FOUND)
    {
        m_arrAcceptedTopShapes.Add(type);
    }
}

bool wxSFDiagramManager::IsTopShapeAccepted(const wxString& type)
{
    if( m_arrAcceptedTopShapes.Index(type) != wxNOT_FOUND )return true;
    else if( m_arrAcceptedTopShapes.Index(wxT("All")) != wxNOT_FOUND )return true;
    else
        return false;
}

void wxSFDiagramManager::GetAssignedConnections(wxSFShapeBase* parent, wxClassInfo* shapeInfo, wxSFShapeBase::CONNECTMODE mode, ShapeList& lines)
{
	wxSFLineShape* pLine;
	
	if( parent->GetId() == -1 ) return;

	SerializableList lstLines;
	// lines are children of root item only so we have not to search recursively...
	GetRootItem()->GetChildren( shapeInfo, lstLines );
	
	if( !lstLines.IsEmpty() )
    {
        SerializableList::compatibility_iterator node = lstLines.GetFirst();
        while(node)
        {
            pLine = (wxSFLineShape*)node->GetData();
            switch(mode)
            {
                case wxSFShapeBase::lineSTARTING:
                    if( pLine->GetSrcShapeId() == parent->GetId() ) lines.Append(pLine);
                    break;

                case wxSFShapeBase::lineENDING:
                    if( pLine->GetTrgShapeId() == parent->GetId() ) lines.Append(pLine);
                    break;

                case wxSFShapeBase::lineBOTH:
                    if( ( pLine->GetSrcShapeId() == parent->GetId() ) ||
					    ( pLine->GetTrgShapeId() == parent->GetId() ) ) lines.Append(pLine);
                    break;
            }
            node = node->GetNext();
        }
    }
}

void wxSFDiagramManager::GetShapes(wxClassInfo* shapeInfo, ShapeList& shapes, xsSerializable::SEARCHMODE mode)
{
    GetItems(shapeInfo, (SerializableList&)shapes, mode);
}

wxSFShapeBase* wxSFDiagramManager::GetShapeAtPosition(const wxPoint& pos, int zorder, SEARCHMODE mode)
{
	int nCounter = 0;
	ShapeList m_lstSortedShapes;
	wxSFShapeBase* pShape;

    // sort shapes list in the way that the line shapes will be at the top of the list
    ShapeList shapes;
    GetShapes(CLASSINFO(wxSFShapeBase), shapes, xsSerializable::searchBFS);

	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
	    pShape = node->GetData();
	    if(pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
	    {
	        m_lstSortedShapes.Insert(pShape);
	        nCounter++;
	    }
	    else
            m_lstSortedShapes.Insert(nCounter, pShape);

        node = node->GetNext();
	}

    // find the topmost shape according to the given rules
    nCounter = 1;
	node = m_lstSortedShapes.GetFirst();
	while(node)
	{
		pShape = (wxSFShapeBase*)node->GetData();
		if(pShape->IsVisible() && pShape->IsActive() && pShape->Contains(pos))
		{
			switch(mode)
			{
			case searchSELECTED:
				if(pShape->IsSelected())
				{
					if(nCounter == zorder)return pShape;
					else
						nCounter++;
				}
				break;

			case searchUNSELECTED:
				if(!pShape->IsSelected())
				{
					if(nCounter == zorder)return pShape;
					else
						nCounter++;
				}
				break;

			case searchBOTH:
				if(nCounter == zorder)return pShape;
				else
					nCounter++;
				break;
			}
		}
		node = node->GetNext();
	}

	return NULL;
}

void wxSFDiagramManager::GetShapesAtPosition(const wxPoint& pos, ShapeList& shapes)
{
	shapes.Clear();
	wxSFShapeBase *pShape;

    ShapeList lstShapes;
    GetShapes(CLASSINFO(wxSFShapeBase), lstShapes);

	ShapeList::compatibility_iterator node = lstShapes.GetFirst();
	while(node)
	{
		pShape = node->GetData();
		if(pShape->IsVisible() && pShape->IsActive() && pShape->Contains(pos))shapes.Append(pShape);
		node = node->GetNext();
	}
}

void wxSFDiagramManager::GetShapesInside(const wxRect& rct, ShapeList& shapes)
{
	shapes.Clear();
	wxSFShapeBase* pShape;

    ShapeList lstShapes;
    GetShapes(CLASSINFO(wxSFShapeBase), lstShapes);

	ShapeList::compatibility_iterator node = lstShapes.GetFirst();
	while(node)
	{
		pShape = node->GetData();
		if(pShape->IsVisible() && pShape->IsActive() && pShape->Intersects(rct))shapes.Append(pShape);
		node = node->GetNext();
	}
}

wxSFShapeBase* wxSFDiagramManager::FindShape(long id)
{
    if(id == -1)return NULL;
    else
        return (wxSFShapeBase*)GetItem(id);
}

void wxSFDiagramManager::GetNeighbours(wxSFShapeBase* parent, ShapeList& neighbours, wxClassInfo *shapeInfo, wxSFShapeBase::CONNECTMODE condir, bool direct)
{
    if(parent)
    {
        parent->GetNeighbours(neighbours, shapeInfo, condir, direct);
    }
    else
    {
        wxASSERT(GetRootItem());

		wxSFShapeBase* pShape;

		SerializableList::compatibility_iterator node = GetRootItem()->GetFirstChildNode();
		while(node)
		{
			pShape = (wxSFShapeBase*)node->GetData();
            pShape->GetNeighbours(neighbours, shapeInfo, condir, direct);
			node = node->GetNext();
		}
    }
}

bool wxSFDiagramManager::HasChildren(wxSFShapeBase* parent)
{
	if(parent->GetFirstChildNode())return true;
	else
        return false;
}

void wxSFDiagramManager::UpdateConnections()
{
	if( !m_lstLinesForUpdate.IsEmpty() )
	{		
		wxSFLineShape* pLine;
		IDPair* pIDPair;
	
        // now check ids
		long oldSrcId, oldTrgId;
		long newSrcId, newTrgId;
		IDList::compatibility_iterator idnode;
		
		ShapeList::compatibility_iterator node = m_lstLinesForUpdate.GetFirst();
		while(node)
        {
			pLine = (wxSFLineShape*)node->GetData();
			newSrcId = oldSrcId = pLine->GetSrcShapeId();
			newTrgId = oldTrgId = pLine->GetTrgShapeId();
			idnode = m_lstIDPairs.GetFirst();
			while(idnode)
            {
				pIDPair = idnode->GetData();
				/*if(pIDPair->m_nNewID != pIDPair->m_nOldID)
				{*/
				if(oldSrcId == pIDPair->m_nOldID) newSrcId = pIDPair->m_nNewID;
				if(oldTrgId == pIDPair->m_nOldID) newTrgId = pIDPair->m_nNewID;
				/*}*/
				idnode = idnode->GetNext();
			}
			pLine->SetSrcShapeId(newSrcId);
			pLine->SetTrgShapeId(newTrgId);
			
			// check whether line's src and trg shapes really exists
			if(!GetItem(pLine->GetSrcShapeId()) || !GetItem(pLine->GetTrgShapeId()))
            {
                RemoveItem(pLine);
            }
			
			node = node->GetNext();
		}
		
		m_lstLinesForUpdate.Clear();
    }
}

void wxSFDiagramManager::UpdateGrids()
{
	if( !m_lstGridsForUpdate.IsEmpty() )
	{		
        // now check ids
		wxSFGridShape* pGrid;
		IDPair* pIDPair;
		int nIndex;
		IDList::compatibility_iterator idnode;
		
		ShapeList::compatibility_iterator node = m_lstGridsForUpdate.GetFirst();
		while(node)
        {
			pGrid = (wxSFGridShape*)node->GetData();
			nIndex = wxNOT_FOUND;
			
			idnode = m_lstIDPairs.GetFirst();
			while(idnode)
            {
				pIDPair = idnode->GetData();
				
				nIndex = pGrid->m_arrCells.Index( pIDPair->m_nOldID );
				if( nIndex != wxNOT_FOUND )	pGrid->m_arrCells[ nIndex ] = pIDPair->m_nNewID;
				
				idnode = idnode->GetNext();
			}
			
			// check whether grid's children really exists
			for( size_t i = 0; i < pGrid->m_arrCells.GetCount(); )
			{
				if( !GetItem( pGrid->m_arrCells[i] ) ) pGrid->RemoveFromGrid( pGrid->m_arrCells[i] );
				else
					i++;
			}
			
			node = node->GetNext();
		}
		
		m_lstGridsForUpdate.Clear();	
    }
}

void wxSFDiagramManager::UpdateAll()
{
	wxSFShapeBase *pShape;
	
	ShapeList lstShapes;
	GetShapes( CLASSINFO(wxSFShapeBase), lstShapes );
	
	ShapeList::compatibility_iterator node = lstShapes.GetFirst();
	while( node )
	{
		pShape = node->GetData();
		// update only shapes withour children because the Update() function is called recursively on all parents
		if( !HasChildren( pShape ) ) pShape->Update();
		
		node = node->GetNext();
	}
}

void wxSFDiagramManager::MoveShapesFromNegatives()
{
	wxSFShapeBase *pShape;
	wxRealPoint shapePos;
	double minx = 0, miny = 0;

	// find the maximal negative position value
    ShapeList shapes;
    GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	ShapeList::compatibility_iterator node = shapes.GetFirst();
	while(node)
	{
		shapePos = node->GetData()->GetAbsolutePosition();

		if(node == shapes.GetFirst())
		{
			minx = shapePos.x;
			miny = shapePos.y;
		}
		else
		{
            if(shapePos.x < minx)minx = shapePos.x;
            if(shapePos.y < miny)miny = shapePos.y;
		}

		node = node->GetNext();
	}

	// move all parents shape so they (and their children) will be located in the positive values only
	if((minx < 0) || (miny < 0))
	{
		node = shapes.GetFirst();
		while(node)
		{
			pShape = node->GetData();

			if(pShape->GetParentShape() == NULL)
			{
				if(minx < 0)pShape->MoveBy(abs((int)minx), 0);
				if(miny < 0)pShape->MoveBy(0, abs((int)miny));
			}
			node = node->GetNext();
		}
	}
}

#ifndef ERDFOREIGNKEY_H
#define ERDFOREIGNKEY_H
#include <wx/wx.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
#include <wx/wxsf/TextShape.h>
#include <wx/wxsf/CircleArrow.h>
#include <wx/wxsf/SolidArrow.h>

#include "wx/wxsf/RoundOrthoShape.h" // Base class: wxSFOrthoLineShape

#include "NArrow.h"
#include "OneArrow.h"
#include "constraint.h"
/*! \brief Class representing line in ERD diagram constructed on Foreign Key base. For more info see wxSF manual */
class ErdForeignKey : public wxSFRoundOrthoLineShape {
public:
	XS_DECLARE_CLONABLE_CLASS(ErdForeignKey);	
	ErdForeignKey();
	ErdForeignKey(const ErdForeignKey& obj);
	ErdForeignKey(Constraint* pConstraint);

	virtual ~ErdForeignKey();
	
	virtual void CreateHandles();

protected:
	Constraint* m_pConstraint;
};

#endif // ERDFOREIGNKEY_H

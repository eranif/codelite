#ifndef CONTEXT_GENERIC_H
#define CONTEXT_GENERIC_H

#include "wx/string.h"
#include "wx/wxscintilla.h"
#include "smart_ptr.h"
#include "context_base.h"

class LEditor;

/**
 * \ingroup LiteEditor
 * \brief the basic editor from which complicated editors derives from (e.g. ContextCpp)
 *
 * \version 1.0
 * first version
 *
 * \date 04-30-2007
 *
 * \author Eran
 *
 *
 */
class ContextGeneric : public ContextBase {
public:
	//---------------------------------------
	// ctors-dtor
	//---------------------------------------
	ContextGeneric(LEditor *container, const wxString &name);
	ContextGeneric() : ContextBase(wxT("Text")) {};
	ContextGeneric(const wxString &name) : ContextBase(name) {};

	virtual ~ContextGeneric();
	LEditor &GetCtrl() { return *m_container; }
	virtual ContextBase *NewInstance(LEditor *container);

	//---------------------------------------
	// Operations
	//---------------------------------------
	virtual void CompleteWord();
	virtual void CodeComplete();
	virtual void GotoDefinition();
	virtual void GotoPreviousDefintion();
	virtual void AutoIndent(const wxChar&);
	virtual void CallTipCancel(){};

	virtual bool IsCommentOrString(long WXUNUSED(pos)){ return false; }

	// event handlers
	virtual void OnDwellEnd(wxScintillaEvent & WXUNUSED(event)) {};
	virtual void OnCallTipClick(wxScintillaEvent& WXUNUSED(event)) {};
	virtual void OnDwellStart(wxScintillaEvent & WXUNUSED(event)) {};
};
#endif // CONTEXT_GENERIC_H


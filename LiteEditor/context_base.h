#ifndef CONTEXT_BASE_H
#define CONTEXT_BASE_H

#include "wx/string.h"
#include "wx/wxscintilla.h"
#include "smart_ptr.h"
#include "wx/filename.h"

class LEditor;

/**
 * \ingroup LiteEditor
 * \brief This class defines the language context of the editor
 *
 * \version 1.0
 * first version
 *
 * \date 04-30-2007
 *
 * \author Eran
 *
 */
class ContextBase : public wxEvtHandler
{
protected:
	LEditor *m_container;
	wxString m_name;

protected:
	void SetName(const wxString &name){m_name = name;}

public:

	// ctor-dtor
	ContextBase(LEditor *container);
	ContextBase(const wxString &name);
	virtual ~ContextBase();

	/**
	 * Return the context parent control
	 */
	LEditor &GetCtrl() { return *m_container; }

	/**
	 * Return the context name
	 */
	const wxString &GetName() const { return m_name; }

	virtual void AutoIndent(const wxChar&);
	// every Context derived class must implement the following methods
	virtual ContextBase *NewInstance(LEditor *container) = 0;
	virtual void CompleteWord() = 0;
	virtual void CodeComplete() = 0;
	virtual void GotoDefinition() = 0;
	virtual void GotoPreviousDefintion() = 0;
	virtual void CallTipCancel() = 0;
	virtual bool IsCommentOrString(long WXUNUSED(pos)) = 0;
	virtual void OnCallTipClick(wxScintillaEvent& WXUNUSED(event)) = 0;
	
	//a functions with default implementation:
	virtual void OnDwellEnd(wxScintillaEvent & event){event.Skip();}
	virtual void OnDbgDwellEnd(wxScintillaEvent & event){event.Skip();}
	virtual void OnDwellStart(wxScintillaEvent & event){event.Skip();}
	virtual void OnDbgDwellStart(wxScintillaEvent & event){event.Skip();}
	virtual void OnKeyDown(wxKeyEvent &event) {event.Skip();}
	virtual void AddMenuDynamicContent(wxMenu *WXUNUSED(menu)) {};
	virtual void RemoveMenuDynamicContent(wxMenu *WXUNUSED(menu)) {};
	virtual void OnSciUpdateUI(wxScintillaEvent& WXUNUSED(event)){}
	virtual void OnFileSaved(){};
	
	//override this method if you wish to provide context based right click menu
	virtual wxMenu *GetMenu() {return NULL;}
};

typedef SmartPtr<ContextBase> ContextBasePtr;
#endif // CONTEXT_BASE_H


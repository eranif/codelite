/////////////////////////////////////////////////////////////////////////////
// Name:        spellcheck.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: spellcheck.h 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#ifndef __SpellCheck__
#define __SpellCheck__
//------------------------------------------------------------
#include "plugin.h"
#include "spellcheckeroptions.h"
#include <wx/timer.h>
//------------------------------------------------------------
class IHunSpell;
class SpellCheck : public IPlugin
{
public:
	wxString GetCurrentWspPath() const { return m_currentWspPath; }

	void     SetCheckContinuous( bool value );
	bool     GetCheckContinuous() const { return m_checkContinuous; }
	bool     IsTag( const wxString& token );
	IEditor* GetEditor();
	wxMenu*  CreateSubMenu();

	SpellCheck( IManager* manager );
	~SpellCheck();

	// --------------------------------------------
	// Abstract methods
	// --------------------------------------------
	virtual clToolBar* CreateToolBar( wxWindow* parent );
	virtual void       CreatePluginMenu( wxMenu* pluginsMenu );
	virtual void       HookPopupMenu( wxMenu* menu, MenuType type );
	virtual void       UnHookPopupMenu( wxMenu* menu, MenuType type );
	virtual void       UnPlug();

	void OnSettings( wxCommandEvent& e );
	void OnCheck( wxCommandEvent& e );
	void OnContinousCheck( wxCommandEvent& e );
	void OnContextMenu( wxCommandEvent& e );
	void OnTimer( wxTimerEvent& e );
	void OnWspLoaded( wxCommandEvent& e );
	void OnWspClosed( wxCommandEvent& e );

	wxMenuItem*         m_sepItem;
	wxEvtHandler*       m_topWin;
	SpellCheckerOptions m_options;
	wxWindow* GetTopWnd() { return m_mgr->GetTheApp()->GetTopWindow(); }
	enum
	{
		IDM_BASE = 20500,
		IDM_SETTINGS
	};

protected:
	void Init();
	void LoadSettings();
	void SaveSettings();

	bool m_checkContinuous;
	IHunSpell*    m_pEngine;
	wxTimer       m_timer;
	wxString      m_currentWspPath;
	wxAuiToolBar* m_pToolbar;
	wxMenu*       m_pSubMenu;
};
//------------------------------------------------------------
#endif // SpellCheck

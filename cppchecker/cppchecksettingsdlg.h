#ifndef CPPCHECH_SETTINGS_DLG_H_INCLUDED
#define CPPCHECH_SETTINGS_DLG_H_INCLUDED

#include "cppchecksettingsdlgbase.h"
#include "cppcheck_settings.h"
class IConfigTool;

class CppCheckSettings;

class CppCheckSettingsDialog : public CppCheckSettingsDialogBase
{
	CppCheckSettings* m_settings;
	IConfigTool*      m_conf;
public:
	CppCheckSettingsDialog(wxWindow* parent, CppCheckSettings* settings, IConfigTool *conf);
	virtual ~CppCheckSettingsDialog();

protected:
	virtual void OnBtnOK       ( wxCommandEvent  &e );
	virtual void OnRemoveFileUI( wxUpdateUIEvent &e );
	virtual void OnRemoveFile  ( wxCommandEvent  &e );
	virtual void OnAddFile     ( wxCommandEvent  &e );
	virtual void OnClearList   ( wxCommandEvent  &e );
	virtual void OnClearListUI ( wxUpdateUIEvent &e );
};

#endif // CPPCHECH_SETTINGS_DLG_H_INCLUDED

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPXDebugSetupWizard.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef PHPXDEBUGSETUPWIZARD_H
#define PHPXDEBUGSETUPWIZARD_H
#include "php_ui.h"

class PHPXDebugSetupWizard : public PHPXDebugSetupWizardBase
{
public:
    PHPXDebugSetupWizard(wxWindow* parent);
    virtual ~PHPXDebugSetupWizard() = default;

protected:
    virtual void OnFinished(wxWizardEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
    void SelectAllIniText();
};
#endif // PHPXDEBUGSETUPWIZARD_H

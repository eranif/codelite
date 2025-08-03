//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : PluginWizard.h
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

#ifndef PLUGINWIZARD_H
#define PLUGINWIZARD_H

#include "gizmos_base.h"
#include "newplugindata.h"

class PluginWizard : public PluginWizardBase
{
public:
    PluginWizard(wxWindow* parent);
    virtual ~PluginWizard() = default;

    bool Run(NewPluginData& pd);
protected:
    virtual void OnProjectPathChanged(wxFileDirPickerEvent& event);
    virtual void OnFinish(wxWizardEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
};
#endif // PLUGINWIZARD_H

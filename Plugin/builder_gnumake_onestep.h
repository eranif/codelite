//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : builder_gnumake1stage.h
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
/*
 * Override of builder_gnumake builder to provide a one-step builder (based on gnu makefile)
 * compile srcs directly to exe/lib, no link step
 */
#ifndef BUILDER_GNUMAKE_ONESTEP_H
#define BUILDER_GNUMAKE_ONESTEP_H

#include "builder_gnumake.h"
#include "codelite_exports.h"
#include "workspace.h"

#include <wx/txtstrm.h>
#include <wx/wfstream.h>

class WXDLLIMPEXP_SDK BuilderGnuMakeOneStep : public BuilderGNUMakeClassic
{
public:
    BuilderGnuMakeOneStep();
    virtual ~BuilderGnuMakeOneStep();

    /*	These methods available but supplied by baseclass BuilderGnuMake
            virtual bool Export(const wxString &project, const wxString &confToBuild, bool isProjectOnly, bool force,
       wxString &errMsg);
            virtual wxString GetBuildCommand(const wxString &project, const wxString &confToBuild);
            virtual wxString GetCleanCommand(const wxString &project, const wxString &confToBuild);
            virtual wxString GetPOBuildCommand(const wxString &project, const wxString &confToBuild);
            virtual wxString GetPOCleanCommand(const wxString &project, const wxString &confToBuild);
            virtual wxString GetSingleFileCmd(const wxString &project, const wxString &confToBuild, const wxString
       &fileName);
            virtual wxString GetPreprocessFileCmd(const wxString &project, const wxString &confToBuild, const wxString
       &fileName, wxString &errMsg);
            virtual wxString GetPORebuildCommand(const wxString &project, const wxString &confToBuild);
    */
protected:
    virtual void CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text);
    virtual void CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName);
    virtual void CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);

private:
    void CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text);
};
#endif // BUILDER_GNUMAKE_ONESTEP_H

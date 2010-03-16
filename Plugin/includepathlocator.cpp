#include "includepathlocator.h"
#include <wx/dir.h>
#include <wx/utils.h>
#include "procutils.h"
#include <wx/fileconf.h>
#include "editor_config.h"

IncludePathLocator::IncludePathLocator(IManager *mgr)
		: m_mgr(mgr)
{
}

IncludePathLocator::~IncludePathLocator()
{
}

void IncludePathLocator::Locate(wxArrayString& paths, wxArrayString &excludePaths)
{
	// try to locate QMAKE
	wxFileConfig  qmakeConf(wxEmptyString, wxEmptyString, m_mgr->GetStartupDirectory() + wxT("/config/qmake.ini"));
	wxString      groupName;
	long          index(0);
	wxArrayString out;
	wxString      qmake(wxT("qmake"));

	if (qmakeConf.GetFirstGroup(groupName, index)) {
		// we got qmake configuration, use it instead of the default qmake command
		qmake = qmakeConf.Read(groupName + wxT("/qmake"));
	}

	// Run: qmake -query QT_INSTALL_PREFIX
	wxString cmd;
	cmd << qmake << wxT(" -query QT_INSTALL_PREFIX");
	ProcUtils::SafeExecuteCommand(cmd, out);

	if (out.IsEmpty() == false ) {
		wxString qt_output (out.Item(0));
		qt_output.Trim().Trim(false);
#if defined(__WXGTK__)||defined(__WXMAC__)
		wxString pathQt4, pathQt3, pathQt;
		pathQt4 << qt_output << wxFileName::GetPathSeparator() << wxT("include") << wxFileName::GetPathSeparator() << wxT("qt4");
		pathQt3 << qt_output << wxFileName::GetPathSeparator() << wxT("include") << wxFileName::GetPathSeparator() << wxT("qt3");
		pathQt  << qt_output << wxFileName::GetPathSeparator() << wxT("include");
		
		if (wxDir::Exists( pathQt4 )) {
			paths.Add( pathQt4 + wxT("/QtCore") );
			paths.Add( pathQt4 + wxT("/QtGui")  );
			paths.Add( pathQt4 + wxT("/QtXml")  );

		} else if (wxDir::Exists( pathQt3 ) ) {
			paths.Add( pathQt3 + wxT("/QtCore") );
			paths.Add( pathQt3 + wxT("/QtGui")  );
			paths.Add( pathQt3 + wxT("/QtXml")  );

		} else if (wxDir::Exists( pathQt ) ) {
			paths.Add( pathQt + wxT("/QtCore") );
			paths.Add( pathQt + wxT("/QtGui")  );
			paths.Add( pathQt + wxT("/QtXml")  );
		}
		
#else // __WXMSW__
		wxString pathWin;
		pathWin << qt_output << wxFileName::GetPathSeparator() << wxT("include") << wxFileName::GetPathSeparator();
		if (wxDir::Exists( pathWin )) {
			paths.Add( pathWin + wxT("QtCore") );
			paths.Add( pathWin + wxT("QtGui")  );
			paths.Add( pathWin + wxT("QtXml")  );
		}
#endif
	}

	// Try wxWidgets
#ifdef __WXMSW__
	// On Windows, just read the content of the WXWIN environment variable
	wxString wxwin;
	if (wxGetEnv(wxT("WX_INCL_HOME"), &wxwin)) {
		// we got the path to the installation of wxWidgets
		if (wxDir::Exists(wxwin)) {
			paths.Add( wxwin );
			excludePaths.Add( wxwin + wxT("\\univ") );
			excludePaths.Add( wxwin + wxT("\\unix") );
		}
	}

	wxString      standardIncludeBase;
	wxGetEnv(wxT("MINGW_INCL_HOME"), &standardIncludeBase);
	if (standardIncludeBase.IsEmpty() == false && wxDir::Exists(standardIncludeBase)) {
		// since we only support codelite's installation of MinGW, we know what to append
		// to the include path
		paths.Add(standardIncludeBase + wxT("\\include"));
		standardIncludeBase << wxT("\\lib\\gcc\\mingw32\\4.4.1\\include\\c++");
		paths.Add( standardIncludeBase );
		excludePaths.Add( standardIncludeBase + wxT("\\debug") );
	}
#else
	// run wx-config and parse the output
	out.Clear();
	ProcUtils::SafeExecuteCommand(wxT("wx-config --cxxflags"), out);
	if (out.IsEmpty() == false) {
		wxString line ( out.Item(0) );
		int where = line.Find(wxT(" -I"));
		while (where != wxNOT_FOUND) {
			line = line.Mid(where + 3);
			paths.Add( line.BeforeFirst(wxT(' ')) );

			where = line.Find(wxT(" -I"));
		}
	}
	wxString      standardIncludeBase(wxT("/usr/include"));
	if (standardIncludeBase.IsEmpty() == false && wxDir::Exists(standardIncludeBase)) {
		paths.Add(standardIncludeBase);

		// Linux: In addition, add the STL path whcih is under /usr/include/c++/X.X.X
		// take the highest number of the X.X.X
		wxArrayString files;
		wxArrayString dirs;
		wxString      base(standardIncludeBase + wxFileName::GetPathSeparator() + wxT("c++") + wxFileName::GetPathSeparator());

		long     highestVersion(0);
		wxString sHighestVersion;

		if (wxDir::Exists( base ) ) {
			wxDir::GetAllFiles(base, &files, wxEmptyString, wxDIR_DIRS|wxDIR_FILES);

			//filter out all non-directories
			for (size_t i=0; i<files.GetCount(); i++) {
				wxFileName fn(files.Item(i));
				wxString p = fn.GetPath().Mid( base.Length() );
				wxString tmp_p(p);
				tmp_p.Replace(wxT("."), wxT(""));
				long number(0);
				tmp_p.ToLong( &number );
				if (number && number > highestVersion) {
					sHighestVersion = p.BeforeFirst(wxFileName::GetPathSeparator());
					highestVersion  = number;
				}
			}

			if (sHighestVersion.IsEmpty() == false) {
				paths.Add( base + sHighestVersion );
				excludePaths.Add( base + sHighestVersion + wxT("/debug") );
			}
		}
	}
#endif	
}

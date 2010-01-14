#ifndef SVNSETTINGSDATA_H
#define SVNSETTINGSDATA_H

#include "serialized_object.h" // Base class
#include "custom_notebook.h"

enum SvnSettingsDataFlags {
	SvnAddFileToSvn        = 0x00000001,
	SvnRetagWorkspace      = 0x00000002,
	SvnUseExternalDiff     = 0x00000004,
	SvnExposeRevisionMacro = 0x00000008,
	SvnRenameFileInRepo    = 0x00000010
};

class SvnSettingsData : public SerializedObject
{
	wxString      m_executable;
	wxString      m_ignoreFilePattern;
	wxString      m_externalDiffViewer;
	wxString      m_sshClient;
	wxString      m_sshClientArgs;
	size_t        m_flags;
	wxArrayString m_urls;
	wxString      m_revisionMacroName;
	size_t        m_svnTabIndex;

public:
	SvnSettingsData()
			: m_executable(wxT("svn"))
			, m_ignoreFilePattern(wxT("*.o *.obj *.exe *.lib *.so *.dll *.a *.dynlib *.exp *.ilk *.pdb *.d *.tags *.suo *.ncb *.bak *.orig *.dll *.mine *.o.d *.session Debug Release DebugUnicode ReleaseUnicode"))
			, m_externalDiffViewer(wxT(""))
			, m_sshClient(wxT(""))
			, m_sshClientArgs(wxT(""))
			, m_flags(SvnAddFileToSvn|SvnRetagWorkspace)
			, m_revisionMacroName(wxT("SVN_REVISION"))
			, m_svnTabIndex(Notebook::npos) {
	}

	virtual ~SvnSettingsData() {
	}

public:
	virtual void DeSerialize(Archive &arch) {
		arch.Read(wxT("m_executable"),                m_executable);
		arch.Read(wxT("m_ignoreFilePattern"),         m_ignoreFilePattern);
		arch.Read(wxT("m_externalDiffViewer"),        m_externalDiffViewer);
		arch.Read(wxT("m_sshClient"),                 m_sshClient);
		arch.Read(wxT("m_sshClientArgs"),             m_sshClientArgs);
		arch.Read(wxT("m_flags"),                     m_flags);
		arch.Read(wxT("m_urls"),                      m_urls);
		arch.Read(wxT("m_revisionMacroName"),         m_revisionMacroName);
		arch.Read(wxT("m_svnTabIndex"),               m_svnTabIndex);
	}

	virtual void Serialize(Archive &arch) {
		arch.Write(wxT("m_executable"),                m_executable);
		arch.Write(wxT("m_ignoreFilePattern"),         m_ignoreFilePattern);
		arch.Write(wxT("m_externalDiffViewer"),        m_externalDiffViewer);
		arch.Write(wxT("m_sshClient"),                 m_sshClient);
		arch.Write(wxT("m_sshClientArgs"),             m_sshClientArgs);
		arch.Write(wxT("m_flags"),                     m_flags);
		arch.Write(wxT("m_urls"),                      m_urls);
		arch.Write(wxT("m_revisionMacroName"),         m_revisionMacroName);
		arch.Write(wxT("m_svnTabIndex"),               m_svnTabIndex);
	}

	void SetSvnTabIndex(const size_t& svnTabIndex) {
		this->m_svnTabIndex = svnTabIndex;
	}
	const size_t& GetSvnTabIndex() const {
		return m_svnTabIndex;
	}
	void SetRevisionMacroName(const wxString& revisionMacroName) {
		this->m_revisionMacroName = revisionMacroName;
	}
	const wxString& GetRevisionMacroName() const {
		return m_revisionMacroName;
	}
	void SetExecutable(const wxString& executable) {
		this->m_executable = executable;
	}
	const wxString& GetExecutable() const {
		return m_executable;
	}
	void SetExternalDiffViewer(const wxString& externalDiffViewer) {
		this->m_externalDiffViewer = externalDiffViewer;
	}
	void SetFlags(const size_t& flags) {
		this->m_flags = flags;
	}
	void SetIgnoreFilePattern(const wxString& ignoreFilePattern) {
		this->m_ignoreFilePattern = ignoreFilePattern;
	}
	void SetSshClient(const wxString& sshClient) {
		this->m_sshClient = sshClient;
	}
	void SetSshClientArgs(const wxString& sshClientArgs) {
		this->m_sshClientArgs = sshClientArgs;
	}
	const wxString& GetExternalDiffViewer() const {
		return m_externalDiffViewer;
	}
	const size_t& GetFlags() const {
		return m_flags;
	}
	const wxString& GetIgnoreFilePattern() const {
		return m_ignoreFilePattern;
	}
	const wxString& GetSshClient() const {
		return m_sshClient;
	}
	const wxString& GetSshClientArgs() const {
		return m_sshClientArgs;
	}
	void SetUrls(const wxArrayString& urls) {
		this->m_urls = urls;
	}
	const wxArrayString& GetUrls() const {
		return m_urls;
	}
};

#endif // SVNSETTINGSDATA_H

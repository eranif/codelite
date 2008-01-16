#ifndef BUILD_CONFIG_SETTINGS_H
#define BUILD_CONFIG_SETTINGS_H

#include "wx/string.h"
#include "singleton.h"
#include "compiler.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"
#include "build_system.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

// Cookie class for the editor to provide reentrance operations
// on various methods (such as iteration)
class WXDLLIMPEXP_LE_SDK BuildSettingsConfigCookie {
public:
	wxXmlNode *child;	
	wxXmlNode *parent;

public:
	BuildSettingsConfigCookie() : child(NULL), parent(NULL) {}
	~BuildSettingsConfigCookie() {}
};

/**
 * \class BuildSettingsConfig the build system configuration
 */
class WXDLLIMPEXP_LE_SDK BuildSettingsConfig 
{
	wxXmlDocument *m_doc;
	wxFileName m_fileName;
protected:
	wxXmlNode* GetCompilerNode(const wxString& name) const;

public:
	BuildSettingsConfig();
	virtual ~BuildSettingsConfig();

	/**
	 * Load the configuration file
	 */
	bool Load();

	/**
	 * Set or update a given compiler using its name as the index
	 */
	void SetCompiler(CompilerPtr cmp);

	/**
	 * Find and return compiler by name
	 */
	CompilerPtr GetCompiler(const wxString &name) const;

	/**
	 * Returns the first compiler found.
	 * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is necessary for the library to make these functions reentrant 
	 * (i.e. allow more than one enumeration on one and the same object simultaneously).
	 */
	CompilerPtr GetFirstCompiler(BuildSettingsConfigCookie &cookie);

	/**
	 * Returns the next compiler.
	 * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is necessary for the library to make these functions reentrant 
	 * (i.e. allow more than one enumeration on one and the same object simultaneously).
	 */
	CompilerPtr GetNextCompiler(BuildSettingsConfigCookie &cookie);
	
	/**
	 * check whether a compiler with a given name already exist
	 */
	bool IsCompilerExist(const wxString &name) const;

	/**
	 * delete compiler
	 */
	void DeleteCompiler(const wxString &name);

	/** 
	 * Add build system 
	 */
	void SetBuildSystem(BuildSystemPtr bs);

	/** 
	 * get build system from configuration by name
	 */ 
	BuildSystemPtr GetBuildSystem(const wxString &name);
};

typedef Singleton<BuildSettingsConfig> BuildSettingsConfigST;

#endif //BUILD_CONFIG_SETTINGS_H

#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include <map>
#include <list>
#include "wx/string.h"
#include "singleton.h"
#include "builder.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

//class BuildManager;

/**
 * \ingroup SDK
 * The build manager class which contains all the builders currently
 * available to the editor
 *
 * Each and every call to this class is protected by a critical section to make this
 * class safe when used inside the compiler thread
 *
 * \version 1.0
 * first version
 *
 * \date 05-19-2007
 *
 * \author Eran
 *
 */
class WXDLLIMPEXP_LE_SDK BuildManager {

	friend class Singleton<BuildManager>;
	std::map<wxString, BuilderPtr> m_builders;
	wxCriticalSection m_cs;

public:
	typedef std::map<wxString, BuilderPtr>::const_iterator ConstIterator;

private:
	BuildManager();
	virtual ~BuildManager();

public:
	void AddBuilder(BuilderPtr builder);
	void RemoveBuilder(const wxString &name);
	void GetBuilders(std::list<wxString> &list);
	BuilderPtr GetBuilder(const wxString &name);
	BuilderPtr GetSelectedBuilder();
};

typedef Singleton<BuildManager> BuildManagerST;

#endif // BUILDMANAGER_H


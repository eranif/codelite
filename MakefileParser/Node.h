#ifndef MAKEFILENODE_H_INCLUDED
#define MAKEFILENODE_H_INCLUDED

#include "wx/string.h"
#include "wx/arrstr.h"

#include <vector>
#include <map>
class Target;

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif // WXDLLIMPEXP_LE_SDK

class WXDLLIMPEXP_LE_SDK MakefileNode
{
	public:
		void addNodes(std::map<wxString, Target*>& nodes);
		wxString toString(wxString prefix = wxEmptyString);
		
		MakefileNode(MakefileNode* parent, Target* firstTarget);
		
		Target* getTarget();
	
	private:
		enum TYPE {
			SOURCE_FILE,
			HEADER_FILE,
			IGNORED_FILE,
			PROJECT,
		};
		
		MakefileNode* m_parent;
		Target* m_target;
		std::vector<MakefileNode*> m_children;
		
		wxArrayString m_dependees;
		wxArrayString m_extentions;
};

typedef std::vector<MakefileNode*> MakefileNodes;

#endif // MAKEFILENODE_H_INCLUDED

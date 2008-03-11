#ifndef __svniconrefreshhandler__
#define __svniconrefreshhandler__

class IManager;
class SubversionPlugin;

class SvnIconRefreshHandler {
	IManager *m_mgr;
	SubversionPlugin *m_plugin;
public:
	SvnIconRefreshHandler(IManager *mgr, SubversionPlugin *plugin);
	~SvnIconRefreshHandler();
	void UpdateIcons();
};
#endif // __svniconrefreshhandler__

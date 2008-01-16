#ifndef DEBUGGERCONFIGTOOL_H
#define DEBUGGERCONFIGTOOL_H

#include "configtool.h"

class DebuggerConfigTool : public ConfigTool {
private:
	DebuggerConfigTool();
public:	
	~DebuggerConfigTool();
	static DebuggerConfigTool *Get();
};
#endif //DEBUGGERCONFIGTOOL_H

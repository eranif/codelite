#include "debuggerconfigtool.h"

DebuggerConfigTool::DebuggerConfigTool()
{
}

DebuggerConfigTool::~DebuggerConfigTool()
{
}

DebuggerConfigTool *DebuggerConfigTool::Get()
{
	static DebuggerConfigTool theTool;
	return &theTool;
}

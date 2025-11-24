//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : continuousbuildconf.h
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

#ifndef __continuousbuildconf__
#define __continuousbuildconf__

#include "serialized_object.h"

class ContinuousBuildConf : public SerializedObject
{
	bool m_enabled;
	size_t m_parallelProcesses;

public:
	ContinuousBuildConf();
	virtual ~ContinuousBuildConf() = default;

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	void SetEnabled(bool enabled) { this->m_enabled = enabled; }
	void SetParallelProcesses(size_t parallelProcesses) { this->m_parallelProcesses = parallelProcesses; }
	bool GetEnabled() const { return m_enabled; }
	size_t GetParallelProcesses() const { return m_parallelProcesses; }
};
#endif // __continuousbuildconf__

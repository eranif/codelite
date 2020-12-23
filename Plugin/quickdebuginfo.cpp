//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickdebuginfo.cpp
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

#include "quickdebuginfo.h"

#define ARCHIVE_WRITE(arg) arch.Write(#arg, arg)
#define ARCHIVE_READ(arg) arch.Read(#arg, arg)

QuickDebugInfo::QuickDebugInfo()
    : m_selectedDbg(0)
{
}

QuickDebugInfo::~QuickDebugInfo() {}

void QuickDebugInfo::DeSerialize(Archive& arch)
{
    ARCHIVE_READ(m_arguments);
    ARCHIVE_READ(m_exeFilepaths);
    ARCHIVE_READ(m_selectedDbg);
    ARCHIVE_READ(m_startCmds);
    ARCHIVE_READ(m_wds);
    ARCHIVE_READ(m_alternateDebuggerExec);

    // SSH related
    ARCHIVE_READ(m_debugOverSSH);
    ARCHIVE_READ(m_sshAccount);
    ARCHIVE_READ(m_remoteExe);
    ARCHIVE_READ(m_remoteDebugger);
    ARCHIVE_READ(m_remoteWD);
    ARCHIVE_READ(m_remoteStartCmds);
    ARCHIVE_READ(m_remoteArgs);
}

void QuickDebugInfo::Serialize(Archive& arch)
{
    ARCHIVE_WRITE(m_arguments);
    ARCHIVE_WRITE(m_exeFilepaths);
    ARCHIVE_WRITE(m_selectedDbg);
    ARCHIVE_WRITE(m_startCmds);
    ARCHIVE_WRITE(m_wds);
    ARCHIVE_WRITE(m_alternateDebuggerExec);
    ARCHIVE_WRITE(m_debugOverSSH);
    ARCHIVE_WRITE(m_sshAccount);

    // SSH related
    ARCHIVE_WRITE(m_debugOverSSH);
    ARCHIVE_WRITE(m_sshAccount);
    ARCHIVE_WRITE(m_remoteExe);
    ARCHIVE_WRITE(m_remoteDebugger);
    ARCHIVE_WRITE(m_remoteWD);
    ARCHIVE_WRITE(m_remoteStartCmds);
    ARCHIVE_WRITE(m_remoteArgs);
}

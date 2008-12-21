//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : volumelocatorthread.cpp
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
#ifdef __WXMSW__


#include "volumelocatorthread.h"
#include "wx/volume.h"

#if wxUSE_FSVOLUME

VolumeLocatorThread::VolumeLocatorThread(wxEvtHandler *owner)
: wxThread(wxTHREAD_JOINABLE)
, m_owner(owner)
{
}

VolumeLocatorThread::~VolumeLocatorThread()
{
}

void *VolumeLocatorThread::Entry()
{
	//Locate all volumes in the system
	//when done, send an event to the owner
	//and terminate
	wxArrayString volumnes = wxFSVolume::GetVolumes();

	//for convinience, create the result as a ';' separated
	//string and send an event to the main thread
	wxString result;
	for(size_t i=0; i<volumnes.GetCount(); i++){
		result << volumnes.Item(i);
		result << wxT(";");
	}

	wxCommandEvent e(wxEVT_THREAD_VOLUME_COMPLETED);
	e.SetString(result);
	m_owner->AddPendingEvent(e);
	return NULL;
}

#endif // wxUSE_FSVOLUME
#endif // MSW


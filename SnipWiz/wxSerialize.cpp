//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : wxSerialize.cpp              
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

//---------------------------------------------------------------------------
// $RCSfile: wxSerialize.cpp $
// $Source: src/wxSerialize.cpp $
// $Revision: 1.16 $
// $Date: 7-sep-2007 11:29:08 $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
//              Christian Buhtz
// Copyright:   (c) Jorgen Bodde
// License:     see LICENSE for details
//---------------------------------------------------------------------------

#undef wxUSE_APPLE_IEEE

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/log.h>
#include "wxSerialize.h"

wxSerialize::wxSerialize(wxInputStream &stream, size_t version, const wxString &header, bool partialMode)
	: m_partialMode(partialMode)
	, m_writeMode(false)
	, m_odstr(m_otmp)
	, m_idstr(stream)
	, m_otmp(&m_tmpostr)
	, m_itmp(m_tmpistr)
{
    InitAll();

    // all ok, use this stream
    if(stream.IsOk())
    {
	    // now we need to reset the code for
	    // reading to work
	    m_errorCode = wxSERIALIZE_ERR_OK;
		m_opened = true;

	    // load header
	    wxString hdr = LoadString();

	    if(IsOk())
	    {
	        // when we have a header to check else ignore and store
	        // for future reference (and writing back maybe?)
	        if(header.IsEmpty() || header.IsSameAs(hdr))
	        {
	            // store header for consulting later
	            m_headerStr = header;

                // check the version number
                size_t ver = LoadUint32();

                if(IsOk())
                {
                    // if version is 0, we accept any version if higher, we
                    // need to see if this stream does not exceed the expected version
                    if(!version || ver <=version)
                    {
                        // this is the point where all is
                        // approved. We can start reading
                        m_version = ver;
						m_status = wxSerializeStatus(m_version, m_headerStr);
                    }
                    else
                    {
                        wxString v1, v2;
                        v1 << version;
                        v2 << ver;

                        LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_WRONGVERSION_s1_s2, v1, v2);
                    }
                }
                else
                    LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_NOVERSION);
	        }
	        else
	            LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_HEADER_s1_s2, header, hdr);
	    }
	    else
	        LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_NOHEADER_s1, header);
	}
	else
	    LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_BADISTREAM);
}

wxSerialize::wxSerialize(wxOutputStream &stream, size_t version, const wxString &header, bool partialMode)
	: m_headerStr(header)
	, m_version(version)
	, m_partialMode(partialMode)
	, m_writeMode(true)
	, m_odstr(stream)
	, m_idstr(m_itmp)
	, m_otmp(&m_tmpostr)
	, m_itmp(m_tmpistr)
{
    InitAll();

    // all ok, use this stream
    if(stream.IsOk())
    {
    	m_opened = true;
		m_errorCode = wxSERIALIZE_ERR_OK;

		// write information
	    SaveString(header);
	    SaveUint32(version);

		m_status = wxSerializeStatus(version, header);

	    // when we are ok, start saving
	    if(!IsOk())
    	    LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_NOVERHDR);
	}
	else
	    LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_BADOSTREAM);
}

void wxSerialize::InitAll()
{
    m_opened = false;
    m_errorCode = wxSERIALIZE_ERR_ILL;
	m_objectLevel = 0;
	m_haveBoundary = false;
}

wxSerialize::~wxSerialize()
{
    Close();
}

bool wxSerialize::Eof()
{
    // only when we are input (reading) we have
    // something to tell about EOF
    if(!m_writeMode)
        return m_idstr.Eof();

    return false;
}

bool wxSerialize::CanStore()
{
    // are we in an ok state?
    if(m_writeMode)
    {
        if(IsOpen())
            return (m_errorCode == wxSERIALIZE_ERR_OK);
    }
    else
        LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_NOREAD);

    return false;
}

bool wxSerialize::CanLoad()
{
    // are we in an ok state?
    if(!m_writeMode)
    {
        if(IsOpen())
        {
            if(!Eof())
                return (m_errorCode == wxSERIALIZE_ERR_OK);
            else
                LogError(wxSERIALIZE_ERR_EOF, wxSERIALIZE_ERR_STR_EOF);
        }
    }
    else
        LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_NOWRITE);

    return false;
}

bool wxSerialize::EnterObject()
{
	// increments the level. This will also mean
	// that with reading we expect to read this level. We skip all
	// headers until we get this level.

	if(IsOk())
	{
		if(!m_writeMode)
		{
			if(CanLoad())
			{
				m_objectLevel ++;
				FindCurrentEnterLevel();
			}
			else
				return false;	// we did not enter
		}
		else
		{
			if(CanStore())
			{
				m_objectLevel++;
				SaveChar(wxSERIALIZE_HDR_ENTER);
			}
			else
				return false;	// we did not enter
		}
	}

	return IsOk();
}

bool wxSerialize::LeaveObject()
{
	// increments the level. This will also mean
	// that with reading we expect to read this level. We skip all
	// headers until we get this level.

	if(IsOk())
	{
		if(!m_writeMode)
		{
			if(CanLoad())
			{
				m_objectLevel --;
				if(m_objectLevel < 0)
					LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_ILL_LEVEL);
				else
					FindCurrentLeaveLevel();
			}
			else
				return false;	// we did not enter
		}
		else
		{
			if(CanStore())
			{
				m_objectLevel--;
				if(m_objectLevel < 0)
					LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_ILL_LEVEL);
				else
					SaveChar(wxSERIALIZE_HDR_LEAVE);
			}
			else
				return false;	// we did not enter
		}
	}

	return IsOk();
}

void wxSerialize::FindCurrentEnterLevel()
{
	// the next read should be the begin marker. If not, we search for the
	// begin marker

	if(m_haveBoundary && m_lastBoundary == wxSERIALIZE_HDR_ENTER)
	{
		m_haveBoundary = false;
		return;
	}

	wxUint8 hdr = LoadChar();	// we do not care about the header
	while(IsOk() && hdr != wxSERIALIZE_HDR_ENTER)
	{
		// here we have data loss, as we need to look for our marker
		m_status.SetNewDataLoss();

		// we should find the enter level, not leave
		if(hdr == wxSERIALIZE_HDR_LEAVE)
			LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_ILL_LEAVE);

		SkipData(hdr);
		hdr = LoadChar();
	}
}

void wxSerialize::FindCurrentLeaveLevel()
{
	bool firstHdr = true;
	unsigned char hdr = 0;
	// the next read should be the leave marker. If not, we search for the
	// leave marker that belongs to our current state (this means skipping
	// data that we cannot read, and potential enter and leave markers
	// that we do not read anyway

	int foundLevel = 1;
	if(m_haveBoundary)
	{
		// reset the boundary scan
		m_haveBoundary = false;

		// determine what to do
		if(m_lastBoundary == wxSERIALIZE_HDR_ENTER)
			foundLevel++;
		else if(m_lastBoundary == wxSERIALIZE_HDR_LEAVE)
			return;
	}

	while(IsOk() && foundLevel > 0)
	{
		if(hdr == wxSERIALIZE_HDR_ENTER)
			foundLevel++;
		else if(hdr == wxSERIALIZE_HDR_LEAVE)
		{
			foundLevel--;
			if(foundLevel < 0)
			{
				LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_ILL_LEAVE);
				return;
			}
		}

		if(foundLevel > 0)
		{
			hdr = LoadChar();

			// here we have data loss, as we need to look for our marker
			// the first header should have been the proper marker (if in sync)
			if(!firstHdr)
				m_status.SetNewDataLoss();

			SkipData(hdr);
		}

		firstHdr = false;
	}
}


void wxSerialize::SkipData(wxUint8 hdr)
{
	switch(hdr)
	{
	case wxSERIALIZE_HDR_BOOL:
		LoadBool();
		break;

	case wxSERIALIZE_HDR_INT8:
		LoadChar();
		break;

	case wxSERIALIZE_HDR_INT16:
		LoadUint16();
		break;

	case wxSERIALIZE_HDR_INT32:
		LoadUint32();
		break;

	case wxSERIALIZE_HDR_INT64:
		LoadUint64();
		break;

	case wxSERIALIZE_HDR_DOUBLE:
		LoadDouble();
		break;

	case wxSERIALIZE_HDR_STRING:
		LoadString();
		break;

	case wxSERIALIZE_HDR_ARRSTRING:
		LoadArrayString();
		break;

	case wxSERIALIZE_HDR_RECORD:
		{
			wxMemoryBuffer buf;
			Load(buf);
		}
		break;

	case wxSERIALIZE_HDR_INT:
		LoadInt();
		break;

    case wxSERIALIZE_HDR_INTINT:
        int val1, val2;
        LoadIntInt(val1, val2);
        break;

    case wxSERIALIZE_HDR_DATETIME:
        LoadDateTime();
        break;

	case wxSERIALIZE_HDR_ENTER:
		break;

	case wxSERIALIZE_HDR_LEAVE:
		break;
	default:
		LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_ILL_UNKNOWN_HDR_s1, GetHeaderName(hdr));
		break;
	}
}

bool wxSerialize::ReadBool(bool& value)
{
    // load boolean value
    if(LoadChunkHeader(wxSERIALIZE_HDR_BOOL))
    {
        bool tmpvalue = LoadBool();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadUint8(wxUint8& value)
{
    // load integer value
    if(LoadChunkHeader(wxSERIALIZE_HDR_INT8))
    {
        wxUint8 tmpvalue = LoadChar();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadUint16(wxUint16& value)
{
    // load integer value
    if(LoadChunkHeader(wxSERIALIZE_HDR_INT16))
    {
        wxUint16 tmpvalue = LoadUint16();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadUint32(wxUint32& value)
{
    // load integer value
    if(LoadChunkHeader(wxSERIALIZE_HDR_INT32))
    {
        wxUint32 tmpvalue = LoadUint32();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadUint64(wxUint64& value)
{
    // load integer value
    if(LoadChunkHeader(wxSERIALIZE_HDR_INT64))
    {
        wxUint64 tmpvalue = LoadUint64();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadInt(int& value)
{
	if(LoadChunkHeader(wxSERIALIZE_HDR_INT))
	{
		int tmpval = LoadInt();

		// assign value
		if(IsOk())
		{
			value = tmpval;
			return true;
		}
	}

	return false;
}

bool wxSerialize::ReadIntInt(int& value1, int& value2)
{
    value1 = 0;
    value2 = 0;
	if(LoadChunkHeader(wxSERIALIZE_HDR_INTINT))
	{
		int tmpval1, tmpval2;
		LoadIntInt(tmpval1, tmpval2);

		// assign value
		if(IsOk())
		{
			value1 = tmpval1;
			value2 = tmpval2;
			return true;
		}
	}

	return false;
}

bool wxSerialize::ReadDouble(wxFloat64& value)
{
    // load wxFloat64 value
    if(LoadChunkHeader(wxSERIALIZE_HDR_DOUBLE))
    {
        wxFloat64 tmpvalue = LoadDouble();

		// when all is ok, assign
		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadString(wxString& value)
{
    if(LoadChunkHeader(wxSERIALIZE_HDR_STRING))
    {
        wxString tmpvalue = LoadString();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadArrayString(wxArrayString& value)
{
    if(LoadChunkHeader(wxSERIALIZE_HDR_ARRSTRING))
    {
        wxArrayString tmpvalue = LoadArrayString();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}

bool wxSerialize::ReadDateTime(wxDateTime& value)
{
    if(LoadChunkHeader(wxSERIALIZE_HDR_DATETIME))
    {
        wxDateTime tmpvalue = LoadDateTime();

		if(IsOk())
		{
            value = tmpvalue;
			return true;
		}
    }

    return false;
}


bool wxSerialize::Read(wxMemoryBuffer &buf)
{
    // load record value
    if(LoadChunkHeader(wxSERIALIZE_HDR_RECORD))
    {
		Load(buf);
        return true;
	}

    return false;
}

void wxSerialize::Load(wxMemoryBuffer &buf)
{
	if(CanLoad())
	{
		wxUint32 size = LoadUint32();
		if(size > 0)
        {
			m_idstr.Read(buf.GetWriteBuf(size), size);
            buf.UngetWriteBuf(size);
        }
	}
}

int wxSerialize::LoadChunkHeader(int expheader)
{
	int hdr = 0;

    if(CanLoad())
	{
		if(m_haveBoundary)
			return 0;

		hdr = (int)LoadChar();
		if(hdr == wxSERIALIZE_HDR_ENTER || hdr == wxSERIALIZE_HDR_LEAVE)
		{
			// remember this state
			m_lastBoundary = hdr;
			m_haveBoundary = true;
			return 0;
		}

        // when header is not ok
        if(hdr != expheader)
        {
        	LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_WRONGCHUNK_s1_s2,
        	         GetHeaderName(expheader), GetHeaderName(hdr));
        	return -1;
        }
    }

    return hdr;
}

wxUint8 wxSerialize::LoadChar()
{
	wxUint8 value = '\0';

	// reads a character from the stream
	if(CanLoad())
	{
		// load unsigned char through ptr to
		// make sure we have no signed / unsigned crap
	    m_idstr.Read((void *)&value, sizeof(wxUint8));
	}

	return value;
}

int wxSerialize::LoadInt()
{
	wxUint8 intsize = 0;
	int tmpval = 0;

	if(CanLoad())
	{
		// get size
		intsize = LoadChar();
		switch(intsize)
		{
		case 1:	// 8 bits
			tmpval = (int)LoadChar();
			break;
		case 2:	// 16 bits
			tmpval = (int)LoadUint16();
			break;
		case 4:	// 32 bits
			tmpval = (int)LoadUint32();
			break;
		case 8:	// 64 bits
			tmpval = (int)LoadUint64();
			break;

		default:
			LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_RINTSIZE);
			break;
		}
	}

	return tmpval;
}

void wxSerialize::LoadIntInt (int& value1, int& value2)
{
	wxUint8 intsize = 0;

	if(CanLoad())
	{
		// get size
		intsize = LoadChar();
		switch(intsize)
		{
		case 1:	// 8 bits
			value1 = (int)LoadChar();
			value2 = (int)LoadChar();
			break;
		case 2:	// 16 bits
			value1 = (int)LoadUint16();
			value2 = (int)LoadUint16();
			break;
		case 4:	// 32 bits
			value1 = (int)LoadUint32();
			value2 = (int)LoadUint32();
			break;
		case 8:	// 64 bits
			value1 = (int)LoadUint64();
			value2 = (int)LoadUint64();
			break;

		default:
			LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_RINTSIZE);
			break;
		}
	}
}

wxString wxSerialize::LoadString()
{
    wxString str;

	if(CanLoad())
	{
		size_t len = LoadUint32();

        //wxLogDebug(wxString::Format(wxT("LoadString len %i"), len));

		if (len > 0)
		{
            // embarassing how I am doing this, but I cannot be certain which
            // unicode method reads a multibyte array and converts it so I will
            // do it per wxInt16 converted to wxChar. When bytes get lost in the
            // conversion then it's tough luck as this build is not unicode then
            str.Alloc(len+1);
            wxInt16 *buf = new wxInt16[len+1];
            for(int i = 0; i < (int)len; i++)
            {
                // we need to load per 16 bits because they
                // need to optionally be swapped to convert
                buf[i] = LoadUint16();
                str.Append((wxChar)buf[i]);
            }

            delete[] buf;
		}
	}
	return str;
}

wxArrayString wxSerialize::LoadArrayString()
{
	wxArrayString str;

	if(CanLoad())
	{
		wxUint32 count = LoadUint32();

		for(wxUint32 i = 0; i < count; i++)
			str.Add(LoadString());
	}

	return str;
}

wxDateTime wxSerialize::LoadDateTime()
{
	wxDateTime dt;

	if(CanLoad())
	{
		// load date
        wxUint8 day = LoadUint8();
        wxUint8 month = LoadUint8();
        wxUint16 year = LoadUint16();
        // load time
        wxUint8 hour = LoadUint8();
        wxUint8 min = LoadUint8();
        wxUint8 sec = LoadUint8();
        // load millisecs
        wxUint16 msec = LoadUint16();
        
        dt = wxDateTime(day, (wxDateTime::Month)month, year, hour, min, sec, msec);
	}

	return dt;
}

wxUint8 wxSerialize::LoadUint8()
{
    return LoadChar();
}

wxUint16 wxSerialize::LoadUint16()
{
	wxUint16 value = 0;

	// reads a 16bits from the stream
	if(CanLoad())
	{
		m_idstr.Read((void *)&value, sizeof(wxUint16));
		return wxUINT16_SWAP_ON_LE(value);
	}

	return value;
}

wxUint32 wxSerialize::LoadUint32()
{
	wxUint32 value = 0;

	// reads a 32bits from the stream
	if(CanLoad())
	{
		m_idstr.Read((void *)&value, sizeof(wxUint32));
		return wxUINT32_SWAP_ON_LE(value);
	}

	return value;
}

wxUint64 wxSerialize::LoadUint64()
{
	wxUint64 value = 0;

	// reads a character from the stream
	if(CanLoad())
	{
		m_idstr.Read((void *)&value, sizeof(wxUint64));
		return wxUINT64_SWAP_ON_LE(value);
	}

	return value;
}

bool wxSerialize::WriteInt(int value)
{
	if(CanStore())
	{
		SaveChar(wxSERIALIZE_HDR_INT);

		// save the size of the int
		SaveChar(sizeof(int));

		// save int itself by proper casting
		switch(sizeof(int))
		{
		case 1:
			SaveChar(value);
			break;
		case 2:
			SaveUint16(value);
			break;
		case 4:
			SaveUint32(value);
			break;
		case 8:
			SaveUint64(value);
			break;

		default:
			LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_SINTSIZE);
			break;

		}
	}

	return IsOk();
}

bool wxSerialize::WriteIntInt(int value1, int value2)
{
	if(CanStore())
	{
		SaveChar(wxSERIALIZE_HDR_INTINT);

		// save the size of the int
		SaveChar(sizeof(int));

		// save int itself by proper casting
		switch(sizeof(int))
		{
		case 1:
			SaveChar(value1);
			SaveChar(value2);
			break;
		case 2:
			SaveUint16(value1);
			SaveUint16(value2);
			break;
		case 4:
			SaveUint32(value1);
			SaveUint32(value2);
			break;
		case 8:
			SaveUint64(value1);
			SaveUint64(value2);
			break;

		default:
			LogError(wxSERIALIZE_ERR_ILL, wxSERIALIZE_ERR_STR_SINTSIZE);
			break;

		}
	}

	return IsOk();
}

bool wxSerialize::Write(const wxMemoryBuffer &buffer)
{
	wxUint32 size = 0;
    
    if(CanStore())
	{
		// save header to the stream
        SaveChar(wxSERIALIZE_HDR_RECORD);

        // save the record count
        size = buffer.GetDataLen();
        SaveUint32(size);
        
        // write data if there is any
        if(size > 0)
            m_odstr.Write(buffer.GetData(), size);
    }
	return IsOk();
}
// ATTN:: uncomment for static build
// Must be at global scope for VC++ 5 (ripped from wxDataInputStream)
//extern "C" wxFloat64 ConvertFromIeeeExtended(const wxInt8 *bytes);

wxFloat64 wxSerialize::LoadDouble()
{
	wxFloat64 value = 0;

	// reads a character from the stream
	if(CanLoad())
	{
//ATTN:: uncomment for static build
//#if wxUSE_APPLE_IEEE
//		wxInt8 buf[10];
//
//		m_idstr.Read((void *)buf, 10);
//		value = ConvertFromIeeeExtended(buf);
//#else
//		#pragma warning "wxSerialize::LoadDouble() not using IeeeExtended - will not work!"
//#endif
	}

	return value;
}

bool wxSerialize::LoadBool()
{
	bool value = false;
	wxUint8 chr;

	if(CanLoad())
	{
		chr = LoadChar();
		if(IsOk())
		    value = (chr != 0);
	}

	return value;
}

void wxSerialize::SaveChar(wxUint8 value)
{
	if(CanStore())
		m_odstr.Write((void *)&value, sizeof(wxUint8));
}

bool wxSerialize::WriteBool(bool value)
{
    wxUint8 nval = 0;

    if(CanStore())
	{
        // set to boolean
        if(value)
            nval = 1;

		SaveChar(wxSERIALIZE_HDR_BOOL);
        SaveChar(nval);
    }

    return IsOk();
}
//ATTN:: uncomment for static build
// Must be at global scope for VC++ 5
//extern "C" void ConvertToIeeeExtended(wxFloat64, wxInt8 *bytes);

bool wxSerialize::WriteDouble(wxFloat64 value)
{
    if(CanStore())
    {
        SaveChar(wxSERIALIZE_HDR_DOUBLE);

		wxInt8 buf[10];
//ATTN:: uncomment for static build
//#if wxUSE_APPLE_IEEE
//		ConvertToIeeeExtended(value, buf);
//#else
//	#if !defined(__VMS__) && !defined(__GNUG__)
//		#pragma warning "wxSerialize::WriteDouble() not using IeeeExtended - will not work!"
//	#endif
//		// fill with zeros when writing
//		memset(buf, 0, 10);
//#endif
		m_odstr.Write(buf, 10);
    }

    return IsOk();
}

bool wxSerialize::WriteString(const wxString& value)
{
    if(CanStore())
    {
		// write header + string
		SaveChar(wxSERIALIZE_HDR_STRING);
		SaveString(value);
    }

    return IsOk();
}

bool wxSerialize::WriteArrayString(const wxArrayString& value)
{
    if(CanStore())
    {
		// write header + string
		SaveChar(wxSERIALIZE_HDR_ARRSTRING);
		SaveUint32(value.Count());
		for(size_t i = 0; i < value.Count(); i++)
			SaveString(value[i]);
    }

    return IsOk();
}

bool wxSerialize::WriteDateTime(const wxDateTime& value)
{
    if(CanStore())
    {
		// write header
		SaveChar(wxSERIALIZE_HDR_DATETIME);

        // decompose		
		wxUint8 day = value.GetDay();
		wxUint8 month = value.GetMonth();
		wxUint16 year =  value.GetYear();
		wxUint8 hour = value.GetHour();
		wxUint8 min = value.GetMinute();
		wxUint8 sec = value.GetSecond();
		wxUint16 msec    = value.GetMillisecond();
		
		// serialize
		SaveChar(day);
		SaveChar(month);
		SaveUint16(year);
		SaveChar(hour);
		SaveChar(min);
		SaveChar(sec);
		SaveUint16(msec);
    }

    return IsOk();
}


void wxSerialize::SaveString(const wxString &value)
{
	if(CanStore())
	{
		size_t len = value.Len();
		SaveUint32(len);
		if(len > 0)
		{
		    // we write in unicode even when we are not compiled
		    // in unicode. This means writing pairs of wxInt16
		    // bytes. We need optional conversion in wxUint16
            for(int i = 0; i < (int)len; i++)
                SaveUint16((wxUint16)value.GetChar(i));
		}
	}
}

bool wxSerialize::WriteUint8(wxUint8 value)
{
    if(CanStore())
    {
		SaveChar(wxSERIALIZE_HDR_INT8);
		SaveChar(value);
	}

    return IsOk();
}

bool wxSerialize::WriteUint16(wxUint16 value)
{
    if(CanStore())
    {
		SaveChar(wxSERIALIZE_HDR_INT16);
		SaveUint16(value);
	}

    return IsOk();
}

bool wxSerialize::WriteUint32(wxUint32 value)
{
    if(CanStore())
    {
		SaveChar(wxSERIALIZE_HDR_INT32);
		SaveUint32(value);
	}

    return IsOk();
}

bool wxSerialize::WriteUint64(wxUint64 value)
{
    if(CanStore())
    {
		SaveChar(wxSERIALIZE_HDR_INT64);
		SaveUint64(value);
	}

    return IsOk();
}

void wxSerialize::SaveUint8(wxUint8 value)
{
    SaveChar(value);
}

void wxSerialize::SaveUint16(wxUint16 value)
{
	wxUint16 tmpval = wxUINT16_SWAP_ON_LE(value);
	if(CanStore())
        m_odstr.Write(&tmpval, sizeof(wxUint16));
}

void wxSerialize::SaveUint32(wxUint32 value)
{
	wxUint32 tmpval = wxUINT32_SWAP_ON_LE(value);
	if(CanStore())
        m_odstr.Write(&tmpval, sizeof(wxUint32));
}

void wxSerialize::SaveUint64(wxUint64 value)
{
	wxUint64 tmpval = wxUINT64_SWAP_ON_LE(value);
	if(CanStore())
        m_odstr.Write(&tmpval, sizeof(wxUint64));
}

// write function

wxString wxSerialize::GetHeaderName(int headername)
{
	wxString desc;

    switch(headername)
    {
		case wxSERIALIZE_HDR_STRING:
        	desc = wxT("string");
        	break;

        case wxSERIALIZE_HDR_INT8:
        	desc = wxT("8bits uint");
        	break;

        case wxSERIALIZE_HDR_INT16:
        	desc = wxT("16bits uint");
        	break;

        case wxSERIALIZE_HDR_INT32:
        	desc = wxT("32bits uint");
        	break;

        case wxSERIALIZE_HDR_INT64:
        	desc = wxT("64bits uint");
        	break;

        case wxSERIALIZE_HDR_DOUBLE:
        	desc = wxT("double");
        	break;

        case wxSERIALIZE_HDR_BOOL:
        	desc = wxT("bool");
        	break;

        case wxSERIALIZE_HDR_RECORD:
        	desc = wxT("data record");
        	break;

        default:
			if(headername < 0x30 || headername > 0x7f)
				desc = wxString::Format(wxT("0x%02X"), headername);
			else
				desc = wxString::Format(wxT("%c"), headername);
            break;
    }

    return desc;
}

int wxSerialize::LogError(int err, int msgcode, const wxString &s1, const wxString &s2)
{
	wxString error;

	// make sure we only report one error. When we already have errors
	// we ignore this one
	if(m_errorCode == wxSERIALIZE_ERR_OK && err != wxSERIALIZE_ERR_OK)
	{
		// close writing and reading
		Close();

		// assign our new error
		m_errorCode = err;

		switch(msgcode)
		{
			case wxSERIALIZE_ERR_STR_HEADER_s1_s2:
				error << wxT("Wrong header in start of stream, expected header '") << s1 <<
				         wxT(" and got '") << s2 << wxT("'");
				break;

			case wxSERIALIZE_ERR_STR_WRONGVERSION_s1_s2:
				error << wxT("Invalid version in stream, got v") << s1 << wxT(" but expected v")
				      << s2 << wxT(" or higher");
				break;

            case wxSERIALIZE_ERR_STR_BADISTREAM:
				error << wxT("Bad input stream");
				break;

            case wxSERIALIZE_ERR_STR_BADOSTREAM:
				error << wxT("Bad output stream");
				break;

            case wxSERIALIZE_ERR_STR_NOHEADER_s1:
				error << wxT("No valid header found in stream but expected header '") << s1
				      << wxT("'");
				break;

            case wxSERIALIZE_ERR_STR_NOVERSION:
				error << wxT("No version information found in stream");
				break;

            case wxSERIALIZE_ERR_STR_NOVERHDR:
				error << wxT("Cannot write version and/or header information to stream");
				break;

            case wxSERIALIZE_ERR_STR_NOWRITE:
				error << wxT("Cannot write while in read mode!");
				break;

            case wxSERIALIZE_ERR_STR_NOREAD:
				error << wxT("Cannot read while in write mode!");
				break;

            case wxSERIALIZE_ERR_STR_EOF:
                error << wxT("End of stream error while reading!");
                break;

            case wxSERIALIZE_ERR_STR_WRONGCHUNK_s1_s2:
				error << wxT("Expected chunk item of type '") << s1
				      << wxT("' but got type '") << s2 << wxT("'");
				break;

            case wxSERIALIZE_ERR_STR_MEMORY_s1:
				error << wxT("Memory allocation error. Cannot allocate ") << s1
				      << wxT(" bytes");
				break;

            case wxSERIALIZE_ERR_STR_READSIZE:
				error << wxT("Record to read is 0 bytes or larger then expected (does not fit maxcount)");
				break;

			case wxSERIALIZE_ERR_STR_RINTSIZE:
				error << wxT("Cannot read back 'int' value because it's of unknown size (need 1, 2, 4 or 8)");
				break;

			case wxSERIALIZE_ERR_STR_SINTSIZE:
				error << wxT("Cannot save 'int' value because it's of unknown size (need 1, 2, 4 or 8)");
				break;

			case wxSERIALIZE_ERR_STR_ILL_LEAVE:
				error << wxT("Sync Error: Illegal LeaveObject() header encountered, expected EnterObject()");
				break;

			case wxSERIALIZE_ERR_STR_ILL_UNKNOWN_HDR_s1:
				error << wxT("Unknown '") << s1 << wxT("' header in stream");
				break;

			case wxSERIALIZE_ERR_STR_ILL_LEVEL:
				error << wxT("Sync Error: Level dropped below 0, too much LeaveObject() calls ?");
				break;

			default:
				error << wxT("Unknown error error (aint that something ...)");
				break;
		}

        // log error to user
		m_errMsg = error;
		wxLogError(m_errMsg);
	}

	return m_errorCode;

}


/*static*/ wxString wxSerialize::GetLibVersionString ()
{
    return wxString::Format(_T("wxSerialize v%d.%d"), GetLibVersionMajor(), GetLibVersionMinor());
}

/*static*/ long wxSerialize::GetLibVersionMajor ()
{
	return WXSERIALIZE_MAJOR_VERSION;
}

/*static*/ long wxSerialize::GetLibVersionMinor ()
{
	return WXSERIALIZE_MINOR_VERSION;
}

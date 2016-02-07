//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : wxSerialize.h              
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
// $RCSfile: wxSerialize.h $
// $Source: include/wx/wxSerialize.h $
// $Revision: 1.16 $
// $Date: 7-sep-2007 11:29:08 $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
//              Christian Buhtz
// Copyright:   (c) Jorgen Bodde <jorgb@xs4all.nl>
// License:     see LICENSE for details
//---------------------------------------------------------------------------

#ifndef _WXSERIALIZE_H_
#define _WXSERIALIZE_H_

#include <wx/sstream.h>
#include <wx/buffer.h>
#include <wx/math.h>
#include <wx/datetime.h>
#include <wx/gdicmn.h>  // for wxSize, wxPoint

#define WXSERIALIZE_MAJOR_VERSION 1
#define WXSERIALIZE_MINOR_VERSION 1

/** \author Jorgen Bodde
    \mainpage

    This is the wxSerialize documentation guide. wxSerialize is a serializing class meant for serializing / de-serializing
	a complete (or partial) object model to a stream. The streaming is made type safe and reliable. This means when an
	error occurs (wrong data read back or unexpected end of file) the wxSerialize class logs an error, and refuses to
	read from the stream, leaving all variables that follow the error untouched, or in a default state.

	With the use of a version number and a header, the stream is checked when reading it back. When, for example you
	wrote a stream with version 1000, and upon reading you expect version 900, the wxSerialize class logs an error that
	the expected version is lower then the encountered version, avoiding a mis-read of data that is not supported in the
	code. With the use of a header string, you can double check if the stream offered is really recognised by the
	application.

	Because wxSerialize uses wxInputStream and wxOutputStream for reading and writing, the contents of the serialization
	can be written to a memory based stream, a compression stream (like wxZLibOutputStream), a tcp/ip socket, from
	an wxURL (from the internet, handy for updates), or any other stream based upon wxInputStream / wxOutputStream.

	The streaming is binary compatible on Linux / Windows and MacOSX which means that if you read back the binary data
	from disk, the same results are read back, the values are properly converted from little to big endian.

	\section serializing_exactly Ok but what is serializing exactly?

	Serializing is putting the contents of a class to an output file. Microsoft's MFC used CArchive (which I attempted
	to keep as API compatible as possible) and boost now also has a binary serializer. So, how does it work? Well it
	is really simple. The power of the serializing lies in the fact that the classes themselves are responsible for
	serializing their data. It is very short in code, and the destination of the serialization stream doesn't really
	matter.

    \section how_work So how does it work?

	Consider the following class:

	\code
	class MyDataClass
	{
	public:
		// the serialization class, that streams from and to the
		// specified stream.
		Serialize(wxSerialize &a);

	private:
		wxString m_name;
		wxString m_address;
		wxUint8 m_age;
		wxArrayString m_siblings;
	};
	\endcode
	How does the serializing work .. We simply implement the Serialize method:
	\code
	bool MyDataClass::Serialize(wxSerialize &a)
	{
		if(a.IsStoring())
		{
			a << m_name << m_address << m_age;
			a << m_siblings;
		}
		else
		{
			a >> m_name >> m_address >> m_age;
			a >> m_siblings;
		}

		// return false when the archive encountered an error
		return a.IsOk();
	}
	\endcode

	This is all there is to it! This small piece of code, serializes the 4 used member vars to the stream, on disk
	or where it needs to go. The following code shows how the class is serialized. First when it is saved to disk:

	\code
	wxFileOutputStream file("somefile.dat");
	wxSerialize a(file);

	m_ourClass->Serialize(a);
	\endcode

	That's all! When the file and archive are out of scope, the file is closed and the object is persistent on disk.
	Now to read the data back:

	\code
	wxFileInputStream file("somefile.dat");
	wxSerialize a(file);

	// let's say we just create it and initialize it
	m_ourClass = new MyDataClass();
	m_ourClass->Serialize(a);
	\endcode

	Nearly identical code isn't it ? Even the new MyDataClass() is not needed, it is simply there to illustrate how
	newly created objects can be serialized back into. Only one method is needed, I called it Serialize() but the
	name does not matter.

	\section why_using So why not write using wxConfig, wxFFile, XML or use XTI?

	This serializer class was designed, keeping in mind a small footprint in memory and disk. When serializing to XML
	you gain flexibility and even can hand-edit it, but the overhead in the class is enormous. You have to figure out
	how the data needs to be stored, attribute names, nested hierarchies, handle errors, etc. XML is basically better
	to write to, if user editing or portability is a big issue.

	Serializing to a binary file can be more useful in the following situations:

	- Writing cache data (e.g. an object model that represents a state in memory). By writing this in a serialized
	  stream to disk, the object model can be quickly read back between sessions, or heavy preprocessing (e.g storing
	  a directory structure to disk) can be avoided.
	- Undo / Redo mechanism. Usually undo / redo mechanisms require a lot of overhead in the object model. For example
	  when the user deletes a data item in a GUI window, this item should not be really deleted, only taken apart from the
	  object model. When using a serializer class, the object can be removed, and later on re-created back on it's
	  rightful place. The good thing is that the serialize methods can both handle undo / redo, but also save the
	  data to disk.
    - Small applications that do not require overhead of XML, SQL, or use wxConfig because these all need name / value
	  conventions. With very small applications that only perform minor tasks, the developer is more distracted by
	  the writing to disk then it's really needed.
    - Writing config. This sounds odd, but when you have a global config object, and it is not really necessary to
	  keep all that data in a text file, serializing the object containing the config is rather easy.

	So when is it not useful?

	- Ofcourse when people want to edit the data afterwards.
	- When the data needs to be exchanged between applications using a standard format
	- When the application needs to process huge data, or store entered data on disk to be queried later on (SQL or
	  XML).

    \section what_dp What about data flexibility?

	People who spent some time serializing data, know that when for example a binary serialized file is written to disk
	and the object is extended with more data, you have a problem with serializing it back.
	Why? The data in the stream is only compatible with the old data makeup, and when adding new member vars how are
	you going to read them back? They are not in the stream to begin with, and you don't even know if they are or not.
	Well I deviced two ways of doing this. Let's for example extend our rather trivial MyDataClass class:

	\code
	class MyDataClass
	{
	public:
		// the serialization class, that streams from and to the
		// specified stream.
		Serialize(wxSerialize &a);

	private:
		wxString m_name;
		wxString m_address;
		wxUint8 m_age;
		wxArrayString m_siblings;

		// NEW DATA ADDED, THIS NEEDS SERIALIZING TOO!
		wxUint32 m_monthlyPay;
		bool m_healthInsurance;
	};
	\endcode


	\subsection version_number Using a version number

	The version number you can give to the stream when you write it back. This version number is stored and checked upon
	reading. When creating the stream you specify the version of the stream. You increment this version when there will
	be binary incompatibility with older streams:

	\code
	wxFileOutputStream file("somefile.dat");	
	wxSerialize a(file, 1001);   // latest version is 1001 (was 1000)

	m_ourClass->Serialize(a);
	\endcode

	Now in the code we can take this in account when reading. When we save, we simply save our new data back. And
	as you guessed the drawback, older applications still in the field cannot read the newer stream:

	\code
	bool MyDataClass::Serialize(wxSerialize &a)
	{
		if(a.IsStoring())
		{
			// simply write all
			a << m_name << m_address << m_age;
			a << m_siblings;

			// always append new data here, so reading back goes in
			// the same exact order as it is written
			a << m_monthlyPay << m_healthInsurance;
		}
		else
		{
			a >> m_name >> m_address >> m_age;
			a >> m_siblings;

			// reading back poses a problem. We can have the data but in
			// an older stream we have not. So we need the version check:
			if(a.GetVersion() > 1000)
			{
				a >> m_monthlyPay >> m_healthInsurance;
			}
		}

		// return false when the archive encountered an error
		return a.IsOk();
	}
	\endcode

	As you might have guessed, this can be some overhead. While developing the app, this can happen a lot
	as the classes grow. So the disadvantages are:

	- Older releases you wrote (and are in the field) cannot read the newer data back. So when you use serialization
	  as part of packages or updates, people are obliged to update, or you must provide multiple versions)
	- All the newer data require a version check at read time, and will bloat your application as you must take in
	  account all older streams. A small overhead, but lots of changes will simply bloat your code with GetVersion()
	  checks which is not really elegant.

	\subsection enter_leave Using EnterObject() and LeaveObject()

	To reduce the code overhead, and maintain binary read compatiblity between older and newer releases of your
	application, I deviced a way which allows you to add member data to your serializing routines and not break
	the reading of older or even newer streams. This is how it works;

	\code
	bool MyDataClass::Serialize(wxSerialize &a)
	{
		a.EnterObject();
		if(a.IsStoring())
		{
			// simply write all
			a << m_name << m_address << m_age;
			a << m_siblings;
			a << m_monthlyPay << m_healthInsurance;
		}
		else
		{
			a >> m_name >> m_address >> m_age;
			a >> m_siblings;
			a >> m_monthlyPay >> m_healthInsurance;
		}
		a.LeaveObject();

		return a.IsOk();
	}
	\endcode

	Done! These two powerful boundaries (EnterObject() and LeaveObject()) tackle two situations:

	-	<b>Reading an old stream in a newer application</b>. So what happens, when for example the older stream does not contain the
		m_monthlyPay and m_healthInsurance, the LeaveObject() boundary was already written in the old code. The stream
		recognises this, and refuses to read data into m_monthlyPay and m_healthInsurance. In these cases the default
		value will be kept in these members. Data that follows after the boundary when LeaveObject was not called,
		will be kept default.
    -	<b>Reading a newer stream with an older application</b>. When the application you released in the field is older then the stream
		being written by the newer release, more data is present in the stream then the old application knows about.
		Here the same situation applies. The LeaveObject() in the old code comes sooner then the expected leave boundary
		in the stream, so wxSerialize will go look for the boundary and ignore all data that is newer.

	The EnterObject() and LeaveObject() construction keeps the stream in sync. The only important thing to remember is
	that you MUST USE these boundaries from the first moment and not when you are adding new member objects later on.
	It is a small sacrifice to gain a lot of flexibility! The overhead in the stream is also very small, because the
	footprint of the boundaries is exactly 1 byte for the enter and 1 byte for the leave marker.

	EnterObject() and LeaveObject() can be used multiple times after eachother, and also nested. So when you serialize
	a class hierarchy and in one of your child objects also data changes, this will be possible.

	The only drawback that is left with this approach (but is also present in <b>all</b> data writing when older applications
	write back data that contained newer information) is that newer data is being overwritten. You must take special
	caution (for example using the version tag or wxSerializeStatus object) in your application to warn the user that
	the data being written will destroy newer information in the serializing stream. The newer release of your
	application can still read this stream, but newer data is defaulted again.

    Also another caveat is that when you replace data being read back it will not work, not with EnterObject() / 
    LeaveObject() but also not with a versioned stream. Consider this example:

	\code
	bool MyDataClass::Serialize(wxSerialize &a)
	{
		a.EnterObject();
		if(a.IsStoring())
		{
			// simply write all
			a << m_name << m_address << m_age;
			a << m_siblings; 
            //a << m_monthlyPay << m_healthInsurance;
            // REPLACED NOW with:
            a << m_surName << m_middleName;
		}
		else
		{
			a >> m_name >> m_address >> m_age;
			a >> m_siblings;
			// WON'T WORK! In older versions the two vars m_monthlypay and m_healthInsurance
            // were stored here, instead we read back this. If the stream we read back do not
            // contain these new replaced variables yet, we are hitting an error state. 
            a >> m_surName >> m_middleName;
		}
		a.LeaveObject();

		return a.IsOk();
	}
	\endcode 

	\subsection archive_status wxSerializeStatus Helper Class

	This class is especially designed to keep the status of the stream between reading and writing. It contains the
	last version being written or read, the header, and more important, an NewDataLoss() flag. This flag will be set
	when the stream being read back is newer then the data model it's beaing serialized into. Writing the data back
	using the wxSerializeStatus object earlier retrieved when reading will prevent the write if the flag NewDataLoss()
	is not reset using OverwriteNew() flag. How is the flag determined? Well, if one time in the data serializing
	process the LeaveObject() method needs to search for the marker (thus skipping newer data in the stream) this
	flag is set.

	This wxSerializeStatus class will also remove the need of writing the same header back as being specified when
	reading, and also the version will be preserved. The member wxSerializeStatus::SetVersion() can be used to specify
	a newer version. Here is an example, first when reading we store the last status and keep it:

	\code
	wxFileInputStream file("somefile.dat");
	wxSerialize a(file);

	// let's say we just create it and initialize it
	m_ourClass = new MyDataClass();
	m_ourClass->Serialize(a);

	// store the status (copy class)
	m_ourStatus = a.GetArchiveStatus();
	\endcode

	When writing back, and this application is older then the newer application, thus an overwrite will result
	in newer information loss:


	\code
	// if the last time we read a newer stream, tell the user
	if(m_ourStatus.NewDataLoss())
	{
		if(::wxMessageDialog( ... "ask if we should overwrite, it will result in dataloss" ...) == wxID_YES)
			m_ourStatus.OverwriteNew();	// reset the flag
	}

	// if the flag is reset we continue else we skip
	if(!m_ourStatus.NewDataLoss())
	{
		wxFileOutputStream file("somefile.dat");
		wxSerialize a(file, m_ourStatus.GetVersion(), m_ourStatus.GetHeader());
		// serialize data
		m_ourClass->Serialize(a);
	}
	else
	{
		wxLogError("Data is not saved!");
	}
	\endcode

	This construction prevents the application of accidentally writing data back and destroying newer serialized data.
	The user must explicitly agree upon this, or the developer must provide a mechanism to backup the old file so that
	newer data is not lost when reading it back in the newer application.
*/

/** \class wxSerializeStatus
	\brief A simple helper class that keeps track of the current version of the archive, the header and
	       more important, when at reading time it appears the stream contains newer data then expected.
		   It will help the developer in asking the user if the stream might be saved back or not. See
		   \ref archive_status overview for more information.
*/
class wxSerializeStatus
{
public:
	/** Constructor of the wxSerializeStatus object. This is the default constructor so you can
	    define it in the class itself without having to do anything for it */
	wxSerializeStatus()
		: m_version(0)
		, m_header(wxT(""))
		, m_newDataLoss(false)
	{
	};

	/** Constructor initializer of the wxSerializeStatus object. Used by wxSerialize to fill in the goods
	    so they can be used later */
	wxSerializeStatus(size_t version, const wxString &header)
		: m_version(version)
		, m_header(header)
		, m_newDataLoss(false)
	{
	};

	~wxSerializeStatus() {
	};

	/** Copy operator to get the latest copy of it from the wxSerialize class, and assign it to your copy */
	void operator=(const wxSerializeStatus &s) {
		m_version = s.m_version;
		m_header = s.m_header;
		m_newDataLoss = s.m_newDataLoss;
	};

	/** With this method you tell wxSerialize that it is ok to overwrite the newer data stream with an older
	    version of it. This needs to be set implicitly. Check out \ref archive_status for more information.
	*/
	void OverwriteNew() {
		m_newDataLoss = false;
	};

	/** This method returns the flag of the current NewDataLoss status. It will be set when the wxSerialize
		encounters an out of sync where there is more data then can be read back.  See \ref enter_leave
	*/
	bool NewDataLoss() const {
		return m_newDataLoss;
	};

	/** Sets the NewDataLoss flag again. This is used by wxSerialize only, or you can use it to prevent other
	    parts of the your code to be executed later when new data loss still occurs. The counterpart of this
		flag is OverwriteNew() */
	void SetNewDataLoss() {
		m_newDataLoss = true;
	};

	/** Returns the version of the last read archive */
	size_t GetVersion() const {
		return m_version;
	};

	/** Returns the header of the last read archive */
	const wxString &GetHeader() const {
		return m_header;
	};

private:
	size_t m_version;
	wxString m_header;
	bool m_newDataLoss;
};

/** \class wxSerialize
	\brief A serializer class that can serialize the contents of your class to or from a stream. It uses wxInputStream
	       and wxOutputStream derived classes for input and output.

		   See the \ref mainpage for more information
*/
class wxSerialize
{
public:
    /** Construct an input stream archive. The stream will be used to retrieve the data from, which can be any
		derived stream like a file, zip stream etc. When constructed as input, the wxSerialize object cannot
		be written to.

        The first integer and string are always read from the stream. This contains the version number (or 0 when
		not used) and the header which is empty if not used.

        When setting the version parameter, wxSerialize will check if the version read back is at least the expected
		version. For example, when version argument is set to 1000, wxSerialize will accept versions 0 .. 1000 for reading
		but will give an error if version 1001 and up is read from the stream. You can use this to set the version to
        at least the supported version for your code. When extending the serialization routines after your app has
		been deployed, the version info can be used to dynamically skip parts while loading from an older stream,
		and warn users that the version of the read stream is older which means incompatibility with
        the older application will occur. This is not the recommended method though, see EnterObject() and LeaveObject()
		for a more flexible solution.

        The header tag will also be compared when it is not empty. When an empty header is passed as parameter, the read
		back header tag is ignored. When the header argument is set, and the read back header is empty or different,
		an error will occur and the stream cannot be read back from. This can provide an extra check if the stream
		will contain the data needed.

        The partialMode flag will do nothing else but setting a flag (IsFullSerialize()) which can be used in
		serializing to skip some relational hierarchies and stuff not needed when the object is only partially
		serialized to the stream (handy for undo / redo mechanisms). When set to true, IsFullSerialize() will
		return false.

        \code
        // open and write
        wxFileInputStream in_stream("somefile.dat");
		wxSerialize a(in_stream, 1000, _T("My Stream"));
        if(a.IsOk())
        {
            a >> m_name;
            a >> m_age;
            a >> m_string >> m_count;
        }
        else
            wxLogError(wxT("Archive cannot be opened because : ") + a.GetLastErrorString());

        // end the archive
        a.Close();
        \endcode

		See also \ref enter_leave and read about data flexibility.

		\sa IsFullSerialize(), wxSerializeStatus

    */
    wxSerialize(wxInputStream &stream, size_t version = 0, const wxString &header = wxEmptyString, bool partialMode = false);

    /** Construct an output stream object. The stream will be used to save the data to, which can be any derived
	    stream like a file, zip stream etc. When constructed as output, the wxSerialize object cannot be read from.

        \code
        // open and write
        wxFileOutputStream out_file("somefile.dat");
		wxSerialize a(out_file, 1000, _T("My Stream"));
        a << m_name;
        a << m_age;
        a << m_string << m_count;

        // end all writing (does not close the stream)
        a.Close();
        \endcode

        The version (1000 in example) is saved in the stream, also the header. At reading time these two are checked
		against what the expected version and header are, so that you can be sure the stream is recognised and
		supported.

        Always save your latest supported version here. This means when you extended your application's serialization,
		increment the version. This will allow you to read back older streams and save them with newer information.
		See \ref enter_leave for more ways to make your serialization more flexible.
    */
    wxSerialize(wxOutputStream &stream, size_t version = 1, const wxString &header = wxEmptyString, bool partialMode = false);

    /** Destructor. The passed stream ombject will not be destroyed, because it is used by reference */
    virtual ~wxSerialize();

    /** Closes the archive. This means all writing or reading done to it will not affect the archive or variables
	    passed by reference. A closed archive will not produce any errors when read from and will not advance in
		searching. A closed archive can be re-opened with Open() to allow reading or writing again. */
    void Close() {
        m_opened = false;
    };

    /** Re-opens the archive. But only when the archive is in OK state (IsOk() == true). By default the archive is
	    opened when the constructor is created and everything went ok */
    void Open() {
        // it can be as simple as this
        m_opened = IsOk();
    };

    /** In reading mode, it returns true when the stream we are reading from returns true on Eof().
        It means that it returns true AFTER an attempt has been made to read past the end of the stream.
        In writing mode Eof() returns always false. When the stream is closed, true is always returned, 
        we can't read or write anymore in that case anyway. Reading more from a stream that is at it's 
        end will produce errors.
	*/
    bool Eof();

	/** Returns true when the wxSerialize is in OK condition. This means no error occurred. Errors that can occur are
	    for example:
	        - The underlying stream reports that it is not OK (not able to write, not able to read)
	        - Reading an unexpected type of variable (e.g. expecting int and reading back string)
	        - Reading from a wxSerialize with a higher version then supported
	        - Reading from a wxSerialize with a wrong header
	        - Writing to the wxSerialize when we are in reading mode
	        - Reading from the wxSerialize when we are in writing mode

	    See GetLastError() to retrieve the error, and GetLastErrorString() for an explanation. When the wxSerialize
		encountered errors, it will remain in an error state, subsequent reads and writes will not have effect to
		either the member variables to read to, or write anything to the stream when writing, for example:

	    \code
	    wxSerialize a(in_stream);

	    // .. some code

        if(a.IsStoring())
        {
            // when wxSerialize is not ok, this does nothing
            // so we can do this safely, and report Ok status below

            a << m_name << m_address << m_age;

            // when we are not ok, we can use a.IsOk() to skip
            // the count loop right away (we cannot write and it saves time)

            a << GetCount();
            for(int i = 0; i < GetCount() && a.IsOk(); i++)
                a << m_value[i];

        }
        else
        {
            // reset your object, in case we
            // get errors, all vars will be initialized

            ResetDefaults();

            // reading back from not ok stream also does not affect
            // the passed member vars they will remain default

            a >> m_name >> m_address >> m_age;

            // be careful here, because count is not set by default
			// so we set it to 0. Now the for loop will quit right away
            int count = 0;
            a >> count;
            for(int i = 0; i < count; i++)
                a >> m_value;
        }

	    // return status to lower part
	    return a.IsOk();
		\endcode
	*/
	bool IsOk() {
		bool noerr = (m_errorCode == wxSERIALIZE_ERR_OK);
		if(m_writeMode)
		    return m_odstr.IsOk() && noerr;
		else
		    return m_idstr.IsOk() && noerr;
	};

	/** Returns the last error set in this string. */
	const wxString &GetLastErrorStr() const {
		return m_errMsg;
	};

	/** This method allows you to specify regions of an object that wxSerialize can identify. This allows
	    you to read back old streams, and still serialize all data back from the stream. All the data that
		is not present in the stream reading back is skipped. This will reduce the problem that old streams
		do not contain all data that is needed, it will simply be skipped. See \ref enter_leave for a bigger
		explanation, basically when using this, you ensure older applications can read newer streams, and
		newer applications can also read older streams.

		The only restriction is that you keep the data in the same order. This means the new added data
		always  needs to be read and written at the end of the original data.

		The method returns false when the object entering did not work (always returns true in write mode
		but can return false plus an error when the current level could not be found. If this is the case
		the wxSerialize class stays in an error state so it won't be harmful trying to read or write anyway.

		For example:

		\code
			wxSerialize a(in_stream);

			if(a.EnterObject())
			{
				// we succesfully entered the object level
				if(a.IsLoading())
				{
					a >> m_name;
					a >> m_age;

					// these vars are not present in the older stream. At this point,
					// all serializing into the variables will not take place. They will
					// be left default, so initializing before the construction gives them
					// a default value, but the stream reader does not give errors, here
					// it is best to use the ReadInt and ReadString directly with a default
					// specifier:

					a.ReadInt(m_peopeCount, 0);	// default no people when not present
					a.ReadUint8(m_numberOfPersons, 1);	// default 1 person
				}
				else
				{
					// save the stream back, now all vars can be read back. The interesting
					// thing is that older programs can still read the streams back as they
					// scan for the LeaveObject() marker at the end of their serializing round

					a << m_name;
					a << m_age;

					a << m_peopeCount;
					a << m_numberOfPersons;
				}
				a.LeaveObject();
			}
		\endcode
	*/
	bool EnterObject();

	/** This the leave method that belongs to the EnterObject() method. Please always close the
		section with the LeaveObject() when an EnterObject() is used. These two constructions can
		be nested also. See \ref enter_leave for more information.

		\sa EnterObject()
	*/
	bool LeaveObject();

	/** Indicates current state is writing. This flag will always be true if wxSerialize is constructed with a
	    wxOutputStream. The IsStoring() does not check wether the state of the stream is ok or not,
		to prevent problems do:

        \code
        wxSerialize a(stream);

        // without the a.IsOk() it will work too, but serializing will
        // go on writing to dev/null without aborting when an error occurred.

        if(a.IsOk())
        {
            if(a.IsStoring())
            {
                // write contents
            }
            else
            {
                // read contents
            }
        }
        \endcode
    */
    bool IsStoring() const {
        return m_writeMode;
    };

    /** Indicates current state is reading. This flag will always be true if wxSerialize is constructed with a
	    wxInputStream. The IsLoading() does not check wether the state of the stream is ok or not, to prevent
		problems do:

        \code
        wxSerialize a(stream);

        // without the a.IsOk() it will work too, but serializing will
        // go on without aborting when an error occurred.

        if(a.IsOk())
        {
            if(a.IsLoading())
            {
                // read contents
            }
            else
            {
                // write contents
            }
        }
        \endcode
    */
    bool IsLoading() const {
        return !m_writeMode;
    };

    /** This is an extra flag independent of IsStoring() or IsLoading(). It can be set to indicate that certain
	    parts of the serialized objects must be omitted, can be useful when not the complete hierarchy needs
		serializing, but only a single object, for example to a temporary stream (undo / redo mechanism). See
		the argument 'partialMode' at the constructors for this class.

        \code
        bool SomeObject::Serialize(wxSerialize &a)
        {
            if(a.IsStoring())
            {
                // store our data
                a << m_name << m_address << m_age;

                // in partial mode, the following will return false, not saving
                // any hierachie information of objects
                if(a.IsFullSerialize())
                {
                    a << m_items.Count();
                    for(size_t i = 0 ; a.IsOk() && i < m_items.Count(); i++)
                        m_items[a]->Serialize(a);
                }
            }
            else
            {
                // load our data
                a >> m_name >> m_address >> m_age;

                // in partial mode, the following will return false, not restoring
                // any hierachie information of objects
                if(a.IsFullSerialize())
                {
                    size_t count = 0;
                    a >> count;

                    m_items.DeleteAll();

                    for(size_t i = 0 ; a.IsOk() && i < count; i++)
                    {
                        SomeChild *child = new SomeChild();
                        child->Serialize(a);
                        m_items.Add(child);
                    }
                }
            }
        }
        \endcode
    */
    bool IsFullSerialize() const {
        return !m_partialMode;
    };

    /** Returns true when the stream is open. When opened, reading and writing can take place. When closed,
	    no reading and writing can occur, but also no error will be reported when tried anyway */
    bool IsOpen() const {
        return m_opened;
    };

    // -- saving serializers --

    /** Convenience operator for serializing. See WriteUInt8() */
    virtual wxSerialize &operator <<(wxUint8 value) {
        WriteUint8(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteUInt16() */
    virtual wxSerialize &operator <<(wxUint16 value) {
        WriteUint16(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteUInt32() */
    virtual wxSerialize &operator <<(wxUint32 value) {
        WriteUint32(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteUInt64() */
	virtual wxSerialize &operator <<(wxUint64 value) {
        WriteUint64(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteInt() */
    virtual wxSerialize &operator <<(int value) {
        WriteInt(value);
	    return *this;
    };

	/** Convenience operator for serializing. See WriteString() */
	virtual wxSerialize &operator <<(const wxString& value) {
        WriteString(value);
	    return *this;
    };

	/** Convenience operator for serializing. See WriteArrayString() */
	virtual wxSerialize &operator <<(const wxArrayString& value) {
        WriteArrayString(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteString() */
	virtual wxSerialize &operator <<(const wxChar* pvalue) {
        wxString value(pvalue);
        WriteString(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteBool() */
	virtual wxSerialize &operator <<(bool value) {
        WriteBool(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteDouble() */
    virtual wxSerialize &operator <<(wxFloat64 value) {
        WriteDouble(value);
	    return *this;
    };

    /** Convenience operator for serializing. See WriteDateTime() */
	virtual wxSerialize &operator <<(const wxDateTime &value) {
        WriteDateTime(value);
	    return *this;
    };

	/** Convenience operator for serializing. See WriteIntInt() */
	virtual wxSerialize &operator <<(const wxSize& value) {
        WriteIntInt(value.GetWidth(), value.GetHeight());
	    return *this;
    };

	/** Convenience operator for serializing. See WriteIntInt() */
	virtual wxSerialize &operator <<(const wxPoint& value) {
        WriteIntInt(value.x, value.y);
	    return *this;
    };

    // -- loading serializers --

    /** Convenience operator for serializing. See ReadUint8() */
    virtual wxSerialize &operator >>(wxUint8& value) {
        ReadUint8(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadUint16() */
    virtual wxSerialize &operator >>(wxUint16& value) {
        ReadUint16(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadUInt32() */
	virtual wxSerialize &operator >>(wxUint32& value) {
        ReadUint32(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadUInt64() */
    virtual wxSerialize &operator >>(wxUint64& value) {
        ReadUint64(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadInt() */
    virtual wxSerialize &operator >>(int &value) {
        ReadInt(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadString() */
	virtual wxSerialize &operator >>(wxString& value) {
        ReadString(value);
		return *this;
    };

	/** Convenience operator for serializing. See ReadArrayString() */
	virtual wxSerialize &operator >>(wxArrayString& value) {
        ReadArrayString(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadBool() */
	virtual wxSerialize &operator >>(bool& value) {
        ReadBool(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadDouble() */
	virtual wxSerialize &operator >>(wxFloat64& value) {
        ReadDouble(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadDateTime() */
	virtual wxSerialize &operator >>(wxDateTime& value) {
        ReadDateTime(value);
		return *this;
    };

    /** Convenience operator for serializing. See ReadIntInt() */
	virtual wxSerialize &operator >>(wxSize& value) {
        int iWidth, iHeight;
        if (ReadIntInt(iWidth, iHeight)) {
            value.SetWidth(iWidth);
            value.SetHeight(iHeight);
        }
		return *this;
    };

    /** Convenience operator for serializing. See ReadIntInt() */
	virtual wxSerialize &operator >>(wxPoint& value) {
        ReadIntInt(value.x, value.y);
		return *this;
    };

	/** Loads a boolean from stream. When the next record to be read is not a bool, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read.
    */
    bool ReadBool(bool& value);

	/** Variant of ReadBool() that will provide a default if the read was not succesful */
	bool ReadBool(bool& value, bool defval) {
		bool val = ReadBool(value);
		if(!val)
			value = defval;
		return val;

	};

	/** Loads a wxUint8 from stream. When the next record to be read is not a wxUint8, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read.
    */
    bool ReadUint8(wxUint8& value);

	/** Variant of ReadUint8() that will provide a default if the read was not succesful */
	bool ReadUint8(wxUint8& value, wxUint8 defval) {
		bool val = ReadUint8(value);
		if(!val)
			value = defval;
		return val;
	};

	/** Loads a wxUint16 from stream. When the next record to be read is not a wxUint16, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read.
    */
    bool ReadUint16(wxUint16& value);

	/** Variant of ReadUint16() that will provide a default if the read was not succesful */
	bool ReadUint16(wxUint16& value, wxUint16 defval) {
		bool val = ReadUint16(value);
		if(!val)
			value = defval;
		return val;
	};

	/** Loads a wxUint32 from stream. When the next record to be read is not a wxUint32, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read.
    */
	bool ReadUint32(wxUint32& value);

	/** Variant of ReadUint32() that will provide a default if the read was not succesful */
	bool ReadUint32(wxUint32& value, wxUint32 defval) {
		bool val = ReadUint32(value);
		if(!val)
			value = defval;
		return val;
	};

	/** Loads a wxUint64 from stream. When the next record to be read is not a wxUint64, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read.
    */
    bool ReadUint64(wxUint64& value);

	/** Variant of ReadUint64() that will provide a default if the read was not succesful */
	bool ReadUint64(wxUint64& value, wxUint64 defval) {
		bool val = ReadUint64(value);
		if(!val)
			value = defval;
		return val;
	};

	/** Loads an int from stream. When the next record to be read is not an int, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read.

		<b>SPECIAL CARE ABOUT THIS FUNCTION MUST BE TAKEN:</b>
		Reading an integer x-platform is potentially unsafe. The size of the integer may vary from 16 bits
		to 64 bits depending on the platform. When sharing data streams, the preferred way of streaming is
		using the wxUint16, wxUint32, wxUint64 data types. However, this method is made as safe as possible
		by also storing the size of the int on the stream. This means reading a stored integer of 4 bytes
		(32 bits) it will be read back as 32 bits, and casted to a 64 bits integer. In theory, this should
		work as the sign flag for a 32 bits container will be extended to match the 64 bits container. The
		only worst case scenario that exists is when an 64 bits int is written, and it needs to be casted
		to a 32 bits integer, problems can occur because the destination container may be too small. No error
		will be logged so be careful storing extreme values that might not fit in smaller integers.
    */
    bool ReadInt(int& value);

	/** Loads an pair of ints from stream. When the next record to be read is not an int, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read.

		<b>SPECIAL CARE ABOUT THIS FUNCTION MUST BE TAKEN:</b>
		Reading an integer x-platform is potentially unsafe. The size of the integer may vary from 16 bits
		to 64 bits depending on the platform. When sharing data streams, the preferred way of streaming is
		using the wxUint16, wxUint32, wxUint64 data types. However, this method is made as safe as possible
		by also storing the size of the int on the stream. This means reading a stored integer of 4 bytes
		(32 bits) it will be read back as 32 bits, and casted to a 64 bits integer. In theory, this should
		work as the sign flag for a 32 bits container will be extended to match the 64 bits container. The
		only worst case scenario that exists is when an 64 bits int is written, and it needs to be casted
		to a 32 bits integer, problems can occur because the destination container may be too small. No error
		will be logged so be careful storing extreme values that might not fit in smaller integers.
    */
    bool ReadIntInt(int& value1, int& value2);

	/** Variant of ReadIntInt() that will provide a default if the read was not succesful */
	bool ReadIntInt(int& value1, int& value2, int defval1, int defval2) {
		bool val = ReadIntInt(value1, value2);
		if(!val) {
			value1 = defval1;
			value2 = defval2;
		}
		return val;
	};

	/** Variant of ReadInt() that will provide a default if the read was not succesful */
	bool ReadInt(int& value, int defval) {
		bool val = ReadInt(value);
		if(!val)
			value = defval;
		return val;
	};


	/** Loads a wxFloat64 from stream. When the next record to be read is not a wxFloat64, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read. The wxFloat64 is
		stored as a IEEE converted standard wxFloat64, and is x-platform so it can be shared between other os'es
    */
    bool ReadDouble(wxFloat64& value);

	/** Variant of ReadDouble() that will provide a default if the read was not succesful */
	bool ReadDouble(wxFloat64& value, wxFloat64 defval) {
		bool val = ReadDouble(value);
		if(!val)
			value = defval;
		return val;
	};

	/** Loads a wxString from stream. When the next record to be read is not a wxString, an error will occur.
	    When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read. For the string
		first the count is stored and then the string itself. When storing an empty string, only a 0 count is
		stored.
    */
	bool ReadString(wxString& value);

	/** Variant of ReadString() that will provide a default if the read was not succesful */
	bool ReadString(wxString& value, const wxString &defval) {
		bool val = ReadString(value);
		if(!val)
			value = defval;
		return val;
	};

	/** Loads a wxArrayString from stream. When the next record to be read is not a wxArrayString, an error
		will occur.  When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read. For the string
		first the count is stored and then the string itself. When storing an empty string, only a 0 count is
		stored.
    */
	bool ReadArrayString(wxArrayString& value);

	/** Variant of ReadArrayString() that will provide a default if the read was not succesful */
	bool ReadString(wxArrayString& value, const wxArrayString &defval) {
		bool val = ReadArrayString(value);
		if(!val)
			value = defval;
		return val;
	};

	/** Loads a wxDateTime from stream. When the next record to be read is not a wxDateTime, an error
		will occur.  When wxSerialize encountered the end of object boundary (see \ref enter_leave) or the archive is
		in an error state, the value is not touched.

        Please note that when an error occurs, all subsequent loading from the stream will result in an
		error. This is to provide a save mechanism out of a read error. So instead of checking every value
		you read back yourself, they can be default and won't produce erratic behaviour.

		The value returned is true when the read was succesful, false when nothing was read. For the string
		first the count is stored and then the string itself. When storing an empty string, only a 0 count is
		stored.
    */
	bool ReadDateTime(wxDateTime& value);

    /** Reads a record from the stream. The wxMemoryBuffer is a growable buffer class, which will contain the data
	    being read. The length of the buffer will be the length present so there is no need to define a
		pre-allocated buffer. When the referenced buffer runs out of scope in your code, it is destroyed safely.

        The read buffer can be up to 4 Gb (count is 32 bits). The buffer will be considered an atomic element
        so when the read fails for the specified number of counts, the buffer is corrupted. This will ofcourse only
        happen when the stream unexpectedly terminates and will also occur with other atomic values like reading
        an integer etc.

        NOTICE FOR PLATFORM BINARY COMPATIBILITY: No conversion takes place when writing the buffer. It is written as
	    a sequence of bytes to the stream. So be careful storing the whole data back into a structure (little to big endian)
	*/
    bool Read(wxMemoryBuffer &buffer);

    /** Saves a bool to stream. It writes a header type first, and after that the value. This means when the boolean
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.
    */
    bool WriteBool(bool value);

    /** Saves a double as a wxFloat64 to stream. It writes a header type first, and after that the value. This means when the wxFloat64
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.

        The wxFloat64 is saved in a cross platform compatible mode, which means the streams can be interchanged between
		MacOS, Linux and Windows without problems.
    */
    bool WriteDouble(wxFloat64 value);

    /** Saves a wxUint8 to stream. It writes a header type first, and after that the value. This means when the wxUint8
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.

        The wxUint8 is saved in a cross platform compatible mode, which means the streams can be interchanged between
		MacOS, Linux and Windows without problems.
    */
    bool WriteUint8(wxUint8 value);

    /** Saves a wxUint16 to stream. It writes a header type first, and after that the value. This means when the wxUint16
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.

        The wxUint16 is saved in a cross platform compatible mode, which means the streams can be interchanged between
		MacOS, Linux and Windows without problems.
    */
    bool WriteUint16(wxUint16 value);

    /** Saves a wxUint32 to stream. It writes a header type first, and after that the value. This means when the wxUint32
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.

        The wxUint32 is saved in a cross platform compatible mode, which means the streams can be interchanged between
		MacOS, Linux and Windows without problems.
    */

    bool WriteUint32(wxUint32 value);

    /** Saves a wxUint64 to stream. It writes a header type first, and after that the value. This means when the wxUint64
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.

        The wxUint64 is saved in a cross platform compatible mode, which means the streams can be interchanged between
		MacOS, Linux and Windows without problems.
    */
    bool WriteUint64(wxUint64 value);

    /** Saves an int to stream. It writes a header type first, and after that the value. This means when the int
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.

		<b>SPECIAL CARE ABOUT THIS FUNCTION MUST BE TAKEN:</b>
		Writing an integer x-platform is potentially unsafe. The size of the integer may vary from 16 bits
		to 64 bits depending on the platform. When sharing data streams, the preferred way of streaming is
		using the wxUint16, wxUint32, wxUint64 data types. See ReadInt() for more information.
	*/
	bool WriteInt(int value);

    /** Saves a pair of ints to a stream. It is used for wxSize and wxPoint. It writes a header type first,
        and after that the value. This means when the int is read back when it is not expected, an error occurs
        and the program can terminate gracefully because no bogus values are read back. True is returned when
        the value is saved properly, and false when it's not.

		<b>SPECIAL CARE ABOUT THIS FUNCTION MUST BE TAKEN:</b>
		Writing an integer x-platform is potentially unsafe. The size of the integer may vary from 16 bits
		to 64 bits depending on the platform. When sharing data streams, the preferred way of streaming is
		using the wxUint16, wxUint32, wxUint64 data types. See ReadInt() for more information.
    */
    bool WriteIntInt(int value1, int value2);

    /** Saves a wxString to stream. It writes a header type first, and after that the value. This means when the wxString
	    is read back when it is not expected, an error occurs and the program can terminate gracefully because no
		bogus values are read back. True is returned when the value is saved properly, and false when it's not.

        The wxString is saved in a cross platform compatible mode, which means the streams can be interchanged between
		MacOS, Linux and Windows without problems. First the count is stored, then the string. With an empty string only
		the 0 count is stored.
    */
    bool WriteString(const wxString& value);

    /** Saves a wxArrayString to stream. It writes a header type first, and after that the value. This means when
		the wxArrayString is read back when it is not expected, an error occurs and the program can terminate
		gracefully because no bogus values are read back. True is returned when the value is saved properly, and
		false when it's not.

        The wxString is saved in a cross platform compatible mode, which means the streams can be interchanged between
		MacOS, Linux and Windows without problems. First the count is stored, then the string. With an empty string only
		the 0 count is stored.
    */
    bool WriteArrayString(const wxArrayString& value);

    /** Saves a wxDateTime to stream. It writes a header type first, and after that the value. This means when
		the wxDateTime is read back when it is not expected, an error occurs and the program can terminate
		gracefully because no bogus values are read back. True is returned when the value is saved properly, and
		false when it's not.

        The wxDateTime is saved in a cross platform compatible mode (e.g. the number of ticks). Please be aware that no
        timezone information or daylight saving information is stored. So if the date read back on a different PC is 
        mis interpreted because the PC is in a different time zone you must find other means of syncing them.
    */
    bool WriteDateTime(const wxDateTime& value);

	/** Saves an arbitrary amount of bytes to the stream using wxMemoryBuffer. It writes a header type first, and after
        that the count to be saved and the bytes in the wxMemoryBuffer. This means when the data is read back when
        it is not expected, an error occurs and the program can terminate gracefully because no bogus values are
        read back. True is returned when the value is saved properly, and false when it's not.

        Data can be saved up to 4Gb. This is not recommended (nor physically possible I think) but the limit is 32 bits for
        the count.

        NOTICE FOR PLATFORM BINARY COMPATIBILITY: No conversion takes place when writing the buffer. It is written as
	    a sequence of bytes to the stream. So be careful storing the whole data back into a structure (little to big endian)
    */
    bool Write(const wxMemoryBuffer &buffer);

    // -- arbitrary functions --

    /** Returns the header that was read by this stream. When writing, the header string contains
        the passed header by the constructor. See GetArchiveStatus() for more information. */
    wxString GetHeaderString() const {
        return m_headerStr;
    };

    /** Returns the version of the stream currently beaing read, or else the version of the stream when it
	    was created. This can be used to exclude parts while reading, if the stream version does not
        support newly added veriables like;

        \code
        // we expect version 1001 at least (lower versions are possible)
        wxSerialize a(in_stream, 1001);

        if(a.IsStoring())
        {
            a << m_name;

            // added since 1001
            a << m_addr;
        }
        else
        {
            a >> m_name;

            // only read when we are above 1000
            if(a.GetVersion() > 1000)
                a >> m_addr;
        }
        \endcode
    */
    size_t GetVersion() const {
        return m_version;
    };

	/** Returns the wxSerializeStatus object. You usually do this when you just read a stream, to get more
	    information about it later, like the version, header, and if the stream contained newer data which
		means writing it back with the older data model you might be using, you destroy information */
	const wxSerializeStatus &GetArchiveStatus() const {
		return m_status;
	};
    /// return the version string of the class wxSerialize
    static wxString GetLibVersionString ();
    /// return the major version number of the class wxSerialize
    static long GetLibVersionMajor ();
    /// return the minor version number of the class wxSerialize
    static long GetLibVersionMinor ();

private:
    // error types
	enum
	{
	    wxSERIALIZE_ERR_OK  =  0,
	    wxSERIALIZE_ERR_EOF = -1,
	    wxSERIALIZE_ERR_ILL = -2,
	    wxSERIALIZE_ERR_MEM = -3
	};

	// all errors that can occur
	enum
	{
		wxSERIALIZE_ERR_STR_HEADER_s1_s2,         /// Illegal header in start of stream
		wxSERIALIZE_ERR_STR_WRONGVERSION_s1_s2,   /// Not a proper version
		wxSERIALIZE_ERR_STR_BADISTREAM,           /// Bad input stream
		wxSERIALIZE_ERR_STR_BADOSTREAM,           /// Bad output stream
		wxSERIALIZE_ERR_STR_NOVERHDR,             /// Cannot write version or header info to stream
		wxSERIALIZE_ERR_STR_NOHEADER_s1,          /// Header expected but not found
		wxSERIALIZE_ERR_STR_NOVERSION,            /// No version found in stream
		wxSERIALIZE_ERR_STR_NOWRITE,              /// Cannot write in read mode
		wxSERIALIZE_ERR_STR_NOREAD,               /// Cannot read in write mode
        wxSERIALIZE_ERR_STR_EOF, 	                /// End of file error in read mode
        wxSERIALIZE_ERR_STR_WRONGCHUNK_s1_s2,     /// Expected header x but got y
        wxSERIALIZE_ERR_STR_MEMORY_s1,            /// Cannot allocate x bytes of memory
		wxSERIALIZE_ERR_STR_READSIZE,				/// Read size mismatch
		wxSERIALIZE_ERR_STR_RINTSIZE,				/// Integer size mismatch
		wxSERIALIZE_ERR_STR_SINTSIZE,				/// Ditto
		wxSERIALIZE_ERR_STR_ILL_LEAVE,			/// Illegal leave marker found
		wxSERIALIZE_ERR_STR_ILL_UNKNOWN_HDR_s1,	/// Unknown header error
		wxSERIALIZE_ERR_STR_ILL_LEVEL				/// Sync level failure!
	};

	// headers
	enum
	{
		wxSERIALIZE_HDR_BOOL      = 'b',
		wxSERIALIZE_HDR_INT8      = 'c',
		wxSERIALIZE_HDR_INT16     = 'w',
		wxSERIALIZE_HDR_INT32     = 'l',
		wxSERIALIZE_HDR_INT64     = 'q',
		wxSERIALIZE_HDR_DOUBLE    = 'd',
		wxSERIALIZE_HDR_STRING    = 's',
		wxSERIALIZE_HDR_RECORD    = 'r',
		wxSERIALIZE_HDR_INT       = 'i',
		wxSERIALIZE_HDR_ARRSTRING = 'a',
		wxSERIALIZE_HDR_DATETIME  = 't',
		wxSERIALIZE_HDR_ENTER     = '<',
		wxSERIALIZE_HDR_LEAVE     = '>',
		wxSERIALIZE_HDR_INTINT    = 'I'
	};

    // pre-check if stream is capable of writing, returns errors
    // when condition is not good (like input stream is defined
    // instead of output)
    bool CanStore();

    // pre-check if stream is capable of reading, returns errors
    // when condition is not good (like output stream is defined
    // instead of input)
    bool CanLoad();

	// loads the header, and sets an error when the expected header is not the same
	// as the header given
	int LoadChunkHeader(int expheader);

    // writes a single char to the stream
	void WriteChar(wxUint8 value);

	// sets an error with arbitrary message using wxLogError
	int LogError(int err, int msgcode, const wxString &s1 = wxEmptyString,
	                                   const wxString &s2 = wxEmptyString);

	// attempts to find the marker that belongs to this
	// current level. We assume the current count but increment when we
	// find one, decrement when we find another ..
	void FindCurrentEnterLevel();

	// finds current leave level. This means all enter levels we encounter
	// we will skip, until we have the level of our object - 1
	void FindCurrentLeaveLevel();

	// skip the data belonging by the header
	void SkipData(wxUint8 hdr);

	// load the buffer from a stream
	void Load(wxMemoryBuffer &buf);

    // reads a string (without header)
	wxString LoadString();

    // reads an array string without header
	wxArrayString LoadArrayString();

    // reads a datetime without header
	wxDateTime LoadDateTime();

    // reads a single char from the stream, returns '\0' when none can be read
	wxUint8 LoadChar();

    // see wxUint8 LoadChar()
    wxUint8 LoadUint8();

	// reads a wxUint16 (without header)
	wxUint16 LoadUint16();

	// reads a wxUint32 (without header)
	wxUint32 LoadUint32();

	// reads a wxUint64 (without header)
	wxUint64 LoadUint64();

    // reads a wxFloat64 (without header)
	wxFloat64 LoadDouble();

	// loads integer
	int LoadInt();

    // loads pair of integers
    void LoadIntInt (int& value1, int& value2);

	// reads a bool (without header)
	bool LoadBool();

    // writes a wxUint8 without header
    void SaveChar(wxUint8 value);

    // see SaveChar(wxUint8)
    void SaveUint8(wxUint8 value);

	// writes a wxUint16 (without header)
	void SaveUint16(wxUint16 value);

	// writes a wxUint32 (without header)
	void SaveUint32(wxUint32 value);

	// writes a wxUint64 (without header)
	void SaveUint64(wxUint64 value);

    // writes a string (without header)
	void SaveString(const wxString &str);

    // translates header name to something readable for errors
	wxString GetHeaderName(int headername);

	/** Sets the error to a specific condition. Valid error conditions are
		- wxSERIALIZE_OK
	    - wxSERIALIZE_ERR_EOF
	    - wxSERIALIZE_ERR_ILL
	    - wxSERIALIZE_ERR_MEM

	    The error will not change from the last error condition when it is
	    already set. Meaning it will keep the last error, even when others
	    follow.
	*/
	void SetError(int err) {
		// only set error when we are still ok
		if(m_errorCode == wxSERIALIZE_ERR_OK)
			m_errorCode = err;
	};

	// initialize function
	void InitAll();

private:
	// last error code issued in wxSerialize
	int m_errorCode;
	// the last logged error message
	wxString m_errMsg;
	// the header received
	wxString m_headerStr;
	// The version written or read
	size_t m_version;
	// opened status (true if open) Close() will close it
	bool m_opened;
	// partial save or load mode
	bool m_partialMode;
	// write or read mode of the stream
	bool m_writeMode;
	// under the hood output data stream (this stream is maintained by us)
	wxOutputStream &m_odstr;
	// under the hood output data stream (this stream is maintained by us)
	wxInputStream &m_idstr;
	// object marker level. Any level above 0 means we have encountered
	// an EnterObject and LeaveObject.
	int m_objectLevel;
	// flag to indicate that we encountered a LEAVE or ENTER marker. This means
	// no read should be done until we reached the LeaveObject() that
	// belongs to our level.
	bool m_haveBoundary;
	// last boundary encountered
	unsigned char m_lastBoundary;
	// temp vars that have only the purpose to initialize streams that
	// need to be initialized by ref. By using wxStringStream variants
	// we can safely read and load from them when this error might occur
	wxString m_tmpostr, m_tmpistr;
	wxStringOutputStream m_otmp;
	wxStringInputStream m_itmp;
	// The archive status object
	wxSerializeStatus m_status;
};

#endif


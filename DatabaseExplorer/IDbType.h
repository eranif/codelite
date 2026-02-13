//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : IDbType.h
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

#ifndef IDBTYPE_H
#define IDBTYPE_H

#include "wx/wxxmlserializer/XmlSerializer.h"

class IDbType : public xsSerializable
{
public:
	IDbType(){}
	IDbType(const IDbType& obj):xsSerializable(obj){}

	/*! \brief Basic dbType parameter */
	enum PROPERTY
	{
		/*! \brief Enable parameter Not NULL */
		dbtNOT_NULL = 1,
		/*! \brief Enable parameter SIZE*/
		dbtSIZE = 2,
		/*! \brief Enable parameter second SIZE */
		dbtSIZE_TWO = 4,
		/*! \brief Enable parameter PRIMARY KEY */
		dbtPRIMARY_KEY = 8,
		/*! \brief Enable parameter UNIQUE */
		dbtUNIQUE = 16,
		/*! \brief Enable parameter AUTO_INCREMENT */
		dbtAUTO_INCREMENT = 32		
		};
	
	/*! \brief Basic type for conversion */
	enum UNIVERSAL_TYPE
	{
		dbtTYPE_INT = 1,
		dbtTYPE_FLOAT = 2,
		dbtTYPE_DECIMAL = 3,
		dbtTYPE_TEXT = 4,
		dbtTYPE_DATE_TIME = 5,
		dbtTYPE_BOOLEAN = 6,
		dbtTYPE_OTHER = 7
		};

		virtual wxString GetTypeName() = 0;
		
		// size
		virtual bool HaveSize() = 0;
		virtual long GetSize() = 0;
		virtual void SetSize(long size) = 0;
		
		// size2
		virtual bool HaveSize2() = 0;
		virtual long GetSize2() = 0;
		virtual void SetSize2(long size) = 0;
		
		
		// primary key
		virtual bool HavePrimaryKey() = 0;
		virtual bool GetPrimaryKey() = 0;
		virtual void SetPrimaryKey(bool primaryKey) = 0;
		
		// unique
		virtual bool HaveUnique() = 0;
		virtual bool GetUnique() = 0;
		virtual void SetUnique(bool unique) = 0;
		
		// not null
		virtual bool HaveNotNull() = 0;
		virtual bool GetNotNull() = 0;
		virtual void SetNotNull(bool notNull) = 0; 
		
		// auto increment
		virtual bool HaveAutoIncrement() = 0;
		virtual bool GetAutoIncrement() = 0;
		virtual void SetAutoIncrement(bool autoIncrement) = 0;
		
		// sql
		virtual wxString ReturnSql() = 0;
		
		virtual long GetPropertyFlags() = 0;
		virtual void SetPropertyFlags(long flags) = 0;
		virtual UNIVERSAL_TYPE GetUniversalType() = 0;
	
};


#endif
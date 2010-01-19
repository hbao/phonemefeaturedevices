/**
 * Copyright (c) 2004-2008 Blue Whale Systems Ltd. All Rights Reserved. 
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER 
 *  
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License version 
 * 2 only, as published by the Free Software Foundation.  
 *  
 * This software is provided "as is," and the copyright holder makes no representations or warranties, express or
 * implied, including but not limited to warranties of merchantability or fitness for any particular purpose or that the
 * use of this software or documentation will not infringe any third party patents, copyrights, trademarks or other
 * rights.
 * 
 * The copyright holder will not be liable for any direct, indirect special or consequential damages arising out of any
 * use of this software or documentation.
 * 
 * See the GNU  General Public License version 2 for more details 
 * (a copy is included at /legal/license.txt).  
 *  
 * You should have received a copy of the GNU General Public License 
 * version 2 along with this work; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 
 * 02110-1301 USA  
 *  
 * Please contact Blue Whale Systems Ltd, Suite 123, The LightBox
 * 111 Power Road, Chiswick, London, W4 5PY, United Kingdom or visit 
 * www.bluewhalesystems.com if you need additional 
 * information or have any questions.  
 */ 



#ifndef __DATA_ACCESS_STATEMENT_IMPL_H__
#define __DATA_ACCESS_STATEMENT_IMPL_H__

#include "MessagingDataAccessConnection.h"
#include "PropertiesImpl.h"
#include <s32file.h>

class CDataAccessConnection;




class CDataAccessStatement : public CProperties, public MDataAccessStatement
{
public:
	/**
	 * Calls AddRef on aDataAccessConnection.
	 */
	static MDataAccessStatement * NewL(CDataAccessConnection * aDataAccessConnection);

public: // MUnknown.
	virtual MUnknown * QueryInterfaceL( TInt aInterfaceId );
	void AddRef() { CEComPlusRefCountedBase::AddRef(); }
	void Release() { CEComPlusRefCountedBase::Release(); }

public: // MDataAccessStatement.
	virtual void SetType( const TStatementType & aStatementType );
	virtual MUnknown * ExecuteL( TInt aInterfaceId );


public: // MProperties implementation.
	virtual TInt GetIntL(const TIntUniqueKey & aKey, TInt aOrdinal ) const
	{
		return CProperties::GetIntL(aKey, aOrdinal);
	}
	virtual void SetIntL(const TIntUniqueKey & aKey, TInt aIntegerValue, TInt aOrdinal)
	{
		CProperties::SetIntL(aKey, aIntegerValue, aOrdinal);
	}
	virtual TBool DeleteInt(const TIntUniqueKey & aKey, TInt aOrdinal)
	{
		return CProperties::DeleteInt(aKey, aOrdinal);
	}

	virtual TInt64 GetInt64L(const TInt64UniqueKey & aKey, TInt aOrdinal) const
	{
		return CProperties::GetInt64L(aKey, aOrdinal);
	}
	virtual void SetInt64L(const TInt64UniqueKey & aKey, TInt64 aIntegerValue, TInt aOrdinal)
	{
		CProperties::SetInt64L(aKey, aIntegerValue, aOrdinal);
	}
	virtual TBool DeleteInt64(const TInt64UniqueKey & aKey, TInt aOrdinal)
	{
		return CProperties::DeleteInt64(aKey, aOrdinal);
	}

	virtual const TDesC8 & GetString8L(const TString8UniqueKey & aKey, TInt aOrdinal) const
	{
		return CProperties::GetString8L(aKey, aOrdinal);
	}
	virtual HBufC8 * GetStringBuffer8L(const TString8UniqueKey & aKey, TInt aOrdinal) const
	{
		return CProperties::GetStringBuffer8L(aKey, aOrdinal);
	}
	virtual void SetString8L(const TString8UniqueKey & aKey, HBufC8 * aBuffer, TInt aOrdinal)
	{
		CProperties::SetString8L(aKey, aBuffer, aOrdinal);
	}
	virtual void SetString8L(const TString8UniqueKey & aKey, const TDesC8 & aStringValue, TInt aOrdinal)
	{
		CProperties::SetString8L(aKey, aStringValue, aOrdinal);
	}
	virtual TBool DeleteString8(const TString8UniqueKey & aKey, TInt aOrdinal)
	{
		return CProperties::DeleteString8(aKey, aOrdinal);
	}

	virtual const TDesC & GetStringL(const TStringUniqueKey & aKey, TInt aOrdinal) const
	{
		return CProperties::GetStringL(aKey, aOrdinal);
	}
	virtual HBufC * GetStringBufferL(const TStringUniqueKey & aKey, TInt aOrdinal) const
	{
		return CProperties::GetStringBufferL(aKey, aOrdinal);
	}
	virtual void SetStringL(const TStringUniqueKey & aKey, HBufC * aBuffer, TInt aOrdinal)
	{
		CProperties::SetStringL(aKey, aBuffer, aOrdinal);
	}
	virtual void SetStringL(const TStringUniqueKey & aKey, const TDesC & aStringValue, TInt aOrdinal)
	{
		CProperties::SetStringL(aKey, aStringValue, aOrdinal);
	}
	virtual TBool DeleteString(const TStringUniqueKey & aKey, TInt aOrdinal)
	{
		return CProperties::DeleteString(aKey, aOrdinal);
	}
	
	virtual void SetObjectL(const TObjectUniqueKey & aKey, MUnknown * aObject /* IN */, TInt aOrdinal)
	{
		CProperties::SetObjectL(aKey, aObject, aOrdinal);
	}
	virtual MUnknown * GetObjectL(const TObjectUniqueKey & aKey, TInt aInterfaceId, TInt aOrdinal )
	{
		return CProperties::GetObjectL(aKey, aInterfaceId, aOrdinal);
	}
	virtual TBool DeleteObject(const TObjectUniqueKey & aKey, TInt aOrdinal)
	{
		return CProperties::DeleteObject(aKey, aOrdinal);
	}

	virtual TBool Dirty() const
	{
		return CProperties::Dirty();
	}

	virtual void SetDirty(TBool aDirty)
	{
		CProperties::SetDirty(aDirty);
	}

	virtual void Reset()
	{
		CProperties::Reset();
	}

	virtual void DebugPrintL(TInt aNestingLevel) const
	{
		CProperties::DebugPrintL(aNestingLevel);
	}


protected:

	CDataAccessStatement(TAny * aConstructionParameters);
	void ConstructL(CDataAccessConnection * aDataAccessConnection);
	virtual ~CDataAccessStatement();

	CDataAccessConnection * iDataAccessConnection; // Owned.
	
	TStatementType iStatementType;


};

#endif /* __DATA_ACCESS_STATEMENT_IMPL_H__ */

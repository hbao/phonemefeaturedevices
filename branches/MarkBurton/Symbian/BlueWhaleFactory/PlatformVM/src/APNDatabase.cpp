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
#include <e32base.h>
#include "APNDatabase.h"

#ifdef __WINSCW__
#define EXPORT_DECL EXPORT_C 
#else
#define EXPORT_DECL 
#endif

EXPORT_DECL CAPNDatabase::CAPNDatabase() : iDatabase(5)
{}

EXPORT_DECL CAPNDatabase::~CAPNDatabase()
{}

EXPORT_DECL void CAPNDatabase::LoadDatabaseL()
{
	_LIT(KUnitedKingdom,"234");
	_LIT(KRepublicOfIreland,"272");
	
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("10"),_L("O2"),_L("mobile.o2.co.uk"),_L("bypass"),_L("web"))));
	
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("15"),_L("Vodafone"),_L("internet"),_L("web"),_L("web"))));
    User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("15"),_L("Vodafone PP"),_L("pp.vodafone.co.uk"),_L("web"),_L("web"))));
    
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("20"),_L("3"),_L("three.co.uk"),_L("guest"),_L("guest"))));
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("30"),_L("T-Mobile"),_L("general.t-mobile.uk"),_L("user"),_L("pass"))));
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("33"),_L("Orange"),_L("orangeinternet"),_L(""),_L("pass"))));
	
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("50"),_L("Wave Telecom"),_L("pepper"),_L(""),_L(""))));
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("55"),_L("Sure Mobile"),_L("internet"),_L(""),_L("pass"))));
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KUnitedKingdom,_L("58"),_L("Manx Telecom"),_L("3gpronto"),_L(""),_L(""))));

	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KRepublicOfIreland,_L("01"),_L("Vodafone Ireland"),_L("hs.vodafone.ie"),_L("vodafone"),_L("vodafone"))));
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KRepublicOfIreland,_L("01"),_L("Vodafone Ireland"),_L("isp.vodafone.ie"),_L("vodafone"),_L("vodafone"))));
	User::LeaveIfError(iDatabase.Append(TOperatorAPN(KRepublicOfIreland,_L("02"),_L("O2 Ireland"),_L("internet"),_L(""),_L(""))));

}

EXPORT_DECL TInt CAPNDatabase::Count()
{
	return iDatabase.Count();	
}

EXPORT_DECL void CAPNDatabase::Close()
{
	iDatabase.Close();
}

TOperatorAPN& CAPNDatabase::GetEntry(TInt aIndex)
{
	return iDatabase[aIndex];
}

EXPORT_DECL TInt CAPNDatabase::GetEntry(const TDesC& aCountryCode,const TDesC& aOperatorCode)
{
	TOperatorAPN key(aCountryCode,aOperatorCode,_L(""),_L(""),_L(""),_L(""));
	TIdentityRelation<TOperatorAPN> byOperator(ByOperator);
	TInt index = iDatabase.Find(key,byOperator);
	return index;
}

TBool CAPNDatabase::ByOperator(const TOperatorAPN& aLeft,const TOperatorAPN& aRight)
{
	if(aLeft.iNetworkId.Compare(aRight.iNetworkId) == 0)
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

EXPORT_DECL TInt CAPNDatabase::GetNext(const TDesC& aCountryCode,const TDesC& aOperatorCode, TInt aIndex)
{
    TInt ret = KErrNotFound;
    TInt count = iDatabase.Count();
    for(TInt i=aIndex;i<count;i++)
    {
        if(aCountryCode.Compare(iDatabase[i].iCountryCode) == 0 &&
           aOperatorCode.Compare(iDatabase[i].iNetworkId) == 0)
        {
            ret = i;
            break;
        }
                
    }
    return ret;
}

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
#ifdef _DEBUGTERMINAL_
#include <e32debug.h>
#include "logger.h"
#include "SocketEngine.h"
#include "DebugTerminal.h"
#include "Application.h"
#include "BuildVersion.h"
#include "EventQueue.h"

const TUint KPort = 8182;

CDebugTerminal::~CDebugTerminal()
{	
	if(iConnection)
	{
		iConnection->Release();
		iConnection = NULL;
	}
	if(iProperties)
	{
		iProperties->Release();
		iProperties = NULL;
	}
	delete iCommand;
	TInt count = iCommands.Count();
	for(TInt i=0;i<count;i++)
	{
		MCommand* cmd = iCommands[i];
		delete cmd;
	}
	iCommands.Close();
}
	
CDebugTerminal::CDebugTerminal(TAny * aConstructionParameters,MTimerStateMachine* aParent) : CEComPlusRefCountedBase(aConstructionParameters),iParent(aParent)
{}

void CDebugTerminal::ConstructL()
{
}

MUnknown * CDebugTerminal::QueryInterfaceL( TInt aInterfaceId )
{
	if(aInterfaceId == KIID_MConnectionCallback)
	{
		AddRef();
		return static_cast<MConnectionCallback*>(this);
	}
	else if(aInterfaceId == KIID_MStateMachineCallback)
	{
		AddRef();
		return static_cast<MStateMachineCallback*>(this);
	}
	else
	{
		return CEComPlusRefCountedBase::QueryInterfaceL(aInterfaceId);
	}
}

void CDebugTerminal::CallbackCommandL( MStateMachine::TCommand aCommand )
{
	RDebug::Print(_L("CDebugTerminal::CallbackCommandL"));
	if(aCommand == KCommandStateMachineFinishedNegotiatingConnection)
	{
		TBuf8<256> buffer;
		buffer.Format(_L8("Welcome to BlueWhalePlatform %S\r\n"),&(BLUEWHALEPLATFORM_SOFTWARE_BUILD_VERSION()));
		iConnection->WriteL(buffer);
	}
	else if(aCommand == KCommandStateMachineAcceptReadBuffer)
	{
		HBufC8* buffer = iProperties->GetStringBuffer8L(KPropertyString8ConnectionReadBuffer);
		if(iCommand->Des().Length() + buffer->Des().Length() < 128)
		{
			iCommand->Des().Append(buffer->Des());
			TInt lineFeed = iCommand->Des().Find(_L8("\n"));
			while(lineFeed != -1)
			{
				iCommand->Des().Delete(lineFeed,1);
				lineFeed = iCommand->Des().Find(_L8("\n"));
			}
			TInt pos = iCommand->Des().Find(_L8("\r"));
			while(pos != -1)
			{
				TPtrC8 command = iCommand->Des().Left(pos); 
				TInt count = iCommands.Count();
				if(command.Left(4).Compare(_L8("help")) == 0)
				{
					iConnection->WriteL(_L8("Current commands:\r\n"));
					for(TInt i=0;i<count;i++)
					{
						iCommands[i]->WriteHelpText(this);
					}
				}
				else
				{
					TBool handled = EFalse;
					for(TInt i=0;i<count;i++)
					{
						if(iCommands[i]->AcceptCommand(command,this))
						{
							handled = ETrue;
							break;
						}
					}
					if(!handled)
					{
						iConnection->WriteL(_L8("Error: Syntax error\r\n"));
					}

				}
				iCommand->Des().Delete(0,pos+1);
				pos = iCommand->Des().Find(_L8("\r"));
			}

		}
	}
}

void CDebugTerminal::ReportStateChanged( TInt aComponentId, MStateMachine::TState aState )
{
	RDebug::Print(_L("CDebugTerminal::ReportStateChanged"));
}

void CDebugTerminal::ReportError(TErrorType aErrorType, TInt aErrorCode)
{
	RDebug::Print(_L("CDebugTerminal::ReportError %d"),aErrorCode);
	if(aErrorCode == KErrEof)
	{
		iConnection->AcceptCommandL(KCommandConnectionServerListen);
	}
}

void CDebugTerminal::StartL(RThread& aThread)
{
	iProperties = DiL( MProperties );
	MConnectionCallback * connectionCallback = QiL(this,MConnectionCallback);
	CleanupReleasePushL(*connectionCallback);
	iProperties->SetObjectL(KPropertyObjectStateMachineCallback, connectionCallback);
	CleanupStack::PopAndDestroy(connectionCallback);
	
	MLogger * logger = DiL(MLogger );
	CleanupReleasePushL( *logger );
	logger->SetLoggingLevel(MLogger::EError);
	iProperties->SetObjectL( KPropertyObjectLogger, logger );
	CleanupStack::PopAndDestroy(logger);

	enum {KReadBufferSize = 2000};
	HBufC8 * readBuffer = HBufC8::NewLC(KReadBufferSize);
	iProperties->SetString8L(KPropertyString8ConnectionReadBuffer, readBuffer);
	CleanupStack::Pop(readBuffer); // Now owned by iProperties.
	
	iConnection = CiL( KCID_MSocketEngine, MWritableConnection );
	iConnection->InitializeL(iProperties);
	
	iProperties->SetObjectL(KPropertyObjectVMWritableConnection,iConnection);
	
	iCommands.Append(new CVersion());
	iCommands.Append(new CKill());
	iCommands.Append(new CResource());
	iCommands.Append(new CKey());
	iCommands.Append(new CCommand());
	iCommands.Append(new CNetworkFail());
	
	iCommand = HBufC8::NewL(128);
	iProperties->SetIntL(KPropertyIntSocketConnectionIap, 0);
	iProperties->SetIntL(KPropertyIntSocketPort,KPort);
	iConnection->AcceptCommandL(KCommandConnectionServerListen);
}

void CDebugTerminal::StopL()
{
	iConnection->AcceptCommandL(KCommandConnectionDisconnect);
	iConnection->Release();
	iConnection = NULL;
	iProperties->Release();
	iProperties = NULL;
}

void CDebugTerminal::SetThread(MThread* aThread)
{}


/////////////////////////////////////////////////////////////////////////////////////

TBool CVersion::AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal)
{
	if(aCmd.Compare(_L8("version")) == 0)
	{
		TBuf8<256> buffer;
		buffer.Format(_L8("version %S\r\n"),&(BLUEWHALEPLATFORM_SOFTWARE_BUILD_VERSION()));
		aTerminal->Connection()->WriteL(buffer);
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

void CVersion::WriteHelpText(CDebugTerminal* aTerminal)
{
	aTerminal->Connection()->WriteL(_L8("version:\t\tDisplay the current build version\r\n"));
}

////////////////////////////////////////////////////////////////////////////////////////////
TBool CKill::AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal)
{
	if(aCmd.Compare(_L8("kill")) == 0)
	{
		aTerminal->Connection()->WriteL(_L8("killing...\r\n"));
		aTerminal->Parent()->AcceptCommandL(KCommandKill,NULL);
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

void CKill::WriteHelpText(CDebugTerminal* aTerminal)
{
	aTerminal->Connection()->WriteL(_L8("kill:\t\tForce the VM statemachine to close\r\n"));
}
////////////////////////////////////////////////////////////////////////////////////////////
TBool CResource::AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal)
{
	if(aCmd.Compare(_L8("resource")) == 0)
	{
#if 0
		HBufC* buffer = HBufC::New(3000);
		if(buffer)
		{
			aTerminal->Parent()->Application()->DebugResources(buffer->Des());
			HBufC8* buffer8 = HBufC8::New(3000);
			if(buffer8)
			{
				buffer8->Des().Copy(buffer->Des());
				aTerminal->Connection()->WriteL(buffer8->Des());
				delete buffer8;
			}
			delete buffer;
		}
#endif
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

void CResource::WriteHelpText(CDebugTerminal* aTerminal)
{
	aTerminal->Connection()->WriteL(_L8("resource:\t\tDisplay current native resource usage\r\n"));
}
////////////////////////////////////////////////////////////////////////////////////////////
TBool CKey::AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal)
{
	TBool ret = EFalse;
	RWsSession ws;
	if(ws.Connect() == KErrNone)
	{
		if(aCmd.Left(11).Compare(_L8("key down 0x")) == 0)
		{
			TLex8 lexer(aCmd.Mid(11));
			TUint key;
			if(lexer.Val(key,EHex) == KErrNone)
			{
				TRawEvent event;
				event.Set(TRawEvent::EKeyDown, key);
				ws.SimulateRawEvent(event);
				aTerminal->Connection()->WriteL(_L8("OK\r\n"));
			}
			else
			{
				aTerminal->Connection()->WriteL(_L8("Error: parsing number\r\n"));
			}
			ret = ETrue;
		}
		else if(aCmd.Left(9).Compare(_L8("key up 0x")) == 0)
		{
			TLex8 lexer(aCmd.Mid(9));
			TUint key;
			if(lexer.Val(key,EHex) == KErrNone)
			{
				TRawEvent event;
				event.Set(TRawEvent::EKeyUp, key);
				ws.SimulateRawEvent(event);
				aTerminal->Connection()->WriteL(_L8("OK\r\n"));
			}
			else
			{
				aTerminal->Connection()->WriteL(_L8("Error: parsing number\r\n"));
			}
			ret = ETrue;
		}
		ws.Close();
	}
	return ret;
}

void CKey::WriteHelpText(CDebugTerminal* aTerminal)
{
	aTerminal->Connection()->WriteL(_L8("key code:\t\tSimulate a key press\r\n"));
}

////////////////////////////////////////////////////////////////////////////////////////////
TBool CCommand::AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal)
{
	TBool ret = EFalse;
	if(aCmd.Left(4).Compare(_L8("cmd ")) == 0)
	{
		TLex8 lexer(aCmd.Mid(4));
		TUint cmd;
		if(lexer.Val(cmd,EDecimal) == KErrNone)
		{
			TEventInfo event;
			event.iIntParam1 = cmd;
			event.iEvent = KMIDPCommandEvent;
			aTerminal->Parent()->Application()->AddEvent(event);
			aTerminal->Connection()->WriteL(_L8("OK\r\n"));
		}
		else
		{
			aTerminal->Connection()->WriteL(_L8("Error: Parsing number\r\n"));
		}
		ret = ETrue;
	}

	return ret;
}

void CCommand::WriteHelpText(CDebugTerminal* aTerminal)
{
	aTerminal->Connection()->WriteL(_L8("cmd code:\t\tSend cmd command to midlet\r\n"));
}
////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUGFAILAPI_
extern TBool gEnabled;
extern TInt gRate;
#endif

TBool CNetworkFail::AcceptCommand(const TDesC8& aCmd,CDebugTerminal* aTerminal)
{
	if(aCmd.Left(9).Compare(_L8("net fail ")) == 0)
	{
		TLex8 lexer(aCmd.Mid(9));
		TUint rate;
		if(lexer.Val(rate,EDecimal) == KErrNone)
		{
			aTerminal->Connection()->WriteL(_L8("OK\r\n"));
#ifdef _DEBUGFAILAPI_
			gEnabled = ETrue;
			gRate = rate;
#endif
		}
		else
		{
			aTerminal->Connection()->WriteL(_L8("Error: Parsing number\r\n"));
		}
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

void CNetworkFail::WriteHelpText(CDebugTerminal* aTerminal)
{
	aTerminal->Connection()->WriteL(_L8("net fail:\t\tEnable network forced errors\r\n"));
}
#endif // _DEBUGTERMINAL_

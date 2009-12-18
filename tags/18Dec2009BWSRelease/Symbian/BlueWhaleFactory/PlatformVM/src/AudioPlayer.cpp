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

#include <OSVersion.h>
#include "AudioPlayer.h"
#include <coemain.h>
#include <bautils.h>
#include "Application.h"
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>
#elif __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
#include <settinginfo.h>
#include <settinginfoids.h>
#elif __UIQ_VERSION_NUMBER >= __UIQ_V3_FP0_VERSION__
#include <hal.h>
#endif
const TInt KSilentProfile = 1;
const TInt KMeetingProfile = 2;
const TInt KStreamCloserDelay = 500000; // 0.5s

#include <mmf\server\mmfdes.h>

CAudioPlayer* CAudioPlayer::NewL()
{
	CAudioPlayer* self = new (ELeave) CAudioPlayer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CAudioPlayer::ConstructL()
{
	iWaiter = new (ELeave) CActiveSchedulerWait;
	iStreamCloser = CPeriodic::NewL(CActive::EPriorityIdle);
}

CAudioPlayer::~CAudioPlayer()
{
	delete iOutputStream;
	delete iStreamCloser;
	delete iWaiter;
}

TBool CAudioPlayer::ProfileAllowsSoundL()
{
	TBool result = ETrue;
#if __S60_VERSION__ >= __S60_V3_FP0_VERSION_NUMBER__
	CRepository* repository = CRepository::NewLC(KCRUidProfileEngine);
	TInt value = KErrNotFound;
	User::LeaveIfError(repository->Get(KProEngActiveProfile, value));
	CleanupStack::PopAndDestroy(repository);
	if ((value == KSilentProfile) || (value == KMeetingProfile))
	{
		result = EFalse;
	}
#elif __S60_VERSION__ >= __S60_V1_FP2_VERSION_NUMBER__
	CSettingInfo* settingInfo = CSettingInfo::NewL(NULL);
	CleanupStack::PushL(settingInfo);
	TInt value = KErrNotFound;
	User::LeaveIfError(settingInfo->Get(SettingInfo::EActiveProfile, value));
	CleanupStack::PopAndDestroy(settingInfo);
	if ((value == KSilentProfile) || (value == KMeetingProfile))
	{
		result = EFalse;
	}
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	result = !BaflUtils::FileExists(CCoeEnv::Static()->FsSession(), _L("c:\\shared\\silentmode.txt"));
#endif
	return result;
}

TInt CAudioPlayer::StreamCloseCallback(TAny* aThis)
{
	CAudioPlayer* This = static_cast<CAudioPlayer*>(aThis);
	TTimeIntervalMicroSeconds position = This->iOutputStream->Position();
	if ((position == This->iLastPosition) && (position > TTimeIntervalMicroSeconds(0)))
	{
		This->iStreamCloser->Cancel();
		This->iLastPosition = TTimeIntervalMicroSeconds(0);
		This->iOutputStream->Stop();
	}
	else
	{
		This->iLastPosition = position;
	}
	return ETrue;
}

void CAudioPlayer::StartL(const TDesC8& aType, TPtr8& aData)
{
	iStreamCloser->Cancel();

#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	TInt uid = 0;
	HAL::Get(HALData::EMachineUid, uid);
	if (uid == KUidZ10Value)
	{
		return; // playing sounds causes Moto Z10 crashes
	}
#endif

	if (!ProfileAllowsSoundL())
	{
		return;
	}
	
	// find a controller that can play this data
	RMMFControllerImplInfoArray controllers;
	CleanupClosePushL(controllers);
	CMMFFormatSelectionParameters* formatParams = CMMFFormatSelectionParameters::NewLC();
	formatParams->SetMatchToMimeTypeL(aType);
	CMMFControllerPluginSelectionParameters* pluginParams = CMMFControllerPluginSelectionParameters::NewLC();
	pluginParams->SetRequiredPlayFormatSupportL(*formatParams);
	pluginParams->ListImplementationsL(controllers);
	if (controllers.Count() == 0)
	{
		User::Leave(KErrNotSupported);
	}
	TUid controllerUid = controllers[0]->Uid();
	CleanupStack::PopAndDestroy(pluginParams);
	CleanupStack::PopAndDestroy(formatParams);
	CleanupStack::PopAndDestroy(&controllers);

	TUint sampleRate = 0;
	TUint numChannels = 0;
#if __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
	// RMMFController::AddDataSource fails with -5 on UIQ so use hardcoded values to get BlueWhale.mp3 working
	// If we later implement generic alerts we'll need to revisit this
	sampleRate = 44100;
	numChannels = 2;
#else
	// use the controller to get the data's sample rate and number of channels
	RMMFController controller;
	TMMFPrioritySettings prioritySettings;
	prioritySettings.iPriority = EMdaPriorityNormal;
	prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;

	User::LeaveIfError(controller.Open(controllerUid, prioritySettings));
	CleanupClosePushL(controller);
	TMMFMessageDestination dataSource;
	TMMFDescriptorConfig sourceCfg;
	sourceCfg().iDes = &aData;
	sourceCfg().iDesThreadId = RThread().Id();

	User::LeaveIfError(controller.AddDataSource(KUidMmfDescriptorSource, sourceCfg, dataSource));
	RMMFAudioControllerCustomCommands customCommands(controller);
	customCommands.GetSourceSampleRate(sampleRate);
	customCommands.GetSourceNumChannels(numChannels);
	CleanupStack::PopAndDestroy(&controller);
#endif
	
	delete iOutputStream;
	iOutputStream = NULL;
	iOutputStream = CMdaAudioOutputStream::NewL(*this);	// have to recreate this each time or OpenDesL will fail
	
	iSettings.Query();
	switch (sampleRate)
	{
		case 8000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
			break;
		case 11025:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate11025Hz;
			break;
		case 12000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate12000Hz;
			break;
		case 16000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
			break;
		case 22050:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate22050Hz;
			break;
		case 24000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate24000Hz;
			break;
		case 32000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate32000Hz;
			break;
		case 44100:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate44100Hz;
			break;
		case 48000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate48000Hz;
			break;
		case 64000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate64000Hz;
			break;
		case 96000:
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate96000Hz;
			break;
		default:
			User::Leave(KErrNotSupported);
	}

	switch (numChannels)
	{
		case 1:
			iSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
			break;
		case 2:
			iSettings.iChannels = TMdaAudioDataSettings::EChannelsStereo;
			break;
		default:
			User::Leave(KErrNotSupported);
	}
	iSettings.iVolume = iOutputStream->MaxVolume();

	if (aType.CompareF(_L8("audio/mpeg")) == 0)
	{
		iOutputStream->SetDataTypeL(KMMFFourCCCodeMP3);	// will leave on WINS due to no MP3 codec access
	}
	else if (aType.CompareF(_L8("audio/aac")) == 0)
	{
		iOutputStream->SetDataTypeL(KMMFFourCCCodeAAC);
	}

	iOutputStream->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceQuality);
	iOutputStream->Open(&iSettings);
	iWaiter->Start();
	User::LeaveIfError(iError);
	iOutputStream->WriteL(aData);
}

void CAudioPlayer::Stop()
{
	iOutputStream->Stop();
}

void CAudioPlayer::MaoscOpenComplete(TInt aError)
{
	iError = aError;
	iWaiter->AsyncStop();
}

void CAudioPlayer::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
{
	iError = aError;
	if (!iError && !iStreamCloser->IsActive())
	{
		// Symbian bug KIS000622 means that on a lot of devices MaoscPlayComplete isn't called
		// In this case we need to call Stop to close the audio hardware (eats battery)
		// Can't do it from this method (another Symbian bug!) so do it via a callback
		iLastPosition = TTimeIntervalMicroSeconds(0);
		TCallBack callBack(StreamCloseCallback, this);
		iStreamCloser->Start(TTimeIntervalMicroSeconds32(KStreamCloserDelay), TTimeIntervalMicroSeconds32(KStreamCloserDelay), callBack);
	}
}

void CAudioPlayer::MaoscPlayComplete(TInt aError)
{
	iError = aError;
}

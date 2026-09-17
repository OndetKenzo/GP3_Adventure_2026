/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#include "Platforms/AkPlatform_Switch/AkSwitchInitializationSettings.h"
#include "AkAudioDevice.h"
#include "Misc/Paths.h"

//////////////////////////////////////////////////////////////////////////
// UAkSwitchInitializationSettings

UAkSwitchInitializationSettings::UAkSwitchInitializationSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CommunicationSettings.DiscoveryBroadcastPort = FAkCommunicationSettings::DefaultDiscoveryBroadcastPort;
	CommunicationSettings.CommandPort = FAkCommunicationSettings::DefaultDiscoveryBroadcastPort + 1;
}

void UAkSwitchInitializationSettings::FillInitializationStructure(FAkInitializationStructure& InitializationStructure) const
{
	CommonSettings.FillInitializationStructure(InitializationStructure);
	CommunicationSettings.FillInitializationStructure(InitializationStructure);
	AdvancedSettings.FillInitializationStructure(InitializationStructure);

#if PLATFORM_SWITCH
	InitializationStructure.PlatformInitSettings.uSampleRate = CommonSettings.SampleRate;
	InitializationStructure.DeviceSettings.threadProperties.uStackSize = 1 * 1024 * 1024;
#if AK_ENABLE_COMMUNICATION
	InitializationStructure.CommSettings.bInitSystemLib = false;
#endif
#endif
}

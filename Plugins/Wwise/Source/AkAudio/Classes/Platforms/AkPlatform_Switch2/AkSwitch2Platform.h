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

#pragma once

#if defined(AK_PLATFORM_RUNTIME_SWITCH2) && AK_PLATFORM_RUNTIME_SWITCH2

#include "Platforms/AkPlatformBase.h"
#include "AkSwitch2InitializationSettings.h"

#define TCHAR_TO_AK(Text) (const ANSICHAR*)(TCHAR_TO_ANSI(Text))

using UAkInitializationSettings = UAkSwitch2InitializationSettings;

struct AKAUDIO_API FAkSwitch2Platform : FAkPlatformBase
{
	static const UAkInitializationSettings* GetInitializationSettings()
	{
		return GetDefault<UAkSwitch2InitializationSettings>();
	}

	static const FString GetPlatformBasePath()
	{
		return FString("Switch2");
	}

	static FString GetDSPPluginsDirectory(const FString& PlatformArchitecture);
};

using FAkPlatform = FAkSwitch2Platform;

#endif

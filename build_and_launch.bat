@echo off
setlocal

:: ============================================================
:: CONFIGURATION 
:: ============================================================
set UE_VERSION=5.6
for /f "tokens=2*" %%A in (
    'reg query "HKLM\SOFTWARE\EpicGames\Unreal Engine\5.6" /v "InstalledDirectory" 2^>nul'
) do set ENGINE_PATH=%%B
set PROJECT_PATH=%~dp0Adventure_MASK.uproject
set PROJECT_NAME=Adventure_MASK
set BUILD_CONFIG=Development
set PLATFORM=Win64
set TARGET=%PROJECT_NAME%Editor
set SOUNDBANKS_PATH=%~dp0Content\WwiseAudio

:: ============================================================
:: DERIVED PATHS 
:: ============================================================
set UBT="%ENGINE_PATH%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
set EDITOR="%ENGINE_PATH%\Engine\Binaries\Win64\UnrealEditor.exe"

echo.
echo [BUILD] Target:   %TARGET%
echo [BUILD] Config:   %BUILD_CONFIG%
echo [BUILD] Platform: %PLATFORM%
echo [BUILD] Project:  %PROJECT_PATH%
echo.

:: ============================================================
:: WWISE SOUNDBANK CHECK
:: ============================================================
echo [WWISE] Checking SoundBanks...
if not exist "%SOUNDBANKS_PATH%" (
    echo [WARNING] Wwise SoundBanks folder not found: %SOUNDBANKS_PATH%
    echo [WARNING] Audio may not work in editor. Generate banks from the Wwise editor first.
    echo.
) else (
    echo [WWISE] SoundBanks folder found. OK.
    echo.
)

:: ============================================================
:: RUN THE BUILD
:: ============================================================
%UBT% %TARGET% %PLATFORM% %BUILD_CONFIG% "%PROJECT_PATH%" -waitmutex

:: ============================================================
:: CHECK RESULT — only launch editor if build succeeded
:: ============================================================
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build FAILED with code %ERRORLEVEL%. Editor will not open.
    echo [ERROR] Check the output above for compiler errors.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Build completed successfully.
echo [LAUNCH]  Starting Unreal Editor...
echo.

start "" %EDITOR% "%PROJECT_PATH%"

exit /b 0

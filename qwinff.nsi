!include "MUI.nsh"
!define APPNAME "QWinFF"
!define VERSION "0.1.5"
!define DESCRIPTION "A cross platform media converter GUI"
!define UNINSTALLER "uninstall.exe"
!define LICENSE "license.txt"
!define CHANGELOG "changelog.txt"

Name "${APPNAME}"
OutFile "qwinff_${VERSION}-setup.exe"
InstallDir $PROGRAMFILES\QWinFF

RequestExecutionLevel admin

var StartMenuFolder

!insertmacro MUI_PAGE_LICENSE ${LICENSE}
!insertmacro MUI_PAGE_DIRECTORY

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\QWinFF"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchProgram"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME $INSTDIR\${CHANGELOG}
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Czech"

Section

	SetOutPath $INSTDIR

	# Program Files
	File qwinff.exe
	File presets.xml
	File *.dll
	File ${LICENSE}
	file ${CHANGELOG}

	CreateDirectory translations
	CreateDirectory ffmpeg

	SetOutPath $INSTDIR\translations
	File translations\*
	SetOutPath $INSTDIR\ffmpeg
	File ffmpeg\*
	SetOutPath $INSTDIR\sox
	File sox\*

	# Create Uninstaller
	WriteUninstaller "$INSTDIR\${UNINSTALLER}"

	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	# Start Menu Shortcuts
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortcut "$SMPROGRAMS\$StartMenuFolder\QWinFF.lnk" "$INSTDIR\qwinff.exe"
	CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall QWinFF.lnk" "$INSTDIR\${UNINSTALLER}"
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Function LaunchProgram
	ExecShell "" "$INSTDIR\qwinff.exe"
FunctionEnd

# Uninstaller

Section "un.Uninstaller"

	Delete $INSTDIR\${UNINSTALLER}

	Delete $INSTDIR\qwinff.exe
	Delete $INSTDIR\presets.xml
	Delete $INSTDIR\*.dll
	Delete $INSTDIR\${LICENSE}
	Delete $INSTDIR\${CHANGELOG}
	Delete $INSTDIR\ffmpeg\*
	RmDir  $INSTDIR\ffmpeg
	Delete $INSTDIR\translations\*
	RmDir  $INSTDIR\translations
	Delete $INSTDIR\sox\*
	RmDir  $INSTDIR\sox
	RmDir  $INSTDIR    # Remove the installation directory if it's empty.

	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	Delete "$SMPROGRAMS\$StartMenuFolder\*.lnk"
	RmDir  "$SMPROGRAMS\$StartMenuFolder"
	DeleteRegKey /ifempty HKLM "Software\QWinFF"

SectionEnd

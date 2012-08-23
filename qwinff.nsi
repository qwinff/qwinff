!include "MUI.nsh"
!define APPNAME "QWinFF"
!define VERSION "0.1.5"
!define DESCRIPTION "A cross platform media converter GUI"
!define UNINSTALLER "uninstall.exe"
!define LICENSE "license.txt"

Name "${APPNAME}"
OutFile "qwinff_${VERSION}-setup.exe"
InstallDir $PROGRAMFILES\QWinFF

!insertmacro MUI_PAGE_LICENSE ${LICENSE}
!insertmacro MUI_PAGE_DIRECTORY
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

	CreateDirectory translations
	CreateDirectory ffmpeg

	SetOutPath $INSTDIR\translations
	File translations\*
	setOutPath $INSTDIR\ffmpeg
	File ffmpeg\*


	# Create Uninstaller
	WriteUninstaller "$INSTDIR\${UNINSTALLER}"

	# Start Menu Shortcuts
	CreateDirectory "$SMPROGRAMS\${APPNAME}"
	CreateShortcut "$SMPROGRAMS\${APPNAME}\QWinFF.lnk" "$INSTDIR\qwinff.exe"
	CreateShortcut "$SMPROGRAMS\${APPNAME}\Uninstall QWinFF.lnk" "$INSTDIR\${UNINSTALLER}"

SectionEnd

# Uninstaller

Function un.onInit
	# confirm dialog
	MessageBox MB_YESNO "This will uninstall QWinFF from your system. Do you wish to continue?" IDYES next
	Abort
next:
FunctionEnd

Section "un.Uninstaller"

	Delete $INSTDIR\${UNINSTALLER}

	Delete $INSTDIR\qwinff.exe
	Delete $INSTDIR\presets.xml
	Delete $INSTDIR\*.dll
	Delete $INSTDIR\${LICENSE}
	Delete $INSTDIR\ffmpeg\*
	RmDir  $INSTDIR\ffmpeg
	Delete $INSTDIR\translations\*
	RmDir  $INSTDIR\translations
	RmDir  $INSTDIR    # Remove the installation directory if it's empty.

	Delete "$SMPROGRAMS\${APPNAME}\*"
	RmDir  "$SMPROGRAMS\${APPNAME}"

SectionEnd

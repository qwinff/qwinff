!define APPNAME "QWinFF"
!define VERSION "0.1.5"
!define DESCRIPTION "A cross platform media converter GUI"
!define UNINSTALLER "uninstall.exe"
!define LICENSE "license.txt"

Name "${APPNAME}"
OutFile "qwinff-${VERSION}-setup.exe"
XPStyle on

Page license
LicenseData ${LICENSE}

Page directory
InstallDir $PROGRAMFILES\QWinFF

Page instfiles
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
	CreateDirectory "$SMPROGRAMS\QWinFF"
	CreateShortcut "$SMPROGRAMS\QWinFF\QWinFF.lnk" "$INSTDIR\qwinff.exe"
	CreateShortcut "$SMPROGRAMS\QWinFF\Uninstall QWinFF.lnk" "$INSTDIR\${UNINSTALLER}"

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

	Delete "$SMPROGRAMS\QWinFF\*"
	RmDir  "$SMPROGRAMS\QWinFF"

SectionEnd

!include "MUI.nsh"
!define APPNAME "QWinFF"
!define VERSION "@QWINFF_VERSION@"
!define DESCRIPTION "A cross platform media converter GUI"
!define UNINSTALLER "uninstall.exe"
!define LICENSE "license.txt"
!define CHANGELOG "changelog.txt"

Name "${APPNAME}"
OutFile "qwinff_${VERSION}-setup.exe"
InstallDir $PROGRAMFILES\QWinFF

RequestExecutionLevel admin

var StartMenuFolder

!insertmacro MUI_PAGE_WELCOME
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

;Languages

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "SpanishInternational"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "Korean"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "Swedish"
!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "NorwegianNynorsk"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "Greek"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Portuguese"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "Ukrainian"
!insertmacro MUI_LANGUAGE "Slovak"
!insertmacro MUI_LANGUAGE "Croatian"
!insertmacro MUI_LANGUAGE "Bulgarian"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "Thai"
!insertmacro MUI_LANGUAGE "Romanian"
!insertmacro MUI_LANGUAGE "Latvian"
!insertmacro MUI_LANGUAGE "Macedonian"
!insertmacro MUI_LANGUAGE "Estonian"
!insertmacro MUI_LANGUAGE "Turkish"
!insertmacro MUI_LANGUAGE "Lithuanian"
!insertmacro MUI_LANGUAGE "Slovenian"
!insertmacro MUI_LANGUAGE "Serbian"
!insertmacro MUI_LANGUAGE "SerbianLatin"
!insertmacro MUI_LANGUAGE "Arabic"
!insertmacro MUI_LANGUAGE "Farsi"
!insertmacro MUI_LANGUAGE "Hebrew"
!insertmacro MUI_LANGUAGE "Indonesian"
!insertmacro MUI_LANGUAGE "Mongolian"
!insertmacro MUI_LANGUAGE "Luxembourgish"
!insertmacro MUI_LANGUAGE "Albanian"
!insertmacro MUI_LANGUAGE "Breton"
!insertmacro MUI_LANGUAGE "Belarusian"
!insertmacro MUI_LANGUAGE "Icelandic"

Section

	SetOutPath $INSTDIR

	# Program Files
	File qwinff.exe
	File presets.xml
	File constants.xml
	File *.dll
	File ${LICENSE}
	file ${CHANGELOG}

	CreateDirectory translations
	CreateDirectory ffmpeg
	CreateDirectory qt-i18n

	SetOutPath $INSTDIR\translations
	File translations\*.qm
	SetOutPath $INSTDIR\ffmpeg
	File ffmpeg\*
	SetOutPath $INSTDIR\sox
	File sox\*
	SetOutPath $INSTDIR\qt-i18n
	File qt-i18n\*.qm

	# Create Uninstaller
	WriteUninstaller "$INSTDIR\${UNINSTALLER}"

	# Create Desktop Shortcut
	CreateShortcut "$DESKTOP\QWinFF.lnk" "$INSTDIR\qwinff.exe"

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
	Delete $INSTDIR\constants.xml
	Delete $INSTDIR\*.dll
	Delete $INSTDIR\${LICENSE}
	Delete $INSTDIR\${CHANGELOG}
	Delete $INSTDIR\ffmpeg\*
	RmDir  $INSTDIR\ffmpeg
	Delete $INSTDIR\translations\*
	RmDir  $INSTDIR\translations
	Delete $INSTDIR\sox\*
	RmDir  $INSTDIR\sox
	Delete $INSTDIR\qt-i18n\*
	RmDir  $INSTDIR\qt-i18n
	RmDir  $INSTDIR    # Remove the installation directory if it's empty.

	Delete "$DESKTOP\QWinFF.lnk"
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	Delete "$SMPROGRAMS\$StartMenuFolder\*.lnk"
	RmDir  "$SMPROGRAMS\$StartMenuFolder"
	DeleteRegKey /ifempty HKLM "Software\QWinFF"

SectionEnd

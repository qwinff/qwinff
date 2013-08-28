@echo off
set DEST_DIR=.\windows_release

:: Save version string to variable %VERSION%
for /f "delims=" %%v in ('sh src\get-version.sh') do (
	@set VERSION=%%v
)

pushd src
lrelease qwinff.pro
qmake
mingw32-make release
popd

:: Create output directory if it does not exist.
if not exist "%DEST_DIR%" mkdir "%DEST_DIR%"
if not exist "%DEST_DIR%\ffmpeg" mkdir "%DEST_DIR%\ffmpeg"
if not exist "%DEST_DIR%\sox" mkdir "%DEST_DIR%\sox"
if not exist "%DEST_DIR%\translations" mkdir "%DEST_DIR%\translations"

:: Copy the final executable to the output directory.
copy ".\src\release\qwinff.exe" "%DEST_DIR%"

:: Copy data files to the output directory.
copy ".\src\presets.xml" "%DEST_DIR%"
sed "s/\(<CheckUpdateOnStartup [^>]*>\)[\t ]*false/\1true/" ".\src\constants.xml" > "%DEST_DIR%/constants.xml"
copy ".\src\translations\*.qm" "%DEST_DIR%\translations"
copy "COPYING-v3.txt" "%DEST_DIR%\license.txt"
copy "CHANGELOG.txt" "%DEST_DIR%\changelog.txt"
sed "s/@QWINFF_VERSION@/%VERSION%/" "qwinff.nsi.in" > "%DEST_DIR%\qwinff.nsi"
unix2dos "%DEST_DIR%\license.txt"
unix2dos "%DEST_DIR%\changelog.txt"

@echo Files have been copied to %DEST_DIR%

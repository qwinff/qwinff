@echo off
set DEST_DIR=.\windows_release_portable

pushd src
lrelease qwinff.pro
mingw32-make clean
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
sed "s/\(<Portable [^>]*>\)[\t ]*false/\1true/" ".\src\constants.xml" > "%DEST_DIR%/constants.xml"
copy ".\src\translations\*.qm" "%DEST_DIR%\translations"
copy "COPYING-v3.txt" "%DEST_DIR%\license.txt"
copy "CHANGELOG.txt" "%DEST_DIR%\changelog.txt"
unix2dos "%DEST_DIR%\license.txt"
unix2dos "%DEST_DIR%\changelog.txt"

@echo Files are copied to %DEST_DIR%
pause

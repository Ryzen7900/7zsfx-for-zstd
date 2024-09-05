@Echo off
rem SetLocal enabledelayedexpansion

set archive_name=c:\tmp\test.7z
set config_name=..\7zSfxMod\configs\test20151229.txt
set test_sfx=c:\tmp\test_sfx.exe

cd ..\output
7z a %archive_name% *.sfx && copy /b 7zsd_LZMA2.sfx + %config_name% +%archive_name% %test_sfx%

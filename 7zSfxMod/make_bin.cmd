@Echo off
rem SetLocal enabledelayedexpansion

Set SfxVersion=160

For /F %%i In (SfxBuild.txt) Do Set %%i

set source_dir=%CD%
set sfx_name=%source_dir%\..\bin\7zsd_%SfxVersion%_%SfxBuild%

cd ..\output
7z a -mx=9 %sfx_name%.7z -xr!7zsd_Debug* *.sfx || exit 1
7z a -mx=9 %sfx_name%_src.7z ..\7-Zip ..\7zSfxMod

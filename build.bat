@echo OFF 
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
echo "Starting Build for all Projects with proposed changes"
echo .  
devenv "Updater.sln" /build Release
echo . 
echo "All builds completed." 
pause
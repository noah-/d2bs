call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
msbuild /p:configuration=debug
msbuild /p:configuration=release

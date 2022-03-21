@echo off

set CLANG_FORMAT_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\Llvm\bin
echo "%PATH%" | find "%CLANG_FORMAT_PATH%" >NUL
if %ERRORLEVEL% neq 0 set PATH=%CLANG_FORMAT_PATH%;%PATH%

pushd %~dp0..\src
for /r %%a in (*.h *.cpp) do (
    clang-format -i -verbose -style=file -fallback-style=none %%a
)
popd

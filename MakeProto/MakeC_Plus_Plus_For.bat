@echo off
set destPath=D:\MakeProto\MakeProto\ProtoFiles
cd "%destPath%"

for /f "delims=" %%d in ('dir /b /a-d /s "*.proto"') do %~dp0/protoc -I=%~dp0/ProtoFiles/ --cpp_out=%~dp0/CPlusPlus/ %~dp0/ProtoFiles/%%~nxd 

pause



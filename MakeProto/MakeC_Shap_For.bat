@echo off
set destPath=D:\MakeProto\ProtoFiles
set delPath=D:\MakeProto\
cd "%destPath%"

set tem=tem.protobin

for /f "delims=" %%d in ('dir /b /a-d /s "*.proto"')do %~dp0/protoc --descriptor_set_out=%~dp0/%tem% --include_imports %%~nxd & %~dp0/protogen -output_directory=%~dp0/CShape/  %~dp0/%tem% & del /s /q %~dp0%tem% 


pause
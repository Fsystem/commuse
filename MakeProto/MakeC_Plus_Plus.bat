@echo �����Ҫ�����proto�ļ����c++�ļ�

@echo off
set person_dir=./ProtoFiles
set person_name=person.proto

@echo on

protoc -I=%person_dir% --cpp_out=./CPlusPlus/ %person_dir%/%person_name%


@echo *******************************************
@echo ***************** Success *****************
@echo *******************************************
pause
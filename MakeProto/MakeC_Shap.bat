@echo on
@echo �����Ҫ���cshape�ļ���proto�����ļ�:
@echo off
set tem=tem.protobin

@echo off �����ļ�����(�����￪ʼ����Ҫ������ļ�����·��)
set person_dir=./ProtoFiles
set person_name=person.proto


@echo on
protoc --descriptor_set_out=%tem% --include_imports %person_dir%/%person_name%
protogen -output_directory=./CShape/  %tem%


@echo off
del /s /q *.protobin

@echo *******************************************
@echo ***************** Success *****************
@echo *******************************************
pause
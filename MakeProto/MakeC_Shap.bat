@echo on
@echo 添加需要输出cshape文件的proto定义文件:
@echo off
set tem=tem.protobin

@echo off 定义文件变量(从这里开始，需要编译的文件名和路径)
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
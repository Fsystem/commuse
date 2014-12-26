@echo off
echo 生成之前执行清理

del /s ns.xsd
del /s soapClient.cpp
del /s soapClientLib.cpp
del /s soapH.h
del /s soapServerLib.cpp
del /s soapStub.h
del /s WebService.nsmap
del /s WebService.paynotify.req.xml
del /s WebService.paynotify.res.xml
del /s WebService.wsdl
del /s soapServer.cpp
del /s soapC.cpp

echo wsdl清理完成，即将进入生成过程请点击任意键！
pause

soapcpp2.exe WebServiceDeclare.h
echo wsdl生成成功
mkdir ..\..\..\运行\debug\wsdl
copy WebService.wsdl ..\..\..\运行\debug\wsdl\WebService.wsdl
echo wsdl拷贝成功成功
pause
@echo off
echo ����֮ǰִ������

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

echo wsdl������ɣ������������ɹ��������������
pause

soapcpp2.exe WebServiceDeclare.h
echo wsdl���ɳɹ�
mkdir ..\..\..\����\debug\wsdl
copy WebService.wsdl ..\..\..\����\debug\wsdl\WebService.wsdl
echo wsdl�����ɹ��ɹ�
pause
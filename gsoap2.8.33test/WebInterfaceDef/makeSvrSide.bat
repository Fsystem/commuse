mkdir WebServerProcess
mkdir ..\debug\wsdl
rem 这个是生成c++类简单封装 soapcpp2.exe -j -SL -x -d WebServerProcess WebInterFaceDef.h
soapcpp2.exe -SL -x -d WebServerProcess WebInterFaceDef.h
copy .\WebServerProcess\WebCall.wsdl ..\debug\wsdl\WebCall.wsdl
pause
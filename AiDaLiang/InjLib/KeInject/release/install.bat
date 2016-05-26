@echo off
copy KeInject.sys %windir%\KeInject.sys /y>nul
sc create KeInject type= kernel start= system error= ignore binPath= %windir%\KeInject.sys>nul
sc start KeInject>nul
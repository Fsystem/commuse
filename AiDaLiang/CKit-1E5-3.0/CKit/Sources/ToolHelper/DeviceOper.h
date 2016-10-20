#ifndef __DeviceOper_H
#define __DeviceOper_H

#include <string>

class CDeviceOper
{
public:
	CDeviceOper(void);
	~CDeviceOper(void);

public:
	std::string		GetDiskSnId();
	std::string		GetMac();
	std::string		GetIeVer();
	std::string		GetFlashVer();
	std::string		GetSystemVer();
};

#endif //__DeviceOper_H
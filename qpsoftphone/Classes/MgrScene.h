#ifndef __MgrScene_H
#define __MgrScene_H
/** 
*   @FileName  : MgrScene.h
*   @Author    : Double Sword
*   @date      : 2012-12-12
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/
#include "AppInc.h"

typedef map<string,CCScene*> MAP_SCENE;
class CMgrScene
{
public:
	static CMgrScene* shareMgrScene();
	//get scene pointer
	CCScene* GetSceneByName(string sSceneName);
	//set scene pointer
	CCScene* CreateScene(string sSceneName);
protected:
private:

	MAP_SCENE					map_scene_;
};
#endif //__MgrScene_H
#include "MgrScene.h"

CMgrScene* CMgrScene::shareMgrScene()
{
	static CMgrScene m;
	return &m;
}

CCScene* CMgrScene::GetSceneByName(string sSceneName)
{
	MAP_SCENE::iterator it = map_scene_.find(sSceneName);
	if (it != map_scene_.end()) return it->second;
	return NULL;
}

CCScene* CMgrScene::CreateScene(string sSceneName)
{
	CCScene* pScene = NULL;
	MAP_SCENE::iterator it = map_scene_.find(sSceneName);
	if (it != map_scene_.end())
	{
		CCAssert(false,"scene has existed");
		return pScene;
	}

	pScene = CCScene::create();
	map_scene_[sSceneName] = pScene;

	return pScene;
}
#include "COThread.h"
#include "HelloWorldScene.h"
#include "../../extensions/cocos-ext.h"
#include "AppMacros.h"


USING_NS_CC;
USING_NS_CC_EXT;

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

        // 1. Add a menu item with "X" image, which is clicked to quit the program.

        // Create a "close" menu item with close icon, it's an auto release object.
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);

        // Place the menu item bottom-right conner.
		pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
			origin.y + pCloseItem->getContentSize().height/2));

        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);

        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);

        // 2. Add a label shows "Hello World".

        // Create a label and initialize with string "Hello World".
		CCLabelTTF* pLabel = CCLabelTTF::create("hello world", "Arial", TITLE_FONT_SIZE);

		// position the label on the center of the screen
		pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
			origin.y + visibleSize.height - pLabel->getContentSize().height));


		pLabel->setTag(100);
        CC_BREAK_IF(! pLabel);
		// Add the label to HelloWorld layer as a child layer.
		this->addChild(pLabel, 1);

        // Get window size and place the label upper. 
        CCSize size = CCDirector::sharedDirector()->getWinSize();

        // 3. Add add a splash screen, show the cocos2d splash image.
        CCSprite* pSprite = CCSprite::create("HelloWorld.png");
        CC_BREAK_IF(! pSprite);

        // Place the sprite on the center of the screen
        pSprite->setPosition(ccp(size.width/2, size.height/2));

        // Add the sprite to HelloWorld layer as a child layer.
        this->addChild(pSprite, 0);


#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		//edit
		CCScale9Sprite* pSp = CCScale9Sprite::spriteWithFile("HelloWorld.png");
		CCEditBox* pEdit = CCEditBox::create(CCSizeMake(100,100),pSp);

		this->addChild(pEdit,0);
#endif

        bRet = true;
    } while (0);

	thread_.CreateThred();
	thread_.setSink(this);

	schedule(schedule_selector(HelloWorld::update));
    return bRet;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void HelloWorld::setLab(const char* pStr)
{
	CCLabelTTF* pLab = dynamic_cast<CCLabelTTF*>(this->getChildByTag(100) );
	if (pLab)
	{
		sTile = pLab->getString();
		sTile+=pStr;

		thread_.DestroyThread();

// 		pLab->setString(sTile.c_str());
// 		pLab->draw();
	}
	
}

void HelloWorld::update(float dt)
{
	CCLabelTTF* pLab = dynamic_cast<CCLabelTTF*>(this->getChildByTag(100) );
	if (pLab)
	{
		//int n = rand();
		//char sz[10]={0};
		//sprintf(sz,"%d",n);
		
		pLab->setString(sTile.c_str());
	}
}
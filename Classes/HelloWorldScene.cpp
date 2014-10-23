#include "HelloWorldScene.h"
#include "resource.h"

USING_NS_CC;
using namespace std;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
	initUI();

   
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::initUI(){
	auto winSize = Director::getInstance()->getVisibleSize();
	auto bg = Sprite::create(bird_bg);
	bg->setPosition(winSize.width / 2 , winSize.height / 2);
	bg->setScale(winSize.width / bg->getContentSize().width , winSize.height / bg->getContentSize().height );
	this->addChild(bg,0);

	auto startBtn = MenuItemImage::create(bird_start_btn,bird_start_btn_pressed,CC_CALLBACK_1(HelloWorld::gameStart,this));
	auto menu = Menu::create(startBtn,NULL);
	menu->setTag(TAG_START_BTN);
	this->addChild(menu,1);

	auto birdlogo = Sprite::create(bird_logo);
	birdlogo->setPosition(winSize.width/2 , winSize.height / 2 + startBtn->getContentSize().height );
	birdlogo->setTag(TAG_LOGO);
	this->addChild(birdlogo,1);

	auto overlog = Sprite::create(bird_gameover);
	overlog->setPosition(ccp(winSize.width / 2, winSize.height / 2 + overlog->getContentSize().height));
	overlog->setTag(TAG_OVER);
	overlog->setVisible(false);
	this->addChild(overlog, 1);

	auto hero = Sprite::create(hero1);
	hero->setPosition(winSize.width / 3 , winSize.height * 0.8 );
	hero->setVisible(false);
	hero->setTag(TAG_HERO);
	this->addChild(hero,1);
	Animation* an = Animation::create();
	an->addSpriteFrameWithFileName(hero1);
	an->addSpriteFrameWithFileName(hero2);
	an->addSpriteFrameWithFileName(hero3);
	an->addSpriteFrameWithFileName(hero4);
	an->addSpriteFrameWithFileName(hero5);
	an->addSpriteFrameWithFileName(hero6);
	an->addSpriteFrameWithFileName(hero7);
	an->addSpriteFrameWithFileName(hero8);
	an->setDelayPerUnit(0.03f/0.5f);
	an->setLoops(-1);
	Animate* anim = Animate::create(an);
	hero->runAction(anim);

	auto score = LabelBMFont::create("0","fonts/futura-48.fnt");
	score->setPosition(Point(winSize.width/2,winSize.height/4*3));
	addChild(score,1);
	score->setVisible(false);
	score->setTag(TAG_SCORE);

	obstacle = new Obstacle();
	this->addChild(obstacle);

	scheduleUpdate();
	//schedule(schedule_selector(HelloWorld::update),0.001f);

	auto dispacher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded,this);
	listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan,this);
	dispacher->addEventListenerWithSceneGraphPriority(listener,this);

	GAME_STATUS = GAME_STATUS_START;

}

void HelloWorld::gameStart(cocos2d::Ref* pSender){

	Size win = Director::getInstance()->getWinSize();

	this->getChildByTag(TAG_START_BTN)->setVisible(false);
	this->getChildByTag(TAG_LOGO)->setVisible(false);
	this->getChildByTag(TAG_SCORE)->setVisible(true);
	this->getChildByTag(TAG_HERO)->setVisible(true);
	obstacle->GAME_STATUS = GAME_STATUS_PLAYING;
	GAME_STATUS = GAME_STATUS_PLAYING;
	isFlying = false;
	score = 0;
	velocity = -3;
	gravity = 0.2;
}

void HelloWorld::update(float time){
	auto winSize = Director::getInstance()->getVisibleSize();
	auto hero = this->getChildByTag(TAG_HERO);
	Rect rHero = ((Sprite*)hero)->getBoundingBox();
	CCLog("time=%f",time);
	switch (GAME_STATUS)
	{
	case GAME_STATUS_PLAYING:
		obstacle->update();
		if(hero->getPositionY()>0&&hero->getPositionY()<winSize.height){
			velocity -= gravity;
			hero->setPositionY(hero->getPositionY()+velocity);
		}
		if(hero->getPositionY() < 0 || hero->getPositionY() > winSize.height){
			CCLOG("HERO POSTIONY: %f",hero->getPositionY());
			GAME_STATUS = GAME_STATUS_GAME_OVER;
			break;
		}
		


		for(int i=0;i<obstacle->obstacleList->count();i++){
			Sprite* obstacleSprite = (Sprite*)obstacle->obstacleList->getObjectAtIndex(i);
			bool pia = rHero.intersectsRect(obstacleSprite->getBoundingBox());
			if(pia){
				GAME_STATUS = GAME_STATUS_GAME_OVER;
				break;
			}
			int oPosX = obstacleSprite->getPositionX() + obstacleSprite->getContentSize().width/2;
			int heroX = hero->getPositionX() - hero->getContentSize().width;
			if(oPosX == heroX){
				score++;
				auto scoreSprite = (LabelBMFont*)this->getChildByTag(TAG_SCORE);
				String* s = String::createWithFormat("%d",score / 2 );
				scoreSprite->setString(s->getCString());
			}
		}
		break;
	case GAME_STATUS_GAME_OVER:
		this->getChildByTag(TAG_OVER)->setVisible(true);
		break;
	case GAME_STATUS_RESTART:
		obstacle->removeAllChildren();
		obstacle->obstacleList->removeAllObjects();
		hero->setPosition(winSize.width/5,winSize.height*0.8);
		hero->setVisible(false);
		this->getChildByTag(TAG_START_BTN)->setVisible(true);
		this->getChildByTag(TAG_LOGO)->setVisible(true);
		this->getChildByTag(TAG_OVER)->setVisible(false);
		this->getChildByTag(TAG_SCORE)->setVisible(false);
		break;
	default:
		break;
	}

}
void HelloWorld::onTouchesEnded(const vector<Touch*>& touches, Event* event){
	Touch* touch = touches[0];
	Point location = touch->getLocation();
	if(GAME_STATUS == GAME_STATUS_PLAYING){
		isFlying = false;
	}
}
void HelloWorld::onTouchesBegan(const vector<Touch*>& touches, Event* event){
	Touch* touch = touches[0];
	Point location = touch->getLocation();
	if(GAME_STATUS == GAME_STATUS_PLAYING){
		CCLOG("GAME_STATUS_PLAYING");
		isFlying = true;
		velocity = 5;
	}else if(GAME_STATUS == GAME_STATUS_GAME_OVER){
		GAME_STATUS = GAME_STATUS_RESTART;
		CCLOG("GAME_STATUS_GAME_OVER");
	}
}
Rect HelloWorld::spriteRect(Sprite* s){
	Point pos = s->getPosition();
	Size cs = s->getContentSize();
	return CCRectMake(pos.x - cs.width / 2,pos.y - cs.height / 2,cs.width,cs.height/2);
}

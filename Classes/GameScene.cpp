#include "GameScene.h"
#include "Definitions.h"
#include "GameOverScene.h"




USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    
    scene->getPhysicsWorld()->setGravity(Vect(0,0));
    
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();
    layer->SetPhysicsWorld(scene->getPhysicsWorld());

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


// on "init" you need to initialize your instance
bool GameScene::init()
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    //background
    auto backgroundSprite = Sprite::create("Background.png");
    backgroundSprite->setPosition(Point(visibleSize.width/2+origin.x, visibleSize.height/2+origin.y));
    this->addChild(backgroundSprite);
    joyStickInitialize();
    
    //character
    character = Character::create();
    // character->setPosition(200,200);
    
    character->InitCharacterSprite("bear1.png");
    
    character->setScale(0.3f);
    character->setPosition(300, 300);
    
    
    this->addChild(character, 1);
        
    //Role
    for (int i =0 ; i<2; i++) {
        Role* role = new Role(this);
        roles.push_back(role);
    }
        
    this->schedule(schedule_selector(GameScene::RoleLogic), 1.0f);
    
    //bomb
    item = new Item(this);
    
    
    auto edgeBody = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT,3);
    edgeBody->setCollisionBitmask(OBSTACLE_COLLISION_BITMASK);
    edgeBody->setContactTestBitmask(true);
    
    auto edgeNode =Node::create();
    edgeNode->setPosition(Point(visibleSize.width/2+origin.x, visibleSize.height/2+origin.y));
    edgeNode->setPhysicsBody(edgeBody);
    this->addChild(edgeNode);
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
    this->scheduleUpdate();
    return true;
}



bool GameScene::onContactBegin(cocos2d::PhysicsContact &contact){
    PhysicsBody *a = contact.getShapeA()->getBody();
    PhysicsBody *b = contact.getShapeB()->getBody();
    
    if(
       (BIRD_COLLISION_BITMASK==a->getCollisionBitmask()&&OBSTACLE_COLLISION_BITMASK==b->getCollisionBitmask())||
       (BIRD_COLLISION_BITMASK==b->getCollisionBitmask()&&OBSTACLE_COLLISION_BITMASK==a->getCollisionBitmask())
      )
    {
        auto scene = GameOverScene::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
    }

    return true;
}

bool GameScene::onTouchBegan( cocos2d::Touch *touch, cocos2d::Event *event){
    
    character->Jump();
    return true;
}


void GameScene::update(float dt){
    Point poi = ccpMult(joyStick->getVelocity(), 50);
    //right
    if(poi.x==0 && poi.y==0){
        
        character->StopAnimation();
        
    }else{
        
        printf("%f,%f/n",poi.x,poi.y);
        float x_1=abs(poi.x);
        float y_1=abs(poi.y);
        float length=pow(x_1*x_1+y_1*y_1,0.5);
        float x=5*(1/length)*x_1;
        float y=5*(1/length)*y_1;
        
        if(poi.x>=0){
            character->SetAnimation("BearAnimation/bearAnimation.plist", "BearAnimation/bearAnimationSheet.png","bear", 8, true);
            if(poi.y>=0){
                
                character->setPosition(character->getPosition().x+x,character->getPosition().y+y );
            }else{
                character->setPosition(character->getPosition().x+x,character->getPosition().y-y );
            }
            
        }else{
            character->SetAnimation("BearAnimation/bearAnimation.plist", "BearAnimation/bearAnimationSheet.png","bear", 8, false);
            if(poi.y>=0){
                character->setPosition(character->getPosition().x-x,character->getPosition().y+y );
            }else{
                character->setPosition(character->getPosition().x-x,character->getPosition().y-y );
            }
        }
        
    }
    
    
}

void GameScene::joyStickInitialize(){
    float joystickRadius = 220;
    joyStick=new SneakyJoystick();
    joyStick->autorelease();
    joyStick->initWithRect(CCRectZero);
    joyStick->setAutoCenter(true);
    joyStick->setHasDeadzone(true);
    joyStick->setDeadRadius(10);
    SneakyJoystickSkinnedBase *joystickSkin=new SneakyJoystickSkinnedBase();
    joystickSkin->autorelease();
    joystickSkin->init();
    joystickSkin->setBackgroundSprite(CCSprite::create("Virtual Joystick/VJ2.png"));
    joystickSkin->setThumbSprite(CCSprite::create("Virtual Joystick/VJ1.png"));
    joystickSkin->getBackgroundSprite()->setOpacity(100);
    joystickSkin->getThumbSprite()->setOpacity(20);
    joystickSkin->getThumbSprite()->setScale(0.8f);
    joystickSkin->getBackgroundSprite()->setScale(0.8f);
    joystickSkin->setPosition(Point(joystickRadius-100,joystickRadius-100));
    
    joystickSkin->setJoystick(joyStick);
    
    this->addChild(joystickSkin);
}


void GameScene::RoleLogic(float dt){
    for(int i=0; i<2; i++){
        roles[i]->move();
    }
    
}





/*
 //code for animation Bear
 SpriteBatchNode* spriteSheet = SpriteBatchNode::create("BearAnimation/bearAnimationSheet.png");
 SpriteFrameCache* cache = SpriteFrameCache::getInstance();
 cache->addSpriteFramesWithFile("BearAnimation/bearAnimation.plist");
 this->addChild(spriteSheet);
 Vector<SpriteFrame*> animFrames(8);
 char str[100]={0};
 for(int i=1; i<=8; i++){
 sprintf(str, "bear%d.png",i);
 SpriteFrame* frame = cache->getSpriteFrameByName(str);
 animFrames.insert(i-1, frame);
 }
 Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
 Sprite* bear1 = Sprite::createWithSpriteFrameName("bear1.png");
 bear1->setPosition(Point(visibleSize.width/2+origin.x, visibleSize.height/2+origin.y));
 bear1->runAction(RepeatForever::create(Animate::create(animation)));
 bear1->setScale(0.3);
 spriteSheet->addChild(bear1);
 */








#include "HelloWorldScene.h"
#include "GB2ShapeCache-x.h"
USING_NS_CC;

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
    
    m_mouseJoint = nullptr;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    //this->addChild(sprite, 0);

    //设置地球和绘图
    {
        b2Vec2 gravity;
        gravity.Set(0.0f, -50.0f); //一个向下10单位的向量，作为重力减速度，Box2D中默认的单位是秒和米
        mWorld = new b2World(gravity); // 创建一个有重力加速度的世界
        
        m_debugDraw = new GLESDebugDraw(20);   //这里新建一个 debug渲染模块
        //mWorld->SetDebugDraw(m_debugDraw);    //设置
        uint32 flags = 0;
        flags += b2Draw::e_shapeBit ;
        //flags += b2Draw::e_centerOfMassBit;   //获取需要显示debugdraw的块
        //flags += b2Draw::e_aabbBit;  //AABB块
        //flags += b2Draw::e_centerOfMassBit; //物体质心
        flags += b2Draw::e_jointBit;  //关节
        //flags += b2Draw::e_shapeBit;   //形状
        //flags += b2Draw::e_centerOfMassBit;
        m_debugDraw->SetFlags(flags);   //需要显示那些东西
        mWorld->SetDebugDraw(m_debugDraw);
    }
    
    
    b2Body* ReBody;
    //创建地面和墙壁
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.position.Set(0,10);
        m_groundBody = mWorld->CreateBody(&bodyDef);//添加地面
        
        //申请到之后设置物体属性
        {
            b2EdgeShape shape;
            b2FixtureDef fd;
            fd.shape = &shape;
            
            shape.Set(b2Vec2(0.0f, 0.0f), b2Vec2(Director::getInstance()->getWinSize().width * PTM_RATIO, 0.0f));
            m_groundBody->CreateFixture(&fd);
            
            shape.Set(b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, Director::getInstance()->getWinSize().height * PTM_RATIO));
            m_groundBody->CreateFixture(&fd);
            
            shape.Set(b2Vec2(Director::getInstance()->getWinSize().width * PTM_RATIO, 0.0f), b2Vec2(Director::getInstance()->getWinSize().width * PTM_RATIO, Director::getInstance()->getWinSize().height * PTM_RATIO));
            m_groundBody->CreateFixture(&fd);
        }
        
        //向世界申请一个矩形
        b2BodyDef bodyDefRe;
        bodyDefRe.type = b2_staticBody;
        bodyDefRe.position.Set((Director::getInstance()->getWinSize().width+400) * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        ReBody = mWorld->CreateBody(&bodyDefRe);
        
        //申请到之后设置物体属性
        {
            b2PolygonShape shape;
            shape.SetAsBox(2,1);
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = & shape;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 1.0f;
            
            ReBody->CreateFixture(&fixtureDef);
        }

    }
    //创建矩形
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(Director::getInstance()->getWinSize().width * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        b2Body* body = mWorld->CreateBody(&bodyDef);
        
        
        //申请到之后设置物体属性
        {
            b2PolygonShape dynamicBox;
            dynamicBox.SetAsBox(2,2);
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = & dynamicBox;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 1.0f;
            
            body->CreateFixture(&fixtureDef);
        }
        
    }
    //创建小球
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((Director::getInstance()->getWinSize().width - 400) * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        b2Body* body = mWorld->CreateBody(&bodyDef);
        
        //申请到之后设置物体属性
        {
            b2CircleShape shape;
            shape.m_radius = 1.0f;
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = & shape;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 1.0f;
            
            body->CreateFixture(&fixtureDef);
        }
        
        //让小球有冲力
        body->ApplyForce(b2Vec2(1000,100),b2Vec2(0,0),true);
    }
    //创建多边形
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((Director::getInstance()->getWinSize().width-800) * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        b2Body* body = mWorld->CreateBody(&bodyDef);
        
        //申请到之后设置物体属性
        {
            b2Transform xf1;
            xf1.q.Set(0.3524f * b2_pi);
            xf1.p = xf1.q.GetXAxis();
            
            b2Vec2 vertices[3];
            vertices[0] = b2Mul(xf1, b2Vec2(-1.0f, 0.0f));
            vertices[1] = b2Mul(xf1, b2Vec2(1.0f, 0.0f));
            vertices[2] = b2Mul(xf1, b2Vec2(0.0f, 0.5f));
            
            b2PolygonShape poly1;
            poly1.Set(vertices, 3);
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = & poly1;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 0.5f;
            
            body->CreateFixture(&fixtureDef);
        }
    }
    //创建小球挂在矩形下
    {
        //向世界申请一个球
        b2BodyDef bodyDef0;
        bodyDef0.type = b2_dynamicBody;
        bodyDef0.position.Set((Director::getInstance()->getWinSize().width + 400) * PTM_RATIO/2,(Director::getInstance()->getWinSize().height+200) * PTM_RATIO/2); //初始位置
        b2Body* circleBody0 = mWorld->CreateBody(&bodyDef0);
        
        //申请到之后设置物体属性
        {
            b2CircleShape shape;
            shape.m_radius = 1.0f;
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = & shape;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 1.0f;
            
            circleBody0->CreateFixture(&fixtureDef);
        }

        
        //向世界申请一个球
        b2BodyDef bodyDef1;
        bodyDef1.type = b2_dynamicBody;
        bodyDef1.position.Set((Director::getInstance()->getWinSize().width+400) * PTM_RATIO/2,(Director::getInstance()->getWinSize().height+400) * PTM_RATIO/2); //初始位置
        b2Body* circleBody1 = mWorld->CreateBody(&bodyDef1);
        
        //申请到之后设置物体属性
        {
            b2PolygonShape shape;
            shape.SetAsBox(1,1);
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = & shape;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 1.0f;
            
            circleBody1->CreateFixture(&fixtureDef);
        }
        
        
        //创建连接线
        b2DistanceJointDef jd;
        b2Vec2 p1, p2, d;
        
        jd.frequencyHz = 2.0f;
        jd.dampingRatio = 0.0f;
        
        jd.bodyA = circleBody0;
        jd.bodyB = ReBody;
        jd.localAnchorA.Set(0.0f, 0.0f);
        jd.localAnchorB.Set(2.0f, -0.0f);
        p1 = jd.bodyA->GetWorldPoint(jd.localAnchorA);
        p2 = jd.bodyB->GetWorldPoint(jd.localAnchorB);
        d = p2 - p1;
        jd.length = d.Length()/7;
        mWorld->CreateJoint(&jd);
        
        jd.frequencyHz = 2.0f;
        jd.dampingRatio = 0.0f;
        
        jd.bodyA = circleBody0;
        jd.bodyB = circleBody1;
        jd.localAnchorA.Set(0.0f, 0.0f);
        jd.localAnchorB.Set(0.0f, -0.0f);
        p1 = jd.bodyA->GetWorldPoint(jd.localAnchorA);
        p2 = jd.bodyB->GetWorldPoint(jd.localAnchorB);
        d = p2 - p1;
        jd.length = d.Length()/7;
        mWorld->CreateJoint(&jd);
    }
    //创建类似弹簧的效果(推力)
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((Director::getInstance()->getWinSize().width+400) * PTM_RATIO/2,(Director::getInstance()->getWinSize().height + 400)* PTM_RATIO/2); //初始位置
        b2Body* body = mWorld->CreateBody(&bodyDef);
        
        //申请到之后设置物体属性
        {
            b2PolygonShape dynamicBox;
            dynamicBox.SetAsBox(2,2);
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = & dynamicBox;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 1.0f;
            
            body->CreateFixture(&fixtureDef);
        }
        
        b2MotorJointDef mjd;
        mjd.Initialize(ReBody, body);
        mjd.maxForce = 1000.0f;
        mjd.maxTorque = 1000.0f;
        mWorld->CreateJoint(&mjd);

    }

    //box2dEdit 文件加载
    GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("bird.plist");
    //加载文件中的物体 big_bird
    {
        
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((Director::getInstance()->getWinSize().width - 500) * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        b2Body* body = mWorld->CreateBody(&bodyDef);

        GB2ShapeCache* cache	=	GB2ShapeCache::sharedGB2ShapeCache();
        cache->addFixturesToBody(body,"big_bird");
    }
    //加载文件中的物体 pig_1_1
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((Director::getInstance()->getWinSize().width - 550) * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        b2Body* body = mWorld->CreateBody(&bodyDef);
        
        GB2ShapeCache* cache	=	GB2ShapeCache::sharedGB2ShapeCache();
        cache->addFixturesToBody(body,"pig_1_1");
    }
    //加载文件中的物体 slingshot
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((Director::getInstance()->getWinSize().width - 650) * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        b2Body* body = mWorld->CreateBody(&bodyDef);
        
        GB2ShapeCache* cache	=	GB2ShapeCache::sharedGB2ShapeCache();
        cache->addFixturesToBody(body,"slingshot");
    }
    
    //添加触摸事件
    // Adds Touch Event Listener
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);//
    
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    
    _eventDispatcher->addEventListenerWithFixedPriority(listener, -10);//先被触摸
    _touchListener = listener;
    
    
    
    
    
    this->scheduleUpdate();
    return true;
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event){
    auto touchLocation = touch->getLocation();
    
    auto nodePosition = convertToNodeSpace( touchLocation );//视图层不是当前场景大小, 所以需要转换视图
    log("Box2DView::onTouchBegan, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);
    
    return MouseDown(b2Vec2(nodePosition.x*PTM_RATIO,nodePosition.y*PTM_RATIO));

}
void HelloWorld::onTouchMoved(Touch* touch, Event* event){
    auto touchLocation = touch->getLocation();
    auto nodePosition = convertToNodeSpace( touchLocation );
    
    log("Box2DView::onTouchMoved, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);
    
    MouseMove(b2Vec2(nodePosition.x*PTM_RATIO,nodePosition.y*PTM_RATIO));
}
void HelloWorld::onTouchEnded(Touch* touch, Event* event){
    auto touchLocation = touch->getLocation();
    auto nodePosition = convertToNodeSpace( touchLocation );
    
    log("Box2DView::onTouchEnded, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);
    
    MouseUp(b2Vec2(nodePosition.x*PTM_RATIO,nodePosition.y*PTM_RATIO));

}
bool HelloWorld::MouseDown(const b2Vec2& p)
{
    m_mouseWorld = p;
    
    if (m_mouseJoint != nullptr)
    {
        return false;
    }
    
    // Make a small box.
    b2AABB aabb;
    b2Vec2 d;
    d.Set(0.001, 0.001);
    aabb.lowerBound = p - d;
    aabb.upperBound = p + d;
    
    // Query the world for overlapping shapes.
    QueryCallback callback(p);
    mWorld->QueryAABB(&callback, aabb);
    
    if (callback.m_fixture)
    {
        b2Body* body = callback.m_fixture->GetBody();
        b2MouseJointDef md;
        md.bodyA = m_groundBody;
        md.bodyB = body;
        md.target = p;
        md.maxForce = 1000.0f * body->GetMass();
        m_mouseJoint = (b2MouseJoint*)mWorld->CreateJoint(&md);
        body->SetAwake(true);
        return true;
    }
    
    return false;
}

void HelloWorld::MouseMove(const b2Vec2& p)
{
    m_mouseWorld = p;
    
    if (m_mouseJoint)
    {
        m_mouseJoint->SetTarget(p);
    }
}

void HelloWorld::MouseUp(const b2Vec2& p)
{
    if (m_mouseJoint)
    {
        mWorld->DestroyJoint(m_mouseJoint);
        m_mouseJoint = nullptr;
    }
    
//    if (m_bombSpawning)
//    {
//        CompleteBombSpawn(p);
//    }
}



void HelloWorld::update(float delta)
{
    int32 velocityIterations = 10;
    int32 positionIterations = 10;
    

    
    mWorld->Step(delta, velocityIterations, positionIterations);
    mWorld->ClearForces();
    
//    Sprite * ball = (Sprite *)(mBallBody->GetUserData());
//    b2Vec2 ballPosition = mBallBody->GetPosition();
//    ball->setPosition(ccp(ballPosition.x / PTM_RATIO, ballPosition.y / PTM_RATIO));
//    
//    CCLOG("update, delta=%f, x=%f, y=%f, v=%f",delta, ballPosition.x, ballPosition.y, mBallBody->GetLinearVelocity().y);
}
void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw(renderer, transform, flags);
    Director* director = Director::getInstance();
    CCASSERT(nullptr != director, "Director is null when seting matrix stack");
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
    
    GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
    //m_test->Step(&settings);
    mWorld->DrawDebugData();
    CHECK_GL_ERROR_DEBUG();
    
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
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
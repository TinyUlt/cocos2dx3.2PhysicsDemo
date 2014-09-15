#include "HelloWorldScene.h"
#include "GB2ShapeCache-x.h"
USING_NS_CC;

const int grandMark = 1 << 0;
const int reMark = 1 << 1;
const int cirMark = 1 << 2;
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
        m_debugDraw = new GLESDebugDraw(1/PTM_RATIO);   //这里新建一个 debug渲染模块
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
        mWorld->SetContactListener(this);
    }
    
    
    b2Body* ReBody;
    //创建地面和墙壁
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.position.Set(0,2);
        m_groundBody = mWorld->CreateBody(&bodyDef);//添加地面
        
        //申请到之后设置物体属性
        {
            b2EdgeShape shape;
            b2FixtureDef fd;
            
            fd.filter.categoryBits = grandMark;
            fd.filter.maskBits = reMark | cirMark;
            
            
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
            fixtureDef.density = 1.0f;//指定密度, 自动推算质量
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 0.5f;
            fixtureDef.filter.groupIndex = -8;//和小球永远不碰撞 小球的groupIndex 也是-8 相同并且是负数, 不同则会碰撞
            fixtureDef.filter.categoryBits = reMark;
            fixtureDef.filter.maskBits = grandMark | cirMark;
            body->CreateFixture(&fixtureDef);
        }
        body->SetSleepingAllowed(true);//设置能否睡觉
        body->SetAwake(false);//设施是否醒着
        body->SetBullet(true);//设置是否以子弹方式定义物体(用于高速击打)
        body->ApplyForce(b2Vec2(1000,1000), b2Vec2(0,0), true);//推力
        body->ApplyTorque(100, true);//扭力
        body->ApplyLinearImpulse(b2Vec2(100,100), b2Vec2(0,0), true);//冲力

        
        //重新设置质量属性
//        b2MassData mass;
//        mass.mass = 0.0;
//        mass.center = b2Vec2(0,0);
//        mass.I = 0;
//        body->SetMassData(&mass);
        
        
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
            fixtureDef.filter.groupIndex = -8;
            
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
            fixtureDef.filter.categoryBits = cirMark;
            fixtureDef.filter.maskBits = grandMark | reMark;
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
        
        
        //创建有弹力的连接线
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
        mjd.collideConnected = true;//设置是否碰撞
        mWorld->CreateJoint(&mjd);
    }
//    //创建类似弹簧的效果(推力)
//    {
//        //向世界申请一个物体
//        b2BodyDef bodyDef;
//        bodyDef.type = b2_dynamicBody;
//        bodyDef.position.Set(0.0f+5+5+10,(Director::getInstance()->getWinSize().height + 400)* PTM_RATIO/2); //初始位置
//        b2Body* body = mWorld->CreateBody(&bodyDef);
//        
//        //申请到之后设置物体属性
//        {
//            b2PolygonShape dynamicBox;
//            dynamicBox.SetAsBox(1,1);
//            
//            b2FixtureDef fixtureDef;
//            fixtureDef.shape = & dynamicBox;
//            fixtureDef.density = 1.0f;
//            fixtureDef.friction = 0.3f;
//            fixtureDef.restitution = 1.0f;
//            
//            body->CreateFixture(&fixtureDef);
//        }
//        
//        
//        
//        //向世界申请一个物体
//        b2BodyDef bodyDef2;
//        bodyDef2.type = b2_dynamicBody;
//        bodyDef2.position.Set(0.0f+5+5,(Director::getInstance()->getWinSize().height + 400)* PTM_RATIO/2); //初始位置
//        b2Body* body2 = mWorld->CreateBody(&bodyDef2);
//        
//        //申请到之后设置物体属性
//        {
//            b2PolygonShape dynamicBox;
//            dynamicBox.SetAsBox(1,1);
//            
//            b2FixtureDef fixtureDef;
//            fixtureDef.shape = & dynamicBox;
//            fixtureDef.density = 1.0f;
//            fixtureDef.friction = 0.3f;
//            fixtureDef.restitution = 1.0f;
//            
//            body2->CreateFixture(&fixtureDef);
//        }
//        
//        b2PulleyJointDef jointDef;
//        jointDef.Initialize(body1, body2, groundAnchor1, groundAnchor2, anchor1, anchor2, ratio);
//        jointDef.maxLength1 = 18.0f;
//        jointDef.maxLength2 = 20.0f;
//        mWorld->CreateJoint(&jointDef);
//    }
    
    //关节旋转
    {
        b2PolygonShape shape;
        shape.SetAsBox(0.5f, 0.125f);
        
        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 20.0f;
        fd.friction = 0.2f;
        
        b2RevoluteJointDef jd;
        //jd.collideConnected = false;
        
        const float32 y = 3.0f;
        b2Body* prevBody = m_groundBody;
        for (int32 i = 0; i < 10; ++i)
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set( i +10+0.5, y);//世界坐标
            b2Body* body = mWorld->CreateBody(&bd);
            body->CreateFixture(&fd);
            
            b2Vec2 anchor(float32(i) +10, y);//节点位置,世界坐标
            jd.Initialize(prevBody, body, anchor);
            mWorld->CreateJoint(&jd);
            
            prevBody = body;
        }

        
    }
    
    
    //发动机demo
    {
        //向世界申请一个矩形, 底座
        b2BodyDef bodyDefRe;
        bodyDefRe.type = b2_staticBody;
        bodyDefRe.position.Set(5,9.5); //初始位置
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

        b2Body* prevBody = ReBody;
        
        // Define crank.第一根轴
        {
            b2PolygonShape shape;
            shape.SetAsBox(0.5f, 2.0f);
            
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(0+5.0f, 7.0f+5+5);
            b2Body* body = mWorld->CreateBody(&bd);
            body->CreateFixture(&shape, 2.0f);
            
            b2RevoluteJointDef rjd;
            rjd.Initialize(prevBody, body, b2Vec2(0+5.0f, 5.0f+5+5));
            rjd.motorSpeed = 1.0f * b2_pi;//旋转速度 正数为逆时针
            rjd.maxMotorTorque = 10000.0f;//给的力
            rjd.enableMotor = true;//允许一直给力
            mWorld->CreateJoint(&rjd);
            
            prevBody = body;
        }
        
        // Define follower.第二根轴
        {
            b2PolygonShape shape;
            shape.SetAsBox(0.5f, 4.0f);
            
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(0.0f+5, 13.0f+5+5);
            b2Body* body = mWorld->CreateBody(&bd);
            body->CreateFixture(&shape, 2.0f);
            
            b2RevoluteJointDef rjd;
            rjd.Initialize(prevBody, body, b2Vec2(0.0f+5, 9.0f+5+5));
            rjd.enableMotor = false;
            mWorld->CreateJoint(&rjd);
            
            prevBody = body;
        }
//
        // Define piston
        {
            //上面的拖
            b2PolygonShape shape;
            shape.SetAsBox(1.5f, 1.5f);
            
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.fixedRotation = true;
            bd.position.Set(0.0f+5, 17.0f+5+5);
            b2Body* body = mWorld->CreateBody(&bd);
            body->CreateFixture(&shape, 2.0f);
            
            //设置移动关节
            b2RevoluteJointDef rjd;
            rjd.Initialize(prevBody, body, b2Vec2(0.0f+5, 17.0f+5+5));
            mWorld->CreateJoint(&rjd);
            
            b2PrismaticJointDef pjd;
            pjd.Initialize(ReBody, body, b2Vec2(0.0f+5, 17.0f+5+5), b2Vec2(0.0f, 1.0f));//参数分别为bodyA,bodyB,运动最远点, 运动方向
            
            pjd.maxMotorForce = 0.0f;//永远给个向上的推力
            pjd.enableMotor = true;
            
            mWorld->CreateJoint(&pjd);
        }
//
//        // Create a payload
//        {
//            b2PolygonShape shape;
//            shape.SetAsBox(1.5f, 1.5f);
//            
//            b2BodyDef bd;
//            bd.type = b2_dynamicBody;
//            bd.position.Set(0.0f, 23.0f);
//            b2Body* body = mWorld->CreateBody(&bd);
//            body->CreateFixture(&shape, 2.0f);
//        }
    }

    //box2dEdit 文件加载
    GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("bird.plist");
    //加载文件中的物体 big_bird
    {
        
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set((Director::getInstance()->getWinSize().width - 500) * PTM_RATIO/2,Director::getInstance()->getWinSize().height * PTM_RATIO/2); //初始位置
        b2Body* birdBody = mWorld->CreateBody(&bodyDef);
        birdBody->SetBullet(true);
        GB2ShapeCache* cache	=	GB2ShapeCache::sharedGB2ShapeCache();
        cache->addFixturesToBody(birdBody,"big_bird");
        
        Sprite* birdSp = Sprite::create("big_bird.png");
        this->addChild(birdSp);
        birdSp->setPosition(Vec2((Director::getInstance()->getWinSize().width - 500)/2,Director::getInstance()->getWinSize().height/2));
        
        birdBody->SetUserData(birdSp);
        bodys.push_back(birdBody);
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
        body->SetBullet(true);
        GB2ShapeCache* cache	=	GB2ShapeCache::sharedGB2ShapeCache();
        cache->addFixturesToBody(body,"slingshot");
        
        
        Sprite* birdSp = Sprite::create("slingshot.png");
        this->addChild(birdSp);
        birdSp->setPosition(Vec2((Director::getInstance()->getWinSize().width - 500)/2,Director::getInstance()->getWinSize().height/2));
        body->SetUserData(birdSp);
        bodys.push_back(body);
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
    
    Device::setAccelerometerEnabled(true);
    Device::setAccelerometerInterval(1/30.0);
    auto listener2 = EventListenerAcceleration::create(CC_CALLBACK_2(HelloWorld::onAcceleration,  this));
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);
    
    
    
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
    
    for (int i = 0; i < bodys.size(); i++) {
        Sprite * bird = (Sprite *)(bodys[i]->GetUserData());
        b2Vec2 ballPosition = bodys[i]->GetPosition();
        float birdRotation = bodys[i]->GetAngle();
        bird->setRotation(360 - CC_RADIANS_TO_DEGREES(birdRotation));
        bird->setPosition(Vec2(ballPosition.x / PTM_RATIO, ballPosition.y / PTM_RATIO));
    }
    
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
void HelloWorld::BeginContact(b2Contact* contact)
{
    //B2_NOT_USED(contact);
    if(contact->GetFixtureA()->GetFilterData().categoryBits == reMark)
    {
        CCLOG("reMark");
    }
    
    if(contact->GetFixtureB()->GetFilterData().categoryBits == reMark)
    {
        CCLOG("reMark");
    }
}

void HelloWorld::onAcceleration(Acceleration* acc, Event* unused_event)
{
    float lenth = -50;
    float radian = acc->x * 3.14159265 / 2;
    float x = sin(radian) * lenth;
    float y = cos(radian) * lenth;
    b2Vec2 gravity(-x , y);//重力感应方向的改变，表示向量力的变化
    mWorld->SetGravity(gravity);//这两句表示判断重力的感应方向的转变
    CCLOG("%f, %f, %f", acc->x, acc->y, acc->z);
    
}
void HelloWorld::EndContact(b2Contact* contact)
{
    //B2_NOT_USED(contact);
}
void HelloWorld::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}
void HelloWorld::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
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

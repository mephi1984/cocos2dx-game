/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include <iostream>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    shipPtr = Sprite::create("ship.png");

    GameSingleton::getInstance().getState().reset();
   
    
    this->addChild(shipPtr, 2);

    mouseListener = EventListenerMouse::create();

    mouseListener->onMouseMove 
        = CC_CALLBACK_1(HelloWorld::onMouseMove, this);
    mouseListener->onMouseDown
        = CC_CALLBACK_1(HelloWorld::onMouseDown, this);

    _eventDispatcher->
        addEventListenerWithSceneGraphPriority(mouseListener, this);

    auto keyListener = EventListenerKeyboard::create();

    keyListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

    scheduleUpdate();

    backgroundPtr = Sprite::create("bkg.png");

    this->addChild(backgroundPtr, 1);

    SetAllPositions();

    proxy.factory = &factory;

    return true;
}

void HelloWorld::onMouseMove(cocos2d::Event* event)
{
    cocos2d::EventMouse* mouseEvent = (cocos2d::EventMouse*)event;

    GameSingleton::getInstance().getState().shipPos.y = mouseEvent->getCursorY();

    SetAllPositions();
}

void HelloWorld::onMouseDown(cocos2d::Event* event)
{
    GameSingleton::getInstance().getState().reset();
    SetAllPositions();
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    if (code == EventKeyboard::KeyCode::KEY_S)
    {
        //MessageBoxA(0, "Key S pressed", "Key S pressed", 0);
        Memento save = GameSingleton::getInstance().getState().SaveData();
        saves.push_back(save);
    }
    else if (code == EventKeyboard::KeyCode::KEY_L)
    {
        //MessageBoxA(0, "Key L pressed", "Key L pressed", 0);
        if (saves.size() > 0)
        {
            Memento save = saves.back();
            //saves.pop_back();
            GameSingleton::getInstance().getState().RestoreFromData(save);
            reload();
        }
    }
}

void HelloWorld::update(float delta)
{
    GameSingleton::getInstance().getState().moveBkg(80 * delta);
    GameSingleton::getInstance().getState().moveObjectPos(200 * delta);

    SetAllPositions();

    GameSingleton::getInstance().getState().objectTimer += delta;

    if (GameSingleton::getInstance().getState().objectTimer > 2.0)
    {
        GameSingleton::getInstance().getState().objectTimer 
            = GameSingleton::getInstance().getState().objectTimer - 2.0;

        auto newObject =
            proxy.createObject();

        std::shared_ptr<Strategy> strategy;

        int type = rand() % 2;
        if (type == 0)
        {
            strategy = std::make_shared<Strategy>();
        }
        else
        {
            strategy = std::make_shared<ZigzagStrategy>();
        }
        GameSingleton::getInstance()
            .getState()
            .objectPos.push_back(
                { 
                    newObject.pos,
                    strategy,
                    type,
                    newObject.sprite->getScale(),
                    newObject.type
                });

        addChild(newObject.sprite, 3);

        objectPtr
            .push_back(
                std::make_unique<ShakingSpriteDecorator>(newObject.sprite, newObject.type)
         );
    }

    for (auto& object : objectPtr)
    {
        float x = object->getPosition().x;
        float y = object->getPosition().y;
        float R = object->getScale() * 300;

        float w = 150;
        float h = 150;

        float px = shipPtr->getPosition().x;
        float py = shipPtr->getPosition().y;

        if ((abs(y - (py - h / 2)) < R 
            || abs(y + (py - h / 2)) < R)
            && (x > px - w / 2)
            && (x < px + w / 2))
        {
            auto& flyingObject = object->sprite;

            flyingObject->accept(*this);

        }
    }
}

void HelloWorld::reload()
{
    for (auto& object : objectPtr)
    {
        object->sprite->sprite->removeFromParent();
    }

    objectPtr.clear();

    for (auto& objectPosAndStrategy : GameSingleton::getInstance().getState().objectPos)
    {
        ObjectData newObject;
        newObject.pos = objectPosAndStrategy.pos;

        if (objectPosAndStrategy.objectType == 0) {
            newObject.sprite = Sprite::create("asteroid.png");
        }
        else {
            newObject.sprite = Sprite::create("cloud.png");
        }
        newObject.type = objectPosAndStrategy.objectType;
        newObject.sprite->setPosition(objectPosAndStrategy.pos);
        newObject.sprite->setScale(objectPosAndStrategy.scale);

        addChild(newObject.sprite, 3);


        objectPtr
            .push_back(
                std::make_unique<ShakingSpriteDecorator>(newObject.sprite, newObject.type)
            );
    }
}

void HelloWorld::SetAllPositions()
{
    shipPtr->setPosition(GameSingleton::getInstance().getState().shipPos);
    backgroundPtr->setPosition(GameSingleton::getInstance().getState().bkgPos);

    for (int i = 0; i < objectPtr.size(); i++)
    {
        auto position = GameSingleton::getInstance().getState().objectPos[i];
        objectPtr[i]->setPosition(position.pos);
    }
}

ObjectData ObjectFactory::createObject()
{
    ObjectData result;

    Vec2 pos = Vec2{ 1400.f, (float)(rand() % 500) };
    int type = rand() % 2;
    std::string name;
    if (type == 0)
    {
        name = "asteroid.png";
    }
    else
    {
        name = "cloud.png";
    }

    result.pos = pos;

    Sprite* newObject = Sprite::create(name);
    newObject->setScale(scale);
    newObject->setPosition(pos);
    result.sprite = newObject;

    scale = scale * 1.15;
    result.type = type;
    return result;
}

ObjectData ObjectProxy::createObject()
{
    auto& objectCreationData = GameSingleton::getInstance()
        .getState().objectCreationData;

    if (objectCreationData.size() > 0)
    {
        ObjectCreationData d
            = objectCreationData[objectCreationData.size() - 1];
        objectCreationData.pop_back();
        ObjectData result;
        result.pos = d.pos;
        if (d.type == 0) {
            result.sprite = Sprite::create("asteroid.png");
        }
        else {
            result.sprite = Sprite::create("cloud.png");
        }
        result.type = d.type;
        result.sprite->setPosition(d.pos);
        result.sprite->setScale(d.scale);
        return result;
    }
    else
    {
        return factory->createObject();
    }
}

void HelloWorld::visitObject(SolidObject& s)
{
    Director::getInstance()->end();
}

void HelloWorld::visitObject(CloudObject& s)
{

}
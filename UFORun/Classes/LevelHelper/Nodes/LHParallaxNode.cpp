//  This file was generated by LevelHelper
//  http://www.levelhelper.org
//
//  LevelHelperLoader.mm
//  Created by Bogdan Vladu
//  Copyright 2011 Bogdan Vladu. All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//  The origin of this software must not be misrepresented; you must not
//  claim that you wrote the original software. If you use this software
//  in a product, an acknowledgment in the product documentation would be
//  appreciated but is not required.
//  Altered source versions must be plainly marked as such, and must not be
//  misrepresented as being the original software.
//  This notice may not be removed or altered from any source distribution.
//  By "software" the author refers to this code file and not the application 
//  that was used to generate this file.
//
////////////////////////////////////////////////////////////////////////////////
#include "LHParallaxNode.h"
#include "LHSettings.h"
#include "../LevelHelperLoader.h"
#include "LHSprite.h"
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
LHParallaxNode::LHParallaxNode(void){
    
}
////////////////////////////////////////////////////////////////////////////////
LHParallaxNode::~LHParallaxNode(void){
//    CCLog("LHParallaxNode dealloc");
    
    unscheduleAllSelectors();
    
    if(NULL != followedSprite)
        followedSprite->parallaxFollowingThisSprite = NULL;
    followedSprite = NULL;
    
    if(removeSpritesOnDelete)
    {
#if COCOS2D_VERSION >= 0x00020000
        CCArray* tempSprites = CCArray::create();
#else
        CCArray* tempSprites = CCArray::array();
#endif
        
        tempSprites->addObjectsFromArray(sprites);
    
        for(unsigned int i = 0; i< tempSprites->count(); ++i)
        {        
            LHParallaxPointObject* pt = (LHParallaxPointObject*)tempSprites->objectAtIndex(i);
            if(pt->ccsprite){
                ((LHSprite*)pt->ccsprite)->setParallaxNode(NULL);
                ((LHSprite*)pt->ccsprite)->removeSelf();
            }
        }
        tempSprites->removeAllObjects();
    }
    
    sprites->removeAllObjects();
    
    delete sprites;
}
////////////////////////////////////////////////////////////////////////////////
bool LHParallaxNode::initWithDictionary(LHDictionary* parallaxDict, LevelHelperLoader* loader){

    if(NULL == parallaxDict)
        return false;
    
#if COCOS2D_VERSION >= 0x00020000
    sprites = CCArray::create();
#else
    sprites = CCArray::array();
#endif
    sprites->retain();
    
    
    followedSprite = NULL;
    isContinuous = parallaxDict->boolForKey("ContinuousScrolling");
    direction = parallaxDict->intForKey("Direction");
    speed = parallaxDict->floatForKey("Speed");
    lastPosition = CCPointMake(0,0);
    paused = false;
    winSize = CCDirector::sharedDirector()->getWinSize();
    screenNumberOnTheRight = 1;
    screenNumberOnTheLeft = 0;
    screenNumberOnTheTop = 0;
    
    removeSpritesOnDelete = false;
    parentLoader = loader;
    
    movedEndListenerObj = NULL;
    movedEndListenerSEL = NULL;
    
    uniqueName  = parallaxDict->stringForKey("UniqueName");
    if(!isContinuous)
        speed = 1.0f;
    
    schedule( schedule_selector(LHParallaxNode::tick) , 1.0f/90.0f);
    return true;
}
////////////////////////////////////////////////////////////////////////////////
LHParallaxNode* LHParallaxNode::nodeWithDictionary(LHDictionary* properties, LevelHelperLoader* loader){
    
    LHParallaxNode *pobNode = new LHParallaxNode();
	if (pobNode && pobNode->initWithDictionary(properties, loader))
    {
	    pobNode->autorelease();
        return pobNode;
    }
    CC_SAFE_DELETE(pobNode);
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::addSprite(LHSprite* sprite, CCPoint ratio)
{
	CCAssert( sprite != NULL, "Argument must be non-nil");
	
	LHParallaxPointObject *obj = createParallaxPointObject(sprite, ratio);
    sprite->setParallaxNode(this);
	obj->body = sprite->getBody();
    obj->isLHSprite = true;
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::addNode(CCNode* node, CCPoint ratio){
  
    if(LHSprite::isLHSprite(node)){
        addSprite((LHSprite*)node, ratio);
    }
    else{
        createParallaxPointObject(node, ratio);
    }
}
////////////////////////////////////////////////////////////////////////////////
LHParallaxPointObject* LHParallaxNode::createParallaxPointObject(CCNode* node, CCPoint ratio){

    CCAssert( node != NULL, "Argument must be non-nil");
	
	LHParallaxPointObject *obj = LHParallaxPointObject::pointWithCCPoint(ratio);
	obj->ccsprite = node;

	obj->position = node->getPosition();
	obj->offset = node->getPosition();
	obj->initialPosition = node->getPosition();
    sprites->addObject(obj);
    
	int scrRight = (int)(obj->initialPosition.x/winSize.width);
	
	if(screenNumberOnTheRight <= scrRight)
		screenNumberOnTheRight = scrRight+1;
    
	int scrLeft = (int)(obj->initialPosition.x/winSize.width);
    
	if(screenNumberOnTheLeft >= scrLeft)
		screenNumberOnTheLeft = scrLeft-1;
    
    
	int scrTop = (int)(obj->initialPosition.y/winSize.height);
	
	if(screenNumberOnTheTop <= scrTop)
		screenNumberOnTheTop = scrTop + 1;
	
	int scrBottom = (int)(obj->initialPosition.y/winSize.height);
    
	if(screenNumberOnTheBottom >= scrBottom)
		screenNumberOnTheBottom = scrBottom-1;
    
    return obj;
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::removeChild(LHSprite* sprite)
{
    if(NULL == sprite) 
        return;
    
    for(unsigned int i = 0; i< sprites->count(); ++i){        
        LHParallaxPointObject* pt = (LHParallaxPointObject*)sprites->objectAtIndex(i);
	
        if(pt->ccsprite == sprite){
            sprites->removeObjectAtIndex(i);
            return;
        }
	}
    
//#endif
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::registerSpriteHasMovedToEndListener(CCObject* object, SEL_CallFuncN method)
{
    movedEndListenerObj = object; 
    movedEndListenerSEL = method;
}
////////////////////////////////////////////////////////////////////////////////
CCArray* LHParallaxNode::spritesInNode(void)
{
#if COCOS2D_VERSION >= 0x00020000
    CCArray* sprs = CCArray::create();
#else
    CCArray* sprs = CCArray::array();
#endif
    
    for(unsigned int i = 0; i < sprites->count(); ++i)
    {
        LHParallaxPointObject* pt = (LHParallaxPointObject*)sprites->objectAtIndex(i);
        if(NULL != pt->ccsprite)
            sprs->addObject((LHSprite*)pt->ccsprite);
    }
    return sprs;
}
////////////////////////////////////////////////////////////////////////////////
std::vector<b2Body*> LHParallaxNode::bodiesInNode(void){
    
    std::vector<b2Body*> sprs;
    
    for(unsigned int i = 0; i < sprites->count(); ++i){
        LHParallaxPointObject* pt = (LHParallaxPointObject*)sprites->objectAtIndex(i);
        if(NULL != pt->body)
            sprs.push_back(pt->body);
    }
    return sprs;
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::setPositionOnPointWithOffset(const CCPoint& pos, 
                                                  LHParallaxPointObject* point, 
                                                  const CCPoint& offset)
{
    if(point == NULL)
        return;
    
    if(!isContinuous)
    {
        if(point->ccsprite != NULL){
            point->ccsprite->setPosition(pos);
        
            if(point->body != NULL){
            
                float angle = point->ccsprite->getRotation();
                point->body->SetAwake(true);
                
                point->body->SetTransform(b2Vec2(pos.x/LHSettings::sharedInstance()->lhPtmRatio(), 
                                                 pos.y/LHSettings::sharedInstance()->lhPtmRatio()), 
                                         CC_DEGREES_TO_RADIANS(-angle));
            }
        }
    }
    else
    {
        if(point->ccsprite != NULL)
        {
            CCPoint newPos = CCPointMake(point->ccsprite->getPosition().x - offset.x,
                                         point->ccsprite->getPosition().y - offset.y);
            point->ccsprite->setPosition(newPos);
            
            if(point->body != NULL){
            
            float angle = point->ccsprite->getRotation();
            point->body->SetTransform(b2Vec2(newPos.x/LHSettings::sharedInstance()->lhPtmRatio(), 
                                             newPos.y/LHSettings::sharedInstance()->lhPtmRatio()), 
                                     CC_DEGREES_TO_RADIANS(-angle));
            }
            
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
CCSize LHParallaxNode::getBounds(float rw, float rh, float radians)
{
    float x1 = -rw/2;
    float x2 = rw/2;
    float x3 = rw/2;
    float x4 = -rw/2;
    float y1 = rh/2;
    float y2 = rh/2;
    float y3 = -rh/2;
    float y4 = -rh/2;
    
    float x11 = x1 * cos(radians) + y1 * sin(radians);
    float y11 = -x1 * sin(radians) + y1 * cos(radians);
    float x21 = x2 * cos(radians) + y2 * sin(radians);
    float y21 = -x2 * sin(radians) + y2 * cos(radians);
    float x31 = x3 * cos(radians) + y3 * sin(radians);
    float y31 = -x3 * sin(radians) + y3 * cos(radians);
    float x41 = x4 * cos(radians) + y4 * sin(radians);
    float y41 = -x4 * sin(radians) + y4 * cos(radians);

    float x_min = MIN(MIN(x11,x21),MIN(x31,x41));
    float x_max = MAX(MAX(x11,x21),MAX(x31,x41));
    
    float y_min = MIN(MIN(y11,y21),MIN(y31,y41));
    float y_max = MAX(MAX(y11,y21),MAX(y31,y41));
 
    return CCSizeMake(x_max-x_min, y_max-y_min);
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::repositionPoint(LHParallaxPointObject* point, double frameTime)
{
    CCSize spriteContentSize = point->ccsprite->getContentSize();
    CCPoint spritePosition = point->ccsprite->getPosition();
    float angle = point->ccsprite->getRotation();
    float rotation = CC_DEGREES_TO_RADIANS(angle);
	float scaleX = point->ccsprite->getScaleX();
	float scaleY = point->ccsprite->getScaleY();
    
    CCSize contentSize = getBounds(spriteContentSize.width,
                                   spriteContentSize.height,
                                   rotation);
    
	switch (direction) {
		case 1: //right to left
		{
            if(spritePosition.x + contentSize.width/2.0f*scaleX <= 0)
                
                //			if(spritePosition.x + contentSize.width*scaleX <= 0)
			{
                if(NULL != point->ccsprite){
                    float difX = spritePosition.x;
                    
                    CCPoint newPos = CCPointMake(winSize.width*screenNumberOnTheRight + difX,
                                                 spritePosition.y);
                    
                    
                    if(point->isLHSprite)
                    {
                        ((LHSprite*)point->ccsprite)->transformPosition(newPos);
                    }
                    else {
                        point->ccsprite->setPosition(newPos);
                    }
                }
                
                
                if(NULL != movedEndListenerObj){
                    (movedEndListenerObj->*movedEndListenerSEL)(point->ccsprite);
                }
			}
		}
			break;
			
		case 0://left to right
		{
            if(spritePosition.x - contentSize.width/2.0f*scaleX >= winSize.width)
			{
				float difX = spritePosition.x - winSize.width;
				
                CCPoint newPos = CCPointMake(winSize.width*screenNumberOnTheLeft + difX,
                                             spritePosition.y);
                
                if(point->isLHSprite)
                {
                    ((LHSprite*)point->ccsprite)->transformPosition(newPos);
                }
                else {
                    point->ccsprite->setPosition(newPos);
                }
                
                if(NULL != movedEndListenerObj){
                    (movedEndListenerObj->*movedEndListenerSEL)(point->ccsprite);
                }
			}
		}
			break;
			
		case 2://up to bottom
		{
            if(spritePosition.y + contentSize.height/2.0f*scaleY <= 0)
                //			if(spritePosition.y + contentSize.height*scaleY <= 0)
			{
				float difY = spritePosition.y;
				
                CCPoint newPos = CCPointMake(spritePosition.x,
                                             winSize.height*screenNumberOnTheTop +difY);
                
                
                if(point->isLHSprite)
                {
                    ((LHSprite*)point->ccsprite)->transformPosition(newPos);
                }
                else {
                    point->ccsprite->setPosition(newPos);
                }
                
                if(NULL != movedEndListenerObj){
                    (movedEndListenerObj->*movedEndListenerSEL)(point->ccsprite);
                }
			}
		}
			break;
			
		case 3://bottom to top
		{
            if(spritePosition.y - contentSize.height/2.0f*scaleY >= winSize.height)
                //			if(spritePosition.y - contentSize.height*scaleY >= winSize.height)
			{
				float difY = spritePosition.y - winSize.height;
                
                CCPoint newPos = CCPointMake(spritePosition.x,
                                             winSize.height*screenNumberOnTheBottom + difY);
                
                if(point->isLHSprite)
                {
                    ((LHSprite*)point->ccsprite)->transformPosition(newPos);
                }
                else {
                    point->ccsprite->setPosition(newPos);
                }
                
                if(NULL != movedEndListenerObj){
                    (movedEndListenerObj->*movedEndListenerSEL)(point->ccsprite);
                }
			}
		}
			break;
		default:
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::setPosition(CCPoint newPosition)
{
    CCNode::setPosition(newPosition);
    visit();
}
////////////////////////////////////////////////////////////////////////////////
void LHParallaxNode::setFollowSprite(LHSprite* sprite,
                                     bool changeXPosition, 
                                     bool changeYPosition){
    
    if(NULL == sprite)
    {
        if(NULL != followedSprite)
            followedSprite->parallaxFollowingThisSprite = NULL;
    }
    
    followedSprite = sprite;
    
    followChangeX = changeXPosition;
    followChangeY = changeYPosition;
    
    if(NULL != sprite)
    {
        lastFollowedSpritePosition = sprite->getPosition();
        sprite->parallaxFollowingThisSprite = this;
    }
}
////////////////////////////////////////////////////////////////////////////////
//void LHParallaxNode::visit(void)
void LHParallaxNode::tick(float dt)
{
    
    if(LHSettings::sharedInstance()->levelPaused() || paused) //level is paused
    {
        return;
    }
    
    if(NULL != followedSprite)
    {
        CCPoint spritePos = followedSprite->getPosition();
        float deltaFX = lastFollowedSpritePosition.x - spritePos.x;
        float deltaFY = lastFollowedSpritePosition.y - spritePos.y;
        lastFollowedSpritePosition = spritePos;
        
        CCPoint lastNodePosition = getPosition();
        if(followChangeX && !followChangeY){
            setPosition(ccp(lastNodePosition.x + deltaFX,
                            lastNodePosition.y));
        }
        else if(!followChangeX && followChangeY){
            setPosition(ccp(lastNodePosition.x,
                            lastNodePosition.y + deltaFY));
        }
        else if(followChangeX && followChangeY){
            setPosition(ccp(lastNodePosition.x + deltaFX,
                            lastNodePosition.y + deltaFY));
        }
    }
    
    double i = -1.0f; //direction left to right //bottom to up
	CCPoint pos = getPosition();
    
    CCPoint deltaPos = CCPointMake(pos.x - lastPosition.x,
                                   pos.y - lastPosition.y);
    
	if(isContinuous || ! (pos.x == lastPosition.x && pos.y == lastPosition.y))
	{
        float   frameTime = dt;//[[NSDate date] timeIntervalSince1970] - time;
        
        for(unsigned int i, j = 0; j< sprites->count(); ++j){
            LHParallaxPointObject* point = (LHParallaxPointObject*)sprites->objectAtIndex(j);
        
		    i = -1; //direction left to right //bottom to up
            if(direction == 1 || direction == 2) //right to left //up to bottom
                i = 1;
            
            LHSprite* spr = (LHSprite*)point->ccsprite;
            CCPoint oldPos = spr->getPosition();
            
            
            if(isContinuous)
            {
                spr->transformPosition(CCPointMake((float)(oldPos.x - i*point->ratio.x*speed*frameTime),
                                                   (float)(oldPos.y - i*point->ratio.y*speed*frameTime)));
                
                repositionPoint(point, frameTime);
            }
            else {
                
                spr->transformPosition(CCPointMake(oldPos.x + point->ratio.x*deltaPos.x/*2.0f*frameTime*/,
                                                   oldPos.y + point->ratio.y*deltaPos.y/*2.0f*frameTime*/));
                
                
            }
		}
	}
    lastPosition = pos;    
}

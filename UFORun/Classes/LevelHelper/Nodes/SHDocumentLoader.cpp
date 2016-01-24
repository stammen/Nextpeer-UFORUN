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

#include "SHDocumentLoader.h"
#include "../LevelHelperLoader.h"
#include "LHSettings.h"

#include "../Utilities/LHDictionary.h"

#include "cocoa/CCNS.h"



bool SHSceneNode::initSceneNodeWithContentOfFile(const std::string& sceneFile){
    
#if COCOS2D_VERSION >= 0x00020100
    std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(sceneFile.c_str());
    LHDictionary* dictionary = (LHDictionary*)LHDictionary::createWithContentsOfFile(fullPath.c_str());
#elif COCOS2D_VERSION >= 0x00020000
    std::string fullPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(sceneFile.c_str());
    LHDictionary* dictionary = (LHDictionary*)LHDictionary::createWithContentsOfFile(fullPath.c_str());
#else
    std::string fullPath = CCFileUtils::fullPathFromRelativePath(sceneFile.c_str());
    LHDictionary* dictionary = (LHDictionary*)CCFileUtils::dictionaryWithContentsOfFile(fullPath.c_str());
#endif    
    
    LHArray* sheetsList = dictionary->arrayForKey("SHEETS_INFO");
    
    for(unsigned int i = 0; i < sheetsList->count(); ++i){
        LHDictionary* dic = sheetsList->dictAtIndex(i);
        sheets->setObject(dic, dic->stringForKey("SheetName"));
    }
    
    
    LHArray* animList = dictionary->arrayForKey("SH_ANIMATIONS_LIST");
    
    for(unsigned int i = 0; i< animList->count(); ++i){
        
        LHDictionary* dic = animList->dictAtIndex(i);
        animations->setObject(dic, dic->stringForKey("UniqueName"));
    }
    
	return true;
}
SHSceneNode::~SHSceneNode(){
    
#if COCOS2D_VERSION >= 0x00020000
    sheets->release();
    sheets = NULL;
    animations->release();
    animations = NULL;
#else
    delete sheets;
    delete animations;
#endif

}

SHSceneNode::SHSceneNode(){
    
#if COCOS2D_VERSION >= 0x00020000
    sheets = (LHDictionary*)CCDictionary::create();
    sheets->retain();
    animations = (LHDictionary*)CCDictionary::create();
    animations->retain();
#else
    sheets = (LHDictionary*)(new CCDictionary<std::string, CCObject*>());
    animations = (LHDictionary*)(new CCDictionary<std::string, CCObject*>());
#endif
    
}
SHSceneNode* SHSceneNode::SHSceneNodeWithContentOfFile(const std::string& sceneFile){
    SHSceneNode *pobNode = new SHSceneNode();
	if (pobNode && pobNode->initSceneNodeWithContentOfFile(sceneFile))
    {
	    pobNode->autorelease();
        return pobNode;
    }
    CC_SAFE_DELETE(pobNode);
	return NULL;
}
LHDictionary* SHSceneNode::infoForSpriteNamed(const std::string& name, const std::string& sheetName){
    
    LHDictionary* sheetsInfo = sheets->dictForKey(sheetName);
    
    if(sheetsInfo){
        
        LHDictionary* spritesInfo = sheetsInfo->dictForKey("Sheet_Sprites_Info");
        LHDictionary* sprInfo = spritesInfo->dictForKey(name);
        
        if(sprInfo)
            return sprInfo;
        
        CCLog("Info for sprite named %s could not be found in sheet named %s", name.c_str(), sheetName.c_str());
        return NULL;
    }
    CCLog("Could not find sheet named %s", sheetName.c_str());
    return NULL;
}
LHDictionary* SHSceneNode::infoForSheetNamed(const std::string& sheetName){
    return sheets->dictForKey(sheetName);    
}
LHDictionary* SHSceneNode::infoForAnimationNamed(const std::string& animName){
    return animations->dictForKey(animName);        
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SHDocumentLoader *SHDocumentLoader::m_sharedInstance = 0;
////////////////////////////////////////////////////////////////////////////////
SHDocumentLoader* SHDocumentLoader::sharedInstance(){
	if (0 == m_sharedInstance){
		m_sharedInstance = new SHDocumentLoader();
	}
    return m_sharedInstance;
}
////////////////////////////////////////////////////////////////////////////////
SHDocumentLoader::~SHDocumentLoader()
{
    
}
//------------------------------------------------------------------------------
SHDocumentLoader::SHDocumentLoader(){
    
    lastSheetDictionary = NULL;
    lastAnimationDictionary= NULL;
    lastSpriteDictionary = NULL;
}
//------------------------------------------------------------------------------
SHSceneNode* SHDocumentLoader::sceneNodeForSHDocument(const std::string& shDocument)
{
    //will create the node if it does not exit
    SHSceneNode* sceneNode = (SHSceneNode*)scenes.objectForKey(shDocument.c_str());
    
    if(sceneNode == NULL){
        sceneNode = SHSceneNode::SHSceneNodeWithContentOfFile(shDocument);
        if(NULL != sceneNode)
            scenes.setObject(sceneNode,shDocument.c_str());
    }
    return sceneNode;
}
//------------------------------------------------------------------------------
LHDictionary* SHDocumentLoader::dictionaryForSpriteNamed(const std::string& spriteName, const std::string& sheetName, const std::string& spriteHelperDocument)
{
    if(lastSprSpriteName == spriteName &&
       lastSprSheetName == sheetName &&
       lastSprDocumentName == spriteHelperDocument &&
       lastSpriteDictionary != NULL){
        return lastSpriteDictionary;
    }

    
    SHSceneNode* shNode = sceneNodeForSHDocument(spriteHelperDocument);
    if(NULL != shNode){
        LHDictionary* info = shNode->infoForSpriteNamed(spriteName, sheetName);
        if(info){
            lastSpriteDictionary = info;
            lastSprSpriteName = spriteName;
            lastSprSheetName = sheetName;
            lastSprDocumentName = spriteHelperDocument;
            
            return info;
        }
        else {
            CCLog("Could not find info for sprite named %s in sheet name %s in document name %s", spriteName.c_str(), sheetName.c_str(), spriteHelperDocument.c_str());
        }
    }
    return NULL;
}
//------------------------------------------------------------------------------
LHDictionary* SHDocumentLoader::dictionaryForSheetNamed(const std::string& sheetName,const std::string& spriteHelperDocument)
{
    if(lastSheetSheetName == sheetName &&
       lastSheetDocumentName == spriteHelperDocument &&
       lastSheetDictionary != NULL){
        return lastSheetDictionary;
    }
    
    SHSceneNode* shNode = sceneNodeForSHDocument(spriteHelperDocument);
    if(NULL != shNode)
    {
        LHDictionary* info = shNode->infoForSheetNamed(sheetName);
        if(info){
            
            lastSheetDictionary = info;
            lastSheetSheetName = sheetName;
            lastSheetDocumentName = spriteHelperDocument;

            return info;
        }
        else {
            CCLog("Could not find info for sheet named %s in document name %s", sheetName.c_str(), spriteHelperDocument.c_str());
        }
    }
    return NULL;
}

LHDictionary* SHDocumentLoader::dictionaryForAnimationNamed(const std::string& animName,const std::string& spriteHelperDocument)
{
    if(lastAnimName == animName &&
       lastAnimDocumentName == spriteHelperDocument &&
       lastAnimationDictionary != NULL){
        return lastAnimationDictionary;
    }
    
    SHSceneNode* shNode = sceneNodeForSHDocument(spriteHelperDocument);
    if(NULL != shNode)
    {
        LHDictionary* info = shNode->infoForAnimationNamed(animName);
        if(info){
            lastAnimationDictionary = info;
            lastAnimName = animName;
            lastAnimDocumentName = spriteHelperDocument;
            
            return info;
        }
        else {
            CCLog("Could not find info for animation named %s in document name %s", animName.c_str(), spriteHelperDocument.c_str());
        }
    }
    return NULL;
}

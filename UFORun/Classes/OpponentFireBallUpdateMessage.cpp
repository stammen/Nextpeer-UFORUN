//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "OpponentFireBallUpdateMessage.h"
#include "ViewPort.h"

#include <cpprest/json.h>

using namespace web;

OpponentFireBallUpdateMessage::OpponentFireBallUpdateMessage()
{
}

MultiplayerMessageType OpponentFireBallUpdateMessage::getMessageType() {
    return MULTIPLAYER_MESSAGE_TYPE_OPPONENT_FIRE_BALL_UPDATE;
}

OpponentFireBallUpdateMessage* OpponentFireBallUpdateMessage::createWithP2PData(const nextpeer::TournamentP2PData &tournamentP2PData)
{
    OpponentFireBallUpdateMessage* message = new OpponentFireBallUpdateMessage();
    if (message->extractDataFromByteVector(tournamentP2PData.message)) {
        
        message->extractHeaderFromData(tournamentP2PData);
        message->autorelease();
        
        return message;
    }
    
    return NULL;
}

OpponentFireBallUpdateMessage* OpponentFireBallUpdateMessage::createWithFireBall(const HeroFireBall* fireBall)
{
    OpponentFireBallUpdateMessage* message = new OpponentFireBallUpdateMessage();
    message->autorelease();
    
    // For some reason Cocos2D doesn't define getPosition() as const, so we'll const_cast here
    CCPoint screenPosition = const_cast<HeroFireBall*>(fireBall)->getPosition();
    
    b2Vec2 worldPos = ViewPort::getInstance()->screenToWorldCoordinate(screenPosition);
    
    message->_worldPositionX = worldPos.x;
    message->_worldPositionY = worldPos.y;
    message->_originPowerUpId = fireBall->getOriginPowerUpId();
    
    return message;
}

std::string OpponentFireBallUpdateMessage::toJson()
{
    OpponentFireBallUpdateMessageStruct messageStruct;
    messageStruct.header = this->getHeaderForDispatch();
    messageStruct.worldPositionX = this->_worldPositionX;
    messageStruct.worldPositionY = this->_worldPositionY;
    messageStruct.originPowerUpId = this->_originPowerUpId;

    json::value h = json::value::object();
    h[U("protocolVersion")] = json::value(messageStruct.header.protocolVersion);
    h[U("messageType")] = json::value(messageStruct.header.messageType);
    h[U("timeStamp")] = json::value(messageStruct.header.timeStamp);

    json::value m = json::value::object();
    m[U("messageStruct")] = json::value(h);
    m[U("worldPositionX")] = json::value(messageStruct.worldPositionX);
    m[U("worldPositionY")] = json::value(messageStruct.worldPositionY);
    m[U("originPowerUpId")] = json::value(messageStruct.originPowerUpId);

    utility::stringstream_t stream;
    m.serialize(stream);
    return std::string(stream.str().begin(), stream.str().end());
}

vector<unsigned char>& OpponentFireBallUpdateMessage::toByteVector()
{
    OpponentFireBallUpdateMessageStruct messageStruct;
    
    messageStruct.header = this->getHeaderForDispatch();
    
    messageStruct.worldPositionX = this->_worldPositionX;
    messageStruct.worldPositionY = this->_worldPositionY;
    messageStruct.originPowerUpId = this->_originPowerUpId;
    
    static vector<unsigned char> byteVector = vector<unsigned char>(sizeof(messageStruct));
    memcpy(&byteVector[0], &messageStruct, sizeof(messageStruct));
    
    return byteVector;
}

bool OpponentFireBallUpdateMessage::extractDataFromByteVector(const vector<unsigned char>& byteVector)
{
    if (byteVector.size() < sizeof(OpponentFireBallUpdateMessageStruct)) {
        // This can't be a valid message
        return false;
    }
    
    OpponentFireBallUpdateMessageStruct* structPtr = (OpponentFireBallUpdateMessageStruct*)&byteVector[0];
    
    this->_worldPositionX = structPtr->worldPositionX;
    this->_worldPositionY = structPtr->worldPositionY;
    this->_originPowerUpId = structPtr->originPowerUpId;
    
    return true;
}
//
//  Nextpeer Sample for Cocos2d-X
//  http://www.nextpeer.com
//
//  Created by Nextpeer development team.
//  Copyright (c) 2014 Innobell, Ltd. All rights reserved.
//

#include "OpponentAvatarNotificationMessage.h"
#include <cpprest/json.h>

using namespace web;

OpponentAvatarNotificationMessage::OpponentAvatarNotificationMessage()
{}

MultiplayerMessageType OpponentAvatarNotificationMessage::getMessageType() {
    return MULTIPLAYER_MESSAGE_TYPE_OPPONENT_AVATAR_NOTIFICATION;
}

OpponentAvatarNotificationMessage* OpponentAvatarNotificationMessage::createWithP2PData(const TournamentP2PData& tournamentP2PData)
{
    OpponentAvatarNotificationMessage* message = new OpponentAvatarNotificationMessage();
    
    if (message->extractDataFromByteVector(tournamentP2PData.message)) {
        
        message->extractHeaderFromData(tournamentP2PData);
        message->autorelease();
        return message;
    }
    
    delete message;
    return NULL;
}

OpponentAvatarNotificationMessage* OpponentAvatarNotificationMessage::createWithPlayerData(const PlayerData &playerData)
{
    OpponentAvatarNotificationMessage* msg = new OpponentAvatarNotificationMessage();
    
    msg->setAvatarIdentifier(playerData.getProfileType());
    
    msg->autorelease();
    
    return msg;
}


PlayerData* OpponentAvatarNotificationMessage::toPlayerData()
{
    PlayerData* data = PlayerData::create(getSenderId(), getSenderName(), getIsRecording());
    
    data->setProfileType(_avatarIdentifier);
    
    return data;
}

vector<unsigned char>& OpponentAvatarNotificationMessage::toByteVector()
{
    OpponentAvatarNotificationMessageStruct messageStruct;
    
    messageStruct.header = this->getHeaderForDispatch();
    
    messageStruct.avatarIdentifier = _avatarIdentifier;
    
    static vector<unsigned char> byteVector = vector<unsigned char>(sizeof(messageStruct));
    memcpy(&byteVector[0], &messageStruct, sizeof(messageStruct));
    
    return byteVector;
}

std::string OpponentAvatarNotificationMessage::toJson()
{
    OpponentAvatarNotificationMessageStruct messageStruct;
    messageStruct.header = this->getHeaderForDispatch();
    messageStruct.avatarIdentifier = _avatarIdentifier;

    json::value h = json::value::object();
    h[U("protocolVersion")] = json::value(messageStruct.header.protocolVersion);
    h[U("messageType")] = json::value(messageStruct.header.messageType);
    h[U("timeStamp")] = json::value(messageStruct.header.timeStamp);

    json::value m = json::value::object();
    m[U("messageStruct")] = json::value(h);
    m[U("avatarIdentifier")] = json::value(messageStruct.avatarIdentifier);
    
    utility::stringstream_t ss;
    m.serialize(ss);
    std::wstring w = ss.str();
    return std::string(w.begin(), w.end());
}

bool OpponentAvatarNotificationMessage::extractDataFromByteVector(const vector<unsigned char>& byteVector)
{
    if (byteVector.size() < sizeof(OpponentAvatarNotificationMessageStruct)) {
        return false;
    }
    
    OpponentAvatarNotificationMessageStruct* structPtr = (OpponentAvatarNotificationMessageStruct*)&byteVector[0];
    _avatarIdentifier = static_cast<GamePlayerProfileType>(structPtr->avatarIdentifier);
    
    return true;
}

bool OpponentAvatarNotificationMessage::fromJson(const std::string data)
{
    utility::stringstream_t ss;

    ss << std::wstring(data.begin(), data.end());
    json::value m = json::value::parse(ss);

    if (!m.has_field(U("avatarIdentifier")))
    {
        return false;
    }

    int a = m.at(U("avatarIdentifier")).as_integer();
    _avatarIdentifier = static_cast<GamePlayerProfileType>(a);

    return true;
}
/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

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
#include "CCUserDefault.h"
#include "platform/CCCommon.h"
#include "support/base64.h"
#include "support/ccUtils.h"
#include "platform/CCFileUtils.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#include "CCWinRTUtils.h"

using namespace Windows::Storage;
using namespace Windows::Foundation;
using namespace std;

#define XML_FILE_NAME "CCUserDefault.xml"

NS_CC_BEGIN


/**
 * WinRT implementation of CCUserDefault
 */
CCUserDefault* CCUserDefault::m_spUserDefault = 0;
string CCUserDefault::m_sFilePath = string("");
bool CCUserDefault::m_sbIsFilePathInitialized = false;

CCUserDefault::~CCUserDefault()
{
    CC_SAFE_DELETE(m_spUserDefault);
    m_spUserDefault = NULL;
}

CCUserDefault::CCUserDefault()
{
    m_spUserDefault = NULL;
}

void CCUserDefault::purgeSharedUserDefault()
{
    m_spUserDefault = NULL;
}

Platform::Object^ getPlatformKeyValue(const char* pKey)
{
    // check key
    if (!pKey)
    {
        return nullptr;
    }

    ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
    auto key = PlatformStringFromString(pKey);
    auto values = localSettings->Values;

    if (values->HasKey(key))
    {
        return values->Lookup(key);
    }

    return nullptr;
}

void setPlatformKeyValue(const char* pKey, PropertyValue^ value)
{
    // check key
    if (!pKey)
    {
        return;
    }

    ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
    auto values = localSettings->Values;
    values->Insert(PlatformStringFromString(pKey), value);
}

bool CCUserDefault::getBoolForKey(const char* pKey)
{
    return getBoolForKey(pKey, false);
}

bool CCUserDefault::getBoolForKey(const char* pKey, bool defaultValue)
{
	bool ret = defaultValue;
    auto value = getPlatformKeyValue(pKey);
    if (value)
    {
        ret = safe_cast<bool>(value);
    }
	return ret;
}

int CCUserDefault::getIntegerForKey(const char* pKey)
{
    return getIntegerForKey(pKey, 0);
}

int CCUserDefault::getIntegerForKey(const char* pKey, int defaultValue)
{
    int ret = defaultValue;
    auto value = getPlatformKeyValue(pKey);
    if (value)
    {
        ret = safe_cast<int>(value);
    }
    return ret;
}

float CCUserDefault::getFloatForKey(const char* pKey)
{
    return getFloatForKey(pKey, 0.0f);
}

float CCUserDefault::getFloatForKey(const char* pKey, float defaultValue)
{
    float ret = (float)getDoubleForKey(pKey, (double)defaultValue);

    return ret;
}

double  CCUserDefault::getDoubleForKey(const char* pKey)
{
    return getDoubleForKey(pKey, 0.0);
}

double CCUserDefault::getDoubleForKey(const char* pKey, double defaultValue)
{
    double ret = defaultValue;
    auto value = getPlatformKeyValue(pKey);
    if (value)
    {
        ret = safe_cast<double>(value);
    }
    return ret;
}

std::string CCUserDefault::getStringForKey(const char* pKey)
{
    return getStringForKey(pKey, "");
}

string CCUserDefault::getStringForKey(const char* pKey, const std::string & defaultValue)
{
    string ret = defaultValue;
    auto value = getPlatformKeyValue(pKey);
    if (value)
    {
        auto result = safe_cast<Platform::String^>(value);
        ret = PlatformStringToString(result);
    }

    return ret;
}

#if 0
Data CCUserDefault::getDataForKey(const char* pKey)
{
    return getDataForKey(pKey, Data::Null);
}

Data CCUserDefault::getDataForKey(const char* pKey, const Data& defaultValue)
{
    Data ret = defaultValue;
    std::string encodedData = getStringForKey(pKey,"");

    if (!encodedData.empty())
    {
        unsigned char* decodedData = nullptr;
        int decodedDataLen = base64Decode((unsigned char*) encodedData.c_str(), (unsigned int) encodedData.length(), &decodedData);
        if (decodedData && decodedDataLen > 0)
        {
            ret.fastSet(decodedData, decodedDataLen);
        }
    }

    return ret;
}
#endif

void CCUserDefault::setBoolForKey(const char* pKey, bool value)
{
    // check key
    if (!pKey)
    {
        return;
    }

    setPlatformKeyValue(pKey, dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(value)));
}

void CCUserDefault::setIntegerForKey(const char* pKey, int value)
{
    // check key
    if (! pKey)
    {
        return;
    }

    setPlatformKeyValue(pKey, dynamic_cast<PropertyValue^>(PropertyValue::CreateInt32(value)));
}

void CCUserDefault::setFloatForKey(const char* pKey, float value)
{
    setDoubleForKey(pKey, value);
}

void CCUserDefault::setDoubleForKey(const char* pKey, double value)
{
    // check key
    if (! pKey)
    {
        return;
    }

    setPlatformKeyValue(pKey, dynamic_cast<PropertyValue^>(PropertyValue::CreateDouble(value)));
}

void CCUserDefault::setStringForKey(const char* pKey, const std::string & value)
{
    // check key
    if (! pKey)
    {
        return;
    }

    setPlatformKeyValue(pKey, dynamic_cast<PropertyValue^>(PropertyValue::CreateString(PlatformStringFromString(value))));
}

#if 0
void CCUserDefault::setDataForKey(const char* pKey, const Data& value) {
    // check key
    if (! pKey)
    {
        return;
    }

    char *encodedData = 0;
    base64Encode(value.getBytes(), static_cast<unsigned int>(value.getSize()), &encodedData);

    setPlatformKeyValue(pKey, dynamic_cast<PropertyValue^>(PropertyValue::CreateString(PlatformStringFromString(encodedData))));

    if (encodedData)
        free(encodedData);
}
#endif

CCUserDefault* CCUserDefault::sharedUserDefault()
{
    if (!m_spUserDefault)
    {
        initXMLFilePath();
        m_spUserDefault = new (std::nothrow) CCUserDefault();
    }

    return m_spUserDefault;
}

bool CCUserDefault::isXMLFileExist()
{
    CCLOGWARN("CC_PLATFORM_WINRT no longer supports XML file for CCUserDefault. CCUserDefault uses native WinRT api.");
    return true;
}

void CCUserDefault::initXMLFilePath()
{
    if (! m_sbIsFilePathInitialized)
    {
        m_sFilePath = "";
        m_sbIsFilePathInitialized = true;
    }
}

// create new xml file
bool CCUserDefault::createXMLFile()
{
    CCLOGWARN("CC_PLATFORM_WINRT no longer supports XML file for CCUserDefault. CCUserDefault uses native WinRT api.");
    return false;
}

const string& CCUserDefault::getXMLFilePath()
{
    CCLOGWARN("CC_PLATFORM_WINRT no longer supports XML file for CCUserDefault. CCUserDefault uses native WinRT api.");
    return m_sFilePath;
}

void CCUserDefault::flush()
{
}

void CCUserDefault::deleteValueForKey(const char* key)
{
    // check the params
    if (!key)
    {
        CCLOG("the key is invalid");
    }

    ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
    auto values = localSettings->Values;
    values->Remove(PlatformStringFromString(key));

    flush();
}

NS_CC_END

#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
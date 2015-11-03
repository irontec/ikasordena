//
//  IkasAPI.cpp
//  IkasGame
//
//  Created by Sergio Garcia on 23/2/15.
//
//

#include "IkasAPI.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/ZipManager.h"
#include "../json/cJSON.h"
#include "../Helpers/AppleHelper.h"

static const Value JsonURL = Value(ApiURL);

bool IkasAPI::init()
{
    _errorCallback = NULL;
    _successCallback = NULL;
    _resultCallback = NULL;
    
    return true;
}


void IkasAPI::checkForLatestZipData(const IkasResultHandler& callback, const IkasHandler& errorCallback)
{
    this->_resultCallback = callback;
    this->_errorCallback = errorCallback;

    auto request = new HttpRequest();
    request->setUrl(JsonURL.asString().c_str());
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(IkasAPI::checkForLatestZipDataCallback ,this));
    HttpClient::getInstance()->send(request);
//    log("checkForLatestZipData..");
}

void IkasAPI::checkForLatestZipDataCallback(HttpClient* client, HttpResponse* response)
{
    
    log("checkForLatestZipDataCallback. response code: %ld", response->getResponseCode());
    
    std::vector<char>* buffer = response->getResponseData();
    log("buffet size :: %lu" , buffer->size());

    if (response->isSucceed() && !buffer->empty() && (response->getResponseCode() >= 200 && response->getResponseCode() < 400)) {
        std::string data(buffer->begin(), buffer->end());
        log("%s", data.c_str());
        cJSON *pRoot = cJSON_Parse(data.c_str());
        if (pRoot != NULL) {
            std::string zipName = cJSON_GetObjectItem(pRoot, "zipValid")->valuestring;
            std::string zipUrl = cJSON_GetObjectItem(pRoot, "downloadZip")->valuestring;
            int compareResult = zipName.compare(GameSettingsManager::getInstance()->getZipName().c_str());
            if (compareResult != 0) {
                GameSettingsManager::getInstance()->setDataAvalaible(true);
                GameSettingsManager::getInstance()->setZipName(zipName);
                GameSettingsManager::getInstance()->setZipUrl(zipUrl);
                if (_resultCallback) {
                    this->_resultCallback(this, true);
                }
            } else {
                if (_resultCallback) {
                    _resultCallback(this, false);
                }
            }
        } else {
            if (_resultCallback) {
                _resultCallback(this, false);
            }
        }
    } else {
        if (_errorCallback) {
            _errorCallback(this);
        }
    }
}


void IkasAPI::downloadLatestZip(const IkasHandler& successCallback, const IkasHandler& errorCallback, const IkasValueHandler progressCallback)
{
    _successCallback = successCallback;
    _errorCallback = errorCallback;
//    _progressCallback = progressCallback;
    
    const char* zipUrl = GameSettingsManager::getInstance()->getZipUrl().c_str();
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //CC_CALLBACK_1(IkasAPI::customProgressCallback, this)
    AppleHelper::downloadZip(zipUrl, ZipManager::getZipFilePath(), CC_CALLBACK_0(IkasAPI::customSuccessCallback, this), CC_CALLBACK_0(IkasAPI::customErrorCallback, this), progressCallback);
#else
    auto request = new HttpRequest();
    request->setUrl(zipUrl);
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(IkasAPI::downloadLatestZipCallback ,this));
    HttpClient::getInstance()->send(request);
#endif
//    log("downloadLatestZip..");
}

void IkasAPI::customSuccessCallback()
{
    log("IkasAPI::successCallback");
    _successCallback(this);
}

void IkasAPI::customErrorCallback()
{
    log("IkasAPI::errorCallback");
    _errorCallback(this);
}

//void IkasAPI::customProgressCallback(double percentage)
//{
//    log("IkasAPI::progressCallback percentaje:%f", percentage);
//}

void IkasAPI::downloadLatestZipCallback(HttpClient* client, HttpResponse* response)
{
    log("downloadLatestZipCallback. response code: %ld", response->getResponseCode());
    
    if (response->isSucceed() && (response->getResponseCode() >= 200 && response->getResponseCode() < 400)) {
        std::vector<char>* buffer = response->getResponseData();
        bool result = ZipManager::getInstance()->saveZipDataToDisk(buffer);
        if (result) {
            if (_successCallback) {
                _successCallback(this);
            }
        } else {
            if (_errorCallback) {
                _errorCallback(this);
            }
        }
    } else {
        if (_errorCallback) {
            _errorCallback(this);
        }
    }
}


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

#include "../Singletons/GamePlayPointsManager.h"
#include "../Helpers/DataFileManager.h"
#include "../Scenes/Lose.h"

static const Value JsonURL = Value(ApiURL);
static const Value RankURL = Value(RankingURL);

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

void IkasAPI::checkForLatestRankingData(const IkasRankingDataHandler& callback, const IkasHandler& errorCallback)
{
    this->_dataCallback = callback;
    this->_errorCallback = errorCallback;
    
    auto request = new HttpRequest();
    request->setUrl(RankURL.asString().c_str());
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(IkasAPI::checkForLatestRankingDataCallback, this));
    HttpClient::getInstance()->send(request);
}

void IkasAPI::checkForLatestRankingDataCallback(HttpClient* client, HttpResponse* response)
{
    log("checkForLatestRankingDataCallback. response code: %ld", response->getResponseCode());
    
    std::vector<char>* buffer = response->getResponseData();
    
    if (response->isSucceed() && !buffer->empty() && (response->getResponseCode() >= 200 && response->getResponseCode() < 400)) {
        std::string data(buffer->begin(), buffer->end());
        cJSON *pRoot = cJSON_Parse(data.c_str());
        if (pRoot != NULL) {
            cJSON *rankingArray = cJSON_GetObjectItem(pRoot, "ranking");
            int totalRankings = cJSON_GetArraySize(rankingArray);
            vector<RankingData*> ranking;
            for (int j = 0; j < totalRankings; j++)
            {
                cJSON *jsonRankingObj = cJSON_GetArrayItem(rankingArray, j);
                std::string rankingUser = cJSON_GetObjectItem(jsonRankingObj, "user")->valuestring;
                int rankingPoints = cJSON_GetObjectItem(jsonRankingObj, "points")->valueint;
                
                RankingData *rankingData = new RankingData();
                rankingData->setpoints(rankingPoints);
                rankingData->setuser(rankingUser.c_str());
                ranking.push_back(rankingData);
            }
            
            if (_dataCallback) {
                _dataCallback(true, ranking);
            }
        } else {
            if (_dataCallback) {
                _dataCallback(false, std::vector<RankingData*>());
            }
        }
    } else {
        if (_errorCallback) {
            _errorCallback(this);
        }
    }
}

void IkasAPI::sendRankingData(string username, int points, const IkasHandler& successCallback, const IkasHandler& errorCallback)
{
    this->_successCallback = successCallback;
    this->_errorCallback = errorCallback;
    
    auto request = new HttpRequest();
    request->setUrl(RankURL.asString().c_str());
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(CC_CALLBACK_2(IkasAPI::sendRankingDataCallback ,this));
    
    
    string pointsStr;
    ostringstream convert;
    convert << points;
    pointsStr = convert.str();
    
    //Post Request
    std::string postData = "";
    postData.append("user=").append(username);
    postData.append("&points=").append(pointsStr);
    
    request->setRequestData(postData.c_str(), strlen(postData.c_str()));
    HttpClient::getInstance()->send(request);
}

void IkasAPI::sendRankingDataCallback(HttpClient *client, HttpResponse* response)
{
    log("sendRankingDataCallback. response code: %ld", response->getResponseCode());
    
    std::vector<char>* buffer = response->getResponseData();
    log("buffet size :: %lu" , buffer->size());
    
    if (response->isSucceed() && !buffer->empty() && (response->getResponseCode() >= 200 && response->getResponseCode() < 400)) {
        _successCallback(this);
    } else {
        _errorCallback(this);
    }
}
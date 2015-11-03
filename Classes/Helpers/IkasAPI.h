//
//  IkasAPI.h
//  IkasGame
//
//  Created by Sergio Garcia on 23/2/15.
//
//

#ifndef __IkasAPI__
#define __IkasAPI__

#include "../Constants/Constants.h"

#include "cocos2d.h"
using namespace cocos2d;

#include "network/HttpClient.h"
using namespace cocos2d::network;

#include <../../cocos2d/extensions/assets-manager/Downloader.h>
using namespace cocos2d::extension;

class IkasAPI : public Ref
{
public:
    virtual bool init();
    CREATE_FUNC(IkasAPI);
    
    void checkForLatestZipData(const IkasResultHandler& callback, const IkasHandler& errorCallback);
    void downloadLatestZip(const IkasHandler& successCallback, const IkasHandler& errorCallback, const IkasValueHandler progressCallback);
    
    void checkForLatestRankingData(const IkasRankingDataHandler& dataCallback, const IkasHandler& errorCallback);
    void sendRankingData(string username, int points, const IkasHandler& successCallback, const IkasHandler& errorCallback);
protected:
    IkasResultHandler _resultCallback;
    IkasHandler _successCallback, _errorCallback;
    IkasRankingDataHandler _dataCallback;
private:
    void checkForLatestZipDataCallback(HttpClient* client , HttpResponse* response);
    void downloadLatestZipCallback(HttpClient* client , HttpResponse* response);
    //    void customProgressCallback(double percentage);
    void customSuccessCallback();
    void customErrorCallback();
    
    void checkForLatestRankingDataCallback(HttpClient* client , HttpResponse* response);
    void sendRankingDataCallback(HttpClient *client, HttpResponse* response);
};

#endif /* defined(__IkasGame__IkasAPI__) */

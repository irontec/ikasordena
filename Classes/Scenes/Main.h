//
//  Main.h
//  IkasGame
//
//  Created by Sergio Garcia on 30/01/15.
//
//

#ifndef __IkasCocosGame__Main__
#define __IkasCocosGame__Main__

#include "cocos2d.h"
#include "../Constants/Constants.h"
#include "../Helpers/IkasAPI.h"
#include "../CustomGUI/SpriteButton.h"

using namespace cocos2d;

class Main : public Layer
{
    Layer *playLayer, *loadingLayer;
public:
    virtual bool init();
    ~Main();
    static Scene* createScene();
    CREATE_FUNC(Main);
    void moveResourcesZipAndUnzip();
    void successDataDownloaded(Ref* sender);
    void errorDataDownloaded(Ref* sender);
    void progressDataDownloaded(double percentage);
protected:
    void switchSoundSettings(Ref* sender);
    void changeScene(Ref* sender);
    void openInfo(Ref* sender);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
private:
    void showLoading(bool show);
    IkasAPI *_api;
    Label *_labelLoadingAnimation;
    void checkNewData();
    
    void downloadLatestData(const IkasHandler& successCallback, const IkasHandler& errorCallback, const IkasValueHandler progressCallback);
    
    SpriteButton *_buttonPlay, *_buttonAbout, *_buttonSFXSettings, *_buttonRanking;
};
#endif /* defined(__IkasCocosGame__Main__) */

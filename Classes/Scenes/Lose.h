//
//  Lose.h
//  IkasGame
//
//  Created by Sergio Garcia on 11/2/15.
//
//

#ifndef __IkasGame__Lose__
#define __IkasGame__Lose__

#include "../Constants/Constants.h"


#include "ui/UIEditBox/UIEditBox.h"
#include "ExtensionMacros.h"
#include "../Helpers/IkasAPI.h"
#include "../CustomGUI/SpriteButton.h"


#include "cocos2d.h"
using namespace cocos2d;

class Lose : public Layer
{
    Layer *resultLayer, *loadingLayer;
public:
    virtual bool init();
    static Scene* createScene();
    CREATE_FUNC(Lose);
    
    SpriteButton* _sendPointsButton;
    ui::EditBox* _usernameBox;
    
//    std::string playerName;
//    int points;
protected:
    void resetGame(Ref* sender);
    void switchSoundSettings(Ref* sender);
    void returnHome(Ref* sender);
    void showLoading(bool show);
    
    
    
    
    
    void sendPoints(Ref* sender);
    
    IkasAPI *_api;
    void sendRanking(const IkasHandler& successCallback, const IkasHandler& errorCallback);
    void successDataUploaded(Ref* sender);
    void errorDataUploaded(Ref* sender);
};
#endif /* defined(__IkasGame__Lose__) */

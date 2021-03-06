//
//  GamePlay.h
//  IkasGame
//
//  Created by Axier Fernandez on 10/2/15.
//
//

#ifndef __IkasGame__GamePlay__
#define __IkasGame__GamePlay__

#include "cocos2d.h"
using namespace cocos2d;

#include "../Constants/CommonProtocols.h"
#include "../CustomGUI/SpriteButton.h"

#include "../GameModels/MainCategory.h"
#include "../GameModels/SubCategory.h"
#include "../GameModels/Option.h"
#include "../Singletons/ZipManager.h"

enum class TouchState
{
    START,
    MOVING,
    FINISH
};

class GamePlay : public Layer, public GamePlayDelegate
{
public:
    virtual bool init();
    static Scene* createScene();
    CREATE_FUNC(GamePlay);
    
    EventListenerTouchOneByOne *addEvents();
    
    virtual bool touchBegan(Touch *touch, Event *pEvent);
    virtual void touchEnded(Touch *touch, Event *pEvent);
    virtual void touchMoved(Touch *touch, Event *pEvent);
    virtual void touchCancelled(Touch *pTouch, Event *pEvent);
    
    // GamePlayDelegate
    virtual void loadNextGamePlay();
    virtual void resumeGamePlay();
    virtual void restartGame();
    virtual void updateScreenGameStats();
    // Time control
    void timeElapsed();
    
    //Validate option
    static bool isValidSubcategory(SubCategory* subCategory);
    static bool isValidWord(string word);
private:
    vector<MainCategory*> _categories;
    vector<SubCategory*> _subCategories;
    vector<int> _loadedIndex;
    
    void clearGameLayer();
    void createGameLayer(Option* option);
    DrawNode* createCircleLayer(Vec2 anchorPoint,  Vec2 position, Size size, Color4F color);
    
    void setupLevelSettings();
    void resetTimer();
    void pauseGame(Ref* sender);

    Label *_labelCategory, *_labelPoints;//, *_labelLevel;
    Layer *_starsLayer;
    Layer *_gameLayer, *_touchLayer;

    
    Layer *_unorderedLayer, *_orderedLayer;
    LayerColor *_resultLayer;
    Map<int, Label*> _unorderedLabels, _orderedLabels;

    Value _currentLives, _totalFails;
    Option* _currentOption;
    EventListenerTouchOneByOne *_moveListener;
    
    TouchState _touchState;
    Vec2 _initialPosition;
    Label *_currentLabel;
    int _initialIndex, _endIndex;
    
    int indexForTouch(Layer *layer,  Touch *touch);
    void startMovingLabel(Touch *touch);
    void updateLabelPosition(Touch *touch);
    void updateLabelToFinalPosition();
    void restoreLabelToInitialPosition();
    
    void showDragResult(bool success);
    
    ProgressTimer* _progress;
    ProgressFromTo* _action;
    Sequence* _sequence;
    
    float _elapsedTime, _currentLevelTime, _maxTime;
    int _totalSuccessScreens;
    
    SpriteButton* _pauseButton;
    void updateLivesView();
    void checkGameStatus();
    void openSuccessScreen();
};
#endif /* defined(__IkasGame__GamePlay__) */

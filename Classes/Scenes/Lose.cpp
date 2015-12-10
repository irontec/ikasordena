//
//  Lose.cpp
//  IkasGame
//
//  Created by Sergio Garcia on 11/2/15.
//
//

#include "Lose.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Singletons/AppManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../CustomGUI/SpriteButton.h"

Scene* Lose::createScene()
{
    SceneManager::getInstance()->saveCurrentScene();
    
    auto *scene = Scene::create();
    auto *layer = Lose::create();
    layer->setTag(2339);
    
    scene->addChild(layer);
    
    return scene;
}

bool Lose::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    
    resultLayer = Layer::create();
    resultLayer->setContentSize(visibleRect.size);
    resultLayer->setPosition(0, 0);
    resultLayer->setVisible(true);
    
    auto background = Sprite::create(ImageManager::getImage("background"), visibleRect);
    background->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    background->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(background);
    
    auto buttonNextGamePlay = SpriteButton::create(ImageManager::getImage("restart"), 1.0f, CC_CALLBACK_1(Lose::resetGame, this));
    buttonNextGamePlay->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonNextGamePlay = ScreenSizeManager::getScreenPositionFromPercentage(50, 50);
    buttonNextGamePlay->setPosition(positionButtonNextGamePlay);
    resultLayer->addChild(buttonNextGamePlay);
    
    auto labelResume = Label::createWithTTF(LanguageManager::getLocalizedText("Lose", "reset"), MainRegularFont, 70);
    labelResume->setAlignment(TextHAlignment::CENTER);
    labelResume->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    labelResume->setTextColor(IkasGrayDark);
    Vec2 positionLabelResume = buttonNextGamePlay->getPosition();
    positionLabelResume.y -= buttonNextGamePlay->getBoundingBox().size.height / 2;
    positionLabelResume.y -= ScreenSizeManager::getHeightFromPercentage(1);
    labelResume->setPosition(positionLabelResume);
    resultLayer->addChild(labelResume);
    
    std::ostringstream totalPoints;
    totalPoints << GamePlayPointsManager::getInstance()->getCurrentPoints() << " " << LanguageManager::getLocalizedText("Lose", "points-value");

    auto labelPointsValue = Label::createWithTTF(totalPoints.str(), MainRegularFont, 75);
    labelPointsValue->setAlignment(TextHAlignment::CENTER);
    labelPointsValue->setAnchorPoint(Point::ANCHOR_MIDDLE);
    labelPointsValue->setTextColor(IkasPurpleLight);
    Vec2 positionLabelPointsValue = buttonNextGamePlay->getPosition();
    positionLabelPointsValue.y += buttonNextGamePlay->getBoundingBox().size.height / 2;
    positionLabelPointsValue.y += labelPointsValue->getBoundingBox().size.height / 2;
    positionLabelPointsValue.y += ScreenSizeManager::getHeightFromPercentage(1);
    labelPointsValue->setPosition(positionLabelPointsValue);
    resultLayer->addChild(labelPointsValue);
    
    auto labelPointsTitle = Label::createWithTTF(LanguageManager::getLocalizedText("Lose", "points-title"), MainRegularFont, 75);
    labelPointsTitle->setAlignment(TextHAlignment::CENTER);
    labelPointsTitle->setAnchorPoint(Point::ANCHOR_MIDDLE);
    labelPointsTitle->setTextColor(IkasPurpleLight);
    Vec2 positionLabelPointsTitle = labelPointsValue->getPosition();
    positionLabelPointsTitle.y += labelPointsValue->getBoundingBox().size.height / 2;
    positionLabelPointsTitle.y += labelPointsTitle->getBoundingBox().size.height / 2;
    positionLabelPointsTitle.y += ScreenSizeManager::getHeightFromPercentage(1);
    labelPointsTitle->setPosition(positionLabelPointsTitle);
    resultLayer->addChild(labelPointsTitle);

    auto labelTitle = Label::createWithTTF(LanguageManager::getLocalizedText("Lose", "title"), MainRegularFont, 90);
    labelTitle->setAlignment(TextHAlignment::CENTER);
    labelTitle->setAnchorPoint(Point::ANCHOR_MIDDLE);
    labelTitle->setTextColor(IkasPurple);
    Vec2 positionLabelTitle = labelPointsTitle->getPosition();
    positionLabelTitle.y += labelPointsTitle->getBoundingBox().size.height / 2;
    positionLabelTitle.y += labelTitle->getBoundingBox().size.height / 2;
    positionLabelTitle.y += ScreenSizeManager::getHeightFromPercentage(1);
    labelTitle->setPosition(positionLabelTitle);
    resultLayer->addChild(labelTitle);
    
    auto buttonSoundSettings = SpriteButton::create(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage), 0.30f, CC_CALLBACK_1(Lose::switchSoundSettings, this));
    buttonSoundSettings->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonSoundSettings = ScreenSizeManager::getScreenPositionFromPercentage(80, 15);
    buttonSoundSettings->setPosition(positionButtonSoundSettings);
    resultLayer->addChild(buttonSoundSettings);
    
    auto buttonHome = SpriteButton::create(ImageManager::getImage("home"), 0.30f, CC_CALLBACK_1(Lose::returnHome, this));
    buttonHome->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonHome = ScreenSizeManager::getScreenPositionFromPercentage(22, 15);
    buttonHome->setPosition(positionButtonHome);
    resultLayer->addChild(buttonHome);
    
    auto buttonEditName = Sprite::create(ImageManager::getImage("editBoxRanking"));
    buttonEditName->setAnchorPoint(Point::ANCHOR_MIDDLE);
    buttonEditName->setScale(0.65f, 0.9f);
    Vec2 positionbuttonEditName = ScreenSizeManager::getScreenPositionFromPercentage(50, 18);
    buttonEditName->setPosition(positionbuttonEditName);
    resultLayer->addChild(buttonEditName);
    
    _usernameBox = ui::EditBox::create(Size(ScreenSizeManager::getWidthFromPercentage(15), ScreenSizeManager::getHeightFromPercentage(12)), ui::Scale9Sprite::create(ImageManager::getImage("editBoxRankingBackground")));
    Vec2 usernamePosition = buttonEditName->getPosition();
    _usernameBox->setPosition(usernamePosition);
    _usernameBox->setFontName(MainRegularFont.c_str());
    _usernameBox->setFontSize(100);
    _usernameBox->setFontColor(Color3B(IkasPurpleLight));
    _usernameBox->setPlaceHolder(LanguageManager::getLocalizedText("Lose", "name").c_str());
    _usernameBox->setPlaceholderFontSize(20);
    _usernameBox->setPlaceholderFontColor(Color3B(IkasPurple));
    _usernameBox->setMaxLength(3);
    _usernameBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    resultLayer->addChild(_usernameBox);
    
    _sendPointsButton = SpriteButton::create(ImageManager::getImage("buttonSendRanking"), 0.50f, CC_CALLBACK_1(Lose::sendPoints, this));
    Vec2 sendPointsButtonPosition = ScreenSizeManager::getScreenPositionFromPercentage(50, 8);
    _sendPointsButton->setPosition(sendPointsButtonPosition);
    resultLayer->addChild(_sendPointsButton);
    
    auto sendPointsLabel = Label::createWithTTF(LanguageManager::getLocalizedText("Lose", "sendPoints"), MainRegularFont, 30);
    sendPointsLabel->setPosition(_sendPointsButton->getPositionX()/2, _sendPointsButton->getPositionY()/2);
    sendPointsLabel->setTextColor(IkasPurpleLight);
    Vec2 sendPointsLabelPosition = _sendPointsButton->getPosition();
    sendPointsLabel->setPosition(sendPointsLabelPosition);
    resultLayer->addChild(sendPointsLabel, 10);
    
    this->addChild(resultLayer);
    
    // Loading Layer
    loadingLayer = Layer::create();
    loadingLayer->setContentSize(visibleRect.size);
    loadingLayer->setPosition(0, 0);
    loadingLayer->setVisible(false);
    
    auto loadingAnimation = Sprite::create(ImageManager::getImage("loading"));
    loadingAnimation->setScale(0.40f);
    loadingAnimation->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionLoadingAnimation = ScreenSizeManager::getScreenPositionFromPercentage(50, 50);
    
    loadingAnimation->setPosition(positionLoadingAnimation);
    loadingLayer->addChild(loadingAnimation);
    
    ActionInterval* rotate = RotateBy::create(1.0f, 360.0f);
    RepeatForever *repeat = RepeatForever::create(rotate);
    loadingAnimation->runAction(repeat);
    
    this->addChild(loadingLayer);

    _api = new IkasAPI();
    return true;
}

void Lose::resetGame(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    GamePlayPointsManager::getInstance()->resetCurrentPoints();
    SceneManager::getInstance()->loadSavedScene();
    AppManager::getInstance()->restartGame();
}

void Lose::switchSoundSettings(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    GameSettingsManager::getInstance()->switchSFX();
    GameSettingsManager::getInstance()->switchMusic();
    SpriteButton *item = static_cast<SpriteButton*>(sender);
    item->setTexture(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage));
}

void Lose::returnHome(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    AppManager::getInstance()->setGamePlayDelegate(NULL);
    SceneManager::getInstance()->runSceneWithType(SceneType::MAIN);
    SceneManager::getInstance()->removeSavedScene();
}


void Lose::showLoading(bool show)
{
    loadingLayer->setVisible(show);
    resultLayer->setVisible(!show);
    
    _sendPointsButton->setVisible(!show);
    _sendPointsButton->setEnabled(!show);
    _usernameBox->setVisible(!show);
    _usernameBox->setEnabled(!show);
}

void Lose::sendPoints(Ref* sender)
{
    auto username = string(_usernameBox->getText());
    if (username != "") {
        showLoading(true);
        
        std::for_each(username.begin(), username.end(), [](char&c){c=std::toupper(c);});
        
        auto points = GamePlayPointsManager::getInstance()->getCurrentPoints();
        
        _api->sendRankingData(username, points, CC_CALLBACK_1(Lose::successDataUploaded, this), CC_CALLBACK_1(Lose::errorDataUploaded, this));
    }
}

void Lose::successDataUploaded(Ref* sender)
{
    showLoading(false);
    SceneManager::getInstance()->runSceneWithType(SceneType::RANKING);
    GamePlayPointsManager::getInstance()->resetCurrentPoints();
    AppManager::getInstance()->setGamePlayDelegate(NULL);
    SceneManager::getInstance()->removeSavedScene();
}

void Lose::errorDataUploaded(Ref* sender)
{
    showLoading(false);
}

//
//  Ranking.cpp
//  IkasIkusi
//
//  Created by Asier Cortes on 23/4/15.
//
//

#include "Ranking.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Singletons/AppManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../Helpers/DataFileManager.h"
#include "../CustomGUI/SpriteButton.h"
#include <string>

const Value DelayMinTime = Value(2.0f);
const Value TableViewItemHeightValue = Value(100);

#define TableViewWidthPercentage 80
#define TableViewHeightPercentage 70
#define TableViewXPositionPercentage 50
#define TableViewYPositionPercentage 50


enum class TablewViewPageDirection
{
    PREVIOUS,
    NEXT
};



Scene* Ranking::createScene()
{
    auto *scene = Scene::create();
    
    auto *layer = Ranking::create();
    layer->setTag(2339);
    
    scene->addChild(layer);
    
    return scene;
}

bool Ranking::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    _totalLabels = 2;
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    
    auto background = Sprite::create(ImageManager::getImage("background"), visibleRect);
    background->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    background->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(background);
    
    auto backgroundWin = Sprite::create(ImageManager::getImage("background-win"), visibleRect);
    backgroundWin->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    backgroundWin->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(backgroundWin);

    
    buttonsLayer = Layer::create();
    buttonsLayer->setContentSize(visibleRect.size);
    buttonsLayer->setPosition(0, 0);
    
    auto buttonBack = SpriteButton::create(ImageManager::getImage("back"), 0.30f, CC_CALLBACK_1(Ranking::returnHome, this));
    buttonBack->setTag(static_cast<int>(SceneType::MAIN));
    buttonBack->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonBack = ScreenSizeManager::getScreenPositionFromPercentage(5, 5);
    positionButtonBack.x += buttonBack->getBoundingBox().size.width / 2;
    positionButtonBack.y += buttonBack->getBoundingBox().size.height / 2;
    buttonBack->setPosition(positionButtonBack);
    buttonsLayer->addChild(buttonBack);
    
    auto buttonAbout = SpriteButton::create(ImageManager::getImage("info"), 0.30f, CC_CALLBACK_1(Ranking::openInfo, this));
    buttonAbout->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonAbout = ScreenSizeManager::getScreenPositionFromPercentage(95, 5);
    positionButtonAbout.x -= buttonAbout->getBoundingBox().size.width / 2;
    positionButtonAbout.y += buttonAbout->getBoundingBox().size.height / 2;
    buttonAbout->setPosition(positionButtonAbout);
    buttonsLayer->addChild(buttonAbout);
    
    auto buttonSFXSettings = SpriteButton::create(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage), 0.30f, CC_CALLBACK_1(Ranking::switchSoundSettings, this));
    buttonSFXSettings->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonSFXSettings = buttonAbout->getPosition();
    positionButtonSFXSettings.x -= ScreenSizeManager::getWidthFromPercentage(5);
    positionButtonSFXSettings.x -= buttonSFXSettings->getBoundingBox().size.width;
    buttonSFXSettings->setPosition(positionButtonSFXSettings);
    buttonsLayer->addChild(buttonSFXSettings);
    
    this->addChild(buttonsLayer);
    
    _labelLoading = Label::createWithTTF(LanguageManager::getLocalizedText("Ranking", "loading"), MainRegularFont, 100);
    _labelLoading->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _labelLoading->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    _labelLoading->setTextColor(IkasPurple);
    this->addChild(_labelLoading);
    
    /* TableView */
    _tableViewRanking = TableView::create(this, Size(ScreenSizeManager::getWidthFromPercentage(TableViewWidthPercentage), ScreenSizeManager::getHeightFromPercentage(TableViewHeightPercentage)));
    _tableViewRanking->setColor(Color3B(Color3B(200, 200, 200)));
    Vec2 positionTablewView = ScreenSizeManager::getScreenPositionFromPercentage(TableViewXPositionPercentage, TableViewYPositionPercentage);
    positionTablewView.x -= _tableViewRanking->getBoundingBox().size.width/2;
    positionTablewView.y -= _tableViewRanking->getBoundingBox().size.height/2;
    _tableViewRanking->setPosition(positionTablewView);
    this->addChild(_tableViewRanking);
    
    auto labelUserTitle = Label::createWithTTF(LanguageManager::getLocalizedText("Ranking", "player"), MainRegularFont, 80);
    labelUserTitle->setAnchorPoint(Point::ANCHOR_MIDDLE);
    labelUserTitle->setPosition(Vec2(_tableViewRanking->getPositionX() + (_tableViewRanking->getBoundingBox().size.width) * 0.25f, _tableViewRanking->getPositionY() + (_tableViewRanking->getBoundingBox().size.height) * 1.1f));
    labelUserTitle->setTextColor(IkasPurpleLight);
    this->addChild(labelUserTitle);
    
    auto labelPointsTitle = Label::createWithTTF(LanguageManager::getLocalizedText("Ranking", "points"), MainRegularFont, 80);
    labelPointsTitle->setAnchorPoint(Point::ANCHOR_MIDDLE);
    labelPointsTitle->setPosition(Vec2(_tableViewRanking->getPositionX() + (_tableViewRanking->getBoundingBox().size.width) * 0.75f, _tableViewRanking->getPositionY() + (_tableViewRanking->getBoundingBox().size.height) * 1.1f));
    labelPointsTitle->setTextColor(IkasPurpleLight);
    this->addChild(labelPointsTitle);
    
    _api = new IkasAPI();
    _api->checkForLatestRankingData(CC_CALLBACK_2(Ranking::successRankingDataDownloaded, this), CC_CALLBACK_1(Ranking::errorDataDownloaded, this));
    
    return true;
}

/*TableViewDataSoruce */
ssize_t Ranking::numberOfCellsInTableView(TableView *table) {
    return _rankingData.size();
}

Size Ranking::tableCellSizeForIndex(TableView *table, ssize_t idx) {
    return Size(_tableViewRanking->getBoundingBox().size.width, TableViewItemHeightValue.asFloat());
}

TableViewCell* Ranking::tableCellAtIndex(TableView *table, ssize_t idx) {
    RankingData *rankingData = _rankingData.at(idx);
    
    table->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    TableViewCell *cell = table->dequeueCell();
    
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    }
    cell->removeAllChildren();
    
    auto colorLayer = LayerColor::create();
    colorLayer->setAnchorPoint(Point::ZERO);
    colorLayer->setContentSize(Size(_tableViewRanking->getBoundingBox().size.width, TableViewItemHeightValue.asFloat()));
    colorLayer->setPosition(Vec2(0 * _tableViewRanking->getBoundingBox().size.width, 0));
    cell->addChild(colorLayer);
    
    string numberRanking;
    ostringstream convert2;
    convert2 << idx + 1;
    numberRanking = convert2.str();
    
    auto padding = 25;
    auto number = Label::createWithTTF(numberRanking, MainRegularFont, 60);
    number->setContentSize(Size(_tableViewRanking->getBoundingBox().size.width / _totalLabels - padding, TableViewItemHeightValue.asFloat()));
    number->setWidth(_tableViewRanking->getBoundingBox().size.width / _totalLabels);
    number->setHeight(TableViewItemHeightValue.asFloat());
    number->setVerticalAlignment(TextVAlignment::CENTER);
    number->setHorizontalAlignment(TextHAlignment::LEFT);
    number->setAnchorPoint(Point::ZERO);
    if (idx == 0) {
        number->setTextColor(IkasPurple);
    } else {
        number->setTextColor(IkasPurpleLight);
    }
    number->setPosition(Vec2(0 * _tableViewRanking->getBoundingBox().size.width + padding/ _totalLabels, 0));
    cell->addChild(number);
    
    
    auto name = Label::createWithTTF(rankingData->getuser(), MainRegularFont, 60);
    name->setContentSize(Size(_tableViewRanking->getBoundingBox().size.width / _totalLabels, TableViewItemHeightValue.asFloat()));
    name->setWidth(_tableViewRanking->getBoundingBox().size.width / _totalLabels);
    name->setHeight(TableViewItemHeightValue.asFloat());
    name->setVerticalAlignment(TextVAlignment::CENTER);
    name->setHorizontalAlignment(TextHAlignment::CENTER);
    name->setAnchorPoint(Point::ZERO);
    if (idx == 0) {
        name->setTextColor(IkasPurple);
    } else {
        name->setTextColor(IkasPurpleLight);
    }
    name->setPosition(Vec2(0 * _tableViewRanking->getBoundingBox().size.width / _totalLabels, 0));
    cell->addChild(name);
    
    string points;
    ostringstream convert;
    convert << rankingData->getpoints();
    points = convert.str();
    
    auto pointsLabel = Label::createWithTTF(points, MainRegularFont, 60);
    pointsLabel->setContentSize(Size(_tableViewRanking->getBoundingBox().size.width / _totalLabels, TableViewItemHeightValue.asFloat()));
    pointsLabel->setHeight(TableViewItemHeightValue.asFloat());
    pointsLabel->setWidth(_tableViewRanking->getBoundingBox().size.width / _totalLabels);
    pointsLabel->setVerticalAlignment(TextVAlignment::CENTER);
    pointsLabel->setHorizontalAlignment(TextHAlignment::CENTER);
    pointsLabel->setAnchorPoint(Point::ZERO);
    if (idx == 0) {
        pointsLabel->setTextColor(IkasPurple);
    } else {
        pointsLabel->setTextColor(IkasPurpleLight);
    }
    pointsLabel->setPosition(Vec2(1 * _tableViewRanking->getBoundingBox().size.width / _totalLabels, 0));
    cell->addChild(pointsLabel);
//    
//    if (idx % 2 == 0) {
//        number->setTextColor(IkasWhite);
//        name->setTextColor(IkasWhite);
//        pointsLabel->setTextColor(IkasWhite);
//        colorLayer->setColor(Color3B(IkasRed));
//        colorLayer->setOpacity(60);
//    } else {
//        number->setTextColor(IkasRed);
//        name->setTextColor(IkasRed);
//        pointsLabel->setTextColor(IkasRed);
//        colorLayer->setColor(Color3B(IkasWhite));
//        colorLayer->setOpacity(0);
//    }
    
    LayerColor *topSeparator;
    if (idx % 2 == 0) {
        topSeparator = LayerColor::create(IkasPurpleLight);
    } else {
        topSeparator = LayerColor::create(IkasPurpleLightAlpha);
    }
    topSeparator->setContentSize(Size(_tableViewRanking->getBoundingBox().size.width, 2));
    topSeparator->setPosition(Vec2(0, TableViewItemHeightValue.asFloat() - 4));
    topSeparator->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    cell->addChild(topSeparator);

    
    if (idx == _rankingData.size() - 1) {
        if (idx % 2 != 0) {
            topSeparator = LayerColor::create(IkasPurpleLight);
        } else {
            topSeparator = LayerColor::create(IkasPurpleLightAlpha);
        }
        topSeparator->setContentSize(Size(_tableViewRanking->getBoundingBox().size.width, 2));
        topSeparator->setPosition(Vec2(0, 0));
        topSeparator->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        cell->addChild(topSeparator);
    }
    return cell;
}

void Ranking::switchSoundSettings(Ref* sender) {
    SoundManager::getInstance()->sfxPlay("button");
    GameSettingsManager::getInstance()->switchSFX();
    GameSettingsManager::getInstance()->switchMusic();
    SpriteButton *item = static_cast<SpriteButton*>(sender);
    item->setTexture(ImageManager::getImage(GameSettingsManager::getInstance()->getIsSFXOn() ? SoundEnableImage : SoundDisableImage));
}

void Ranking::changeScene(Ref* sender) {
    SoundManager::getInstance()->sfxPlay("button");
    SpriteButton *item = static_cast<SpriteButton*>(sender);
    SceneManager::getInstance()->runSceneWithType(static_cast<SceneType>(item->getTag()));
}

void Ranking::openInfo(Ref* sender) {
    SoundManager::getInstance()->sfxPlay("button");
    Application::getInstance()->openURL(InfoURL);
}

/* API Callbacks */

void Ranking::successRankingDataDownloaded(bool result, vector<RankingData*> data) {
    if (result) {
        _rankingData = data;
        this->removeChild(_labelLoading);
        _tableViewRanking->reloadData();
    } else {
        // TODO: Manage error
    }
}

void Ranking::errorDataDownloaded(Ref* sender) {
    // TODO: Manage error
}

void Ranking::returnHome(Ref* sender) {
    SoundManager::getInstance()->sfxPlay("button");
    SceneManager::getInstance()->runSceneWithType(SceneType::MAIN);
}

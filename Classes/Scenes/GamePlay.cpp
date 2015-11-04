//
//
//  GamePlay.cpp
//  IkasGame
//
//  Created by Axier Fernandez on 10/2/15.
//
//

#include "GamePlay.h"
#include "../Singletons/GameSettingsManager.h"
#include "../Singletons/GamePlayPointsManager.h"
#include "../Singletons/AppManager.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../Helpers/DataFileManager.h"

const Value PlayMaxTime = Value(40.0f);
const Value TimeToReduce = Value(2.0f);
const Value TimeMinPercentage = Value(25);
const Value SuccessTime = Value(1.5);


const Value MaxLetters = Value(15);
const Value MaxLives = Value(6);
const Value LetterLayerSize = Value(140);
const Value LetterFontSizeLayerRelation = Value(0.85);
const Value LetterFontSize = Value(LetterLayerSize.asFloat() * LetterFontSizeLayerRelation.asFloat());
const Value LetterLayerMargin = Value(10);
const Value LetterLayerBorderWidth = Value(8);

const float GameLayerBorderWidth = 2;
const Color4B GameLayerBorderColor = IkasGrayLight;

Scene* GamePlay::createScene()
{
    auto *scene = Scene::create();
    auto *layer = GamePlay::create();
    layer->setTag(2339);
    
    scene->addChild(layer);
    
    return scene;
}

bool GamePlay::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    DataFileManager *dataManager = DataFileManager::getInstance();
    _categories = dataManager->getMainCategories();
    if (_categories.empty()) {
        dataManager->parseDataFile();
        _categories = dataManager->getMainCategories();
    }
    _subCategories = _categories.at(GamePlayPointsManager::getInstance()->getCurrentCategory())->getFilteredSubCategoriesByLevel(GamePlayPointsManager::getInstance()->getCurrentDifficulty());
    AppManager::getInstance()->setGamePlayDelegate(this);
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    
    auto background = Sprite::create(ImageManager::getImage("background"), visibleRect);
    background->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    background->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(background);

    _gameLayer = LayerColor::create(Color4B(255, 255, 255, 255));
    float margins = ScreenSizeManager::getHeightFromPercentage(3);
    
    Size gameLayerSize = Size(0, 0);
    gameLayerSize.height = ScreenSizeManager::getHeightFromPercentage(100);
    gameLayerSize.height -= margins * 2;
    gameLayerSize.width = ScreenSizeManager::getWidthFromPercentage(65);

    _gameLayer->setContentSize(gameLayerSize);
    
    _gameLayer->setPosition(ScreenSizeManager::getWidthFromPercentage(100) - margins - _gameLayer->getBoundingBox().size.width, this->getBoundingBox().size.height / 2 - _gameLayer->getBoundingBox().size.height / 2);
    
    auto borderLayer = LayerColor::create(GameLayerBorderColor);
    Size borderLayerSize = _gameLayer->getBoundingBox().size;
    borderLayerSize.width += 2 * GameLayerBorderWidth;
    borderLayerSize.height += 2 * GameLayerBorderWidth;
    borderLayer->setContentSize(borderLayerSize);
    Vec2 borderLayerPosition = _gameLayer->getPosition();
    borderLayerPosition.x -= GameLayerBorderWidth;
    borderLayerPosition.y -= GameLayerBorderWidth;
    borderLayer->setPosition(borderLayerPosition);
    
    this->addChild(borderLayer);
    this->addChild(_gameLayer);

    _touchLayer = Layer::create();
    _touchLayer->setContentSize(gameLayerSize);
    _gameLayer->addChild(_touchLayer);
    
    auto listener = addEvents();
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _touchLayer);
    _touchState = TouchState::FINISH;
    
    
    auto hudLayer = Layer::create();
    Size hudLayerSize = Size(0, 0);
    hudLayerSize.width = _gameLayer->getBoundingBox().origin.x - 2 * margins;
    hudLayerSize.height = _gameLayer->getBoundingBox().size.height;
    hudLayer->setContentSize(hudLayerSize);
    
    hudLayer->setPosition(margins, this->getBoundingBox().size.height / 2 - hudLayer->getBoundingBox().size.height / 2);
    this->addChild(hudLayer);
    
    Size miniLayerSizes = Size(0, 0);
    miniLayerSizes.width = hudLayer->getBoundingBox().size.width;
    miniLayerSizes.height = hudLayer->getBoundingBox().size.height / 10;
    
    auto layerCategory = Layer::create();
    layerCategory->setPosition(margins / 2, hudLayer->getBoundingBox().size.height * 0.55);
    
    auto labelTitleCategory = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "category-title"), MainRegularFont, 45);
    labelTitleCategory->setWidth(miniLayerSizes.width);
    labelTitleCategory->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    labelTitleCategory->setVerticalAlignment(TextVAlignment::BOTTOM);
    labelTitleCategory->setTextColor(IkasGrayDark);
    layerCategory->addChild(labelTitleCategory);
    
    _labelCategory = Label::createWithTTF("0", MainBoldFont, 50);
    _labelCategory->setWidth(miniLayerSizes.width);
    _labelCategory->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelCategory->setVerticalAlignment(TextVAlignment::TOP);
    _labelCategory->setTextColor(IkasRed);
    layerCategory->addChild(_labelCategory);
    
    miniLayerSizes.height = labelTitleCategory->getContentSize().height + _labelCategory->getContentSize().height;
    layerCategory->setContentSize(miniLayerSizes);
    
    labelTitleCategory->setPosition(0, layerCategory->getBoundingBox().size.height);
    _labelCategory->setPosition(0, 0);
    
    hudLayer->addChild(layerCategory);
    
    
    auto layerLevel = Layer::create();
    layerLevel->setPosition(margins / 2, hudLayer->getBoundingBox().size.height * 0.4);
    
    auto labelTitleLevel = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "level-title"), MainRegularFont, 45);
    labelTitleLevel->setWidth(miniLayerSizes.width);
    labelTitleLevel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    labelTitleLevel->setVerticalAlignment(TextVAlignment::BOTTOM);
    labelTitleLevel->setTextColor(IkasGrayDark);
    layerLevel->addChild(labelTitleLevel);
    
    _labelLevel = Label::createWithTTF("0", MainBoldFont, 50);
    _labelLevel->setWidth(miniLayerSizes.width);
    _labelLevel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelLevel->setVerticalAlignment(TextVAlignment::TOP);
    _labelLevel->setTextColor(IkasRed);
    layerLevel->addChild(_labelLevel);
    
    miniLayerSizes.height = labelTitleLevel->getContentSize().height + _labelLevel->getContentSize().height;
    layerLevel->setContentSize(miniLayerSizes);
    
    labelTitleLevel->setPosition(0, layerLevel->getBoundingBox().size.height);
    _labelLevel->setPosition(0, 0);
    
    hudLayer->addChild(layerLevel);
    
    
    auto layerPoints = Layer::create();
    layerPoints->setPosition(margins / 2, hudLayer->getBoundingBox().size.height * 0.25);
    
    auto labelTitlePoints = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "points-title"), MainRegularFont, 45);
    labelTitlePoints->setWidth(miniLayerSizes.width);
    labelTitlePoints->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    labelTitlePoints->setVerticalAlignment(TextVAlignment::BOTTOM);
    labelTitlePoints->setTextColor(IkasGrayDark);
    layerPoints->addChild(labelTitlePoints);
    
    _labelPoints = Label::createWithTTF("0", MainBoldFont, 50);
    _labelPoints->setWidth(miniLayerSizes.width);
    _labelPoints->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelPoints->setVerticalAlignment(TextVAlignment::TOP);
    _labelPoints->setTextColor(IkasRed);
    layerPoints->addChild(_labelPoints);
    
    miniLayerSizes.height = labelTitlePoints->getContentSize().height + _labelPoints->getContentSize().height;
    layerPoints->setContentSize(miniLayerSizes);
    
    labelTitlePoints->setPosition(0, layerPoints->getBoundingBox().size.height);
    _labelPoints->setPosition(0, 0);
    
    hudLayer->addChild(layerPoints);
    
    _pauseButton = SpriteButton::create(ImageManager::getImage("pause"), 0.4f, CC_CALLBACK_1(GamePlay::pauseGame, this));
    _pauseButton->setPosition(hudLayer->getBoundingBox().size.width / 2, hudLayer->getBoundingBox().size.height * 0.10);
    _pauseButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
    hudLayer->addChild(_pauseButton);
    
    // Timer!
    Sprite* timerBackground = Sprite::create(ImageManager::getImage("timer-background"));
    timerBackground->setScale(0.6f);
    timerBackground->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 centerPos = Vec2(hudLayer->getBoundingBox().size.width / 2, hudLayer->getBoundingBox().size.height * 0.85);
    timerBackground->setPosition(centerPos);
    
    Sprite* timer = Sprite::create(ImageManager::getImage("timer-in"));
    
    _progress = ProgressTimer::create(timer);
    _progress->setScale(timerBackground->getScale());
    _progress->setAnchorPoint(timerBackground->getAnchorPoint());
    _progress->setPosition(timerBackground->getPosition());
    
    hudLayer->addChild(timerBackground);
    hudLayer->addChild(_progress);
    
    _progress->setType(ProgressTimer::Type::RADIAL);
    _progress->setMidpoint(Point::ANCHOR_MIDDLE);
    
    // Lives for all gameplays -> If renew when success screen pass to loadNextGameplay method
    _currentLives = MaxLives;
    
    this->setupLevelSettings();
    this->restartGame();
    
    return true;
}

void GamePlay::setupLevelSettings()
{
    _totalSuccessScreens = 0;
    Difficulty difficulty = GamePlayPointsManager::getInstance()->getCurrentDifficulty();
    switch (difficulty) {
            break;
        case Difficulty::EASY:
            _maxTime = PlayMaxTime.asFloat() * 1.50f;
            break;
        case Difficulty::MEDIUM:
            _maxTime = PlayMaxTime.asFloat() * 1.00f;
            break;
        case Difficulty::HARD:
            _maxTime = PlayMaxTime.asFloat() * 0.85f;
            break;
        default:
            _maxTime = PlayMaxTime.asFloat();
            break;
    }
}

void GamePlay::resetTimer()
{
    _action = NULL;
    _sequence = NULL;
    _progress->stopAllActions();

    _action = ProgressFromTo::create(_currentLevelTime - _elapsedTime, _elapsedTime / _currentLevelTime * 100, 100);
    
    CallFunc* callback = CallFunc::create(CC_CALLBACK_0(GamePlay::timeElapsed, this));
    _sequence = Sequence::create(_action, callback, NULL);
    _progress->runAction(_sequence);
}

void GamePlay::pauseGame(Ref* sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    SceneManager::getInstance()->runSceneWithType(SceneType::PAUSE);
    _elapsedTime += _sequence->getElapsed();
    _progress->stopAction(_sequence);
}

// GamePlayDelegate
void GamePlay::loadNextGamePlay()
{
    // Aleatory unique order
    if (_loadedIndex.size() >= _subCategories.size() * 0.75) {
        _loadedIndex.clear();
    }
    int subCategoryPosition;
    SubCategory* subCategory;
    do {
        subCategoryPosition = rand() % _subCategories.size();
        subCategory = _subCategories.at(subCategoryPosition);
        if (subCategory->getOptions().size() < 1 &&// Only need one item
            !GamePlay::isValidSubcategory(subCategory)) {//Need to
            subCategoryPosition = -1;
            _loadedIndex.push_back(subCategoryPosition);
        }
    } while (find(_loadedIndex.begin(), _loadedIndex.end(), subCategoryPosition) != _loadedIndex.end() || subCategoryPosition == -1);
    _loadedIndex.push_back(subCategoryPosition);
    
    vector<Option*> options = _subCategories.at(subCategoryPosition)->getOptions();
    
    
    // Get aleatory option
    Option *option;
    bool isValid = false;
    do {
        int optionPosition = rand() % options.size();
        option = options.at(optionPosition);
        isValid = GamePlay::isValidWord(option->getName());
    } while (!isValid);
    
    //Clear and create new game layer
    clearGameLayer();
    createGameLayer(option);
    
    _elapsedTime = 0;
    
   float timeDifficulty = _totalSuccessScreens * TimeToReduce.asFloat();
    _currentLevelTime = _maxTime;
    _currentLevelTime -= timeDifficulty;
    
    float minValue = (_maxTime * TimeMinPercentage.asInt() / 100);
    _currentLevelTime = (minValue > _currentLevelTime ? minValue : _currentLevelTime);
    this->resetTimer();
}

bool GamePlay::isValidSubcategory(SubCategory* subCategory) {
    bool hasValidOptions = false;
    vector<Option*> options = subCategory->getOptions();
    for (int i = 0; i < options.size(); i++) {
        Option* option = options.at(i);
        if (GamePlay::isValidWord(option->getName())) {
            hasValidOptions = true;
            break;
        }
    }
    if (!hasValidOptions) {
        log("no hay opciones válidas");
    }
    return hasValidOptions;
}


bool GamePlay::isValidWord(string word) {
    if (word.size() <= MaxLetters.asInt() &&
        word.find_first_of(" ") == std::string::npos) {// No whitespaces
        return true;
    } else {
        return false;
    }
}


void GamePlay::clearGameLayer()
{
    _gameLayer->removeAllChildrenWithCleanup(true);
//    auto childrens = _gameLayer->getChildren();
//    for (int i = 0; i < childrens.size(); i++) {
//        auto children = childrens.at(i);
//        _gameLayer->removeChild(children);
//    }
    _currentOption = NULL;
}

void GamePlay::createGameLayer(Option* option)
{
    _currentOption = option;
    
    auto height = _gameLayer->getBoundingBox().size.height;
    auto width = _gameLayer->getBoundingBox().size.width;
    auto splitSize = MIN(width, height) / 2;
    auto imageMaxSize = splitSize * 0.80;//Set top middle with margins of 10%
    
    auto imageRect = Rect(0, 0, imageMaxSize, imageMaxSize);
    auto optionImage = Sprite::create(ZipManager::getInstance()->getDataFolderFullPath("hd/options/" + option->getFileName()));
    auto optionImageSize = optionImage->getContentSize();
    
    auto scale = 1.0f;
    if (optionImageSize.width > optionImageSize.height) {
        scale = imageMaxSize / optionImageSize.width;
    } else {
        scale = imageMaxSize / optionImageSize.height;
    }
    optionImage->setScale(scale);
    
    optionImage->setAnchorPoint(Point::ANCHOR_MIDDLE);
    optionImage->setPosition(Vec2(width / 2, (height / 4) * 3));
    _gameLayer->addChild(optionImage);
    
    
    float currentLetterLayerSize = LetterLayerSize.asFloat();
    float currentLetterFontSize = LetterFontSize.asFloat();
    float currentLetterLayerMargin = LetterLayerMargin.asFloat();
    float currentLetterLayerBorderWidth = LetterLayerBorderWidth.asFloat();
    
    int totalLayerWidth = option->getName().size() * currentLetterLayerSize + (option->getName().size() - 1) * currentLetterLayerMargin;
    float maxWidth = _gameLayer->getBoundingBox().size.width - 2 * currentLetterLayerMargin;
    if (totalLayerWidth > maxWidth) {
        // Update maxSizes
        currentLetterLayerMargin = currentLetterLayerMargin / 2;
        currentLetterLayerBorderWidth = currentLetterLayerBorderWidth / 2;
        
        currentLetterLayerSize = (maxWidth - (option->getName().size() - 1) * currentLetterLayerMargin - 2 * currentLetterLayerMargin) / option->getName().size();
        currentLetterFontSize = currentLetterLayerSize * LetterFontSizeLayerRelation.asFloat();
        
        totalLayerWidth = option->getName().size() * currentLetterLayerSize + (option->getName().size() - 1) * currentLetterLayerMargin;
    }
    
    // Generate UppperCase string vector
    vector<string> upperCaseName;
    for (int i = 0; i < option->getName().size(); i++) {
        string letter(1, toupper(option->getName().at(i)));
        upperCaseName.push_back(letter.c_str());
    }

    // Generate unordered vector with letters
    vector<string> name = upperCaseName;
    Vector<Label*> unorderedLetters;
    do {
        auto position = rand() % name.size();
        auto letter = name.at(position);
        Label* letterLabel = Label::createWithTTF(letter, MainBoldFont, currentLetterFontSize);
        unorderedLetters.pushBack(letterLabel);
        name.erase(name.begin() + position);
    } while (name.size() > 0);
    
    auto orderedLayer = Layer::create();
    orderedLayer->setContentSize(Size(totalLayerWidth, currentLetterLayerSize));
    orderedLayer->ignoreAnchorPointForPosition(false);
    orderedLayer->setAnchorPoint(Point::ANCHOR_MIDDLE);
    orderedLayer->setPosition(Vec2(_gameLayer->getBoundingBox().size.width / 2, _gameLayer->getBoundingBox().size.height / 4 - ScreenSizeManager::getHeightFromPercentage(5)));

    for (int i = 0; i < unorderedLetters.size(); i++) {
        auto layer = LayerColor::create(IkasRed);
        auto inLayer = LayerColor::create(IkasWhite);
        
        layer->setContentSize(Size(currentLetterLayerSize, currentLetterLayerSize));
        inLayer->setContentSize(Size(currentLetterLayerSize - 2 * currentLetterLayerBorderWidth, currentLetterLayerSize - 2 * currentLetterLayerBorderWidth));
        inLayer->setPosition(Vec2(currentLetterLayerBorderWidth, currentLetterLayerBorderWidth));
        
        layer->setPosition(Vec2(i * currentLetterLayerSize + i * currentLetterLayerMargin, 0));
        
        layer->addChild(inLayer);
        orderedLayer->addChild(layer);
    }
    _gameLayer->addChild(orderedLayer);
    
    //    auto unorderedLayer = LayerColor::create(Color4B(5, 5, 5, 255));
    auto unorderedLayer = Layer::create();
    unorderedLayer->setContentSize(Size(totalLayerWidth, currentLetterLayerSize));
    unorderedLayer->ignoreAnchorPointForPosition(false);
    unorderedLayer->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
//    unorderedLayer->setPosition(Vec2(_gameLayer->getBoundingBox().size.width / 2, _gameLayer->getBoundingBox().size.height / 4 + ScreenSizeManager::getHeightFromPercentage(5)));
    unorderedLayer->setPosition(Vec2(_gameLayer->getBoundingBox().size.width / 2, orderedLayer->getBoundingBox().origin.y + orderedLayer->getBoundingBox().size.height + currentLetterLayerMargin));
    

    for (int i = 0; i < unorderedLetters.size(); i++) {
        auto label = unorderedLetters.at(i);
        auto layer = LayerColor::create(IkasRed);
        auto inLayer = LayerColor::create(IkasWhite);
        
        layer->setContentSize(Size(currentLetterLayerSize, currentLetterLayerSize));
        inLayer->setContentSize(Size(currentLetterLayerSize - 2 * currentLetterLayerBorderWidth, currentLetterLayerSize - 2 * currentLetterLayerBorderWidth));
        inLayer->setPosition(Vec2(currentLetterLayerBorderWidth, currentLetterLayerBorderWidth));
        label->setContentSize(Size(currentLetterLayerSize - 3 * currentLetterLayerBorderWidth, currentLetterLayerSize - 3 * currentLetterLayerBorderWidth));
        
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setPosition(layer->getBoundingBox().size.width / 2, layer->getBoundingBox().size.height / 2);
        
        label->setColor(Color3B(IkasRed));
        
        layer->setPosition(Vec2(i * currentLetterLayerSize + i * currentLetterLayerMargin, 0));
        
        layer->addChild(inLayer);
        layer->addChild(label);
        unorderedLayer->addChild(layer);
    }
    _gameLayer->addChild(unorderedLayer);
    
}

void GamePlay::resumeGamePlay()
{
    this->resetTimer();
}

void GamePlay::restartGame()
{
    GamePlayPointsManager::getInstance()->resetCurrentPoints();
    this->loadNextGamePlay();
}

void GamePlay::timeElapsed()
{
    _elapsedTime = 0;
    _progress->stopAction(_sequence);
    SceneManager::getInstance()->runSceneWithType(SceneType::LOSE);
}

void GamePlay::updateScreenGameStats()
{
    Difficulty difficulty = GamePlayPointsManager::getInstance()->getCurrentDifficulty();
    std::string levelString = "";
    switch (difficulty) {
        case Difficulty::EASY:
            levelString = LanguageManager::getLocalizedText("Level", "easy");
            break;
        case Difficulty::MEDIUM:
            levelString = LanguageManager::getLocalizedText("Level", "medium");
            break;
        case Difficulty::HARD:
            levelString = LanguageManager::getLocalizedText("Level", "hard");
            break;
        default:
            break;
    }
    
    _labelLevel->setString(levelString);
    
    float points = GamePlayPointsManager::getInstance()->getCurrentPoints();
    std::ostringstream pointsString;
    pointsString << points;
    _labelPoints->setString(pointsString.str());
    
    int index = GamePlayPointsManager::getInstance()->getCurrentCategory();
    std::string categoryName = _categories.at(index)->getName();
    _labelCategory->setString(categoryName);
}

EventListenerTouchOneByOne *GamePlay::addEvents()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    
    listener->onTouchBegan = [&](cocos2d::Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            return false;
        }
        return GamePlay::touchBegan(touch, event);
    };
    
    listener->onTouchMoved = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchMoved(touch, event);
    };
    
    listener->onTouchEnded = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchEnded(touch, event);
    };
    
    listener->onTouchCancelled = [=](Touch* touch, Event* event)
    {
        Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
        Rect rect = _touchLayer->getBoundingBox();
        if (!rect.containsPoint(touchLocation)) {
            _touchState = TouchState::FINISH;
            return;
        }
        GamePlay::touchCancelled(touch, event);
    };
    
    return listener;
}

void GamePlay::openSuccessScreen()
{
    _totalSuccessScreens++;
    GamePlayPointsManager::getInstance()->addSuccessPoints();
    SceneManager::getInstance()->runSceneWithType(SceneType::WIN);
    _pauseButton->setVisible(true);
}

bool GamePlay::touchBegan(Touch *touch, Event *pEvent)
{
    if (_touchState != TouchState::FINISH) {
        return false;
    }
    
    Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
    _touchState = TouchState::START;
    return true;
}

void GamePlay::touchEnded(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    
    Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
    _touchState = TouchState::FINISH;
}

void GamePlay::touchMoved(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    
    Vec2 touchLocation = _touchLayer->convertTouchToNodeSpace(touch);
    _touchState = TouchState::MOVING;
}

void GamePlay::touchCancelled(Touch *pTouch, Event *pEvent)
{
    _touchState = TouchState::FINISH;
}

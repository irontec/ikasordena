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
const Value LetterFontSizeLayerRelation = Value(0.75);
const Value LetterFontSize = Value(LetterLayerSize.asFloat() * LetterFontSizeLayerRelation.asFloat());
const Value LetterLayerMargin = Value(10);
const Value LetterLayerBorderWidth = Value(4);

const float GameLayerBorderWidth = 2;
const Color4B GameLayerBorderColor = IkasYellow;

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
    
    _moveListener = addEvents();
    
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
    labelTitleCategory->setTextColor(IkasPurple);
    layerCategory->addChild(labelTitleCategory);
    
    _labelCategory = Label::createWithTTF("0", MainBoldFont, 50);
    _labelCategory->setWidth(miniLayerSizes.width);
    _labelCategory->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelCategory->setVerticalAlignment(TextVAlignment::TOP);
    _labelCategory->setTextColor(IkasYellow);
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
    labelTitleLevel->setTextColor(IkasPurple);
    layerLevel->addChild(labelTitleLevel);
    
    _starsLayer = Layer::create();
    _starsLayer->setPosition(0, 0);
    _starsLayer->setContentSize(Size(miniLayerSizes.width, miniLayerSizes.height - labelTitleLevel->getBoundingBox().size.height));
    layerLevel->addChild(_starsLayer);
    
//    _labelLevel = Label::createWithTTF("0", MainBoldFont, 50);
//    _labelLevel->setWidth(miniLayerSizes.width);
//    _labelLevel->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
//    _labelLevel->setVerticalAlignment(TextVAlignment::TOP);
//    _labelLevel->setTextColor(IkasYellow);
//    layerLevel->addChild(_labelLevel);
    
//    miniLayerSizes.height = labelTitleLevel->getContentSize().height + _labelLevel->getContentSize().height;
    miniLayerSizes.height = labelTitleLevel->getContentSize().height + _starsLayer->getContentSize().height;
    layerLevel->setContentSize(miniLayerSizes);
    
    labelTitleLevel->setPosition(0, layerLevel->getBoundingBox().size.height);
//    _labelLevel->setPosition(0, 0);
    
    hudLayer->addChild(layerLevel);
    
    
    auto layerPoints = Layer::create();
    layerPoints->setPosition(margins / 2, hudLayer->getBoundingBox().size.height * 0.25);
    
    auto labelTitlePoints = Label::createWithTTF(LanguageManager::getLocalizedText("GamePlay", "points-title"), MainRegularFont, 45);
    labelTitlePoints->setWidth(miniLayerSizes.width);
    labelTitlePoints->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    labelTitlePoints->setVerticalAlignment(TextVAlignment::BOTTOM);
    labelTitlePoints->setTextColor(IkasPurple);
    layerPoints->addChild(labelTitlePoints);
    
    _labelPoints = Label::createWithTTF("0", MainBoldFont, 50);
    _labelPoints->setWidth(miniLayerSizes.width);
    _labelPoints->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _labelPoints->setVerticalAlignment(TextVAlignment::TOP);
    _labelPoints->setTextColor(IkasYellow);
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
    
    Sprite* timerBorder = Sprite::create(ImageManager::getImage("timer-border"));
    timerBorder->setScale(timerBackground->getScale());
    timerBorder->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 borderCenterPos = Vec2(hudLayer->getBoundingBox().size.width / 2, hudLayer->getBoundingBox().size.height * 0.85);
    timerBorder->setPosition(borderCenterPos);

    
    Sprite* timer = Sprite::create(ImageManager::getImage("timer-in"));
    
    _progress = ProgressTimer::create(timer);
    _progress->setScale(timerBackground->getScale());
    _progress->setAnchorPoint(timerBackground->getAnchorPoint());
    _progress->setPosition(timerBackground->getPosition());
    
    hudLayer->addChild(timerBackground);
    hudLayer->addChild(_progress);
    hudLayer->addChild(timerBorder);
    
    _progress->setType(ProgressTimer::Type::RADIAL);
    _progress->setMidpoint(Point::ANCHOR_MIDDLE);
    
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
    // Reset fails
    _totalFails = 0;
    
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
    updateLivesView();
    
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
    auto childrens = _gameLayer->getChildren();
    for (int i = 0; i < childrens.size(); i++) {
        auto children = childrens.at(i);
        _gameLayer->removeChild(children);
    }
    _currentOption = NULL;
}

void GamePlay::createGameLayer(Option* option)
{
    _orderedLabels.clear();
    _unorderedLabels.clear();
    
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
    
    
//    DrawNode *circle = DrawNode::create();
//    circle->setAnchorPoint(optionImage->getAnchorPoint());
//    circle->setPosition(optionImage->getPosition());
//    Size visibleSize = optionImage->getBoundingBox().size;
//    
//    int radius = visibleSize.width / 2;
//    //Border
////    float angle = CC_DEGREES_TO_RADIANS(360);
////    int segments = 360;
////    circle->drawCircle(Vec2(visibleSize.width / 2,visibleSize.height / 2), radius, angle, segments, false, Color4F(10, 10, 10, 150));
//    
//    //Complete
//    for (float angle = 0; angle <= 2 * M_PI; angle += 0.005)
//    {
//        circle->drawSegment(Point(0.0, 0.0), Point(radius * cos(angle), radius * sin(angle)), 1, Color4F(1.0, 0.0, 0.0, 1.0));
//    }
//    _gameLayer->addChild(circle);
    
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
        Label* letterLabel = Label::createWithTTF(letter, MainRegularFont, currentLetterFontSize);
        letterLabel->setTag(position);
        
        unorderedLetters.pushBack(letterLabel);
        name.erase(name.begin() + position);
    } while (name.size() > 0);
    
     _orderedLayer = Layer::create();
    _orderedLayer->setContentSize(Size(totalLayerWidth, currentLetterLayerSize));
    _orderedLayer->ignoreAnchorPointForPosition(false);
    _orderedLayer->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _orderedLayer->setPosition(Vec2(_gameLayer->getBoundingBox().size.width / 2, _gameLayer->getBoundingBox().size.height / 4 - ScreenSizeManager::getHeightFromPercentage(5)));

    for (int i = 0; i < unorderedLetters.size(); i++) {
        auto layer = LayerColor::create(IkasYellowLight);
        layer->setTag(i);
        auto inLayer = LayerColor::create(IkasWhite);
        inLayer->setTag(5151);
        layer->setContentSize(Size(currentLetterLayerSize, currentLetterLayerSize));
        inLayer->setContentSize(Size(currentLetterLayerSize - 2 * currentLetterLayerBorderWidth, currentLetterLayerSize - 2 * currentLetterLayerBorderWidth));
        inLayer->setPosition(Vec2(currentLetterLayerBorderWidth, currentLetterLayerBorderWidth));
        
        layer->setPosition(Vec2(i * currentLetterLayerSize + i * currentLetterLayerMargin, 0));
        
        layer->addChild(inLayer);
        _orderedLayer->addChild(layer);
    }
    _gameLayer->addChild(_orderedLayer);
    
    //    auto unorderedLayer = LayerColor::create(Color4B(5, 5, 5, 255));
     _unorderedLayer = Layer::create();
    _unorderedLayer->setContentSize(Size(totalLayerWidth, currentLetterLayerSize));
    _unorderedLayer->ignoreAnchorPointForPosition(false);
    _unorderedLayer->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    _unorderedLayer->setPosition(Vec2(_gameLayer->getBoundingBox().size.width / 2, _orderedLayer->getBoundingBox().origin.y + _orderedLayer->getBoundingBox().size.height + currentLetterLayerMargin));
    
    for (int i = 0; i < unorderedLetters.size(); i++) {
        auto label = unorderedLetters.at(i);
        auto layer = LayerColor::create(IkasYellowLight);
//        auto layer = createCircleLayer(Point::ANCHOR_BOTTOM_LEFT, Vec2(i * currentLetterLayerSize + i * currentLetterLayerMargin, 0), Size(currentLetterLayerSize, currentLetterLayerSize), Color4F(IkasYellowLight));
        
        layer->setTag(i);
                auto inLayer = LayerColor::create(IkasYellowLight);
//        auto inLayer = createCircleLayer(Point::ANCHOR_BOTTOM_LEFT, Vec2(currentLetterLayerBorderWidth, currentLetterLayerBorderWidth), Size(currentLetterLayerSize - 2 * currentLetterLayerBorderWidth, currentLetterLayerSize - 2 * currentLetterLayerBorderWidth), Color4F(IkasYellowLight));
        
        layer->setContentSize(Size(currentLetterLayerSize, currentLetterLayerSize));
        layer->setPosition(Vec2(i * currentLetterLayerSize + i * currentLetterLayerMargin, 0));
        
        inLayer->setContentSize(Size(currentLetterLayerSize - 2 * currentLetterLayerBorderWidth, currentLetterLayerSize - 2 * currentLetterLayerBorderWidth));
        inLayer->setPosition(Vec2(currentLetterLayerBorderWidth, currentLetterLayerBorderWidth));
        label->setContentSize(Size(currentLetterLayerSize - 10, currentLetterLayerSize - 10));
        
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setPosition(layer->getBoundingBox().size.width / 2, layer->getBoundingBox().size.height / 2);
        
        label->setColor(Color3B(IkasPurple));
        
        
        layer->addChild(inLayer);
        layer->addChild(label);
        _unorderedLabels.insert(i, label);
        _unorderedLayer->addChild(layer);
    }
    _gameLayer->addChild(_unorderedLayer);
    if (_touchLayer != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_moveListener);
    }
    
    _touchLayer = Layer::create();
    _touchLayer->setContentSize(_gameLayer->getContentSize());
    _gameLayer->addChild(_touchLayer);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_moveListener, _touchLayer);
    _touchState = TouchState::FINISH;
    
}

DrawNode* GamePlay::createCircleLayer(Vec2 anchorPoint,  Vec2 position, Size size, Color4F color) {
    
    DrawNode *circle = DrawNode::create();
    circle->setAnchorPoint(anchorPoint);
    circle->setPosition(position);
    Size visibleSize = size;
    
    int radius = visibleSize.width / 2;
    //Border
    //    float angle = CC_DEGREES_TO_RADIANS(360);
    //    int segments = 360;
    //    circle->drawCircle(Vec2(visibleSize.width / 2,visibleSize.height / 2), radius, angle, segments, false, Color4F(10, 10, 10, 150));
    
    //Complete
    for (float angle = 0; angle <= 2 * M_PI; angle += 0.001)
    {
        circle->drawSegment(Point(0.0, 0.0), Point(radius * cos(angle), radius * sin(angle)), 1, color);
    }
    return circle;
//    _gameLayer->addChild(circle);
}

void GamePlay::updateLivesView()
{
    // Check _currentLivez > 0 !!!!!!
    log("current lives: %d", _currentLives.asInt());
}

void GamePlay::resumeGamePlay()
{
    this->resetTimer();
}

void GamePlay::restartGame()
{
    GamePlayPointsManager::getInstance()->resetCurrentPoints();
    _currentLives = MaxLives;
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
//    std::string levelString = "";
    int stars = 0;
    switch (difficulty) {
        case Difficulty::EASY:
            stars = 1;
//            levelString = LanguageManager::getLocalizedText("Level", "easy");
            break;
        case Difficulty::MEDIUM:
            stars = 2;
//            levelString = LanguageManager::getLocalizedText("Level", "medium");
            break;
        case Difficulty::HARD:
            stars = 3;
//            levelString = LanguageManager::getLocalizedText("Level", "hard");
            break;
        default:
            break;
    }
//    _labelLevel->setString(levelString);
    


    auto currentStars = _starsLayer->getChildren();
    for (int i = 0; i < currentStars.size(); i++) {
        auto starToRemove = currentStars.at(i);
        starToRemove->removeFromParentAndCleanup(true);
    }

    for (int i = 0; i < stars; i++) {
        auto rect = Rect(0, 0, _starsLayer->getBoundingBox().size.height, _starsLayer->getBoundingBox().size.height);
        auto star = Sprite::create(ImageManager::getImage("star"));
        auto scale = _starsLayer->getBoundingBox().size.height / star->getContentSize().height;
        star->setScale(scale - 0.1);
        star->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        auto margin = 6;
        if (i == 0) {
            margin = 0;
        }
        star->setPosition(Vec2((i * _starsLayer->getBoundingBox().size.height) + margin, 0));
        
        _starsLayer->addChild(star);
    }
    
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

void GamePlay::checkGameStatus()
{
    auto currentName = _currentOption->getName();
    if (_orderedLabels.size() == currentName.size()) {
        bool successResult = true;
        for (int i = 0; i < _orderedLabels.size(); i++) {
            string letter(1, toupper(currentName.at(i)));
            auto label = _orderedLabels.at(i);
            if (letter != label->getString()) {
                successResult = false;
                break;
            }
        }
        if (successResult) {
            if (_totalFails.asInt() == 0 && _currentLives.asInt() < MaxLives.asInt()) {
                _currentLives = _currentLives.asInt() + 1;
            }
            _pauseButton->setVisible(false);
            _elapsedTime += _sequence->getElapsed();
            _progress->stopAction(_sequence);
            
            // All image success
            DelayTime *delayAction = DelayTime::create(SuccessTime.asFloat());
            CallFunc *mySelector = CallFunc::create(CC_CALLBACK_0(GamePlay::openSuccessScreen, this));
            
            this->runAction(Sequence::create(delayAction, mySelector, nullptr));
        }
    } else {
        if (_currentLives.asInt() < 0) {
            timeElapsed();
        }
    }
}

void GamePlay::openSuccessScreen()
{
    _totalSuccessScreens++;
    GamePlayPointsManager::getInstance()->addSuccessPoints();
    SceneManager::getInstance()->runSceneWithType(SceneType::WIN);
    _pauseButton->setVisible(true);
}

int GamePlay::indexForTouch(Layer *layer, Touch *touch) {
    int position = -1;
    for (int i = 0; i < _currentOption->getName().size(); i++) {
        Vec2 letterLocation = layer->convertTouchToNodeSpace(touch);
        Layer *letterLayer = (Layer *)layer->getChildByTag(i);
        if (letterLayer != nullptr) {
            if (letterLayer->getBoundingBox().containsPoint(letterLocation)) {
                position = i;
                break;
            }
        }
    }
    return position;
}

void GamePlay::startMovingLabel(Touch *touch)
{
    auto touchLayerPosition = _touchLayer->convertTouchToNodeSpace(touch);
    _unorderedLayer->getChildByTag(_initialIndex)->removeChild(_currentLabel);
    _currentLabel->setPosition(touchLayerPosition);
    _touchLayer->addChild(_currentLabel);
    _currentLabel->setScale(1.30);
}

void GamePlay::updateLabelPosition(Touch *touch)
{
    auto newPosition = _touchLayer->convertTouchToNodeSpace(touch);
    _currentLabel->setPosition(newPosition);
}

void GamePlay::updateLabelToFinalPosition()
{
    _touchLayer->removeChild(_currentLabel);
    auto letterLayer = _orderedLayer->getChildByTag(_endIndex);
    _currentLabel->setPosition(letterLayer->getBoundingBox().size.width / 2, letterLayer->getBoundingBox().size.height / 2);
    letterLayer->addChild(_currentLabel);
    _orderedLabels.insert(_endIndex, _currentLabel);
    _currentLabel->setTag(-1);//Invalidate letter
}

void GamePlay::restoreLabelToInitialPosition()
{
    _touchLayer->removeChild(_currentLabel);
    _currentLabel->setPosition(_initialPosition);
    _unorderedLayer->getChildByTag(_initialIndex)->addChild(_currentLabel);
}

void GamePlay::showDragResult(bool success)
{
    _currentLabel->setScale(1);
    
    auto labelLayer = _orderedLayer->getChildByTag(_endIndex);
    auto colorInLayer = labelLayer->getChildByTag(5151);
    
    if (success) {
        _resultLayer = LayerColor::create(IkasSuccess);
    } else {
        _resultLayer = LayerColor::create(IkasError);
    }

    _resultLayer->setContentSize(colorInLayer->getContentSize());
    _resultLayer->setPosition(colorInLayer->getPosition());
    
    _resultLayer->setOpacity(1.0);
    
    labelLayer->addChild(_resultLayer);
    auto fadeIn = FadeTo::create(0.2, 255);
    auto fadeDuration = FadeTo::create(1.5, 255);
    auto fadeOut = FadeTo::create(0.15, 0);
    
    auto cleanCallback = CallFunc::create([&](){
        if (_resultLayer != NULL) {
            _resultLayer->removeFromParentAndCleanup(true);
            _resultLayer = NULL;
        }
    });
    Sequence *sequence = Sequence::create(fadeIn, fadeDuration, fadeOut, cleanCallback, NULL);
    _resultLayer->runAction(sequence);
}

bool GamePlay::touchBegan(Touch *touch, Event *pEvent)
{
    if (_touchState != TouchState::FINISH) {
        return false;
    }
    int position = indexForTouch(_unorderedLayer, touch);
    if (position == -1) {
        return false;
    } else if (_unorderedLabels.at(position)->getTag() == -1) {
        return false;
    }
    _touchState = TouchState::START;
    _initialIndex = position;
    _currentLabel = _unorderedLabels.at(position);
    _initialPosition = _currentLabel->getPosition();
    
    startMovingLabel(touch);
    return true;
}

void GamePlay::touchEnded(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    _touchState = TouchState::FINISH;
    
    int position = indexForTouch(_orderedLayer, touch);
    if (position == -1) {
        restoreLabelToInitialPosition();
        return;
    }
    _endIndex = position;
    auto letter = _unorderedLabels.at(_initialIndex)->getString();
    string neededLetter(1, toupper(_currentOption->getName().at(_endIndex)));
    
    auto currentLabelInOrderedLayer = _orderedLabels.at(_endIndex);
    
       if (_unorderedLabels.at(_initialIndex)->getTag() == -1) {//Check in touchBegan
        log("Letra ya movida con anterioridad");
    } else if (currentLabelInOrderedLayer != nullptr) {
        restoreLabelToInitialPosition();
    } else if (letter == neededLetter) {
        SoundManager::getInstance()->successPlay();
        showDragResult(true);
        updateLabelToFinalPosition();
    } else {
        SoundManager::getInstance()->failurePlay();
        
        auto restoreCallback = CallFunc::create([&](){
            restoreLabelToInitialPosition();
        });
        auto showDragResultCallback = CallFunc::create([&]() {
            showDragResult(false);
        });
        auto delay = FadeTo::create(0.2, _currentLabel->getOpacity());

        auto actionMoveSequence = Sequence::create(showDragResultCallback, delay, restoreCallback, NULL);
        _currentLabel->runAction(actionMoveSequence);
        
        _totalFails = _totalFails.asInt() + 1;
        _currentLives = _currentLives.asInt() - 1;
        updateLivesView();
    }
    checkGameStatus();
}

void GamePlay::touchMoved(Touch *touch, Event *pEvent)
{
    if (_touchState == TouchState::FINISH) {
        return;
    }
    _touchState = TouchState::MOVING;
    updateLabelPosition(touch);
}

void GamePlay::touchCancelled(Touch *touch, Event *pEvent)
{
    _touchState = TouchState::FINISH;
    restoreLabelToInitialPosition();
}

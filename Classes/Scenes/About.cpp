//
//  About.cpp
//  Ikasesteka
//
//  Created by Sergio Garcia on 2/3/15.
//
//

#include "About.h"
#include "../Singletons/SceneManager.h"
#include "../Singletons/SoundManager.h"
#include "../Helpers/LanguageManager.h"
#include "../Helpers/ScreenSizeManager.h"
#include "../Helpers/ImageManager.h"
#include "../CustomGUI/SpriteButton.h"
#include "extensions/GUI/CCScrollView/CCScrollView.h"

#define InfoLabelXPositionPercentage 45
#define InfoLabelMaxWidthPercentage 70

#define InfoLabelScrollUpdatePercentage 10

enum class ButtonType
{
    TOP,
    BOTTOM,
    BACK
};

Scene* About::createScene()
{
    auto *scene = Scene::create();
    
    auto *layer = About::create();
    layer->setTag(2334);
    
    scene->addChild(layer);
    
    return scene;
}

bool About::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Rect visibleRect = ScreenSizeManager::getVisibleRect();
    
    auto background = Sprite::create(ImageManager::getImage("background"), visibleRect);
    background->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(50, 50));
    background->setAnchorPoint(Point::ANCHOR_MIDDLE);
    this->addChild(background);
    
    auto buttonBack = SpriteButton::create(ImageManager::getImage("back"), 0.30f, CC_CALLBACK_1(About::buttonPressed, this));
    buttonBack->setTag(static_cast<int>(ButtonType::BACK));
    buttonBack->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonBack = ScreenSizeManager::getScreenPositionFromPercentage(5, 5);
    positionButtonBack.x += buttonBack->getBoundingBox().size.width / 2;
    positionButtonBack.y += buttonBack->getBoundingBox().size.height / 2;
    buttonBack->setPosition(positionButtonBack);
    this->addChild(buttonBack);
    
    auto info = Sprite::create(ImageManager::getImage("info"));
    info->setScale(0.75f);
    info->setPosition(ScreenSizeManager::getScreenPositionFromPercentage(5, 95));
    info->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    this->addChild(info);
    
    auto labelAboutTitle = Label::createWithTTF(LanguageManager::getLocalizedText("About", "title"), MainRegularFont, 140);
    labelAboutTitle->setAlignment(TextHAlignment::LEFT);
    labelAboutTitle->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    labelAboutTitle->setTextColor(IkasGrayDark);
    Vec2 positionLabelAboutTitle = info->getPosition();
    positionLabelAboutTitle.x += info->getBoundingBox().size.width;
    positionLabelAboutTitle.x += ScreenSizeManager::getWidthFromPercentage(5);
    positionLabelAboutTitle.y -= info->getBoundingBox().size.height/2;
    labelAboutTitle->setPosition(positionLabelAboutTitle);
    this->addChild(labelAboutTitle);
    
    labelAbout = Label::createWithTTF(LanguageManager::getLocalizedText("About", "information"), MainRegularFont, 50);
    labelAbout->setWidth(ScreenSizeManager::getWidthFromPercentage(InfoLabelMaxWidthPercentage));
    labelAbout->setTextColor(IkasGrayDark);
    labelAbout->setVerticalAlignment(TextVAlignment::TOP);
    
    float tableTop = info->getPosition().y - info->getBoundingBox().size.height - ScreenSizeManager::getHeightFromPercentage(5);
    float tableBottom = buttonBack->getPosition().y + buttonBack->getBoundingBox().size.height/2 + ScreenSizeManager::getHeightFromPercentage(5);
    
    tableViewAbout = TableView::create(this, Size(ScreenSizeManager::getWidthFromPercentage(InfoLabelMaxWidthPercentage), tableTop - tableBottom));
    tableViewAbout->setDirection(TableView::Direction::VERTICAL);
    Vec2 positionTablewView = ScreenSizeManager::getScreenPositionFromPercentage(InfoLabelXPositionPercentage, 0);
    positionTablewView.y = tableBottom;
    positionTablewView.x -= tableViewAbout->getBoundingBox().size.width/2;
    tableViewAbout->setPosition(positionTablewView);
    this->addChild(tableViewAbout);
    tableViewAbout->reloadData();
    
    auto buttonTop = SpriteButton::create(ImageManager::getImage("top"), 0.30f, CC_CALLBACK_1(About::buttonPressed, this));
    buttonTop->setTag(static_cast<int>(ButtonType::TOP));
    buttonTop->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonTop = tableViewAbout->getPosition();
    positionButtonTop.x += tableViewAbout->getBoundingBox().size.width;
    positionButtonTop.y += tableViewAbout->getBoundingBox().size.height;
    positionButtonTop.x += ScreenSizeManager::getWidthFromPercentage(5);
    positionButtonTop.x += buttonTop->getBoundingBox().size.width / 2;
    positionButtonTop.y -= buttonTop->getBoundingBox().size.height / 2;
    buttonTop->setPosition(positionButtonTop);
    this->addChild(buttonTop);
    
    auto buttonBottom = SpriteButton::create(ImageManager::getImage("bottom"), 0.30f, CC_CALLBACK_1(About::buttonPressed, this));
    buttonBottom->setTag(static_cast<int>(ButtonType::BOTTOM));
    buttonBottom->setAnchorPoint(Point::ANCHOR_MIDDLE);
    Vec2 positionButtonBottom = tableViewAbout->getPosition();
    positionButtonBottom.x += tableViewAbout->getBoundingBox().size.width;
    positionButtonBottom.x += ScreenSizeManager::getWidthFromPercentage(5);
    positionButtonBottom.x += buttonBottom->getBoundingBox().size.width / 2;
    positionButtonBottom.y += buttonBottom->getBoundingBox().size.height / 2;
    buttonBottom->setPosition(positionButtonBottom);
    this->addChild(buttonBottom);
    
    
    //Initializing and binding
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(About::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    
    return true;
}

ssize_t About::numberOfCellsInTableView(TableView *table)
{
    return 1;
}

Size About::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    
    return labelAbout->getContentSize();
}

TableViewCell* About::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TableViewCell *cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    }
    cell->removeAllChildren();
    
    labelAbout->setAnchorPoint(Point::ANCHOR_MIDDLE);
    labelAbout->setPosition(labelAbout->getBoundingBox().size.width/2, labelAbout->getBoundingBox().size.height/2);
    
    cell->addChild(labelAbout);
    
    return cell;
}

void About::buttonPressed(Ref *sender)
{
    SoundManager::getInstance()->sfxPlay("button");
    auto button = static_cast<MenuItemSprite*>(sender);
    switch (static_cast<ButtonType>(button->getTag())) {
        case ButtonType::TOP: {
            Vec2 offset = tableViewAbout->getContentOffset();
            float positiveYOffset = -offset.y;
            float newOffset = positiveYOffset + labelAbout->getContentSize().height * InfoLabelScrollUpdatePercentage/100;
            
            float contentHeight = tableViewAbout->getContentSize().height;
            float boundingBoxHeight = tableViewAbout->getBoundingBox().size.height;
            if (newOffset > contentHeight - boundingBoxHeight) {
                newOffset = contentHeight - boundingBoxHeight;
            }
            newOffset = -newOffset;
            offset.y = newOffset;
            tableViewAbout->setContentOffsetInDuration(offset, 0.1f);
        }
            break;
        case ButtonType::BOTTOM: {
            Vec2 offset = tableViewAbout->getContentOffset();
            float positiveYOffset = -offset.y;
            float newOffset = positiveYOffset - labelAbout->getContentSize().height * InfoLabelScrollUpdatePercentage/100;
            
            if (newOffset < 0) {
                newOffset = 0;
            }
            newOffset = -newOffset;
            offset.y = newOffset;
            tableViewAbout->setContentOffsetInDuration(offset, 0.1f);
        }
            break;
        case ButtonType::BACK:
            SceneManager::getInstance()->returnToLastScene();
            break;
        default:
            break;
    }
}

void About::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    // exit to Main only if back key pressed (KeyCode == 6)
    if (static_cast<int>(keyCode) != 6) return;
    SceneManager::getInstance()->returnToLastScene();
}

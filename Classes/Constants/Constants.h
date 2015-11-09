//
//  Constants.h
//  IkasGame
//
//  Created by Sergio Garcia on 30/01/15.
//
//

#ifndef __IkasCocosGame__Constants
#define __IkasCocosGame__Constants

#include "cocos2d.h"
using namespace cocos2d;

/**
 * Embedded zip name
 */
static std::string DefaultZipName = "ikasordena-563774185acd0.zip";

/**
 * URL´s
 */
static std::string ApiURL = "http://www.ikastek.net/ikastek-klear/apps-data/ikasordena";
static std::string RankingURL = "http://ikastek.net/ikastek-klear/rest/rankings?app=8";
static std::string InfoURL = "http://www.ikastek.net/aplikazioak/umeentzako/ikasordena/";

/**
 * Custom callbacks
 */
typedef std::function<void(Ref*)> IkasHandler;
typedef std::function<void(Ref*, bool)> IkasResultHandler;

#include "../GameModels/RankingData.h"
typedef std::function<void(bool, std::vector<RankingData*>)> IkasRankingDataHandler;

typedef std::function<void()> IkasEmptyHandler;
typedef std::function<void(double)> IkasValueHandler;

/**
 * Main fonts names
 */
static std::string MainRegularFont = "fonts/Oxygen-Regular.ttf";
static std::string MainBoldFont = "fonts/Oxygen-Bold.ttf";
static std::string MainLightFont = "fonts/Oxygen-Light.ttf";

/**
 * Sound settings filename
 */
static std::string SoundEnableImage = "sound-on";
static std::string SoundDisableImage = "sound-off";

/**
 * App colors
 */
static cocos2d::Color4B IkasPink = cocos2d::Color4B(231, 58, 82, 255);
static cocos2d::Color4B IkasRed = cocos2d::Color4B(190, 22, 34, 255);
static cocos2d::Color4B IkasWhite = cocos2d::Color4B(255, 255, 255, 255);
static cocos2d::Color4B IkasGrayLight = cocos2d::Color4B(173, 173, 173, 255);
static cocos2d::Color4B IkasGrayDark = cocos2d::Color4B(87, 87, 87, 255);
static cocos2d::Color4B IkasPinkAlpha = cocos2d::Color4B(231, 58, 82, 130);

static cocos2d::Color4B IkasSuccess = cocos2d::Color4B(32, 155, 49, 255);
static cocos2d::Color4B IkasError = cocos2d::Color4B(183, 7, 0, 255);

/**
 * Different scenes used
 */
enum class SceneType
{
	NONE,
    SPLASH,
	MAIN,
    CATEGORIES,
    LEVEL,
    GAMEPLAY,
    PAUSE,
    WIN,
    LOSE,
    RANKING
};

/**
 * Game difficuty options
 */
enum class Difficulty
{
    EMPTY = -1,
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
};

/**
 * Different checkBox types used in Game Settings Scene
 */
enum class CheckBoxType
{
    NONE,
    MUSIC,
    SFX
};

/**
 * Different checkBox types used in Game Settings Scene
 */
enum class CustomLanguage
{
    EUSKARA = 0,
    SPANISH
};

#endif

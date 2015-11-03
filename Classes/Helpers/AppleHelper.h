//
//  AppleHelper.h
//  IkasGame
//
//  Created by Sergio Garcia on 24/2/15.
//
//

#ifndef __IkasGame__AppleHelper__
#define __IkasGame__AppleHelper__

#include "../Constants/Constants.h"
#include <string>
#include <stdio.h>

#include "cocos2d.h"
using namespace cocos2d;

class AppleHelper
{
public:
    static void ignorePathFromiCloud(std::string path , bool isDir, bool ignore);
    static void downloadZip(std::string zipURL, std::string zipPath, const IkasEmptyHandler successCallback, const IkasEmptyHandler errorCallback, const IkasValueHandler progressCallback);
};

#endif /* defined(__IkasGame__AppleHelper__) */

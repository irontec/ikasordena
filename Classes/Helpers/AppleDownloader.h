//
//  AppleDownloader.h
//  Ikasesteka
//
//  Created by Sergio García on 24/3/15.
//
//

#include "../Constants/Constants.h"
#include <string>
#include <stdio.h>

#include "cocos2d.h"
using namespace cocos2d;

@interface AppleDownloader : NSObject

- (void)download:(std::string)zipURL toPath:(std::string)zipPath withSuccessCallback:(const IkasEmptyHandler)successCallback errorCallback:(const IkasEmptyHandler)errorCallback progressCallback:(const IkasValueHandler)progressCallback;
@end

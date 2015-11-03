//
//  AppleDownloader.m
//  Ikasesteka
//
//  Created by Sergio García on 24/3/15.
//
//

#import "AppleDownloader.h"
#import "../Singletons/ZipManager.h"
#import "AppleHelper.h"

@interface AppleDownloader () {
    NSMutableData *receivedData;
    long long expectedFileSize;
    
    IkasEmptyHandler mSuccessCallback;
    IkasEmptyHandler mErrorCallback;
    IkasValueHandler mProgressCallback;
}
@end

@implementation AppleDownloader

- (void)download:(std::string)zipURL toPath:(std::string)zipPath withSuccessCallback:(const IkasEmptyHandler)successCallback errorCallback:(const IkasEmptyHandler)errorCallback progressCallback:(const IkasValueHandler)progressCallback
{
    mSuccessCallback = successCallback;
    mErrorCallback = errorCallback;
    mProgressCallback = progressCallback;
    
    NSString *finalZipURL = [NSString stringWithCString:zipURL.c_str()
                                               encoding:[NSString defaultCStringEncoding]];
    NSURL *url = [NSURL URLWithString:finalZipURL];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    
    receivedData = [[NSMutableData alloc] initWithLength:0];
    NSURLConnection *connection = [[NSURLConnection alloc] initWithRequest:request
                                                                  delegate:self
                                                          startImmediately:YES];
    [connection start];
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    [UIApplication sharedApplication].networkActivityIndicatorVisible = YES;

    [receivedData setLength:0];
    
    if ([response expectedContentLength] == NSURLResponseUnknownLength) {
        expectedFileSize = 0;
    }
    else {
        expectedFileSize = [response expectedContentLength];
    }
    
    NSLog(@"expectedFileSize: %lld", expectedFileSize);
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    [receivedData appendData:data];
    float percentaje = 0;
    if (expectedFileSize != 0) {
        percentaje = ((float)[receivedData length] / expectedFileSize) * 100;
        mProgressCallback((int)percentaje);
    } else {
        //if no value not notify
//        mProgressCallback(0);
    }
//    NSLog(@"progress: %f", percentaje); // Debug
    
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    [UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
    mErrorCallback();
}

- (NSCachedURLResponse *)connection:(NSURLConnection *)connection willCacheResponse:(NSCachedURLResponse *)cachedResponse {
    return nil;
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
    [UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
    
    std::string zipPath = ZipManager::getZipFilePath();
    NSString *finalZipPath = [NSString stringWithCString:zipPath.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
    [receivedData writeToFile:finalZipPath atomically:YES];
    AppleHelper::ignorePathFromiCloud(zipPath, true, true);
    
    mSuccessCallback();
}


@end
/*!
 * iOS SDK
 *
 * Tencent is pleased to support the open source community by making
 * Hippy available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "HippyImageView.h"
#import <objc/runtime.h>
#import "HippyUtils.h"
#import "UIView+Hippy.h"
#import "HippyImageViewCustomLoader.h"
#import "HippyBridge+LocalFileSource.h"
#import "HippyImageCacheManager.h"
#import "HippyAnimatedImage.h"
#import "HippyDefaultImageProvider.h"
#import <Accelerate/Accelerate.h>

NSString *const HippyImageErrorDomain = @"HippyImageErrorDomain";

typedef NS_ENUM(NSUInteger, ImageDataError) {
    ImageDataUnavailable = 10001,
    ImageDataNotExist,
    ImageDataReceivedError,
    ImageDataBlurredError,
};

typedef struct _BorderRadiusStruct {
    CGFloat topLeftRadius;
    CGFloat topRightRadius;
    CGFloat bottomLeftRadius;
    CGFloat bottomRightRadius;
} BorderRadiusStruct;

static NSOperationQueue *hippy_image_queue(void) {
    static dispatch_once_t onceToken;
    static NSOperationQueue *_hippy_image_queue = nil;
    dispatch_once(&onceToken, ^{
        _hippy_image_queue = [[NSOperationQueue alloc] init];
        _hippy_image_queue.maxConcurrentOperationCount = 1;
    });
    return _hippy_image_queue;
}

static NSOperationQueue *animated_image_queue(void) {
    static dispatch_once_t onceToken;
    static NSOperationQueue *_animatedImageOQ = nil;
    dispatch_once(&onceToken, ^{
        _animatedImageOQ = [[NSOperationQueue alloc] init];
    });
    return _animatedImageOQ;
}

static BOOL HippyImageNeedsShrinkForSize(UIImage *inputImage, CGSize size) {
    CGSize inputImageSize = inputImage.size;
    if (inputImageSize.width > size.width || inputImageSize.height > size.height) {
        return YES;
    }
    return NO;
}

UIImage *HippyBlurredImageWithRadiusv(UIImage *inputImage, CGFloat radius, NSError **error) {
    CGImageRef imageRef = inputImage.CGImage;
    CGFloat imageScale = inputImage.scale;
    UIImageOrientation imageOrientation = inputImage.imageOrientation;

    // Image must be nonzero size
    if (CGImageGetWidth(imageRef) * CGImageGetHeight(imageRef) == 0) {
        return inputImage;
    }

    // convert to ARGB if it isn't
    if (CGImageGetBitsPerPixel(imageRef) != 32 || CGImageGetBitsPerComponent(imageRef) != 8
        || !((CGImageGetBitmapInfo(imageRef) & kCGBitmapAlphaInfoMask))) {
        UIGraphicsImageRendererFormat *format = [UIGraphicsImageRendererFormat preferredFormat];
        format.opaque = NO;
        format.scale = inputImage.scale;
        UIGraphicsImageRenderer *renderer = [[UIGraphicsImageRenderer alloc] initWithSize:inputImage.size format:format];
        UIImage *renderedImage = [renderer imageWithActions:^(UIGraphicsImageRendererContext * _Nonnull context) {
            [inputImage drawAtPoint:CGPointZero];
        }];
        imageRef = renderedImage.CGImage;
    }
    
    vImage_Buffer buffer1, buffer2;
    memset(&buffer1, 0, sizeof(vImage_Buffer));
    memset(&buffer2, 0, sizeof(vImage_Buffer));
    void *tempBuffer = NULL;
    CFDataRef dataSource = NULL;
    CGContextRef ctx = NULL;
    CGImageRef blurredImageRef = NULL;
    @try {
        buffer1.width = buffer2.width = CGImageGetWidth(imageRef);
        buffer1.height = buffer2.height = CGImageGetHeight(imageRef);
        buffer1.rowBytes = buffer2.rowBytes = CGImageGetBytesPerRow(imageRef);
        size_t bytes = buffer1.rowBytes * buffer1.height;
        buffer1.data = malloc(bytes);
        if (NULL == buffer1.data) {
            return inputImage;
        }
        buffer2.data = malloc(bytes);
        if (NULL == buffer2.data) {
            free(buffer1.data);
            return inputImage;
        }
        // A description of how to compute the box kernel width from the Gaussian
        // radius (aka standard deviation) appears in the SVG spec:
        // http://www.w3.org/TR/SVG/filters.html#feGaussianBlurElement
        uint32_t boxSize = floor((radius * imageScale * 3 * sqrt(2 * M_PI) / 4 + 0.5) / 2);
        boxSize |= 1;  // Ensure boxSize is odd

        // create temp buffer
        tempBuffer = malloc(
            (size_t)vImageBoxConvolve_ARGB8888(&buffer1, &buffer2, NULL, 0, 0, boxSize, boxSize, NULL, kvImageEdgeExtend + kvImageGetTempBufferSize));

        // copy image data
        dataSource = CGDataProviderCopyData(CGImageGetDataProvider(imageRef));
        memcpy(buffer1.data, CFDataGetBytePtr(dataSource), bytes);
        CFRelease(dataSource);
        dataSource = NULL;

        // perform blur
        vImage_Error error;
        error = vImageBoxConvolve_ARGB8888(&buffer1, &buffer2, tempBuffer, 0, 0, boxSize, boxSize, NULL, kvImageEdgeExtend);
        if (error) {
            return inputImage;
        }
        error = vImageBoxConvolve_ARGB8888(&buffer2, &buffer1, tempBuffer, 0, 0, boxSize, boxSize, NULL, kvImageEdgeExtend);
        if (error) {
            return inputImage;
        }
        error = vImageBoxConvolve_ARGB8888(&buffer1, &buffer2, tempBuffer, 0, 0, boxSize, boxSize, NULL, kvImageEdgeExtend);
        if (error) {
            return inputImage;
        }

        // free buffers
        free(buffer2.data);
        buffer2.data = NULL;
        free(tempBuffer);
        tempBuffer = NULL;

        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGBitmapInfo bitmapInfoMasked = CGImageGetBitmapInfo(imageRef);
        CGBitmapInfo bitmapInfo = bitmapInfoMasked & kCGBitmapByteOrderMask;
        CGImageAlphaInfo alphaInfo = CGImageGetAlphaInfo(imageRef);
        if (alphaInfo == kCGImageAlphaNone || alphaInfo == kCGImageAlphaOnly) {
            alphaInfo = kCGImageAlphaNoneSkipFirst;
        } else if (alphaInfo == kCGImageAlphaFirst) {
            alphaInfo = kCGImageAlphaPremultipliedFirst;
        } else if (alphaInfo == kCGImageAlphaLast) {
            alphaInfo = kCGImageAlphaPremultipliedLast;
        }
        // "The constants for specifying the alpha channel information are declared with the `CGImageAlphaInfo` type but can be passed to this parameter safely." (source: docs)
        bitmapInfo |= alphaInfo;
        // create image context from buffer
        ctx = CGBitmapContextCreate(
            buffer1.data, buffer1.width, buffer1.height, 8, buffer1.rowBytes, colorSpace, bitmapInfo);
        CGColorSpaceRelease(colorSpace);
        // create image from context
        blurredImageRef = CGBitmapContextCreateImage(ctx);
        UIImage *outputImage = [UIImage imageWithCGImage:blurredImageRef scale:imageScale orientation:imageOrientation];
        CGImageRelease(blurredImageRef);
        blurredImageRef = NULL;
        CGContextRelease(ctx);
        ctx = NULL;
        free(buffer1.data);
        buffer1.data = NULL;
        return outputImage;
    } @catch (NSException *exception) {
        if (buffer1.data) {
            free(buffer1.data);
        }
        if (buffer2.data) {
            free(buffer2.data);
        }
        if (tempBuffer) {
            free(tempBuffer);
        }
        if (dataSource) {
            CFRelease(dataSource);
        }
        if (blurredImageRef) {
            CGImageRelease(blurredImageRef);
        }
        if (ctx) {
            CGContextRelease(ctx);
        }
        if (error) {
            NSDictionary *useInfo = @{NSLocalizedDescriptionKey: exception.reason ?: @""};
            *error = [NSError errorWithDomain:HippyImageErrorDomain code:ImageDataBlurredError userInfo:useInfo];
        }
        return inputImage;
    }
}

NSError *imageErrorFromParams(NSInteger errorCode, NSString *errorDescription) {
    return [NSError errorWithDomain:HippyImageErrorDomain code:errorCode userInfo:@ { NSLocalizedDescriptionKey: errorDescription ?: @"" }];
}

@interface UIImage (Hippy)
@property (nonatomic, copy) CAKeyframeAnimation *hippyKeyframeAnimation;
@end

@interface HippyImageView () {
    NSURLSessionDataTask *_task;
    NSURL *_imageLoadURL;
    long long _totalLength;
    NSMutableData *_data;
    __weak CALayer *_borderWidthLayer;
    BOOL _needsUpdateBorderRadiusManully;
    CGSize _size;
    CGRect _imageRect;
}

/// Animated Image Operation (should avoid multi-thread manipulation)
@property (nonatomic, strong) HippyAnimatedImageOperation *animatedImageOperation;

/// Animated Image Manipulation Lock
@property (nonatomic, strong) NSLock *animatedImageOpLock;

/// The image source that's being loaded from the network
@property (atomic, strong) NSString *pendingImageSourceUri;

/// The image source that's currently displayed
@property (atomic, strong) NSString *imageSourceUri;

@end

@implementation HippyImageView

- (instancetype)initWithBridge:(HippyBridge *)bridge {
    if (self = [super init]) {
        _bridge = bridge;
        self.clipsToBounds = YES;
        _needsUpdateBorderRadiusManully = NO;
        _borderTopLeftRadius = CGFLOAT_MAX;
        _borderTopRightRadius = CGFLOAT_MAX;
        _borderBottomLeftRadius = CGFLOAT_MAX;
        _borderBottomRightRadius = CGFLOAT_MAX;
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)didMoveToWindow {
    [super didMoveToWindow];
    if (!self.window) {
        [self cancelImageLoad];
    } else if ([self shouldChangeImageSource]) {
        [self reloadImage];
    }
}

- (void)didReceiveMemoryWarning {
    [self clearImageIfDetached];
}

- (void)appDidEnterBackground {
    [self clearImageIfDetached];
}

- (void)appWillEnterForeground {
}

- (void)clearImageIfDetached {
    if (!self.window) {
        [self clearImage];
    }
}

- (void)setSource:(NSArray *)source {
    if (![_source isEqualToArray:source]) {
        _source = [source copy];
        self.animatedImage = nil;
        [self updateImage:nil];
        [self reloadImage];
    }
}

- (void)setDefaultImage:(UIImage *)defaultImage {
    if (_defaultImage != defaultImage) {
        BOOL shouldUpdateImage = (self.image == _defaultImage);
        _defaultImage = defaultImage;
        if (shouldUpdateImage) {
            [self updateImage:_defaultImage];
        }
    }
}

- (void)setCapInsets:(UIEdgeInsets)capInsets {
    if (!UIEdgeInsetsEqualToEdgeInsets(_capInsets, capInsets)) {
        if (UIEdgeInsetsEqualToEdgeInsets(_capInsets, UIEdgeInsetsZero) || UIEdgeInsetsEqualToEdgeInsets(capInsets, UIEdgeInsetsZero)) {
            _capInsets = capInsets;
            [self reloadImage];
        } else {
            _capInsets = capInsets;
            [self updateImage:self.image];
        }
    }
}

- (void)setBlurRadius:(CGFloat)blurRadius {
    if (blurRadius != _blurRadius) {
        _blurRadius = blurRadius;
        [self reloadImage];
    }
}

- (void)setFrame:(CGRect)frame {
    BOOL result = CGRectEqualToRect(frame, _imageRect);
    if (result) {
        return;
    }
    [super setFrame:frame];
    _size = frame.size;
    _imageRect = frame;
    [self reloadImage];
    [self updateCornerRadius];
}

- (void)setResizeMode:(HippyResizeMode)resizeMode {
    if (_resizeMode != resizeMode) {
        _resizeMode = resizeMode;

        if (_resizeMode == HippyResizeModeRepeat) {
            self.contentMode = UIViewContentModeScaleToFill;
        } else {
            self.contentMode = (UIViewContentMode)resizeMode;
        }
        if (self.image) {
            [self updateImage:self.image];
        }
    }
}

- (void)setRenderingMode:(UIImageRenderingMode)renderingMode {
    if (_renderingMode != renderingMode) {
        _renderingMode = renderingMode;
        [self updateImage:self.image];
    }
}

- (BOOL)shouldChangeImageSource {
    // We need to reload if the desired image source is different from the current image
    // source AND the image load that's pending
    NSDictionary *source = [self.source firstObject];
    if (source) {
        NSString *desiredImageSource = source[@"uri"];

        return ![desiredImageSource isEqualToString:self.imageSourceUri] && ![desiredImageSource isEqualToString:self.pendingImageSourceUri];
    }
    return NO;
}

static void decodeAndLoadImageAsync(HippyImageView *imageView, id<HippyImageProviderProtocol> instance,
                                    NSString *imageUrl, BOOL isAnimatedImage, NSData * _Nullable imgDataToBeCached) {
    if (!imageView) return;
    if (isAnimatedImage) {
        [imageView.animatedImageOpLock lock];
        if (imageView.animatedImageOperation) {
            [imageView.animatedImageOperation cancel];
        }
        imageView.animatedImageOperation = [[HippyAnimatedImageOperation alloc] initWithAnimatedImageProvider:instance
                                                                                                    imageView:imageView
                                                                                                     imageURL:imageUrl];
        [animated_image_queue() addOperation:imageView.animatedImageOperation];
        [imageView.animatedImageOpLock unlock];
    } else {
        __weak __typeof(imageView)weakSelf = imageView;
        [hippy_image_queue() addOperationWithBlock:^{
            __strong __typeof(weakSelf)strongSelf = weakSelf;
            if (!strongSelf) return;
            UIImage *image = [instance image];
            if (image) {
                if (imgDataToBeCached) {
                    [[HippyImageCacheManager sharedInstance] setImageCacheData:imgDataToBeCached forURLString:imageUrl];
                }
                [strongSelf loadImage:image url:imageUrl error:nil needBlur:YES needCache:YES];
            } else {
                NSString *errorMessage = [NSString stringWithFormat:@"image data unavailable for uri %@", imageUrl];
                NSError *theError = imageErrorFromParams(ImageDataUnavailable, errorMessage);
                [strongSelf loadImage:nil url:imageUrl error:theError needBlur:YES needCache:NO];
            }
        }];
    }
}

- (void)reloadImage {
    NSDictionary *source = [self.source firstObject];
    if (source && CGRectGetWidth(self.frame) > 0 && CGRectGetHeight(self.frame) > 0) {
        if (_onLoadStart) {
            _onLoadStart(@{});
        }
        NSString *uri = source[@"uri"];
        self.pendingImageSourceUri = uri;
        NSData *data = [[HippyImageCacheManager sharedInstance] imageCacheDataForURLString:uri];
        HippyBridge *strongBridge = self.bridge;
        if (!strongBridge) {
            return;
        }
        if (data) {
            // Cache Data
            Class<HippyImageProviderProtocol> ipClass = imageProviderClassFromBridge(data, strongBridge);
            id<HippyImageProviderProtocol> instance = [ipClass imageProviderInstanceForData:data];
            if (instance) {
                BOOL isAnimatedImage = [instance imageCount] > 1;
                decodeAndLoadImageAsync(self, instance, uri.copy, isAnimatedImage, nil);
                return;
            }
        }

        NSData *uriData = [uri dataUsingEncoding:NSUTF8StringEncoding];
        if (nil == uriData) {
            return;
        }
        CFURLRef urlRef = CFURLCreateWithBytes(NULL, [uriData bytes], [uriData length], kCFStringEncodingUTF8, NULL);
        NSURL *source_url = CFBridgingRelease(urlRef);
        if ([HippyBridge isHippyLocalFileURLString:uri]) {
            NSString *localPath = [strongBridge absoluteStringFromHippyLocalFileURLString:uri];
            BOOL isDirectory = NO;
            BOOL fileExist = [[NSFileManager defaultManager] fileExistsAtPath:localPath isDirectory:&isDirectory];
            if (fileExist && !isDirectory) {
                // Local File Data
                NSData *imageData = [NSData dataWithContentsOfFile:localPath];
                Class<HippyImageProviderProtocol> ipClass = imageProviderClassFromBridge(imageData, strongBridge);
                id<HippyImageProviderProtocol> instance = [self instanceImageProviderFromClass:ipClass imageData:imageData];
                BOOL isAnimatedImage = [ipClass isAnimatedImage:imageData];
                decodeAndLoadImageAsync(self, instance, source_url.absoluteString, isAnimatedImage, nil);
            } else {
                NSString *errorMessage = [NSString stringWithFormat:@"local image data not exist %@", source_url.absoluteString];
                NSError *error = imageErrorFromParams(ImageDataNotExist, errorMessage);
                [self loadImage:nil url:source_url.absoluteString error:error needBlur:YES needCache:NO];
            }
            return;
        }
        
        __weak __typeof(self) weakSelf = self;
        typedef void (^HandleBase64CompletedBlock)(NSString *);
        HandleBase64CompletedBlock handleBase64CompletedBlock = ^void(NSString *base64Data) {
            HippyImageView *strongSelf = weakSelf;
            if (!strongSelf) {
                return;
            }
            NSRange range = [base64Data rangeOfString:@";base64,"];
            if (NSNotFound != range.location) {
                // Base64 Data
                base64Data = [base64Data substringFromIndex:range.location + range.length];
                NSData *imageData = [[NSData alloc] initWithBase64EncodedString:base64Data options:NSDataBase64DecodingIgnoreUnknownCharacters];
                Class<HippyImageProviderProtocol> ipClass = imageProviderClassFromBridge(imageData, strongBridge);
                id<HippyImageProviderProtocol> instance = [strongSelf instanceImageProviderFromClass:ipClass imageData:imageData];
                BOOL isAnimatedImage = [ipClass isAnimatedImage:imageData];
                decodeAndLoadImageAsync(strongSelf, instance, uri.copy, isAnimatedImage, nil);
            }
        };

        HippyDirectEventBlock onProgress = self.onProgress;
        typedef void (^HandleImageCompletedBlock)(NSURL *);
        HandleImageCompletedBlock handleImageCompletedBlock = ^void(NSURL *source_url) {
            HippyImageView *strongSelf = weakSelf;
            if (!strongSelf) {
                return;
            }
            [strongBridge.imageLoader imageView:strongSelf
                                      loadAtUrl:source_url
                               placeholderImage:strongSelf.defaultImage
                                       progress:^(long long currentLength,
                                                  long long totalLength) {
                if (onProgress) {
                    onProgress(@{@"loaded": @((double)currentLength), @"total": @((double)totalLength)});
                }
            }
                                      completed:^(NSData * _Nullable data,
                                                  NSURL * _Nonnull url,
                                                  NSError * _Nullable error,
                                                  UIImage * _Nullable image,
                                                  HippyImageCustomLoaderControlOptions options) {
                HippyImageView *strongSelf = weakSelf;
                if (!strongSelf) return;
                NSString *imageUrl = url.absoluteString;
                
                /* When using custom decoder, user may need to skip sdk's internal decoding process,
                 * (only for non-GIF images), so hippy provided a control parameter `options`.
                 * Note that skiping internal decoding also skips the internal `downsample optimization`.
                 */
                Class<HippyImageProviderProtocol> ipClass = imageProviderClassFromBridge(data, strongBridge);
                id<HippyImageProviderProtocol> instance = [strongSelf instanceImageProviderFromClass:ipClass imageData:data];
                BOOL isAnimatedImage = [ipClass isAnimatedImage:data];
                if (!isAnimatedImage && (options & HippyImageCustomLoaderControl_SkipDecodeOrDownsample)) {
                    if (image) {
                        [strongSelf loadImage:image url:imageUrl error:nil needBlur:YES needCache:YES];
                    } else {
                        NSString *errorMsg = [NSString stringWithFormat:@"image data unavailable for uri %@", imageUrl];
                        NSError *theError = imageErrorFromParams(ImageDataUnavailable, errorMsg);
                        [strongSelf loadImage:nil url:imageUrl error:theError needBlur:NO needCache:NO];
                    }
                } else {
                    decodeAndLoadImageAsync(strongSelf, instance, imageUrl, isAnimatedImage, nil);
                }
            }];
        };

        if (strongBridge.imageLoader && source_url) {
            if (_defaultImage) {
                self.image = _defaultImage;
            }

            if ([[source_url absoluteString] hasPrefix:@"data:image/"]) {
                handleBase64CompletedBlock([source_url absoluteString]);
            } else {
                if (_imageLoadURL) {
                    [strongBridge.imageLoader cancelImageDownload:self withUrl:_imageLoadURL];
                }
                _imageLoadURL = source_url;
                handleImageCompletedBlock(source_url);
            }

        } else {
            if ([uri hasPrefix:@"data:image/"]) {
                handleBase64CompletedBlock(uri);
            } else if (source_url) {
                if (_task) {
                    [self cancelImageLoad];
                }
                NSURLSessionConfiguration *sessionConfiguration = [NSURLSessionConfiguration ephemeralSessionConfiguration];
                NSURLSession *session = [NSURLSession sessionWithConfiguration:sessionConfiguration delegate:self delegateQueue:hippy_image_queue()];
                _task = [session dataTaskWithURL:source_url];
                [_task resume];
            }
        }
    }
}

- (void)cancelImageLoad {
    self.pendingImageSourceUri = nil;
    NSDictionary *source = [self.source firstObject];
    if (_bridge.imageLoader) {
        [self.animatedImageOpLock lock];
        if (self.animatedImageOperation) {
            [self.animatedImageOperation cancel];
            self.animatedImageOperation = nil;
        }
        [self.animatedImageOpLock unlock];
        [_bridge.imageLoader cancelImageDownload:self withUrl:source[@"uri"]];
    } else {
        [_task cancel];
        _task = nil;
        [hippy_image_queue() addOperationWithBlock:^{
            self->_data = nil;
            self->_totalLength = 0;
        }];
    }
}

- (void)clearImage {
    [self cancelImageLoad];
    [self.layer removeAnimationForKey:@"contents"];
    self.image = nil;
    self.imageSourceUri = nil;
}

- (UIImage *)imageFromData:(NSData *)data {
    if (nil == data) {
        return nil;
    }
    Class<HippyImageProviderProtocol> ipClass = imageProviderClassFromBridge(data, self.bridge);
    id<HippyImageProviderProtocol> instance = [self instanceImageProviderFromClass:ipClass imageData:data];
    return [instance image];
}

#pragma mark -
- (void)URLSession:(__unused NSURLSession *)session
              dataTask:(__unused NSURLSessionDataTask *)dataTask
    didReceiveResponse:(NSURLResponse *)response
     completionHandler:(void (^)(NSURLSessionResponseDisposition disposition))completionHandler {
    if (_task == dataTask) {
        _totalLength = response.expectedContentLength;
        completionHandler(NSURLSessionResponseAllow);
        NSUInteger capacity = NSURLResponseUnknownLength != _totalLength ? (NSUInteger)_totalLength : 256;
        _data = [[NSMutableData alloc] initWithCapacity:capacity];
    }
}

- (void)URLSession:(__unused NSURLSession *)session dataTask:(__unused NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data {
    if (_task == dataTask) {
        [_data appendData:data];
        if (_onProgress && NSURLResponseUnknownLength != _totalLength) {
            _onProgress(@{ @"loaded": @((double)_data.length), @"total": @((double)_totalLength) });
        }
    }
}

- (void)URLSession:(__unused NSURLSession *)session task:(nonnull NSURLSessionTask *)task didCompleteWithError:(nullable NSError *)error {
    if (_task == task) {
        NSString *urlString = [[[task originalRequest] URL] absoluteString];
        if (!error) {
            if ([_data length] > 0) {
                Class<HippyImageProviderProtocol> ipClass = imageProviderClassFromBridge(_data, self.bridge);
                id<HippyImageProviderProtocol> instance = [self instanceImageProviderFromClass:ipClass imageData:_data];
                BOOL isAnimatedImage = [ipClass isAnimatedImage:_data];
                // no need to copy _data since we have make sure to run in same hippy_image_queue()
                decodeAndLoadImageAsync(self, instance, urlString, isAnimatedImage, _data);
            } else {
                NSURLResponse *response = [task response];
                if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
                    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
                    NSUInteger statusCode = [httpResponse statusCode];
                    NSString *errorMessage = [NSString stringWithFormat:@"no data received, HTTPStatusCode is %zd, url is %@", statusCode, urlString];
                    NSError *imgError = imageErrorFromParams(ImageDataUnavailable, errorMessage);
                    [self loadImage:nil url:urlString error:imgError needBlur:NO needCache:NO];
                }
            }
        } else {
            NSError *imgError = imageErrorFromParams(ImageDataReceivedError, error.localizedDescription);
            [self loadImage:nil url:urlString error:imgError needBlur:YES needCache:NO];
        }
    }
    [session finishTasksAndInvalidate];
}

#pragma mark -

- (void)loadImage:(UIImage *)image url:(NSString *)url error:(NSError *)error needBlur:(BOOL)needBlur needCache:(BOOL)needCache {
    if (error) {
        if (_onError && error.code != NSURLErrorCancelled) {
            _onError(@{ @"error": error.localizedDescription, @"errorCode": @(error.code), @"errorURL": url });
        }
        if (_onLoadEnd) {
            _onLoadEnd(nil);
        }
        return;
    }
    NSDictionary *source = [self.source firstObject];
    NSString *sourceURL = [source objectForKey:@"uri"];
    NSString *encodedSourceURL = [[NSURL URLWithString:sourceURL] absoluteString];
    BOOL shouldContinue = url && sourceURL && ([url isEqualToString:encodedSourceURL] || [url isEqualToString:sourceURL]);
    if (!shouldContinue) {
        return;
    }
    __weak __typeof(self) weakSelf = self;
    void (^setImageBlock)(UIImage *) = ^(UIImage *image) {
        HippyImageView *strongSelf = weakSelf;
        if (!strongSelf) {
            return;
        }
        strongSelf.pendingImageSourceUri = nil;
        strongSelf.imageSourceUri = url;
        if (image.hippyKeyframeAnimation) {
            [strongSelf.layer addAnimation:image.hippyKeyframeAnimation forKey:@"contents"];
        } else {
            [strongSelf.layer removeAnimationForKey:@"contents"];
            [strongSelf updateImage:image];
        }

        if (strongSelf.onLoad)
            strongSelf.onLoad(@{ @"width": @(image.size.width), @"height": @(image.size.height), @"url": url ?: @"" });
        if (strongSelf.onLoadEnd)
            strongSelf.onLoadEnd(nil);
    };

    if (_blurRadius > 100 && [NSProcessInfo processInfo].physicalMemory <= 1024 * 1024 * 1000) {
        _blurRadius = 100;
    }

    CGFloat br = _blurRadius;
    if (_blurRadius > __FLT_EPSILON__ && needBlur) {
        // Blur on a background thread to avoid blocking interaction
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            NSError *error = nil;
            UIImage *blurredImage = HippyBlurredImageWithRadiusv(image, br, &error);
            if (error) {
                NSError *finalError = HippyErrorFromErrorAndModuleName(error, self.bridge.moduleName);
                HippyFatal(finalError);
            }
            if (needCache) {
                [[HippyImageCacheManager sharedInstance] setImage:blurredImage forURLString:url blurRadius:br];
            }
            HippyExecuteOnMainQueue(^{
                setImageBlock(blurredImage);
            });
        });
    } else {
        HippyExecuteOnMainQueue(^{
            if (needCache) {
                [[HippyImageCacheManager sharedInstance] setImage:image forURLString:url blurRadius:br];
            }
            setImageBlock(image);
        });
    }
}

- (void)updateImage:(UIImage *)image {
    image = image ?: _defaultImage;
    if (!image) {
        self.image = nil;
        self.imageSourceUri = nil;
        return;
    }

    if (_renderingMode != image.renderingMode) {
        image = [image imageWithRenderingMode:_renderingMode];
    }

    if (HippyResizeModeRepeat == _resizeMode) {
        image = [image resizableImageWithCapInsets:_capInsets resizingMode:UIImageResizingModeTile];
    }
    else if (HippyResizeModeCenter == _resizeMode) {
        if (HippyImageNeedsShrinkForSize(image, self.bounds.size)) {
            self.contentMode = UIViewContentModeScaleAspectFit;
        }
    }
    else if (!UIEdgeInsetsEqualToEdgeInsets(UIEdgeInsetsZero, _capInsets)) {
        // Applying capInsets of 0 will switch the "resizingMode" of the image to "tile" which is undesired
        image = [image resizableImageWithCapInsets:_capInsets resizingMode:UIImageResizingModeStretch];
    }

    // Apply trilinear filtering to smooth out mis-sized images
    //    self.layer.minificationFilter = kCAFilterTrilinear;
    //    self.layer.magnificationFilter = kCAFilterTrilinear;

    if (image.images) {
        HippyAssert(NO, @"GIF should not invoke this");
    } else {
        self.image = image;
    }
}

- (void)updateCornerRadius {
    if (_borderWidthLayer) {
        [_borderWidthLayer removeFromSuperlayer];
    }
    if ([self needsUpdateCornerRadiusManully] && ![self isAllCornerRadiussEqualToCornerRadius]) {
        CGRect contentRect = self.bounds;
#ifdef HippyLog
        CGFloat width = CGRectGetWidth(contentRect);
        CGFloat height = CGRectGetHeight(contentRect);
        BOOL flag1 = _borderTopLeftRadius <= MIN(width, height) / 2;
        if (!flag1) {
            HippyLog(@"[warning] _borderTopLeftRadius must be shorter than width / 2");
        }
        BOOL flag2 = _borderTopRightRadius <= MIN(width, height) / 2;
        if (!flag2) {
            HippyLog(@"[warning] _borderTopRightRadius must be shorter than width / 2");
        }
        BOOL flag3 = _borderBottomLeftRadius <= MIN(width, height) / 2;
        if (!flag3) {
            HippyLog(@"[warning] _borderBottomLeftRadius must be shorter than width / 2");
        }
        BOOL flag4 = _borderBottomRightRadius <= MIN(width, height) / 2;
        if (!flag4) {
            HippyLog(@"[warning] _borderBottomRightRadius must be shorter than width / 2");
        }
#endif

        BorderRadiusStruct boderRadiusStruct = [self properBorderRadius];
        UIBezierPath *bezierPath = [self bezierPathFromBorderRadius:boderRadiusStruct contentRect:contentRect];
        CAShapeLayer *mask = [CAShapeLayer layer];
        mask.path = bezierPath.CGPath;
        self.layer.mask = mask;

        CAShapeLayer *borderLayer = [CAShapeLayer layer];
        borderLayer.path = bezierPath.CGPath;
        borderLayer.fillColor = [UIColor clearColor].CGColor;
        borderLayer.strokeColor = self.layer.borderColor;
        borderLayer.lineWidth = self.layer.borderWidth * 2;
        borderLayer.frame = contentRect;
        _borderWidthLayer = borderLayer;
        [self.layer addSublayer:borderLayer];
    } else {
        //radius must be smaller than MIN(self.frame.size.width, self.frame.size.height) / 2.0
        CGFloat minOfRadius = MIN(self.frame.size.width, self.frame.size.height) / 2.0f;
        double radius = HippyZeroIfNaN(MIN(minOfRadius, _borderRadius));
        self.layer.mask = nil;
        self.layer.cornerRadius = radius;
    }
}

- (UIBezierPath *)bezierPathFromBorderRadius:(BorderRadiusStruct)boderRadiusStruct contentRect:(CGRect)contentRect {
    CGFloat minX = CGRectGetMinX(contentRect);
    CGFloat minY = CGRectGetMinY(contentRect);
    CGFloat maxX = CGRectGetMaxX(contentRect);
    CGFloat maxY = CGRectGetMaxY(contentRect);

    UIBezierPath *bezierPath = [UIBezierPath bezierPath];
    CGPoint p1 = CGPointMake(minX + boderRadiusStruct.topLeftRadius, minY);
    [bezierPath moveToPoint:p1];
    CGPoint p2 = CGPointMake(maxX - boderRadiusStruct.topRightRadius, minY);
    [bezierPath addLineToPoint:p2];
    CGPoint p3 = CGPointMake(maxX - boderRadiusStruct.topRightRadius, minY + boderRadiusStruct.topRightRadius);
    [bezierPath addArcWithCenter:p3 radius:boderRadiusStruct.topRightRadius startAngle:M_PI_2 + M_PI endAngle:0 clockwise:YES];

    CGPoint p4 = CGPointMake(maxX, maxY - boderRadiusStruct.bottomRightRadius);
    [bezierPath addLineToPoint:p4];
    CGPoint p5 = CGPointMake(maxX - boderRadiusStruct.bottomRightRadius, maxY - boderRadiusStruct.bottomRightRadius);
    [bezierPath addArcWithCenter:p5 radius:boderRadiusStruct.bottomRightRadius startAngle:0 endAngle:M_PI_2 clockwise:YES];

    CGPoint p6 = CGPointMake(minX + boderRadiusStruct.bottomLeftRadius, maxY);
    [bezierPath addLineToPoint:p6];
    CGPoint p7 = CGPointMake(minX + boderRadiusStruct.bottomLeftRadius, maxY - boderRadiusStruct.bottomLeftRadius);
    [bezierPath addArcWithCenter:p7 radius:boderRadiusStruct.bottomLeftRadius startAngle:M_PI_2 endAngle:M_PI clockwise:YES];

    CGPoint p8 = CGPointMake(minX, minY + boderRadiusStruct.topLeftRadius);
    [bezierPath addLineToPoint:p8];
    CGPoint p9 = CGPointMake(minX + boderRadiusStruct.topLeftRadius, minY + boderRadiusStruct.topLeftRadius);
    [bezierPath addArcWithCenter:p9 radius:boderRadiusStruct.topLeftRadius startAngle:M_PI endAngle:M_PI + M_PI_2 clockwise:YES];
    [bezierPath closePath];
    return bezierPath;
}

- (void)setBorderTopLeftRadius:(CGFloat)borderTopLeftRadius {
    _borderTopLeftRadius = borderTopLeftRadius;
    _needsUpdateBorderRadiusManully = YES;
}

- (void)setBorderTopRightRadius:(CGFloat)borderTopRightRadius {
    _borderTopRightRadius = borderTopRightRadius;
    _needsUpdateBorderRadiusManully = YES;
}

- (void)setBorderBottomLeftRadius:(CGFloat)borderBottomLeftRadius {
    _borderBottomLeftRadius = borderBottomLeftRadius;
    _needsUpdateBorderRadiusManully = YES;
}

- (void)setBorderBottomRightRadius:(CGFloat)borderBottomRightRadius {
    _borderBottomRightRadius = borderBottomRightRadius;
    _needsUpdateBorderRadiusManully = YES;
}

- (void)setBorderRadius:(CGFloat)borderRadius {
    _borderRadius = borderRadius;
}

- (void)setBackgroundSize:(NSString *)backgroundSize {
    //do nothing
}

- (BOOL)needsUpdateCornerRadiusManully {
    return _needsUpdateBorderRadiusManully;
}

- (BOOL)isAllCornerRadiussEqualToCornerRadius {
#define CornerRadiusCompare(x) (x != CGFLOAT_MAX && fabs(_borderRadius - x) > CGFLOAT_EPSILON)
    if (_needsUpdateBorderRadiusManully) {
        if (CornerRadiusCompare(_borderTopLeftRadius) ||
            CornerRadiusCompare(_borderTopRightRadius) ||
            CornerRadiusCompare(_borderBottomLeftRadius) ||
            CornerRadiusCompare(_borderBottomRightRadius)) {
            return NO;
        }
    }
    return YES;
}

- (BorderRadiusStruct)properBorderRadius {
    BorderRadiusStruct radius = { 0, 0, 0, 0 };
    CGFloat halfWidth = CGRectGetWidth(self.bounds) / 2.f;
    CGFloat halfHeight = CGRectGetHeight(self.bounds) / 2.f;
    CGFloat halfSide = MIN(halfWidth, halfHeight);
    CGFloat topLeftRadius = _borderTopLeftRadius != CGFLOAT_MAX ? _borderTopLeftRadius : _borderRadius;
    CGFloat topRightRadius = _borderTopRightRadius != CGFLOAT_MAX ? _borderTopRightRadius : _borderRadius;
    CGFloat bottomLeftRadius = _borderBottomLeftRadius != CGFLOAT_MAX ? _borderBottomLeftRadius : _borderRadius;
    CGFloat bottomRightRadius = _borderBottomRightRadius != CGFLOAT_MAX ? _borderBottomRightRadius : _borderRadius;
    radius.topLeftRadius = MIN(topLeftRadius, halfSide);
    radius.topRightRadius = MIN(topRightRadius, halfSide);
    radius.bottomLeftRadius = MIN(bottomLeftRadius, halfSide);
    radius.bottomRightRadius = MIN(bottomRightRadius, halfSide);
    return radius;
}

- (id<HippyImageProviderProtocol>)instanceImageProviderFromClass:(Class<HippyImageProviderProtocol>)cls imageData:(NSData *)data {
    id<HippyImageProviderProtocol> instance = [cls imageProviderInstanceForData:data];
    if ([instance isKindOfClass:[HippyDefaultImageProvider class]]) {
        HippyDefaultImageProvider *provider = (HippyDefaultImageProvider *)instance;
        provider.imageViewSize = _size;
        provider.downSample = _downSample;
    }
    return instance;
}

#pragma mark - Private Accessories

- (NSLock *)animatedImageOpLock {
    if (!_animatedImageOpLock) {
        @synchronized (self) {
            if (!_animatedImageOpLock) {
                _animatedImageOpLock = [[NSLock alloc] init];
            }
        }
    }
    return _animatedImageOpLock;
}

@end

@implementation UIImage (Hippy)

- (CAKeyframeAnimation *)hippyKeyframeAnimation {
    return objc_getAssociatedObject(self, _cmd);
}

- (void)setHippyKeyframeAnimation:(CAKeyframeAnimation *)hippyKeyframeAnimation {
    objc_setAssociatedObject(self, @selector(hippyKeyframeAnimation), hippyKeyframeAnimation, OBJC_ASSOCIATION_COPY_NONATOMIC);
}

@end

@implementation HippyConvert (HippyResizeMode)

HIPPY_ENUM_CONVERTER(HippyResizeMode, (@{
    @"cover": @(HippyResizeModeCover),
    @"contain": @(HippyResizeModeContain),
    @"stretch": @(HippyResizeModeStretch),
    @"center": @(HippyResizeModeCenter),
    @"repeat": @(HippyResizeModeRepeat),
}),
    HippyResizeModeStretch, integerValue)

@end

@implementation HippyAnimatedImageOperation

- (id)initWithAnimatedImageProvider:(id<HippyImageProviderProtocol>)imageProvider imageView:(HippyImageView *)imageView imageURL:(NSString *)url {
    self = [super init];
    if (self) {
        _imageProvider = imageProvider;
        _url = url;
        _imageView = imageView;
    }
    return self;
}

- (id)initWithAnimatedImageData:(NSData *)data imageView:(HippyImageView *)imageView imageURL:(NSString *)url {
    self = [super init];
    if (self) {
        _animatedImageData = data;
        _url = url;
        _imageView = imageView;
    }
    return self;
}

- (void)main {
    if (![self isCancelled] && (_animatedImageData || _imageProvider) && _imageView) {
        HippyAnimatedImage *animatedImage = nil;
        if (_imageProvider) {
            animatedImage = [HippyAnimatedImage animatedImageWithAnimatedImageProvider:_imageProvider];
        } else if (_animatedImageData) {
            animatedImage = [HippyAnimatedImage animatedImageWithGIFData:_animatedImageData];
        }
        if (![self isCancelled] && _imageView) {
            __weak HippyImageView *wIV = _imageView;
            __weak NSString *wURL = _url;
            dispatch_async(dispatch_get_main_queue(), ^{
                HippyImageView *sIV = wIV;
                NSString *sURL = wURL;
                [sIV loadImage:animatedImage.posterImage url:sURL error:nil needBlur:YES needCache:NO];
                sIV.animatedImage = animatedImage;
            });
        }
    }
}

@end

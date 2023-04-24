#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

typedef NS_ENUM(NSUInteger, TDFRasterTarget) {
  /// 渲染到屏幕
  TDFRasterTargetScreen,

  /// 渲染到纹理
  TDFRasterTargetTexture,

   /// 先渲染到纹理，再渲染到屏幕
  TDFRasterTargetTextureToScreen,
};

typedef NS_ENUM(NSUInteger, TDFThreadModel) {
  /// 默认模式
  /// UI线程独立，Raster和Platform共用Driver
  /// 将忽略raster_task_driver的配置
  TDFThreadModelDefault,

  /// 性能优先模式
  /// UI、Raster、Platform将使用指定的Driver运行
  TDFThreadModelPerformancePriority,

  /// 同步优先模式
  /// UI、Raster、Platform将使用相同的Driver运行，即在主线程运行
  TDFThreadModelSynchronicityPriority,
};

/// @brief 图形API接口配置选择
typedef NS_ENUM(NSUInteger, TDFGraphicsAPI) {
  TDFGraphicsAPIMetal,

  TDFGraphicsAPIOpenGLES,

  TDFGraphicsAPISoftware,
};

@interface TDFEngineConfig : NSObject

/// @brief 线程模型
@property(nonatomic, assign) TDFThreadModel threadModel;

/// @brief 光栅化目标
@property(nonatomic, assign) TDFRasterTarget rasterTarget;

/// @brief 使用模拟vsync信号模式
@property(nonatomic, assign) BOOL isSimulateVsync;

/// @brief 自定义vsyncDriver的时间间隔(单位: 纳秒)
@property(nonatomic, assign) int64_t vsyncDriverInterval;

/// @brief 是否接管UI事件的传递 默认为NO
/// YES: TDF将不接收UIEvent事件，由开发者自行传递UIEvent事件
/// NO: TDF将自行接收UIEvent事件
@property(nonatomic, assign) BOOL isTakeOverUIEvent;

/// @brief 帧频率范围
@property(nonatomic, assign) CAFrameRateRange preferredFrameRateRange
  API_AVAILABLE(macos(12.0), ios(15.0), watchos(8.0), tvos(15.0));

/// @brief 默认配置
+ (instancetype)defaultConfiguration;

@end

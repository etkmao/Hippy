//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once
#include <memory>
#include "core/engine/schedule/task_driver.h"
#include "core/platform/android/event/platform_event_handler_android.h"
#include "core/platform/android/jni/jni_df_helper.h"
#include "core/platform/android/jni/jni_java_ref.h"
#include "core/platform/android/platform_android.h"
#include "core/platform/android/pointer_buffer_holder.h"
#include "core/platform/android/view/platform_view_android.h"
#include "core/platform/common/output/texture_output.h"
#include "core/platform/common/output/texture_to_view_output.h"
#include "core/platform/common/output/view_output.h"
#include "core/platform/common/tdf_engine.h"
#include "core/tdfi/shell.h"
#include "core/tdfi/shell_config.h"

namespace tdfcore {

// 映射java层枚举值com/tencent/tdf/TDFEngineConfig$RasterTarget 对应的int value
const static jint kJavaEnumRasterTarget$View = 0;
const static jint kJavaEnumRasterTarget$Texture = 1;
const static jint kJavaEnumRasterTarget$TextureToView = 2;

// 映射java层枚举值com/tencent/tdf/embed/ThreadMode 对应的int value
const static jint kJavaEnumThreadMode$Default = 0;
const static jint kJavaEnumThreadMode$PerformancePriority = 1;
const static jint kJavaEnumThreadMode$SynchronicityPriority = 2;

/**
 * @brief TDFEngine在Android平台C++的实现，主要是通过JNI桥接Java侧方法
 */
class TDFEngineAndroid : public TDFEngine {
 public:
  TDFEngineAndroid(JavaWeakGlobalRef<jobject> jni_weak_ref, JavaWeakGlobalRef<jobject> engine_configuration);

  void Init() override final;

  /**
   * @brief 分发Android平台手势数据
   * @param packet Android平台手势数据buffer holder
   */
  void DispatchEventPacket(std::unique_ptr<PointerBufferHolder> packet);

  /**
   * @brief 更新ViewportMetrics
   * @param viewport_metrics 新的ViewportMetrics
   */
  void UpdateViewportMetrics(const ViewportMetrics &viewport_metrics) override;

  using UpdateGestureInfoClosure = std::function<void(int64_t, bool, const TRect &, uint32_t)>;

  /**
   * 业务侧在当前Holder注册手势信息监听
   */
  void RegisterGestureInfoClosure(const UpdateGestureInfoClosure &closure);

  void RemoveGestureInfoClosure(const UpdateGestureInfoClosure &closure);

  /**
   * @brief 平台侧同步EmbeddedView的手势信息，在Shell广播给对应view_id的View
   */
  void UpdateGestureInfo(int64_t view_id, bool use_custom_hit_test_rect, int32_t left, int32_t top, int32_t right,
                         int32_t bottom, uint32_t gestures);

 protected:
  std::shared_ptr<Platform> MakePlatform() override final;

  std::shared_ptr<PlatformView> MakePlatformView() override final;

  std::shared_ptr<TaskDriver> MakePipelineDriver() override final;

 private:
  TDFEngineConfig ParseEngineConfig(jobject configuration);

  const JavaWeakGlobalRef<jobject> jni_weak_ref_;
};

}  // namespace tdfcore

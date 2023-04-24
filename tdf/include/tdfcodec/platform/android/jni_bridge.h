//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/image_info.h"
#include "core/platform/android/jni/jni_helper.h"
#include "core/platform/android/jni/jni_java_ref.h"
#include "tdfcodec/image_codec.h"

namespace tdfcore {

/**
 * @brief 对应android/graphics/Bitmap类
 */
struct JNIBitmap {
  struct Config {
    std::shared_ptr<JavaGlobalRef<jclass>> java_class;
    jfieldID argb8888;
  };

  struct CompressFormat {
    std::shared_ptr<JavaGlobalRef<jclass>> java_class;
    jfieldID png;
    jfieldID jpeg;
    jfieldID webp;
  };

  std::shared_ptr<JavaGlobalRef<jclass>> java_class;
  jmethodID create_bitmap_from_pixels;
  jmethodID create_bitmap_with_matrix;
  jmethodID compress;
  Config config;
  CompressFormat compress_format;
};

/**
 * @brief 对应android/graphics/BitmapFactory类
 */
struct JNIBitmapFactory {
  struct Options {
    std::shared_ptr<JavaGlobalRef<jclass>> java_class;
    jmethodID constructor;
    jfieldID just_decode_bounds;
    jfieldID preferred_config;
    jfieldID premultiplied;
    jfieldID width;
    jfieldID height;
    jfieldID sample_size;
  };

  std::shared_ptr<JavaGlobalRef<jclass>> java_class;
  jmethodID decode_byte_array;
  Options options;
};

/**
 * @brief 对应java/io/ByteArrayOutputStream类
 */
struct JNIByteArrayOutputStream {
  std::shared_ptr<JavaGlobalRef<jclass>> java_class;
  jmethodID constructor;
  jmethodID to_byte_array;
};

struct JNIByteArrayInputStream {
  std::shared_ptr<JavaGlobalRef<jclass>> java_class;
  jmethodID constructor;
};

struct JNIExifInterface {
  std::shared_ptr<JavaGlobalRef<jclass>> java_class;
  jmethodID constructor_stream;
  jmethodID get_attribute_int;
};

struct JNIMatrix {
  std::shared_ptr<JavaGlobalRef<jclass>> java_class;
  jmethodID constructor;
  jmethodID set_values;
};

/**
 * @brief 初始化图片编解码相关的JNI
 */
void InitJNIForImageCodec(JNIEnv *env);

/**
 * @brief 创建byte[]类型的对象
 */
jbyteArray MakeJNIByteArray(JNIEnv *env, const void *data, jsize size);

/**
 * @brief 创建int[]类型的对象
 */
jintArray MakeJNIIntArray(JNIEnv *env, const jint *data, jsize size);

/**
 * @brief 创建一个解码配置选项，对应android/graphics/BitmapFactory::Options类型
 */
jobject MakeJNIBitmapFactoryDecodeOptions(JNIEnv *env, AlphaType alpha_type, int sample_size);

/**
 * @brief 获取图片尺寸
 */
ISize JustDecodeBoundsJNIBitmap(JNIEnv *env, jbyteArray image_raw_data, jint data_size);

/**
 * @brief 解码图片
 * @param image_raw_data 图片文件数据，对应byte[]类型
 * @param data_size 图片文件数据的长度
 * @param factory_options 解码配置选项，对应android/graphics/BitmapFactory::Options类型
 * @return 解码后的位图对象，对应android/graphics/Bitmap类型
 */
jobject MakeJNIBitmapFromRawData(JNIEnv *env, jbyteArray image_raw_data, jint data_size, jobject factory_options);

/**
 * @brief 通过像素缓冲区，创建一个位图对象，对应android/graphics/Bitmap类型
 * @param pixels 需为ColorType::kBGRA类型的像素缓冲区
 */
jobject MakeJNIBitmapFromPixels(JNIEnv *env, const jint *pixels, jint width, jint height);

/**
 * @brief 编码图片
 * @param bitmap 位图，对应android/graphics/Bitmap类型
 * @param format 图片格式，仅支持PNG、JPEG、WEBP
 * @param quality 图片编码压缩参数，有效范围(0 ~ 100]
 * @return 图片文件数据，对应byte[]类型
 */
jbyteArray CompressJNIBitmap(JNIEnv *env, jobject bitmap, ImageCodecFormat format, jint quality);

/**
 * 通过ExifInterface获取图片的旋转角度
 * @param image_raw_data 图片文件数据，对应byte[]类型
 * @return Exif对应的旋转枚举值
 */
jint GetOrientation(JNIEnv* env, jbyteArray image_raw_data);

/**
 * 根据矩阵转换Bitmap
 * @param env
 * @param bitmap 原始Bitmap
 * @param matrix 变换矩阵
 * @return 转换后的Bitmap
 */
jobject TransformBitmap(JNIEnv* env, jobject bitmap, uint32_t w, uint32_t h, const TM33 &matrix);

}  // namespace tdfcore

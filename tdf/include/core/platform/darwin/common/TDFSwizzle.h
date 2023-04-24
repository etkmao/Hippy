//
//  tdf_convert_utils.h
//  core
//
//

#pragma once

#import <Foundation/Foundation.h>

namespace tdfcore {

/**
 * @brief 替换一个OC类的实例方法的实现
 * @param target_class 目标OC类
 * @param original_sel 原方法
 * @param new_sel 新方法
 * @return 是否成功
 */
BOOL SwizzleInstanceMethodForClass(Class target_class, SEL original_sel, SEL new_sel);

}  // namespace tdfcore

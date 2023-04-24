//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/common/macro.h"
#include "core/common/m44.h"

namespace tdfcore {

/**
 * @brief SkM44Mixer 矩阵混合器（拆分、组合）
 */
class TM44Mixer {
 public:
  /**
   * @brief TM44 矩阵拆分为 translation、scale、rotation 三个分量
   * @param matrix 被拆分的 TM44
   * @param translation 拆分后的 translation 分量输出
   * @param scale 拆分后的 scale 分量输出
   * @param rotation 拆分后的 rotation 分量输出
   */
  static void Decompose(const TM44 &matrix, TV3 &translation, TV3 &scale, TV4 &rotation);

  /**
   * @brief 将 translation、scale、rotation 三个分量组合为 TM44 矩阵，rotation 为三元素
   * @param translation 分量
   * @param scale 分量
   * @param rotation 分量
   * @return 组合后的 TM44 矩阵
 */
  static TM44 Compose(const TV3 &translation, const TV3 &scale, const TV3 &rotation);
};

}  // namespace tdfcore

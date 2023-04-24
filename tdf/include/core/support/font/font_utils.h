//
// Copyright (c) Tencent Corporation. All rights reserved.
//
#pragma once

#include <vector>
#include "core/support/font/types.h"

namespace tdfcore {

int CountTextElements(const void *text, size_t byte_length, TTextEncoding encoding);

std::vector<Unichar> ConvertToUTF32(const void *text, size_t byte_length, TTextEncoding encoding);

}

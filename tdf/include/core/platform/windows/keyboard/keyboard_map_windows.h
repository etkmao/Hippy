//
// Copyright (c) Tencent Corporation. All rights reserved.
//

#pragma once

#include "core/platform/common/keyboard/keyboard_constants.h"
#include <unordered_map>

static const std::unordered_map<int, KeyboardKey> kWindowsKeyMap = {{3, KeyboardKey::kCancel},
                                                          {8, KeyboardKey::kBackspace},
                                                          {9, KeyboardKey::kTab},
                                                          {12, KeyboardKey::kClear},
                                                          {13, KeyboardKey::kEnter},
                                                          {16, KeyboardKey::kShiftLeft},
                                                          {17, KeyboardKey::kControlLeft},
                                                          {19, KeyboardKey::kPause},
                                                          {20, KeyboardKey::kCapsLock},
                                                          {21, KeyboardKey::kLang1},
                                                          {23, KeyboardKey::kJunjaMode},
                                                          {24, KeyboardKey::kFinalMode},
                                                          {25, KeyboardKey::kKanaMode},
                                                          {27, KeyboardKey::kEscape},
                                                          {28, KeyboardKey::kConvert},
                                                          {30, KeyboardKey::kAccept},
                                                          {31, KeyboardKey::kModeChange},
                                                          {32, KeyboardKey::kSpace},
                                                          {33, KeyboardKey::kPageUp},
                                                          {34, KeyboardKey::kPageDown},
                                                          {35, KeyboardKey::kEnd},
                                                          {36, KeyboardKey::kHome},
                                                          {37, KeyboardKey::kArrowLeft},
                                                          {38, KeyboardKey::kArrowUp},
                                                          {39, KeyboardKey::kArrowRight},
                                                          {40, KeyboardKey::kArrowDown},
                                                          {41, KeyboardKey::kSelect},
                                                          {42, KeyboardKey::kPrint},
                                                          {43, KeyboardKey::kExecute},
                                                          {44, KeyboardKey::kPrintScreen},
                                                          {45, KeyboardKey::kInsert},
                                                          {46, KeyboardKey::kDelete},
                                                          {47, KeyboardKey::kHelp},
                                                          {48, KeyboardKey::kDigit0},
                                                          {49, KeyboardKey::kDigit1},
                                                          {50, KeyboardKey::kDigit2},
                                                          {51, KeyboardKey::kDigit3},
                                                          {52, KeyboardKey::kDigit4},
                                                          {53, KeyboardKey::kDigit5},
                                                          {54, KeyboardKey::kDigit6},
                                                          {55, KeyboardKey::kDigit7},
                                                          {56, KeyboardKey::kDigit8},
                                                          {57, KeyboardKey::kDigit9},
                                                          {65, KeyboardKey::kCharA},
                                                          {66, KeyboardKey::kCharB},
                                                          {67, KeyboardKey::kCharC},
                                                          {68, KeyboardKey::kCharD},
                                                          {69, KeyboardKey::kCharE},
                                                          {70, KeyboardKey::kCharF},
                                                          {71, KeyboardKey::kCharG},
                                                          {72, KeyboardKey::kCharH},
                                                          {73, KeyboardKey::kCharI},
                                                          {74, KeyboardKey::kCharJ},
                                                          {75, KeyboardKey::kCharK},
                                                          {76, KeyboardKey::kCharL},
                                                          {77, KeyboardKey::kCharM},
                                                          {78, KeyboardKey::kCharN},
                                                          {79, KeyboardKey::kCharO},
                                                          {80, KeyboardKey::kCharP},
                                                          {81, KeyboardKey::kCharQ},
                                                          {82, KeyboardKey::kCharR},
                                                          {83, KeyboardKey::kCharS},
                                                          {84, KeyboardKey::kCharT},
                                                          {85, KeyboardKey::kCharU},
                                                          {86, KeyboardKey::kCharV},
                                                          {87, KeyboardKey::kCharW},
                                                          {88, KeyboardKey::kCharX},
                                                          {89, KeyboardKey::kCharY},
                                                          {90, KeyboardKey::kCharZ},
                                                          {91, KeyboardKey::kMetaLeft},
                                                          {92, KeyboardKey::kMetaRight},
                                                          {93, KeyboardKey::kContextMenu},
                                                          {95, KeyboardKey::kSleep},
                                                          {96, KeyboardKey::kNumPad0},
                                                          {97, KeyboardKey::kNumPad1},
                                                          {98, KeyboardKey::kNumPad2},
                                                          {99, KeyboardKey::kNumPad3},
                                                          {100, KeyboardKey::kNumPad4},
                                                          {101, KeyboardKey::kNumPad5},
                                                          {102, KeyboardKey::kNumPad6},
                                                          {103, KeyboardKey::kNumPad7},
                                                          {104, KeyboardKey::kNumPad8},
                                                          {105, KeyboardKey::kNumPad9},
                                                          {106, KeyboardKey::kNumPadMultiply},
                                                          {107, KeyboardKey::kNumPadAdd},
                                                          {108, KeyboardKey::kNumPadComma},
                                                          {109, KeyboardKey::kNumPadSubstract},
                                                          {110, KeyboardKey::kNumPadDecimal},
                                                          {111, KeyboardKey::kNumPadDivide},
                                                          {112, KeyboardKey::kF1},
                                                          {113, KeyboardKey::kF2},
                                                          {114, KeyboardKey::kF3},
                                                          {115, KeyboardKey::kF4},
                                                          {116, KeyboardKey::kF5},
                                                          {117, KeyboardKey::kF6},
                                                          {118, KeyboardKey::kF7},
                                                          {119, KeyboardKey::kF8},
                                                          {120, KeyboardKey::kF9},
                                                          {121, KeyboardKey::kF10},
                                                          {122, KeyboardKey::kF11},
                                                          {123, KeyboardKey::kF12},
                                                          {124, KeyboardKey::kF13},
                                                          {125, KeyboardKey::kF14},
                                                          {126, KeyboardKey::kF15},
                                                          {127, KeyboardKey::kF16},
                                                          {128, KeyboardKey::kF17},
                                                          {129, KeyboardKey::kF18},
                                                          {130, KeyboardKey::kF19},
                                                          {131, KeyboardKey::kF20},
                                                          {132, KeyboardKey::kF21},
                                                          {133, KeyboardKey::kF22},
                                                          {134, KeyboardKey::kF23},
                                                          {135, KeyboardKey::kF24},
                                                          {144, KeyboardKey::kNumLock},
                                                          {145, KeyboardKey::kScrollLock},
                                                          {146, KeyboardKey::kNumPadEqual},
                                                          {160, KeyboardKey::kShiftLeft},
                                                          {161, KeyboardKey::kShiftRight},
                                                          {162, KeyboardKey::kControlLeft},
                                                          {163, KeyboardKey::kControlRight},
                                                          {164, KeyboardKey::kAltLeft},
                                                          {165, KeyboardKey::kAltRight},
                                                          {166, KeyboardKey::kBrowserBack},
                                                          {167, KeyboardKey::kBrowserForward},
                                                          {168, KeyboardKey::kBrowserRefresh},
                                                          {169, KeyboardKey::kBrowserStop},
                                                          {170, KeyboardKey::kBrowserSearch},
                                                          {171, KeyboardKey::kBrowserFavorites},
                                                          {172, KeyboardKey::kBrowserHome},
                                                          {173, KeyboardKey::kAudioVolumeMute},
                                                          {174, KeyboardKey::kAudioVolumeDown},
                                                          {175, KeyboardKey::kAudioVolumeUp},
                                                          {178, KeyboardKey::kMediaStop},
                                                          {179, KeyboardKey::kMediaPlayPause},
                                                          {180, KeyboardKey::kLaunchMail},
                                                          {186, KeyboardKey::kSemicolon},
                                                          {187, KeyboardKey::kEqual},
                                                          {188, KeyboardKey::kComma},
                                                          {189, KeyboardKey::kMinus},
                                                          {190, KeyboardKey::kPeriod},
                                                          {191, KeyboardKey::kSlash},
                                                          {192, KeyboardKey::kBackQuote},
                                                          {219, KeyboardKey::kBracketLeft},
                                                          {220, KeyboardKey::kBackslash},
                                                          {221, KeyboardKey::kBracketRight},
                                                          {222, KeyboardKey::kQuote},
                                                          {246, KeyboardKey::kAttn},
                                                          {250, KeyboardKey::kPlay}};
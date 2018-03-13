#ifndef COMMON_H_
#define COMMON_H_

#ifdef _WIN32

#define MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT
#include <mathfu/glsl_mappings.h>
#include <mathfu/constants.h>

#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <array>
#include <type_traits>
#include <utility>

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <cstdio>

#define ERR_GUARD_VULKAN(Expr) do { VkResult res__ = (Expr); if (res__ < 0) assert(0); } while(0)

#endif // #ifdef _WIN32

#endif

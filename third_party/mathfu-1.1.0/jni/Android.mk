# Copyright 2014 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:=$(call my-dir)/..

MATHFU_DIR?=$(LOCAL_PATH)
include $(LOCAL_PATH)/android_config.mk

# Conditionally include libstlport (so include path is added to CFLAGS) if
# it's not being built using the NDK build process.
define add-stlport-includes
$(eval \
  ifeq ($(NDK_PROJECT_PATH),)
  include external/stlport/libstlport.mk
  endif)
endef

# mathfu-cflags: disable_simd force_padding debug
# Expands to the compiler flags for applications or libraries that use MathFu.
# Where disable_simd specifies whether SIMD code should be disabled,
# force_padding specifies whether padding should be added to data structures
# in SIMD mode (-1 = default, 0 = padding off, 1 = padding on).
#
# NOTE: armeabi-v7a-hard has been deprecated.  For more information see,
#  https://android.googlesource.com/platform/ndk/+/master/docs/HardFloatAbi.md
define mathfu-cflags
  $(if $(subst 0,,$(strip $(1))),-DMATHFU_COMPILE_WITHOUT_SIMD_SUPPORT,\
    $(if $(subst -1,,$(strip $(2))),\
               -DMATHFU_COMPILE_FORCE_PADDING=$(strip $(2)),)) \
  $(if $(APP_DEBUG),-DDEBUG=1,-DDEBUG=0) \
  $(if $(filter armeabi-v7a-hard,$(TARGET_ARCH_ABI)),\
             -mfpu=neon -mhard-float -mfloat-abi=hard) \
  $(if $(filter x86,$(TARGET_ARCH_ABI)),-msse) \
  $(if $(filter x86_64,$(TARGET_ARCH_ABI)),-msse4.1)
endef


# Configure common local variables to build mathfu adding $(1) to the end of
# the build target's name.
define mathfu-module
$(eval \
  LOCAL_MODULE:=libmathfu$(1)
  LOCAL_MODULE_TAGS:=optional
  LOCAL_COPY_HEADERS_TO:=mathfu$(1))
endef

# Configure local variables to build mathfu adding $(1) to the end of the
# build target's name, disabling SIMD depending upon the value of $(2) (see
# mathfu-cflags $(1)) and configuring padding (see mathfu-cflags $(2))
# with $(3).
define mathfu-build
$(eval \
  $$(call mathfu-module,$(1))
  LOCAL_SRC_FILES:=
  LOCAL_COPY_HEADERS:=\
    $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/include/mathfu/*.h))
  LOCAL_CFLAGS:=$$(call mathfu-cflags,$(2),$(3))
  LOCAL_EXPORT_CFLAGS:=$$(LOCAL_CFLAGS)
  LOCAL_EXPORT_C_INCLUDES:=\
	$(LOCAL_PATH)/include \
	$(DEPENDENCIES_VECTORIAL_DIR)/include
  LOCAL_EXPORT_LDLIBS:=-lm
  LOCAL_ARM_MODE:=arm
  LOCAL_ARM_NEON:=$(if $(filter \
    armeabi-v7a armeabi-v7a-hard,$(TARGET_ARCH_ABI)),true,)
  $$(call add-stlport-includes))
endef

# --- libmathfu ---
# Target which builds an empty static library so that it's possible for
# projects using this module to add the appropriate flags and includes to
# their compile command line.  This builds mathfu using the default build
# configuration specified in ${mathfu}/android_config.mk
include $(CLEAR_VARS)
$(call mathfu-build,,$(MATHFU_DISABLE_SIMD),$(MATHFU_FORCE_PADDING))
include $(BUILD_STATIC_LIBRARY)

# --- libmathfu_no_simd ---
# Builds an empty static library (similar to libmathfu).
# This build configuration has SIMD disabled.
include $(CLEAR_VARS)
$(call mathfu-build,_no_simd,1,-1)
include $(BUILD_STATIC_LIBRARY)

# --- libmathfu_simd ---
# Builds an empty static library (similar to libmathfu).
# This build configuration has SIMD enabled and padding enabled.
include $(CLEAR_VARS)
$(call mathfu-build,_simd_padding,0,1)
include $(BUILD_STATIC_LIBRARY)

# --- libmathfu_simd_no_padding ---
# Builds an empty static library (similar to libmathfu).
# This build configuration has SIMD enabled and padding disabled.
include $(CLEAR_VARS)
$(call mathfu-build,_simd_no_padding,0,0)
include $(BUILD_STATIC_LIBRARY)

mathfu_cflags:=

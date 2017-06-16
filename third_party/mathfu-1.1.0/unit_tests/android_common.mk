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
include $(CLEAR_VARS)

# Configure the locations of MathFu's dependencies.
MATHFU_DIR:=$(LOCAL_PATH)/../..
MATHFU_DIR_BASENAME:=$(notdir $(abspath $(MATHFU_DIR)))
TEST_DIR:=$(LOCAL_PATH)
include $(MATHFU_DIR)/android_config.mk

namespace:=$(if $(NDK_PROJECT_PATH),,_mathfu)
LOCAL_MODULE:=$(LOCAL_TEST_NAME)${namespace}
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:=$(notdir $(abspath $(TEST_DIR))).cpp
LOCAL_C_INCLUDES:=$(MATHFU_DIR)/unit_tests
LOCAL_LDLIBS:=-llog -landroid
LOCAL_WHOLE_STATIC_LIBRARIES:=\
	libfplutil_main \
	libfplutil_print
# MATHFU_LIB (by default libmathfu) is used to select the build configuration
# for the target using mathfu.
LOCAL_STATIC_LIBRARIES:=\
	android_native_app_glue \
	libgtest \
	$(MATHFU_LIB)
LOCAL_CFLAGS:=-Wall -Werror
LOCAL_ARM_MODE:=arm
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(abspath $(MATHFU_DIR)/..))
$(call import-add-path,$(abspath $(DEPENDENCIES_FPLUTIL_DIR)))
$(call import-add-path,$(abspath $(DEPENDENCIES_GTEST_DIR)/..))

$(call import-module,$(MATHFU_DIR_BASENAME)/jni)
$(call import-module,libfplutil/jni)
$(call import-module,googletest)
$(call import-module,android/native_app_glue)

LOCAL_TEST_NAME:=
MATHFU_DIR:=
TEST_DIR:=

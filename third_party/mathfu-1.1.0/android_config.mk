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

FIND_FPLUTIL_OK_IF_NOT_FOUND:=1
include $(call my-dir)/jni/find_fplutil.mk

ifneq ($(FPLUTIL_DIR),)
  # If fplutil is found, grab the project locations from it.
  include $(FPLUTIL_DIR)/buildutil/android_common.mk

else
  # If fplutil is not found, assume project locations are in 'dependencies'
  # or are set externally with the DEPENDENCIES_ROOT value.
  #
  # If the dependencies directory exists either as a subdirectory or as the
  # container of this project directory, assume the dependencies directory is
  # the root directory for all libraries required by this project.
  $(foreach dep_dir,$(wildcard $(MATHFU_DIR)/dependencies) \
                    $(wildcard $(MATHFU_DIR)/../../dependencies),\
    $(eval DEPENDENCIES_ROOT?=$(dep_dir)))

  ifeq ($(DEPENDENCIES_ROOT),)
    $(error "Cannot find directory with dependent projects.")
  endif

  # Location of the vectorial library.
  DEPENDENCIES_VECTORIAL_DIR?=$(DEPENDENCIES_ROOT)/vectorial
  # Location of the googletest library.
  DEPENDENCIES_GTEST_DIR?=$(DEPENDENCIES_ROOT)/fplutil/libfplutil/jni/libs/googletest
  # Location of the fplutil library.
  DEPENDENCIES_FPLUTIL_DIR?=$(DEPENDENCIES_ROOT)/fplutil
endif

# Whether to disable SIMD.
MATHFU_DISABLE_SIMD?=0
# Whether to force padding of data structures with SIMD enabled.
# -1 = default, 0 = no padding, 1 = padding
MATHFU_FORCE_PADDING?=-1

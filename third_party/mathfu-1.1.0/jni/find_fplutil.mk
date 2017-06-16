# Copyright 2016 Google Inc. All rights reserved.
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

# Find the fplutil directory and set it in `FPLUTIL_DIR`.
#
# We search some standard locations, such as
# (1) the variable $(DEPENDENCIES_ROOT), which can be specified
#     in an environment variable,
# (2) the "dependencies" directory that gets created when cloning from GitHub,
# (3) several levels up in the directory tree.
#
# Notes
# -----
# - fplutil is the project where we keep all our shared code, so the code in
#   this file (which locates fplutil) can unfortunately not be shared.
# - Since this file is duplicated in all FPL projects (except fplutil itself),
#   please copy new versions to all FPL projects whenever you make a change.

FIND_FPLUTIL_DIR:=$(call my-dir)

$(foreach dir,$(wildcard $(DEPENDENCIES_ROOT)) \
              $(wildcard $(FIND_FPLUTIL_DIR)/../dependencies/fplutil) \
              $(wildcard $(FIND_FPLUTIL_DIR)/../../dependencies/fplutil) \
              $(wildcard $(FIND_FPLUTIL_DIR)/../fplutil) \
              $(wildcard $(FIND_FPLUTIL_DIR)/../../fplutil) \
              $(wildcard $(FIND_FPLUTIL_DIR)/../../../fplutil) \
              $(wildcard $(FIND_FPLUTIL_DIR)/../../../../fplutil),\
  $(eval FPLUTIL_DIR?=$(dir)))

ifeq ($(FPLUTIL_DIR),)
  ifndef (FIND_FPLUTIL_OK_IF_NOT_FOUND)
    $(error "Cannot file fplutil project.")
  endif
endif

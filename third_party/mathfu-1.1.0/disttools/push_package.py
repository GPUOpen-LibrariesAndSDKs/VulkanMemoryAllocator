#!/usr/bin/python
# Copyright 2014 Google Inc. All Rights Reserved.
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

"""Push this project and its dependencies to another git repo.

Uses fplutil/disttools/push_package.py to push the remote of
this git repository and its dependencies (defined by config.json)
to another git repository.
"""

import os
import sys
sys.path.extend((
    os.path.realpath(os.path.join(
        os.path.dirname(__file__), os.path.pardir, 'dependencies', 'fplutil')),
    os.path.realpath(os.path.join(
        os.path.dirname(__file__), os.path.pardir, os.path.pardir,
        'fplutil'))))
import disttools.push_package  # pylint: disable=g-import-not-at-top

## The directory containing this file.
THIS_DIR = os.path.realpath(os.path.dirname(__file__))

## Default root directory of the project.
PROJECT_DIR = os.path.realpath(os.path.join(THIS_DIR, os.path.pardir))

## Default package configuration file.
CONFIG_JSON = os.path.realpath(os.path.join(THIS_DIR, 'config.json'))


def main():
  """See fplutil/disttools/push_package.py.

  Returns:
    0 if successful, non-zero otherwise.
  """
  return disttools.push_package.main(disttools.push_package.parse_arguments(
      project_dir=PROJECT_DIR, config_json=CONFIG_JSON))


if __name__ == '__main__':
  sys.exit(main())

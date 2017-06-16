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

"""Generate html documentation from markdown and doxygen comments."""

import os
import sys

THIS_DIR = os.path.realpath(os.path.dirname(__file__))
PROJECT_DIR = os.path.realpath(os.path.join(THIS_DIR, os.pardir))
sys.path.extend(
    [os.path.realpath(os.path.join(PROJECT_DIR, os.pardir, 'fplutil')),
     os.path.realpath(os.path.join(PROJECT_DIR, 'dependencies', 'fplutil'))])
import docs  # pylint: disable=C6204


def main():
  """Generate html documentation from markdown and doxygen comments.

  Returns:
    0 if successful, 1 otherwise.
  """
  sys.argv.extend(('--linklint-dir', THIS_DIR,
                   '--source-dir', os.path.join(THIS_DIR, 'src'),
                   '--project-dir', PROJECT_DIR))
  return docs.generate_docs.main()

if __name__ == '__main__':
  sys.exit(main())

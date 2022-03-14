# coding:utf-8
#!/usr/bin/python
#
# Copyright (c) 2021-present Sparky Studios. All rights reserved.
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
#
"""
Clean generated Amplitude binary assets.
"""

import common
import os
import sys


def main(argv):
    """Cleans the generated Amplitude binary assets.

    Returns:
      Returns 0 on success.
    """

    projectPath = common.get_o3de_project_path()

    try:
        common.clean_flatbuffer_binaries(
            common.get_conversion_data(os.path.join(
                projectPath, "sounds", "amplitude_project"))
        )
        print("Amplitude binary assets cleaned successfully.")
    except common.BuildError as error:
        common.handle_build_error(error)
        return 1

    return 0


if __name__ == '__main__':
    main(sys.argv)

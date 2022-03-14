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
Boostraps O3DE editor access to the python scripts within the Sparky Studio's Amplitude Gem
"""
# ------------------------------------------------------------------------
# standard imports
import os
import inspect
import site
from pathlib import Path
# ------------------------------------------------------------------------
_MODULENAME = 'Gems.SSAmplitudeAudio.bootstrap'

# script directory
_MODULE_PATH = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
_MODULE_PATH = Path(_MODULE_PATH)
site.addsitedir(_MODULE_PATH.resolve())
# ------------------------------------------------------------------------
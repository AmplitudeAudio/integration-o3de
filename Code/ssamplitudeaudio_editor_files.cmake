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

set(FILES
    Source/Editor/AmplitudeEditor.qrc

    Source/Config.h

    Source/AmplitudeAudioEditorSystemComponent.cpp
    Source/AmplitudeAudioEditorSystemComponent.h

    Source/Editor/AmplitudeAudioLoader.cpp
    Source/Editor/AmplitudeAudioLoader.h
    Source/Editor/AmplitudeAudioSystemControl.cpp
    Source/Editor/AmplitudeAudioSystemControl.h
    Source/Editor/AmplitudeAudioSystemEditor.cpp
    Source/Editor/AmplitudeAudioSystemEditor.h

    Source/Builder/AmplitudeAudioControlBuilderComponent.cpp
    Source/Builder/AmplitudeAudioControlBuilderComponent.h
    Source/Builder/AmplitudeAudioControlBuilderWorker.cpp
    Source/Builder/AmplitudeAudioControlBuilderWorker.h
)

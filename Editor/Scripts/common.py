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

import glob
import os
import shutil
import subprocess
import sys
from pathlib import Path

# The Amplitude SDK schemas directory.
SCHEMA_PATHS = [
    os.path.join(os.environ['SS_AMPLITUDE_ROOT_PATH'], "sdk", "schemas")
]

# Name of the flatbuffer executable.
FLATC = shutil.which("flatc")

# Directory where unprocessed sound flatbuffer data can be found.
SOUNDS_DIR_NAME = 'sounds'

# Directory where unprocessed collection flatbuffer data can be found.
COLLECTIONS_DIR_NAME = 'collections'

# Directory where unprocessed sound bank flatbuffer data can be found.
SOUNDBANKS_DIR_NAME = 'soundbanks'

# Directory where unprocessed event flatbuffer data can be found.
EVENTS_DIR_NAME = 'events'

# Directory where unprocessed attenuation flatbuffer data can be found.
ATTENUATORS_DIR_NAME = 'attenuators'

# Directory where unprocessed switch flatbuffer data can be found.
SWITCHES_DIR_NAME = 'switches'

# Directory where unprocessed switch containers flatbuffer data can be found.
SWITCH_CONTAINERS_DIR_NAME = 'switch_containers'

# Directory where unprocessed rtpc flatbuffer data can be found.
RTPC_DIR_NAME = 'rtpc'

# Directory where unprocessed effect flatbuffer data can be found.
EFFECTS_DIR_NAME = 'effects'

# Directory where unprocessed environment flatbuffer data can be found.
ENVIRONMENTS_DIR_NAME = 'environments'


class FlatbuffersConversionData(object):
    """Holds data needed to convert a set of json files to flatbuffer binaries.

    Attributes:
      schema: The path to the flatbuffer schema file.
      input_files: A list of input files to convert.
    """

    def __init__(self, schema, input_files):
        """Initializes this object's schema and input_files."""
        self.schema = schema
        self.input_files = input_files


class BuildError(Exception):
    """Error indicating there was a problem building assets."""

    def __init__(self, argv, error_code, message=None):
        Exception.__init__(self)
        self.argv = argv
        self.error_code = error_code
        self.message = message if message else ""


def run_subprocess(argv):
    try:
        process = subprocess.Popen(argv, shell=True)
    except OSError as e:
        sys.stdout.write("Cannot find executable?")
        raise BuildError(argv, 1, message=str(e))
    process.wait()
    if process.returncode:
        sys.stdout.write("Process has exited with 1")
        raise BuildError(argv, process.returncode)


def convert_json_to_flatbuffer_binary(flatc, json, schema, out_dir):
    """Run the flatbuffer compiler on the given json file and schema.

    Args:
      flatc: Path to the flatc binary.
      json: The path to the json file to convert to a flatbuffer binary.
      schema: The path to the schema to use in the conversion process.
      out_dir: The directory to write the flatbuffer binary.

    Raises:
      BuildError: Process return code was nonzero.
    """
    command = [flatc, "-o", out_dir]
    for path in SCHEMA_PATHS:
        command.extend(["-I", path])
    command.extend(["-b", schema, json])
    run_subprocess(command)


def needs_rebuild(source, target):
    """Checks if the source file needs to be rebuilt.

    Args:
      source: The source file to be compared.
      target: The target file which we may need to rebuild.

    Returns:
      True if the source file is newer than the target, or if the target file
      does not exist.
    """
    return not os.path.isfile(target) or (
        os.path.getmtime(source) > os.path.getmtime(target)
    )


def processed_json_path(path):
    """Take the path to a raw json asset and convert it to target bin path."""
    return path.replace(
        ".json",
        ".amconfig" if path.endswith("audio_config.json")
        else ".ambus" if path.endswith("buses.json")
        else ".ambank" if SOUNDBANKS_DIR_NAME in path
        else ".amcollection" if COLLECTIONS_DIR_NAME in path
        else ".amevent" if EVENTS_DIR_NAME in path
        else ".amattenuation" if ATTENUATORS_DIR_NAME in path
        else ".amswitch" if SWITCHES_DIR_NAME in path
        else ".amswitchcontainer" if SWITCH_CONTAINERS_DIR_NAME in path
        else ".amrtpc" if RTPC_DIR_NAME in path
        else ".amsound" if SOUNDS_DIR_NAME in path
		else ".amenv" if ENVIRONMENTS_DIR_NAME in path
        else ".ambin",
    ).replace("amplitude_project", 'amplitude_assets')


def processed_json_filename(path):
    """Take the path to a raw json asset and return the filename of the binary asset."""
    return os.path.basename(processed_json_path(path))


def generate_flatbuffer_binaries(flatc, conversion_data):
    """Run the flatbuffer compiler on the all of the flatbuffer json files.

    Args:
      flatc: Path to the flatc binary.
      target_directory: Path to the target assets directory.
    """
    for element in conversion_data:
        schema = element.schema
        for json in element.input_files:
            target = processed_json_path(json)
            target_file_dir = os.path.dirname(target)
            if not os.path.exists(target_file_dir):
                os.makedirs(target_file_dir)
            if needs_rebuild(json, target) or needs_rebuild(schema, target):
                convert_json_to_flatbuffer_binary(
                    flatc, json, schema, target_file_dir)


def find_in_paths(name, paths):
    """Searches for a file with named `name` in the given paths and returns it."""
    for path in paths:
        full_path = os.path.join(path, name)
        if os.path.isfile(full_path):
            return full_path
    # If not found, just assume it's in the PATH.
    return name


def clean_flatbuffer_binaries(conversion_data):
    """Delete all the processed flatbuffer binaries.

    Args:
      target_directory: Path to the target assets directory.
      conversion_data: List of files to be converted.
    """
    for element in conversion_data:
        for json in element.input_files:
            path = processed_json_path(json)
            if os.path.isfile(path):
                os.remove(path)


def handle_build_error(error):
    """Prints an error message to stderr for BuildErrors."""
    sys.stderr.write(
        "Error running command `%s`. Returned %s.\n%s\n"
        % (" ".join(error.argv), str(error.error_code), str(error.message))
    )


def get_o3de_project_path():
    """figures out the o3de project path if not found defaults to the engine folder"""
    _PATH_O3DE_PROJECT = None
    try:
        import azlmbr  # this file will fail outside of O3DE
    except ImportError as e:
        sys.stderr.write("Cannot run this script outside an O3DE editor.")
    else:
        # execute if no exception, this would indicate we are in O3DE land
        # allow for external ENVAR override
        _PATH_O3DE_PROJECT = Path(
            os.getenv('PATH_O3DE_PROJECT', azlmbr.paths.projectroot))
    finally:
        # if None, fallback to engine folder
        if not _PATH_O3DE_PROJECT:
            sys.stderr.write("Unable to detect the O3DE project root path.")

    return _PATH_O3DE_PROJECT


def get_conversion_data(projectPath):
    # A list of json files and their schemas that will be converted to binary files
    # by the flatbuffer compiler.
    return [
        FlatbuffersConversionData(
            schema=find_in_paths(
                'engine_config_definition.fbs', SCHEMA_PATHS),
            input_files=[os.path.join(projectPath, 'audio_config.json')]),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'buses_definition.fbs', SCHEMA_PATHS),
            input_files=[os.path.join(projectPath, 'buses.json')]),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'sound_bank_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, SOUNDBANKS_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'collection_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, COLLECTIONS_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'sound_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, SOUNDS_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'event_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, EVENTS_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'attenuation_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, ATTENUATORS_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'switch_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, SWITCHES_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'switch_container_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, SWITCH_CONTAINERS_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'rtpc_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, RTPC_DIR_NAME, '**/*.json'), recursive=True)),
        FlatbuffersConversionData(
            schema=find_in_paths(
                'effect_definition.fbs', SCHEMA_PATHS),
            input_files=glob.glob(os.path.join(projectPath, EFFECTS_DIR_NAME, '**/*.json'), recursive=True)),
    ]

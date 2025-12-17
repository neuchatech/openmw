# OpenMW Windows Installation and Setup Guide

This guide details the process of compiling OpenMW from source on Windows 11 and setting up Morrowind game files for play.

## Prerequisites

Ensure the following tools are installed and available in your system PATH:

1.  **Visual Studio 2019 or 2022**: Required for the MSVC compiler (C++ desktop development workload).
2.  **CMake**: Build system generator.
3.  **Ninja**: Fast build system (recommended).
4.  **Git**: Version control.
5.  **Python 3**: Scripting support.
6.  **7-Zip**: Archive extraction.

## Compilation

The easiest way to build OpenMW on Windows is using the provided CI script.

1.  Open a **Git Bash** terminal (or any bash-compatible shell).
2.  Navigate to the OpenMW source directory:
    ```bash
    cd /d/data/code/openmw
    ```
3.  Run the MSVC build script:
    ```bash
    ./CI/before_script.msvc.sh -g -D -p
    ```
    *   `-g`: Generates the Visual Studio solution.
    *   `-D`: Configures a Debug build (useful for development).
    *   `-p`: Automates dependency handling.
    *   *Note: Using `-D` creates a debug build. For a release build (faster performance), omit `-D`.*

4.  Alternatively, for a Ninja build (faster compilation):
    ```bash
    ./CI/before_script.msvc.sh -n -r -p
    ```
    *   `-n`: Uses Ninja build system.
    *   `-r`: Configures a Release build.

## Game Files Setup

To play Morrowind with OpenMW, you must have the original game files installed locally.

1.  **Install Morrowind**: Install the game via Steam, GOG, or from original installation media.
2.  **Locate Data Files**: Ensure you know where the `Data Files` folder is (e.g., `C:\Program Files (x86)\Steam\steamapps\common\Morrowind\Data Files`).
3.  **Run the Wizard**: 
    *   After compilation, locate and run `openmw-wizard.exe`.
    *   The wizard will attempt to auto-detect your Morrowind installation.
    *   If auto-detection fails, browse to your Morrowind installation directory when prompted.
    *   The wizard will import your settings and generate the `openmw.cfg` file.

## Playing the Game

1.  Run `openmw-launcher.exe`.
2.  Manage your data files and graphics settings if desired.
3.  Click **Play**.

## Advanced Configuration

### Increasing Render Distance

The default maximum render distance in the in-game settings is limited (approx 7168 or 8000). To increase this beyond the limit:

1.  Navigate to your user configuration folder: `Documents\My Games\OpenMW` (or `~/.config/openmw` on Linux).
2.  Open or create `settings.cfg`.
3.  Add (or modify) the `[Camera]` section:
    ```ini
    [Camera]
    # Set to your desired distance (e.g., 20000.0)
    viewing distance = 20000.0
    ```
4.  Save the file and restart the game. The slider in-game might look maxed out or wrong, but the engine will use your value.

## Troubleshooting

*   **Missing Dependencies**: If the build script fails due to missing libraries, ensure the `-p` flag was used to fetch dependencies, and check that CMake can find them.
*   **Game Not Found**: If the wizard cannot find the game, ensure you have the `Morrowind.esm` and `Morrowind.bsa` files in the directory you are selecting.

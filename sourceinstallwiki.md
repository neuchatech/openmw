This page will describe how to set a dev env for several OSes.
The android build is described on a [[Android build|dedicated page]].

=Third-Party Libraries and Tools=

* OpenMW currently uses the following libraries:
** [http://www.openscenegraph.org/ OpenSceneGraph 3.6.5] - OpenGL rendering toolkit
** [http://www.libsdl.org/ SDL2] - input and windowing
** [http://mygui.info/ MyGUI 3.4.3] - GUI toolkit
** [http://www.bulletphysics.com/ Bullet ≥ 2.86] - the physics engine
** [http://connect.creativelabs.com/openal/default.aspx OpenAL] or [http://kcat.strangesoft.net/openal.html#download OpenAL soft] - for sound playback
** [http://www.ffmpeg.org/ FFmpeg] For sound decoding and BINK video playback
** [https://github.com/lz4/lz4 LZ4] Extremely fast de-compression algorithm
** [https://www.sqlite.org/index.html SQLite] - Persistent storage, cache
** [https://luajit.org/ LuaJIT] - Sandboxed scripting using Sol

* OpenMW additionally uses the following tools:
** [http://www.cmake.org/ CMake] - used to manage the build process
** [http://git-scm.com/ Git] - used as the source control management system

For those unfamiliar with Git, the Sphere RPG Engine group has a fairly useful short wiki page entitled [http://wiki.spheredev.org/index.php/Git_for_the_lazy "Git for the lazy"]. Make sure to also check out the [https://about.gitlab.com/handbook/questions/ GitLab FAQ].

== General notes on dependencies ==


=== MyGUI ===

By default, MyGUI will build with the Ogre3D platform enabled, but Ogre3D is not a hard dependency. If you do not want to have Ogre installed, then configure MyGUI with the dummy platform, and disable tools & demos:

<syntaxhighlight lang="cmake">
cmake .. -DMYGUI_RENDERSYSTEM=1 -DMYGUI_BUILD_DEMOS=OFF -DMYGUI_BUILD_TOOLS=OFF -DMYGUI_BUILD_PLUGINS=OFF -DMYGUI_DONT_USE_OBSOLETE=ON
</syntaxhighlight>

=== OpenSceneGraph ===

It is recommended to use the [https://github.com/OpenMW/osg OpenMW/osg] fork of OpenSceneGraph. OpenMW still works with "vanilla" OpenSceneGraph, but may run at slightly lower frame rate.

==== OSG plugins ====

OSG comes with numerous plugins, most of which are not required by OpenMW. If you install OSG from a package manager, then you will have tons of bloat on your system that OpenMW does not need (another reason to build your own).

Only the following plugins are required for OpenMW to function properly:

Texture formats:
* osgdb_dds
* osgdb_tga
* osgdb_bmp
* osgdb_jpeg
* osgdb_png

TrueType fonts support (used for profiler):

* osgdb_freetype

[[Native_Mesh_Format|.osg format]] support:
* osgdb_osg
* osgdb_serializers_osg

.dae (Collada) format support: 

* osgdb_dae

Notes: 
- the jpeg/png plugins will not be built unless you have 'libpng' and 'libjpeg' headers installed on your system.
- the Collada (dae) plugin requires libcollada library/headers installed on your system.


If you are creating a release package, the library files for the plugins need to go in ''<library path>/osgPlugins-<OSG_VERSION>''.

When using the OSG git master (to be 3.6), or the OpenMW OSG fork, you can use the following cmake arguments to avoid unneeded plugins from being built:

 -DBUILD_OSG_PLUGINS_BY_DEFAULT=0 -DBUILD_OSG_PLUGIN_OSG=1 -DBUILD_OSG_PLUGIN_DAE=1 -DBUILD_OSG_PLUGIN_DDS=1 -DBUILD_OSG_PLUGIN_TGA=1 -DBUILD_OSG_PLUGIN_BMP=1 -DBUILD_OSG_PLUGIN_JPEG=1 -DBUILD_OSG_PLUGIN_PNG=1 -DBUILD_OSG_PLUGIN_FREETYPE=1 -DBUILD_OSG_DEPRECATED_SERIALIZERS=0

=== S3TC texture compression support ===

OpenMW requires S3TC texture compression support. If you are using a proprietary video driver, you will usually have S3TC support out of the box. With open-source drivers on Linux, you may need to install S3TC support separately. An indicator for missing S3TC support are completely white or pink textures.

On Ubuntu:
<pre>
sudo apt-get install libtxc-dxtn-s2tc0
</pre>

=Windows=

== MSVC 2019-2022 ==

While it's theoretically possible to do everything manually, very, very few people have succeeded, usually due to nonsensical linker errors. Instead, it's recommended that you use our prebuild script. It's what our team members do.

Make sure you're aware of different [[Development_Environment_Setup#Build_Configurations|Build Configurations]] before being surprised that a Debug build is slower than an official release.

OpenMW currently supports MSVC via three build systems, namely Visual Studio Solutions, NMake Makefiles, and Ninja build files.

* Solution files have the best integration into Visual Studio.
* NMake doesn't support parallel builds, so are pointlessly slow if you've got more than one core, and have no particular other benefits. It doesn't support multiple build configurations, so uses much more disk space if you use several.
* Ninja is ''theoretically'' faster than MSBuild/DevEnv (which build solution files) but has less reliable integration with Visual Studio. It only supports multiple build configurations with CMake 3.17 and later.

The tools to build VS Solutions and NMake Makefiles are provided as part of both Visual Studio and the MSVC command-line build tools, but you'll need to install Ninja separately (<code>choco install ninja</code> if you have Chocolatey).

=== What you need ===

* Visual Studio 2019 or 2022 (Community edition is enough) ''or'' the command-line build tools for Visual Studio (<code>choco install visualstudio2022buildtools</code> or <code>choco install visualstudio2019buildtools</code> if you use Chocolatey).
**  Make sure you've got the C++ toolset installed, which isn't necessarily selected by default.
*** This can be chosen during or after installation from the Visual Studio Installer, where it's called ''Desktop development with C++'' in the workloads tab. If you do add this to an existing installation, the Visual Studio Installer is available in the Start menu, and you'll want to choose to modify the existing installation.
* Ninja if you're using Ninja (<code>choco install ninja</code> if you have Chocolatey).
* VSWhere '''in your path''' if you're using Ninja or NMake.
** <code>choco install vswhere</code> works if you have Chocolatey, but make sure you reload your shell or otherwise refresh your environment variables afterwards.
** It also comes with Visual Studio and the command-line build tools, but isn't on the path by default, and isn't in a reliable location. Feel free to add that version to the path for the shell session you use instead of installing it again.
* Git for Windows (<code>choco install git.install</code> if you have Chocolatey).
** Other Git implementations will be sufficient to clone the OpenMW repo, but Git for Windows also provides the Bash shell used to run the script. You ''might'' be able to use a full MSYS2 installation, as it should be the same, just configured slightly differently.
*** Note that WSL does not work for this as the script needs to execute windows executables.
*** Note that a full Cygwin install does not work for this as it tries to emulate Unix file permissions, and the script was not written with this in mind.
* 7-zip '''in your path'''
** <code>choco install 7zip.install</code> works if you have Chocolatey, but make sure you reload your shell or otherwise refresh your environment variables afterwards.
** If you don't use Chocolatey, install 7zip like any other application, then use the instructions in [https://www.howtogeek.com/118594/how-to-edit-your-system-path-for-easy-command-line-access/ this guide] to add it to your path.
* CMake '''in your path'''
** <code>choco install cmake.install --installargs 'ADD_CMAKE_TO_PATH=User'</code> for Chocolatey, but make sure you reload your shell or otherwise refresh your environment variables afterwards. If you'd rather CMake was in the path for all users instead of just the current one, swap <code>ADD_CMAKE_TO_PATH=User</code> for <code>ADD_CMAKE_TO_PATH=System</code>.
** If you don't use Chocolatey, install CMake like any other application, making sure to choose ''Add CMake to the system PATH for all users'' or ''Add CMake to the system PATH for the current user'' when offered.
* Python '''in your path'''
** <code>choco install python</code> works if you have Chocolatey, but make sure you reload your shell or otherwise refresh your environment variables afterwards.
** If you don't use Chocolatey, you can install Python like any other application, making sure to check the ''Add Python <version> to PATH'' box in the installer.
** Python from the Windows store is probably fine too, but no one's ever tested it.
** If your Windows install is up-to-date, you might find you've already got <code>python.exe</code> in your path without manually installing it. This isn't an actual Python interpreter, and just opens the Windows Store page for Python if you run it, or crashes with a permissions error. Fake Python is not sufficient to run the prebuild script.
* You shouldn't be using Windows 7 any more anyway, but there are potential issues with its default version of PowerShell. You won't get helped if this causes problems.
* ~10GB of free space per build platform (32/64-bit, different build tools etc.) for all the dependencies and build artefacts.
* Time. A slowish machine will take about an hour, but obviously a faster one will take much less. If you're not using NMake, this should scale well with the number of cores available.

=== What you do ===

* Decide if you're using <span style="color:purple">Solution files</span>, <span style="color:green">Ninja</span>, or <span style="color:maroon">NMake</span>. Solution files are recommended if you're unsure.
* Pick a directory to do this in.
** Its path '''cannot contain spaces'''. (We can't change this until Qt can be installed with a path containing spaces.)
** Its path should either be short (e.g. <code>C:\OpenMW-dev\</code>) or you should enable long file paths in your registry (for example by following [https://www.howtogeek.com/266621/how-to-make-windows-10-accept-file-paths-over-260-characters/ this guide]).
* Clone the OpenMW repo. <code>git clone https://gitlab.com/OpenMW/openmw/</code> (or your own fork of it).
* Open a Git Bash shell in the repo.
** Remember that other Bash implementations such as WSL or Cygwin won't work.
* Run the following command: <code>CI/before_script.msvc.sh -k -p Win64 -v 2022</code>.
** <span style="color:green">If you're using Ninja, add <code> -N</code> to the command.</span>
** <span style="color:maroon">If you're using NMake, add <code> -n</code> to the command.</span>
** Substitute <code>Win64</code> with <code>Win32</code> if doing a 32-bit build. This is no longer supported, but you might be able to bodge it if you build deps yourself.
** <code>2022</code> can also be substituted with <code>2019</code> for MSVC/VS 2019
** '''For <span style="color:purple">Solution files</span> and <span style="color:green">Ninja with recent versions of CMake</span> this will set up and install runtime resources for all build configurations, so all will be buildable and will run. <span style="color:maroon">For NMake <span style="color:green">(or Ninja if your CMake version is below 3.17)</span> it will only set up the Debug configuration.</span> The differences between the configurations are discussed [[Development_Environment_Setup#Build_Configurations|here]]'''
*** To set up Release, add <code> -c Release</code> to the command.
*** To set up RelWithDebInfo, add <code> -c RelWithDebInfo</code> to the command.
*** If we ever change the default, adding <code> -c Debug</code> will set up the Debug configuration.
*** <span style="color:maroon">For NMake <span style="color:green">(or Ninja if your CMake version is below 3.17)</span> you can still set up multiple configurations at once by specifying several on the command line, but this will use significantly more disk space.</span>
** If you want debug symbols for dependency libraries (e.g. if you want to investigate a bug in one of them), add <code> -P</code>. Alternatively, you can tell your debugger to use <code>https://openmw-sym.rgw.ctrl-c.liu.se</code> as a symbol server, and they'll be fetched on demand.
** If you want to build the unit tests, add <code> -t</code>.
** Other, more niche, options exist, and are documented in the script's help text, available by running <code>CI/before_script.msvc.sh -h</code>

* Wait for the build environment to be set up.

* <span style="color:purple">For Solution files:</span>
** Go to the build folder that was just created (<code>MSVC2022_64</code> if you've used VS 2022 and specified 64-bit) and open <code>OpenMW.sln</code>.
** Make sure the Configuration dropdown at the top of Visual Studio is set to the one you want to use.
** If you want to use Visual Studio's debugger, make sure you've set a startup project in the Solution Explorer by right clicking one and choosing Set as Startup Project. (The default startup project is <code>ALL_BUILD</code> which doesn't have an executable as it's a meta project used to build all other OpenMW projects at once, so cannot be started.)
** Press F5 to build the current startup project and launch it with the debugger, or Ctrl+Shift+B to build all projects.

* <span style="color:green">For Ninja:</span>
** <code>cd</code> into the build folder that was just created (<code>MSVC2022_64_Ninja</code> if you've used MSVC 2022, specified 64-bit and are using a recent CMake version) either in the Bash shell you just used, or another of your choosing.
** Activate the correct MSVC environment in the current shell. Helper scripts are provided to make it easy to match the build you just configured:
*** For Bash, use <code>source activate_msvc.sh</code>.
*** For PowerShell, use <code>.\ActivateMSVC.ps1</code>.
*** For CMD, use <code>ActivateMSVC.bat</code>.
** Run <code>ninja</code> to build all targets, or add target names to build specific targets, e.g. <code>ninja openmw-cs</code>.
** With CMake older than 3.17.0, different build configurations need to be built by calling <code>ninja</code> in different folders. With newer CMake versions, they're all available in the same folder and you can build a specific configuration by adding <code> -f build-<configuration>.ninja</code> to the command, e.g. <code>ninja -f build-RelWithDebInfo.ninja openmw-cs</code> to build just the CS in the RelWithDebInfo configuration.

* <span style="color:maroon">For NMake:</span>
** <code>cd</code> into the build folder that was just created (<code>MSVC2022_64_NMake_Debug</code> if you've used MSVC 2022, specified 64-bit either specified the Debug configuration or left it with the default) either in the Bash shell you just used, or another of your choosing.
** Activate the correct MSVC environment in the current shell. Helper scripts are provided to make it easy to match the build you just configured:
*** For Bash, use <code>source activate_msvc.sh</code>.
*** For PowerShell, use <code>.\ActivateMSVC.ps1</code>.
*** For CMD, use <code>ActivateMSVC.bat</code>.
** Run <code>nmake</code> to build all targets, or add target names to build specific targets, e.g. <code>nmake openmw-cs</code>.

=== If this doesn't work ===
First, double check that you've followed the instructions properly. Some common problems are in bold. If you still have issues, make sure to report them somewhere official, such as the OpenMW forums, as otherwise we won't know anything needs fixing.

=== Updating dependencies ===
Once in a while, the versions of dependencies listed in the script will be updated. For some of these, they can be updated just by running the script again. However, for some others, the process has not been automated yet. The rough steps to do so manually are as follows:

* If you don't have any particular reason to keep the existing build directory (the <code>MSVC<year><_64?><build system><configuration></code> directory), simply delete it and use the script to regenerate it. You will get a new build environment using updated dependencies.
* If you need to keep the build directory, the process is more involved:
** Delete the directory the dependency was extracted to (<code>build directory/deps/<dependency name></code>)
** Remove any lines mentioning the dependency from <code>build directory/CMakeCache.txt</code>
** Run the script again.

=== Build Configurations ===
* Debug builds are slow, but the easiest to use with a debugger.
* Release builds are fast, but cannot be effectively used with a debugger.
* RelWithDebInfo builds are fast, and can be used with a debugger, but some lines may be executed in a different order to how they appear in the source code, and some may be skipped altogether, making debugging more difficult.

None of these are quite the same as official release builds - they have a different setting called [https://www.google.com/search?q=Link%20Time%20Optimization Link Time Optimization (LTO)/Interprocedural Optimisation] turned on. As it's a time-costly option, automatic builds and the instructions above do not have LTO enabled. It may affect performance, but we have no credible reports of the effect being significant.

Enabling LTO can be done by ticking a checkbox in the CMake GUI, or passing <code>-DOPENMW_LTO_BUILD=1</code> on the command line when configuring with CMake.

''NOTE'': This setting only affects Release builds - it would hurt debuggability for RelWithDebInfo and Debug builds, undermining their whole purpose, so we keep it off for them.

To enable LTO for an existing build:
* Open the CMake GUI. This should show up if you type ''cmake gui'' into the Start menu.
* Set the build directory to the one with your OpenMW solution file, Ninja build files, or NMake makefiles. It should pick up all the settings the build script set.
* Tick the ''OPENMW_LTO_BUILD'' option (it might be fastest to type LTO into the filter bar).
* Press Generate.
* If using Visual Studio, it will then ask to reload the solution.
* Build again.

=Linux=

==Installing Dependencies==

===Arch===
There is a development package available for OpenMW on the AUR.
It can be found at [https://aur.archlinux.org/packages/openmw-git/].

Information about building packages from the Arch User Repository (AUR) can be found at the [http://wiki.archlinux.org/index.php/Arch_User_Repository Arch wiki].

===Debian and Ubuntu===

====Key requirements====

Because some dependencies of OpenMW are not in the Ubuntu repositories we made a development repository on Launchpad which provides packages for the missing dependencies.

To be able to install these packages you need to add the openmw Launchpad PPA by running:
<syntaxhighlight lang="bash">
sudo add-apt-repository ppa:openmw/openmw
</syntaxhighlight>

and to make sure that the ppa will be used run:
<syntaxhighlight lang="bash">
sudo apt-get update
</syntaxhighlight>

The stable PPA might not provide the packages or versions of the packages required for development builds for backward compatibility purposes.
If you found this to be the case, try adding the daily build PPA:
<syntaxhighlight lang="bash">
sudo add-apt-repository ppa:openmw/openmw-daily
</syntaxhighlight>

====Packages====
You will need g++ in order to compile OpenMW.

To setup a build environment and install all dependencies type:
<syntaxhighlight lang="bash">

 sudo apt-get install git build-essential cmake \
 libopenal-dev libopenscenegraph-dev libbullet-dev libsdl2-dev \
 libmygui-dev libunshield-dev liblz4-dev libtinyxml-dev libqt6opengl6-dev \
 libqt6svg6-dev libboost-filesystem-dev libboost-program-options-dev \
 libboost-iostreams-dev libavcodec-dev libavformat-dev libavutil-dev \
 libswscale-dev libswresample-dev librecast-dev libluajit-5.1-dev \
 libsqlite3-dev libyaml-cpp-dev libcollada-dom-dev \
 qt6-l10n-tools qt6-tools-dev qt6-tools-dev-tools


</syntaxhighlight>

<!-- TODO UPDATE
===Gentoo===

All required dependencies, as well as an ebuild for OpenMW (<tt>games-engine/openmw</tt>) 
are available in the Portage tree. However to setup a development environment without 
emerging the OpenMW package, follow the steps below.
Please note that this is a setup that worked, in other words: some use flags may be optional.

<syntaxhighlight lang="bash">
# make sure libsdl is compiled with the X and video USE flags
echo "media-libs/libsdl2 X video" >> /etc/portage/package.use
# make sure that the USE flags just set are applied if they need to
emerge --ask --verbose --changed-use @world

# emerge dependencies, but don't pollute the world file which means that the 
# dependencies will not be updated (it's usually not needed)
emerge -av --oneshot dev-vcs/git ">=dev-games/mygui-3.2.2" \
">=dev-libs/boost-1.56.0-r1" dev-libs/tinyxml ">=dev-qt/qtcore-4.8.6-r2:4" \
">=dev-qt/qtgui-4.8.6-r4:4" media-libs/freetype:2 media-libs/libsdl2 \
media-libs/openal ">=sci-physics/bullet-2.80" virtual/ffmpeg \
dev-qt/qtxmlpatterns:4 app-arch/unshield virtual/pkgconfig 
</syntaxhighlight>

Starting with OpenMW-0.37 (including current git builds) openscenegraph is required. 
<syntaxhighlight  lang="bash">
# set use flags needed to build OSG
echo "dev-games/openscenegraph ffmpeg jpeg png qt4 sdl svg truetype zlib" >> /etc/portage/package.use
# build OSG
emerge -av1 ">=dev-games/openscenegraph-3.2.1"
</syntaxhighlight>

It should be possible now to build OpenMW using cmake or other tools.

In order to be able to build the documentation, execute
<syntaxhighlight lang="bash">
emerge -av1 app-doc/doxygen media-gfx/graphviz
</syntaxhighlight>

-->

===Fedora Workstation===

====Key requirements====

Because some dependencies of OpenMW are not in the Fedora Workstation repositories, you need to [http://rpmfusion.org/Configuration/ enable RPMFusion] free and non-free repositories which provide packages for the missing dependencies.

====Packages====
You will need g++ in order to compile OpenMW.

To setup a build environment and install the dependencies type:
<syntaxhighlight lang="bash">

sudo dnf group install development-tools
sudo dnf install openal-devel OpenSceneGraph-devel OpenSceneGraph-Collada SDL2-devel \
qt6-qtbase-devel qt6-qtsvg-devel qt6-linguist qt6-qttools-devel boost-devel boost-filesystem \
boost-thread boost-program-options boost-system boost-iostreams ffmpeg-devel ffmpeg-libs \
bullet-devel tinyxml-devel lz4-devel cmake luajit-devel sqlite-devel gtest-devel gmock-devel \
yaml-cpp-devel libXt-devel unshield-devel mygui-devel

</syntaxhighlight>

====Bullet====

Remember installing [https://github.com/bulletphysics/bullet3 Bullet] libraries?
Well, there wasn't any particular reason to install them,
since the ones in Fedora repositories are not built with double-precision support enabled.

You'll have to build Bullet from source as well. Make sure to enable double-precision in Bullet's CMake (USE_DOUBLE_PRECISION).

You can tinker with its other build settings to your liking as OpenMW currently only needs BulletCollision library.

After building Bullet, you should get back to OpenMW's CMake settings.

You will need to point CMake to Bullet libraries and source code. By this point you should be able to finish Makefile generation successfully.

===openSUSE Tumbleweed===

====Packages====
You will need g++ in order to compile OpenMW.

To setup a build environment and install all dependencies type:
<syntaxhighlight lang="bash">
sudo zypper in git cmake \
ffmpeg-6-lib{avcodec,avformat,avutil,swscale,swresample}-devel \
libbullet-devel unshield-devel libOpenSceneGraph-devel OpenSceneGraph-plugins OpenSceneGraph-plugin-collada \
libboost_{system,filesystem,program_options,iostreams}-devel \
libXt-devel \
MyGUI-devel SDL2-devel luajit-devel \
qt6-base-devel qt6-svg-devel qt6-linguist-devel qt6-wayland \
doxygen sqlite3-devel yaml-cpp-devel \
liblz4-devel openal-soft-devel

</syntaxhighlight>

openSUSE also supports installing packages from their cmake BuildRequires. For example, one can enter `sudo zypper in 'cmake(yaml-cpp)'` to install `yaml-cpp-devel`.

==Fetching the source==

Download the source from the main downloads page: [http://code.google.com/p/openmw/downloads/list], or get the latest git snapshot:

<syntaxhighlight lang="bash">

 git clone https://gitlab.com/OpenMW/openmw

 cd openmw

</syntaxhighlight>

==Building==

===With make===

Note that this is an out-of-source build. Generally, this is the preferred method when working with cmake.
<syntaxhighlight lang="bash">
 mkdir build
 cd build
 cmake ../
</syntaxhighlight>
This creates a regular makefile.

Now we can start compiling. Normally you can just run

<syntaxhighlight lang="bash">
make
</syntaxhighlight>

but if you have multiple cores, you can decrease your compile time by doing this:

<syntaxhighlight lang="bash">
make -j`getconf _NPROCESSORS_ONLN`
</syntaxhighlight>

Note** If you are having undefined reference errors while compiling, its possible that you have previously installed a different openscenegraph version than what openMW depends on. 

To remove it, you can use 
 
<syntaxhighlight>

#removes just package
apt-get remove <yourOSGversion>

#or 

#removes configurations as well
apt-get remove --purge <yourOSGversion> 
</syntaxhighlight>


If this doesn't work, it might be necessary to manually remove the libraries installed by the package.

You can usually find them in one of these:
<syntaxhighlight>
/usr/lib
/usr/local/library
/usr/lib/<cpu_architecture>-linux-gnu
</syntaxhighlight>

= macOS =

Note: These steps use the MacOS build scripts that CI runs to build the app.

First, clone the repository:

<pre>$ git clone https://gitlab.com/OpenMW/openmw/</pre>

Then, within the `openmw` source, run:

<pre>
$ ./CI/before_install.macos.sh
$ ./CI/before_script.macos.sh  
$ cd build 
$ make -j $(sysctl -n hw.logicalcpu) package
</pre>

The apps should have been built. '''Use the ones inside the dmg'''. You can open them directly or run binaries directly to see the output:

<pre>$ OpenMW.app/Contents/MacOS/openmw-launcher
$ OpenMW.app/Contents/MacOS/openmw
$ OpenMW-CS.app/Contents/MacOS/OpenMW-CS</pre>

=Data=

For a detailed description of how to obtain the data files check [[Installation_Instructions#Install_Game_Files]].

# Readme

## License
MIT License.

## Build instructions

Installing is easy, but not too much because unfortunately Bullet generates projects with absolute paths and so they cannot be committed.
You'll need CMake.
Steps:

* Run `git clone --recursive git@github.com:Tomcc/dojo3D.git` to grab dojo3D and all its dependencies (note the `--recursive`)
* Run CMake's `configure` step in the `vr150/dojo3D/bulletphysics` folder, with that same folder as the destination
* Enable `USE_MSVC_RUNTIME_LIBRARY_DLL` to allow Bullet to link with the rest of the game
* Hit generate.
* Now, you can either add `dojo3D.vcxproj`, `dojo.vcxproj`, `BulletDynamics.vcxproj`, `BulletCollision.vcxproj` and `LinearMath.vcxproj` to your solution, or you can build them manually and then use them as libraries.
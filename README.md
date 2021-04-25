# WIP: Cross-Platform UnrealEngine-FMIPlugin

![FMUinUE4](docs/fmuUEBP.PNG)

- Cross-Platform (At least Win64 and Linux64)
- A MVP (minimally viable product) of simulating FMUs within the Unreal Engine environment.
- This MVP relies on the [FMIKit repositorty](https://github.com/CATIA-Systems/FMIKit-Simulink) (non-simulink portion)
- Special thanks to Torsten Sommer for his work on FMIKit!
- [Short Demo video](https://youtu.be/r3NeJKJt4Z8)
  
# Prerequistes

This work was tested using the following. It may work on something else but no gurantees.
- Arch Linux rolling (should not matter, since UE4 ships its own toolchain on linux)
- Unreal Engine 4.24 source build
- [Conan UE4 Cli](https://github.com/adamrehn/conan-ue4cli) and l0wl3vels fork of [ue4-conan-recipes](https://github.com/l0wl3vel/ue4-conan-recipes)
- IDE: VS Code

FMU
- The included example FMU, `test.fmu`, and is a [Lorenz System model](https://en.wikipedia.org/wiki/Lorenz_system).
  - The Modelica file is included in the `Modelica` folder.
- This FMU is generated using openmodelica 1.17.0 with modelica-4.0.0 with source code included, so a tool like [fmpy](https://github.com/CATIA-Systems/FMPy) can be used to easily recompile on other platforms.

# Regarding `A_FMU`

In this current version, the `A_FMU` under `FMIKit C++ Classes/FMIKit/Public` contains the magic to make the FMU run. Users are highly encouraged to look at `A_FMU.cpp` if they need to understand more intimately the implementation.
- `mResults` returns the results requested by the user `mStoredVariables`.
  - `mResults` only returns values when `mAutoSimulateTick` = True. Else it is empty and variables can be retrieved using the `GetReal()` function.
- `mModelVariables` are the names of all the variables found in the model.

# Installation (Conan steps missing)

Follow the following steps to get this project up and running on your own computer.

- Download and unzip the folder into your desired location **OR** clone the repository.
- Generate VSCode project files : 

  pathtoUE/GenerateProjectFiles.sh -projectfiles -project='pathto/UnrealEngine-FMIPlugin/FMIPlugin/FMIPlugin.uproject' -game -rocket -progress -VSCode

- Open the FMIPlugin/FMIPlugin.code-workspace in VS Code
- Press CTRL+P and select "Task: Run Build Task" and select "FMIKitEditor Linux Development Build"
- When the build finished successfully open the FMIPlugin.uproject in UE4

# Test Installation (Outdated)

These examples use the `test.fmu` included in the repo.

- `Level_0`
  - This level provides example blueprints (`BP_FMU`) demonstrating the automatic and manual options for simulating an FMU.
  - `BP_FMU` implements the `A_FMU` class
- `Level_1`
  - Simple use of  `A_FMU` and printing a variable to the screen via the level blueprint.

# Known Issues

- Currently only floats and booleans (i.e., 0/1) are supported variables in `A_FMU`.

# Todo

- Ship with Conan dependency prebuilts
- Update documentation
- Write test cases
- ~~Support more variable types~~
- Support multiple FMU instances
- Build better example projects
- Better simulation parameter setup
- Validate Windows support
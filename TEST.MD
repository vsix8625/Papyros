# Pounce build system v0.9.1

## Table of Contents
- [Commands](#commands)
  - [help](#help)
  - [init](#init)
  - [build](#build)
  - [rebuild](#rebuild)
  - [run](#run)
  - [clean](#clean)
  - [config](#config)
  - [new](#new)
  - [version](#version)
  - [sync](#sync)
  - [status](#status)
  - [close](#close)

### Commands:

#### help
- Show general help or for a specific command.  
  ```bash
     pounce help        # show general help 
     pounce help config # show help for the config command   
  ```
  Aliases: --help , -h  

---

#### init 
- Initialize `pounce` in a new or existing project directory  
  ```bash 
     pounce init .                 # Initialize pounce in current directory 
     pounce init my_project        # Initialize pounce in a new directory named `my_project`
  ```
  Aliases: -i  

  Options: -git                  
  ```bash
     pounce init my_project -git   # Initialize pounce in my_project with Git repository  
  ```

  - Initializing `pounce` will create a `.pounce` directory in project root with necessary  
    data and files needed for `pounce` operations.  
  - A `.pounce/init.lock` will also be created with version metadata checks
  - `pounce close` will removes this `.pounce` directory and its contents from the project 
     without interfering with other project files.   

  > **Note:** All `pounce` commands expect `help`, `version` and `init`
    require `pounce` to be  initialized in current working directory.

---

#### build 
- Build the project based on the configuration   
  ```bash
     pounce build
  ```
  Aliases: -b  
  
  Options: --batch
  ```bash
     pounce build --batch  
  ```

  - If `use_cmake` setting is set to `true` this command will build the project with `cmake` and `ninja`   
    Also it will create a default `CMakeLists.txt` in project root if one does not exist.  
  - If not `use_cmake` is set, pounce will attempt to build the project with a its own build system.  
    Using incremental builds with an option for batch compilation.   
  
  > **Note:** Batch build will always clean the build directory before running  

---

#### rebuild 
- Clean and build the project  
  ```bash 
     pounce rebuild
  ```

  Aliases: -rb

--- 

#### run 
- Run the latest binary created  
  ```bash
     pounce run
  ```

  Aliases: -r

--- 

#### clean 
- Clean build directory  
  ```bash
     pounce clean
  ```

  Aliases: -c

  Options: --trash
  ```bash 
     pounce clean --trash  # Clean .pounce/trash 
  ```

--- 

#### config 
- View and edit project and global configuration files   
  ```bash
     pounce config --show  
  ```
  Aliases: -C 


| Option          | Description                                      |
|-----------------|--------------------------------------------------|
| --show, -s      | Show both global and local configuration        |
| --show-local,-sl| Show local configuration only                    |
| --edit, -e      | Open global config in default editor            |
| --edit-local,-el| Open local config in default editor             |
| --repair, -r    | Repair global configuration                     |


  - There is two types of configuration files.
        Global:  
           Stored at `$HOME/.config/pounce/pounce.conf` for Linux and 
           `%APPDATA%/pounce/pounce.conf` for Windows systems. 
           This configuration applies to all pounce projects. 
        Local: 
           Found at `pounce.build`, this configuration is specific to single project 
           overriding global settings. 

        > **Note:** Still WIP as with `v0.9.1` some settings may do nothing yet

---

#### new 
- Create new files or project templates  
  ```bash 
     pounce new --main
  ```
  Aliases: -n 

| Option            | Description                                           |
|------------------|-------------------------------------------------------|
| `--main`          | Generates a `main.c` with Hello, World in `src` directory |
| `--pounce.build`  | Generate a default `pounce.build` in current directory |
| `--cmakelist`     | Generate a default `CMakeLists.txt` in current directory |
| `--cmakelist-lib` | Generate a default library-style `CMakeLists.txt` in current directory |
| `--pair`          | Create `src/mymodule.c` and `src/mymodule.h`        |
| `--pair-inc`      | Create `src/mymodule.c` and `inc/mymodule.h`        |
| `--pair-cpp`      | Create `src/mymodule.cpp` and `src/mymodule.hpp`    |
| `--pair-cpp-inc`  | Create `src/mymodule.cpp` and `inc/mymodule.hpp`    |


 ```bash
    pounce new mymodule --pair   # All --pair options require a name 
 ``` 

---

#### version 
- Print version information  
  ```bash 
     pounce version
  ``` 
  Aliases: --version , -v

---

#### sync 
- Sync project metadata with pounce binary version
  ```bash 
     pounce sync
  ``` 
  Aliases: -sy

---

#### status 
- Print `pounce` status information 
  ```bash 
     pounce status
  ``` 
  Aliases: -st

---

#### close
- Remove `.pounce` configuration in current directory 
  ```bash 
     pounce close
  ``` 
  Aliases: -cl

  Options: -nc    # No confirmation prompt

---


# Advanced Computer Animation Assignment 04

1. kinematics
2. bvh viewer
3. inverse kinematics
4. interactively-controllable character (current)

## Compile

### On Ubuntu 18.04

#### Prerequirement

```
sudo apt install cmake xorg-dev libglu1-mesa-dev
```

#### Download and Install GLFW 3

```
wget https://github.com/glfw/glfw/archive/3.3.tar.gz
tar zxf 3.3.tar.gz
cd glfw-3.3
cmake .
make
sudo make install
```

#### GLFW 3 Install Check

```
pkg-config --libs glfw3
```

#### Compile

```
cd (directory-of-my-assignment)
make
./bin/AdvancedAnimationAssignment01
```

### On Windows 10

#### Prerequirement

1. Install MINGW-w64 and Code::Blocks
2. Set Code::Blocks compiler with installed Mingw

#### Compile

1. Open AdvancedAnimationAssignment01.cbp
2. Press 'F9' (Compile)
3. Press 'Ctrl + F10' (Run)

## How to Run

```
make run
```

### Interfaces

#### to controll viewer



#### to controll character

| Key \ State | Stand | Walk | Run |
| --- | --- | --- | --- |
| 'W' | walk start -> Walk | run -> Run | - |
| 'Q' | - | turn veer left | |
| 'A' | - | turn left | |
| 'S' | turn backward | walk\_stop -> Stand | |
| 'D' | - | | |
| 'E' | - | | | 
| 'X' | jump | | |
| ' ' | forward jump | | |

# Advanced Computer Animation Assignment 01

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
2. Press 'F9'
3. Press 'Ctrl + F10'

## How to Use

Drag the screen with your mouse to rotate the camera view.
You can use 'WASD' keys for moving the focus of camera.

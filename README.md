# Advanced Computer Animation Assignment 03

1. kinematics
2. bvh viewer
3. inverse kinematics (current)

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

## How to Run

before run, prepare an bvh file please named 'Trial001.bvh' from [here](http://mrl.snu.ac.kr/courses/CourseAnimation/2014fall/MotionData.zip)

```
make run
```

## How to Use

There are three scenes. Press '1', '2', '3' to switch scene, please.

1. Scene 1. Crossing the legs (the left toes is on the right knee, and these are green parts), the human body rotates its hand (red parts) drawing the cycle (radius 5). And its head (blue one) is toward to up-front.
2. Scene 2. The human body draws 'LOVE' with its right fingers.
3. Scene 3. The human body stands.

The default scene is the scene 3.

### Interfaces

Drag the screen with your mouse to rotate the camera view.
You can use 'WASD' keys for moving the focus of camera.
You can zoom in and out the screen by scrolling with shift key.

Press the 'space bar' key to stop the motion. To resume, press it again.

run: all
	./bin/AdvancedAnimationAssignment01

all: obj/main.o obj/screen/screen.o obj/kinematics/human/human.o obj/kinematics/open-gl-link/open-gl-link.o obj/kinematics/open-gl-hinge/open-gl-hinge.o \
    obj/kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.o obj/geomatric-primitives/sphere.o obj/geomatric-primitives/cylinder.o \
		obj/geomatric-primitives/cuboid.o obj/geomatric-primitives/common.o
	mkdir -p bin
	g++ -o bin/AdvancedAnimationAssignment01 obj/main.o \
		obj/screen/screen.o \
		obj/kinematics/human/human.o \
		obj/kinematics/open-gl-link/open-gl-link.o \
		obj/kinematics/open-gl-hinge/open-gl-hinge.o \
		obj/kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.o \
		obj/geomatric-primitives/sphere.o \
		obj/geomatric-primitives/cylinder.o \
		obj/geomatric-primitives/cuboid.o \
		obj/geomatric-primitives/common.o \
		-lm -lGL -lGLU -lglfw3 -lpthread -lX11 -ldl

obj/geomatric-primitives/common.o: geomatric-primitives/common.cpp geomatric-primitives/common.hpp
	mkdir -p obj/geomatric-primitives
	g++ -Wall -O2 -c geomatric-primitives/common.cpp -o obj/geomatric-primitives/common.o

obj/geomatric-primitives/cuboid.o: obj/geomatric-primitives/common.o geomatric-primitives/cuboid.cpp geomatric-primitives/cuboid.hpp
	mkdir -p obj/geomatric-primitives
	g++ -Wall -O2 -c geomatric-primitives/cuboid.cpp -o obj/geomatric-primitives/cuboid.o

obj/geomatric-primitives/cylinder.o: obj/geomatric-primitives/common.o geomatric-primitives/cylinder.cpp geomatric-primitives/cylinder.hpp
	mkdir -p obj/geomatric-primitives
	g++ -Wall -O2 -c geomatric-primitives/cylinder.cpp -o obj/geomatric-primitives/cylinder.o

obj/geomatric-primitives/sphere.o: obj/geomatric-primitives/common.o geomatric-primitives/sphere.cpp geomatric-primitives/sphere.hpp
	mkdir -p obj/geomatric-primitives
	g++ -Wall -O2 -c geomatric-primitives/sphere.cpp -o obj/geomatric-primitives/sphere.o

obj/kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.o: obj/geomatric-primitives/sphere.o kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.cpp kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.hpp kinematics/ball-and-socket/ball-and-socket.hpp kinematics/ball-and-socket/ball-and-socket.hpp kinematics/joint/joint.hpp kinematics/link/link.hpp kinematics/drawable.hpp 
	mkdir -p obj/kinematics/open-gl-ball-and-socket
	g++ -Wall -O2 -c kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.cpp -o obj/kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.o

obj/kinematics/open-gl-hinge/open-gl-hinge.o: obj/geomatric-primitives/sphere.o kinematics/open-gl-hinge/open-gl-hinge.cpp kinematics/open-gl-hinge/open-gl-hinge.hpp kinematics/hinge/hinge.hpp kinematics/hinge/hinge.hpp kinematics/joint/joint.hpp kinematics/link/link.hpp kinematics/drawable.hpp 
	mkdir -p obj/kinematics/open-gl-hinge
	g++ -Wall -O2 -c kinematics/open-gl-hinge/open-gl-hinge.cpp -o obj/kinematics/open-gl-hinge/open-gl-hinge.o

obj/kinematics/open-gl-link/open-gl-link.o: obj/geomatric-primitives/cylinder.o kinematics/open-gl-link/open-gl-link.cpp kinematics/open-gl-link/open-gl-link.hpp kinematics/link/link.hpp kinematics/link/link.hpp kinematics/joint/joint.hpp kinematics/link/link.hpp kinematics/drawable.hpp 
	mkdir -p obj/kinematics/open-gl-link
	g++ -Wall -O2 -c kinematics/open-gl-link/open-gl-link.cpp -o obj/kinematics/open-gl-link/open-gl-link.o

obj/kinematics/human/human.o: obj/kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.o obj/kinematics/open-gl-hinge/open-gl-hinge.o obj/kinematics/open-gl-link/open-gl-link.o kinematics/human/human.hpp
	mkdir -p obj/kinematics/human
	g++ -Wall -O2 -c kinematics/human/human.cpp -o obj/kinematics/human/human.o

obj/screen/screen.o: screen/screen.h
	mkdir -p obj/screen
	g++ -Wall -O2 -c screen/screen.cpp -o obj/screen/screen.o

obj/main.o: main.cpp obj/geomatric-primitives/common.o obj/geomatric-primitives/cuboid.o obj/geomatric-primitives/cylinder.o obj/geomatric-primitives/sphere.o obj/kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.o obj/kinematics/open-gl-hinge/open-gl-hinge.o obj/kinematics/open-gl-link/open-gl-link.o obj/kinematics/human/human.o camera/camera.h obj/screen/screen.o
	mkdir -p obj
	g++ -Wall -O2 -c main.cpp -o obj/main.o


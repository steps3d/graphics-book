#
# Makefile to build OpenGL examples for Linux
#
# Author: Alex V. Boreskoff, <steps3d@narod.ru>, <steps3d@gmail.com>
#

OPENCLPATH = /usr/local/cuda-9.1/include/
GLUTPATH   = /usr/include/GL
CFLAGS     = -std=c++11 -x c++ -g -I/usr/include -I/usr/include/GL -Wall -I$(GLUTPATH)/include/GL -I$(OPENCLPATH) -I. -I../Core -I../Libs/SOIL
CONLYFLAGS = -g -I/usr/include/GL -Wall -I$(GLUTPATH)/include/GL
LIBS       = -L/usr/X11R6/lib -L$(GLUTPATH)/lib -L/usr/lib64/nvidia-experimental-310 -lglut -lGL -lXt -lX11 -lm -lGLEW
CC         = g++

OBJS     = GlutWindow.o TgaImage.o Program.o Data.o stringUtils.o VertexBuffer.o BasicMesh.o Texture.o SOIL.o stb_image_aug.o image_helper.o image_DXT.o randUtils.o Framebuffer.o Camera.o  ScreenQuad.o MeshLoader.o

ALL = test test-classes sphere

all: $(ALL)

test: test.o $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS) -lc -lassimp

test-classes: test-classes.o $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS) -lc -lassimp

sphere: sphere.o $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS) -lc -lassimp


clean:
	rm -r -f $(ALL) *.o 2> /dev/null

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o: ../Core/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o: ../Libs/SOIL/%.c
	gcc $(CFLAGS) $(INCLUDE) -c $<

%.o: %.cu
	nvcc $(INCLUDE) -c $<




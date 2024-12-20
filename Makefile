CFLAGS = -std=c++17 -O3
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lfmod -lfmodL
INCPATH = ./lib
INCAUDIO = ./lib/FMODEngine2023/api/core/inc

VulkanTriangle: main.cpp
	g++ -I$(INCPATH) -I$(INCAUDIO) -o VulkanTriangle main.cpp $(LDFLAGS) $(CFLAGS)


.PHONY: test clean

test: VulkanTriangle
	./VulkanTriangle

clean:
	rm -f VulkanTriangle

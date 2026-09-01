CFLAGS = -std=c++17 -O3
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCPATH = ./lib

VulkanTriangle: main.cpp
	g++ -I$(INCPATH) -o VulkanTriangle main.cpp $(LDFLAGS) $(CFLAGS)


.PHONY: test clean

test: VulkanTriangle
	./VulkanTriangle

clean:
	rm -f VulkanTriangle

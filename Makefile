CC ?= cc
SDL3_SOURCE_DIR ?= ../SDL3-3.4.20
SDL3_BUILD_DIR ?= build/sdl3
TARGET ?= build/framebuffer

CPPFLAGS += -I$(SDL3_SOURCE_DIR)/include
CFLAGS ?= -O2 -Wall -Wextra
CFLAGS += -std=c11
LDFLAGS += -L$(SDL3_BUILD_DIR) -Wl,-rpath,'$$ORIGIN/sdl3'
LDLIBS += -lSDL3 -lm

SDL3_LIBRARY := $(SDL3_BUILD_DIR)/libSDL3.so
SDL3_CONFIG_STAMP := $(SDL3_BUILD_DIR)/.configured

.PHONY: all clean

all: $(TARGET)

$(SDL3_CONFIG_STAMP):
	mkdir -p "$(SDL3_BUILD_DIR)"
	cmake -S "$(SDL3_SOURCE_DIR)" -B "$(SDL3_BUILD_DIR)" \
		-DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF \
		-DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF
	touch "$@"

$(SDL3_LIBRARY): $(SDL3_CONFIG_STAMP)
	cmake --build "$(SDL3_BUILD_DIR)" --target SDL3-shared --parallel

$(TARGET): main.c $(SDL3_LIBRARY)
	mkdir -p "$(dir $@)"
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f "$(TARGET)"

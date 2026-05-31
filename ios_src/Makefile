# Makefile untuk kompilasi iOS Dylib di GitHub Actions (macOS Runner)

SDK = $(shell xcrun --sdk iphoneos --show-sdk-path)
CXX = $(shell xcrun --sdk iphoneos --find clang++)

# Flags untuk kompilasi Objective-C++ dan arsitektur ARM64
CXXFLAGS = -isysroot $(SDK) -arch arm64 -miphoneos-version-min=12.0 -O2 -fobjc-arc -std=c++17 -I./imgui

# Nama dylib output
TARGET = libmlbb_cheat.dylib

# Daftar source file
SRC = Main.mm \
      ImGuiOverlay.mm \
      esp/config.cpp \
      Il2CppResolver.cpp \
      imgui/imgui.cpp \
      imgui/imgui_draw.cpp \
      imgui/imgui_tables.cpp \
      imgui/imgui_widgets.cpp \
      imgui/backends/imgui_impl_metal.mm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -dynamiclib -o $@ $^ -framework Foundation -framework UIKit -framework Metal -framework MetalKit -framework CoreGraphics
	@echo "Build success! Output: $(TARGET)"

clean:
	rm -f $(TARGET)

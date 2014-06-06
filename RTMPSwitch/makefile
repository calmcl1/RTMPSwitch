CWD=$(shell pwd)

INCLUDE_DIR=$(CWD)/lib
SO_DIR=$(CWD)/so
BUILD_DIR=$(CWD)/build

travis:
	mkdir $(BUILD_DIR) -p
	g++ $(CWD)/RTMPSwitch.cpp -I"$(INCLUDE_DIR)" -L"$(SO_DIR)" \
	-o "$(BUILD_DIR)/RTMPSwitch" \
	$(shell pkg-config --cflags --libs gstreamer-0.10)

all:
	mkdir $(BUILD_DIR) -p
	g++ $(CWD)/RTMPSwitch.cpp -I"$(INCLUDE_DIR)" -L"$(SO_DIR)" \
	-o "$(BUILD_DIR)/RTMPSwitch" \
	$(shell pkg-config --cflags --libs gstreamer-1.0)
	
clean:
	rm -rf "$(BUILD_DIR)"
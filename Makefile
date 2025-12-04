UNAME_S := $(shell uname -s)
BUILD_DIR = build
VCPKG_DIR = vcpkg
VCPKG_EXE = $(VCPKG_DIR)/vcpkg

ifeq ($(UNAME_S),Linux)
	PRESET_DEBUG = linux-debug
	PRESET_RELEASE = linux-release
	NPROCS = $(shell nproc)
endif
ifeq ($(UNAME_S),Darwin)
	PRESET_DEBUG = osx-debug
	PRESET_RELEASE = osx-release
	NPROCS = $(shell sysctl -n hw.ncpu)
endif

.PHONY: all clean fclean re debug release tests server client run-server run-client

all: debug

$(VCPKG_EXE):
	git submodule update --init --recursive
	@if [ "$(UNAME_S)" = "Windows_NT" ]; then \
		cd $(VCPKG_DIR) && ./bootstrap-vcpkg.bat; \
	else \
		cd $(VCPKG_DIR) && ./bootstrap-vcpkg.sh; \
	fi

debug: $(VCPKG_EXE)
	cmake --preset $(PRESET_DEBUG)
	cmake --build --preset $(PRESET_DEBUG) -j $(NPROCS)

release: $(VCPKG_EXE)
	cmake --preset $(PRESET_RELEASE)
	cmake --build --preset $(PRESET_RELEASE) -j $(NPROCS)

server: $(VCPKG_EXE)
	cmake --preset $(PRESET_DEBUG)
	cmake --build --preset $(PRESET_DEBUG) --target r-type_server -j $(NPROCS)

client: $(VCPKG_EXE)
	cmake --preset $(PRESET_DEBUG)
	cmake --build --preset $(PRESET_DEBUG) --target r-type_client -j $(NPROCS)

run-server: server
	./$(BUILD_DIR)/$(PRESET_DEBUG)/server/r-type_server

run-client: client
	./$(BUILD_DIR)/$(PRESET_DEBUG)/client/r-type_client

tests: debug
	ctest --preset $(PRESET_DEBUG)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -rf vcpkg_installed

re: fclean all
# Detect OS
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    PRESET_DEBUG = windows-debug
    PRESET_RELEASE = windows-release
    NPROCS := $(NUMBER_OF_PROCESSORS)
    SHELL := cmd.exe
    .SHELLFLAGS := /c
else
    DETECTED_OS := $(shell uname -s)
    ifeq ($(DETECTED_OS),Linux)
        PRESET_DEBUG = linux-debug
        PRESET_RELEASE = linux-release
        NPROCS := $(shell nproc)
    endif
    ifeq ($(DETECTED_OS),Darwin)
        PRESET_DEBUG = osx-debug
        PRESET_RELEASE = osx-release
        NPROCS := $(shell sysctl -n hw.ncpu)
    endif
endif

BUILD_DIR = build
VCPKG_DIR = vcpkg
HOOKS_DIR = hooks
GIT_HOOKS_DIR = .git/hooks

.PHONY: all clean fclean re debug release tests coverage server client run-server run-client setup_hooks

all: debug

setup_hooks:
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(GIT_HOOKS_DIR)" mkdir "$(GIT_HOOKS_DIR)"
	@xcopy /E /I /Y "$(HOOKS_DIR)" "$(GIT_HOOKS_DIR)" >nul 2>&1 || echo Hooks setup skipped
else
	@mkdir -p $(GIT_HOOKS_DIR)
	@cp -r $(HOOKS_DIR)/* $(GIT_HOOKS_DIR)/
	@chmod +x $(GIT_HOOKS_DIR)/*
endif

vcpkg-bootstrap:
	@git submodule update --init --recursive
ifeq ($(DETECTED_OS),Windows)
	@cd $(VCPKG_DIR) && bootstrap-vcpkg.bat
else
	@cd $(VCPKG_DIR) && ./bootstrap-vcpkg.sh
endif
    
format:
ifeq ($(DETECTED_OS),Windows)
	@echo "Format not supported on Windows yet (requires Unix find/xargs)"
else
	find . -type f \
		-not -path "*/build/*" \
		-not -path "*/vcpkg/*" \
		-not -path "*/vcpkg_installed/*" \
		-not -path "*/cmake-build-debug/*" \
		-not -path "*/.git/*" \
		\( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" -o -name "*.cc" \) \
		-exec clang-format -i -verbose -style=file {} +
endif

debug: setup_hooks vcpkg-bootstrap
	cmake --preset $(PRESET_DEBUG)
	cmake --build --preset $(PRESET_DEBUG) -j $(NPROCS)
ifeq ($(DETECTED_OS),Windows)
	@if not exist "compile_commands.json" mklink "compile_commands.json" "$(BUILD_DIR)\$(PRESET_DEBUG)\compile_commands.json"
	@if exist "$(BUILD_DIR)\$(PRESET_DEBUG)\server\Debug\r-type_server.exe" copy /Y "$(BUILD_DIR)\$(PRESET_DEBUG)\server\Debug\r-type_server.exe" "r-type_server.exe" >nul
	@if exist "$(BUILD_DIR)\$(PRESET_DEBUG)\client\Debug\r-type_client.exe" copy /Y "$(BUILD_DIR)\$(PRESET_DEBUG)\client\Debug\r-type_client.exe" "r-type_client.exe" >nul
else
	@ln -sf $(BUILD_DIR)/$(PRESET_DEBUG)/compile_commands.json .
	@if [ -f "$(BUILD_DIR)/$(PRESET_DEBUG)/server/r-type_server" ]; then cp "$(BUILD_DIR)/$(PRESET_DEBUG)/server/r-type_server" ./r-type_server; fi
	@if [ -f "$(BUILD_DIR)/$(PRESET_DEBUG)/client/r-type_client" ]; then cp "$(BUILD_DIR)/$(PRESET_DEBUG)/client/r-type_client" ./r-type_client; fi
endif

release: setup_hooks vcpkg-bootstrap
	cmake --preset $(PRESET_RELEASE)
	cmake --build --preset $(PRESET_RELEASE) -j $(NPROCS)
ifeq ($(DETECTED_OS),Windows)
	@if exist "$(BUILD_DIR)\$(PRESET_RELEASE)\server\Release\r-type_server.exe" copy /Y "$(BUILD_DIR)\$(PRESET_RELEASE)\server\Release\r-type_server.exe" "r-type_server.exe" >nul
	@if exist "$(BUILD_DIR)\$(PRESET_RELEASE)\client\Release\r-type_client.exe" copy /Y "$(BUILD_DIR)\$(PRESET_RELEASE)\client\Release\r-type_client.exe" "r-type_client.exe" >nul
else
	@if [ -f "$(BUILD_DIR)/$(PRESET_RELEASE)/server/r-type_server" ]; then cp "$(BUILD_DIR)/$(PRESET_RELEASE)/server/r-type_server" ./r-type_server; fi
	@if [ -f "$(BUILD_DIR)/$(PRESET_RELEASE)/client/r-type_client" ]; then cp "$(BUILD_DIR)/$(PRESET_RELEASE)/client/r-type_client" ./r-type_client; fi
endif

server: setup_hooks vcpkg-bootstrap
	cmake --preset $(PRESET_DEBUG)
	cmake --build --preset $(PRESET_DEBUG) --target r-type_server -j $(NPROCS)
ifeq ($(DETECTED_OS),Windows)
	@if exist "$(BUILD_DIR)\$(PRESET_DEBUG)\server\Debug\r-type_server.exe" copy /Y "$(BUILD_DIR)\$(PRESET_DEBUG)\server\Debug\r-type_server.exe" "r-type_server.exe" >nul
else
	@if [ -f "$(BUILD_DIR)/$(PRESET_DEBUG)/server/r-type_server" ]; then cp "$(BUILD_DIR)/$(PRESET_DEBUG)/server/r-type_server" ./r-type_server; fi
endif

client: setup_hooks vcpkg-bootstrap
	cmake --preset $(PRESET_DEBUG)
	cmake --build --preset $(PRESET_DEBUG) --target r-type_client -j $(NPROCS)
ifeq ($(DETECTED_OS),Windows)
	@if exist "$(BUILD_DIR)\$(PRESET_DEBUG)\client\Debug\r-type_client.exe" copy /Y "$(BUILD_DIR)\$(PRESET_DEBUG)\client\Debug\r-type_client.exe" "r-type_client.exe" >nul
else
	@if [ -f "$(BUILD_DIR)/$(PRESET_DEBUG)/client/r-type_client" ]; then cp "$(BUILD_DIR)/$(PRESET_DEBUG)/client/r-type_client" ./r-type_client; fi
endif

run-server: server
ifeq ($(DETECTED_OS),Windows)
	@r-type_server.exe
else
	@./r-type_server
endif

run-client: client
ifeq ($(DETECTED_OS),Windows)
	@r-type_client.exe
else
	@./r-type_client
endif

tests: debug
	ctest --preset $(PRESET_DEBUG)

coverage: setup_hooks vcpkg-bootstrap
ifeq ($(DETECTED_OS),Windows)
	@echo "ERROR: Coverage is not supported on Windows"
	@echo "Please use Linux or macOS for coverage reports"
	@exit 1
else
	@if ! command -v lcov > /dev/null; then \
		echo "ERROR: lcov is not installed"; \
		echo "Please install it with:"; \
		echo "  - macOS: brew install lcov"; \
		echo "  - Linux: sudo apt-get install lcov"; \
		exit 1; \
	fi
	@echo "Building with coverage enabled..."
	cmake --preset $(PRESET_DEBUG) -DENABLE_COVERAGE=ON
	cmake --build --preset $(PRESET_DEBUG) -j $(NPROCS)
	@echo "Running tests..."
	ctest --preset $(PRESET_DEBUG) --output-on-failure
	@echo "Generating coverage report..."
	@mkdir -p coverage
	@lcov --capture --directory $(BUILD_DIR)/$(PRESET_DEBUG) --output-file coverage/coverage.info \
		--ignore-errors inconsistent,inconsistent \
		--ignore-errors mismatch,mismatch \
		--ignore-errors unused,unused \
		--exclude '/usr/*' \
		--exclude '*/vcpkg_installed/*' \
		--exclude '*/vcpkg/*' \
		--exclude '*/tests/*' \
		--exclude '*/build/*' \
		--exclude '*/.cache/*' \
		--exclude '*/IHost.hpp'
	@lcov --remove coverage/coverage.info '/usr/*' '*/vcpkg_installed/*' '*/vcpkg/*' '*/tests/*' '*/build/*' '*/.cache/*' '*/IHost.hpp' -o coverage/coverage.info \
		--ignore-errors inconsistent,inconsistent \
		--ignore-errors unused,unused
	@echo "Generating XML coverage report for SonarQube..."
	@if command -v genhtml > /dev/null; then \
		genhtml coverage/coverage.info --output-directory coverage/html; \
		echo "HTML coverage report generated in coverage/html"; \
	fi
	@if command -v python3 > /dev/null; then \
		if python3 -c "import lcov_cobertura" 2>/dev/null; then \
			python3 -m lcov_cobertura coverage/coverage.info -o coverage/coverage.xml; \
			echo "XML coverage report generated: coverage/coverage.xml"; \
		else \
			echo "WARNING: lcov_cobertura not installed. Install with: pip3 install lcov_cobertura"; \
			echo "Generating gcov files for SonarQube..."; \
			find $(BUILD_DIR)/$(PRESET_DEBUG) -name "*.gcda" -exec gcov -pb {} + > /dev/null 2>&1; \
			mkdir -p coverage/gcov; \
			find . -name "*.gcov" -exec mv {} coverage/gcov/ \; 2>/dev/null || true; \
		fi \
	else \
		echo "WARNING: Python3 not found. Cannot generate XML coverage report."; \
	fi
	@echo ""
	@echo "========================================"
	@echo "Coverage Report"
	@echo "========================================"
	@lcov --list coverage/coverage.info \
		--ignore-errors inconsistent,inconsistent
endif

clean:
ifeq ($(DETECTED_OS),Windows)
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)" 2>nul
	@if exist "coverage" rmdir /S /Q "coverage" 2>nul
	@del /S /Q *.gcda *.gcno *.gcov 2>nul || echo.
	@if exist "r-type_server.exe" del /Q "r-type_server.exe" 2>nul
	@if exist "r-type_client.exe" del /Q "r-type_client.exe" 2>nul
else
	@rm -rf $(BUILD_DIR)
	@rm -rf coverage
	@find . -name "*.gcda" -delete 2>/dev/null || true
	@find . -name "*.gcno" -delete 2>/dev/null || true
	@find . -name "*.gcov" -delete 2>/dev/null || true
	@rm -f r-type_server r-type_client
endif

fclean: clean
ifeq ($(DETECTED_OS),Windows)
	@if exist "vcpkg_installed" rmdir /S /Q "vcpkg_installed" 2>nul
else
	@rm -rf vcpkg_installed
endif

re: fclean all
PROJECT := $(notdir $(CURDIR))
config ?= release
jobs ?= $(shell nproc 2>/dev/null || echo 4)
PREMAKE ?= $(or $(wildcard $(HOME)/tools/premake5),premake5)
CCACHE := $(shell command -v ccache 2>/dev/null)

ifneq ($(CCACHE),)
	BUILD_ENV := CCACHE_CPP2=yes CC="$(CCACHE) gcc" CXX="$(CCACHE) g++"
endif

CONFIG_NAME := $(shell printf '%s' "$(config)" | awk '{ print toupper(substr($$0,1,1)) substr($$0,2) }')
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

ifeq ($(OS),Windows_NT)
	HOST_SYSTEM := windows
	EXE_EXT := .exe
else ifeq ($(UNAME_S),Darwin)
	HOST_SYSTEM := macosx
	EXE_EXT :=
else
	HOST_SYSTEM := linux
	EXE_EXT :=
endif

BIN_DIR := build/bin/$(CONFIG_NAME)-$(HOST_SYSTEM)-x86_64/$(PROJECT)

.PHONY: configure build run format lint prepare-for-commit clean

configure:
	@tmp=$$(mktemp); cp Makefile $$tmp; \
	$(PREMAKE) gmake2; \
	mv Makefile build/Makefile; \
	cp $$tmp Makefile; chmod 0644 Makefile; \
	rm -f $$tmp
	@if [ -d build/obj ] && grep -Rqs 'source/' build/obj; then rm -rf build/obj; fi

build: configure
	$(BUILD_ENV) $(MAKE) --no-print-directory -f build/Makefile config=$(config) $(PROJECT) -j$(jobs)

run: build
	cd $(BIN_DIR) && ./$(PROJECT)$(EXE_EXT)

format:
	@command -v clang-format >/dev/null || { echo "clang-format is required for make format"; exit 1; }
	@find src -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.c' \) -print0 | xargs -0 clang-format -i

lint:
	@! grep -RInE '\b(throw|try|catch)\b' src --include='*.cpp' --include='*.h' --include='*.hpp'
	@! grep -RIn 'namespace std' src --include='*.cpp' --include='*.h' --include='*.hpp'
	@! grep -RInP '\([^;{}]*\b[A-Za-z_][A-Za-z0-9_:<>]*\s+[A-Za-z_][A-Za-z0-9_]*\s*=[^=][^;{}]*\)\s*(const\s*)?(override\s*)?(final\s*)?;' src --include='*.h' --include='*.hpp'
	@! grep -RInE '(^|[^$$[:alnum:]_])source/' . --exclude=Makefile --exclude-dir=.git --exclude-dir=build --exclude-dir=deps --exclude-dir=.github

prepare-for-commit: format lint

clean:
	@if [ -f build/Makefile ]; then $(MAKE) --no-print-directory -f build/Makefile clean; fi

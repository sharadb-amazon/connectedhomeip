#
# This is project CMakeLists.txt
#

cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(chip-tests)
idf_build_set_property(CXX_COMPILE_OPTIONS "-DCHIP_SUPPORT_FOREIGN_TEST_DRIVERS;-DCHIP_TARGET_STYLE_EMBEDDED;-Wno-deprecated-declarations;-std=gnu++14;-Os;-DLWIP_IPV6_SCOPES=0;-DCHIP_HAVE_CONFIG_H" APPEND)
idf_build_set_property(C_COMPILE_OPTIONS "-DLWIP_IPV6_SCOPES=0;-std=gnu11" APPEND)

# The list of extra component dirs must be in sync with that in all-clusters-app/esp32/Makefile
set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/third_party/connectedhomeip/config/esp32/components"
)

#set(args "#!/bin/sh\n")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/build/chip")
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/build/chip/esp32_elf_builder.sh"
    "#!/bin/sh\n"
    "set -e\n"
    "set -x\n"
    "$(CXX) $(CXXFLAGS) $(CPPFLAGS) -L$(PROJECT_PATH)/build/chip/lib -Wl,--whole-archive '$$1' -Wl,--no-whole-archive -lnlunit-test $(LDFLAGS) -lnlfaultinjection '$$2' -o $(PROJECT_PATH)/build/chip-tests.elf -Wl,-Map=$(APP_MAP)\n"
    "$(ESPTOOLPY) elf2image $(ESPTOOL_FLASH_OPTIONS) $(ESPTOOL_ELF2IMAGE_OPTIONS) -o $(PROJECT_PATH)/build/chip/chip-tests.bin $(PROJECT_PATH)/build/chip-tests.elf\n"
    "ln -sf $(PROJECT_PATH)/build/partitions.bin $(PROJECT_PATH)/build/chip/partitions.bin\n"
)

#file(MAKE_DIRECTORY "build/chip/bootloader")
#file(WRITE "${build/chip/bootloader}/bootloader.bin"
#    "ln -sf $(PROJECT_PATH)/build/bootloader/bootloader.bin $(PROJECT_PATH)/build/chip/bootloader.bin"
#    " ln -sf $(PROJECT_PATH)/idf.sh $(PROJECT_PATH)/build/chip/env.sh")

#set(esp32_elf_builder "${build/chip}/esp32_elf_builder.sh")
#file(GENERATE OUTPUT "${esp32_elf_builder}" CONTENT "${args}")
#add_custom_command(OUTPUT "${esp32_elf_builder}"
#COMMAND "source ${esp32_elf_builder}")

              #add_custom_target(esp32_elf_builder DEPENDS "${esp32_elf_builder}")

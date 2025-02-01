# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(Matriz_5x5_IRQ C CXX ASM)

pico_sdk_init()

add_executable(Matriz_5x5_IRQ
    Matriz_5x5_IRQ.c
    lib/animacao/animacao.c
)

# Adiciona diretórios de inclusão
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib/animacao
)

# Adiciona o arquivo PIO
file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(Matriz_5x5_IRQ ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)


pico_enable_stdio_usb(Matriz_5x5_IRQ 1)
pico_enable_stdio_uart(Matriz_5x5_IRQ 0)


target_link_libraries(Matriz_5x5_IRQ
    pico_stdlib
    hardware_pio
    hardware_clocks
	hardware_adc
)

pico_add_extra_outputs(Matriz_5x5_IRQ)
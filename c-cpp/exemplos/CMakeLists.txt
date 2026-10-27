cmake_minimum_required(VERSION 3.20)
project(referencia_c_cpp LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()
find_package(Threads REQUIRED)
option(REF_SANITIZERS "Ativar ASan e UBSan em GCC/Clang compativeis" OFF)

function(ref_configurar alvo)
    if(MSVC)
        target_compile_options(${alvo} PRIVATE /W4 /utf-8
            $<$<COMPILE_LANGUAGE:CXX>:/permissive->
            $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>
            $<$<COMPILE_LANGUAGE:CXX>:/EHsc>)
    else()
        target_compile_options(${alvo} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        if(REF_SANITIZERS)
            target_compile_options(${alvo} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${alvo} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
endfunction()

foreach(nome basico vetor lista_fila)
    add_executable(c_${nome} c/${nome}.c)
    ref_configurar(c_${nome})
    add_test(NAME c_${nome} COMMAND c_${nome})
endforeach()

foreach(nome sequencias grafos estruturas moderno concorrencia)
    add_executable(cpp_${nome} cpp/${nome}.cpp)
    ref_configurar(cpp_${nome})
    add_test(NAME cpp_${nome} COMMAND cpp_${nome})
endforeach()
target_link_libraries(cpp_concorrencia PRIVATE Threads::Threads)

add_library(ref_api STATIC ffi/api.c)
ref_configurar(ref_api)
add_executable(ffi_exemplo ffi/main.cpp)
target_link_libraries(ffi_exemplo PRIVATE ref_api)
ref_configurar(ffi_exemplo)
add_test(NAME ffi_exemplo COMMAND ffi_exemplo)


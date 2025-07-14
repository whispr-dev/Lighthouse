# 🏰 Ultimate Lighthouse Beacon System CMakeLists.txt
# Powered by RTC's Jsonifier - The Fastest JSON Library
# Production-Ready Build System with Maximum Optimizations

cmake_minimum_required(VERSION 3.20)
project(UltimateLighthouseBeacon 
    VERSION 3.0.0 
    DESCRIPTION "The Fastest JSON-Powered Network Monitor in Existence"
    LANGUAGES CXX)

# 🚀 C++20 Standard Required for Maximum Performance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 🔥 CPU Architecture Detection and Optimization
include(CheckCXXSourceCompiles)
include(ProcessorCount)
ProcessorCount(CPU_COUNT)

# Detect CPU capabilities for maximum SIMD optimization
function(detect_cpu_features)
    message(STATUS "🔍 Detecting CPU capabilities for maximum performance...")
    
    # Test for AVX-512
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() { 
            __m512i vec = _mm512_setzero_si512(); 
            return 0; 
        }" 
        HAS_AVX512)
    
    # Test for AVX2
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() { 
            __m256i vec = _mm256_setzero_si256(); 
            return 0; 
        }" 
        HAS_AVX2)
    
    # Test for AVX
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() { 
            __m128i vec = _mm_setzero_si128(); 
            return 0; 
        }" 
        HAS_AVX)
    
    # Test for BMI2
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() { 
            unsigned int result = _pdep_u32(1, 2); 
            return 0; 
        }" 
        HAS_BMI2)
    
    # Test for POPCNT
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() { 
            int result = _mm_popcnt_u64(1); 
            return 0; 
        }" 
        HAS_POPCNT)
    
    # Calculate CPU instruction level for RTC Jsonifier
    set(CPU_INSTRUCTION_LEVEL 0)
    if(HAS_POPCNT)
        math(EXPR CPU_INSTRUCTION_LEVEL "${CPU_INSTRUCTION_LEVEL} + 1")
        message(STATUS "✅ POPCNT support detected")
    endif()
    if(HAS_BMI2)
        math(EXPR CPU_INSTRUCTION_LEVEL "${CPU_INSTRUCTION_LEVEL} + 8")
        message(STATUS "✅ BMI2 support detected")
    endif()
    if(HAS_AVX)
        math(EXPR CPU_INSTRUCTION_LEVEL "${CPU_INSTRUCTION_LEVEL} + 32")
        message(STATUS "✅ AVX support detected")
    endif()
    if(HAS_AVX2)
        math(EXPR CPU_INSTRUCTION_LEVEL "${CPU_INSTRUCTION_LEVEL} + 64")
        message(STATUS "✅ AVX2 support detected")
    endif()
    if(HAS_AVX512)
        math(EXPR CPU_INSTRUCTION_LEVEL "${CPU_INSTRUCTION_LEVEL} + 128")
        message(STATUS "🚀 AVX-512 support detected - MAXIMUM POWER!")
    endif()
    
    set(JSONIFIER_CPU_INSTRUCTIONS ${CPU_INSTRUCTION_LEVEL} PARENT_SCOPE)
    message(STATUS "🔥 CPU Instruction Level: ${CPU_INSTRUCTION_LEVEL}")
endfunction()

detect_cpu_features()

# 🚀 Fetch RTC's Jsonifier - The Ultimate JSON Library
include(FetchContent)

message(STATUS "🚀 Fetching RTC's Jsonifier - The Fastest JSON Library...")
FetchContent_Declare(
    Jsonifier
    GIT_REPOSITORY https://github.com/RealTimeChris/Jsonifier.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# Configure Jsonifier with our detected CPU capabilities
set(JSONIFIER_CPU_INSTRUCTIONS ${JSONIFIER_CPU_INSTRUCTIONS} CACHE STRING "CPU instruction set level")
FetchContent_MakeAvailable(Jsonifier)

# 🔥 Platform-Specific Dependencies
if(WIN32)
    # Windows dependencies
    find_library(WS2_32_LIB ws2_32)
    find_library(WININET_LIB wininet)
    if(NOT WS2_32_LIB OR NOT WININET_LIB)
        message(FATAL_ERROR "🚨 Required Windows libraries not found!")
    endif()
    set(PLATFORM_LIBS ${WS2_32_LIB} ${WININET_LIB})
    message(STATUS "✅ Windows networking libraries found")
else()
    # Linux dependencies
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CURL REQUIRED libcurl)
    set(PLATFORM_LIBS ${CURL_LIBRARIES})
    set(PLATFORM_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
    message(STATUS "✅ libcurl found: ${CURL_VERSION}")
endif()

# 🚀 Compiler-Specific Optimizations
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(OPTIMIZATION_FLAGS 
        -O3 -march=native -mtune=native 
        -ffast-math -funroll-loops 
        -fomit-frame-pointer -flto
        -DNDEBUG)
    if(HAS_AVX512)
        list(APPEND OPTIMIZATION_FLAGS -mavx512f -mavx512dq -mavx512cd -mavx512bw -mavx512vl)
    elseif(HAS_AVX2)
        list(APPEND OPTIMIZATION_FLAGS -mavx2 -mbmi2)
    elseif(HAS_AVX)
        list(APPEND OPTIMIZATION_FLAGS -mavx)
    endif()
    message(STATUS "🔥 GCC optimization flags configured")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(OPTIMIZATION_FLAGS 
        -O3 -march=native -mtune=native 
        -ffast-math -funroll-loops 
        -fomit-frame-pointer -flto
        -DNDEBUG)
    if(HAS_AVX512)
        list(APPEND OPTIMIZATION_FLAGS -mavx512f -mavx512dq -mavx512cd -mavx512bw -mavx512vl)
    elseif(HAS_AVX2)
        list(APPEND OPTIMIZATION_FLAGS -mavx2 -mbmi2)
    elseif(HAS_AVX)
        list(APPEND OPTIMIZATION_FLAGS -mavx)
    endif()
    message(STATUS "🔥 Clang optimization flags configured")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(OPTIMIZATION_FLAGS 
        /O2 /Oi /Ot /Oy /GL /DNDEBUG)
    if(HAS_AVX512)
        list(APPEND OPTIMIZATION_FLAGS /arch:AVX512)
    elseif(HAS_AVX2)
        list(APPEND OPTIMIZATION_FLAGS /arch:AVX2)
    elseif(HAS_AVX)
        list(APPEND OPTIMIZATION_FLAGS /arch:AVX)
    endif()
    message(STATUS "🔥 MSVC optimization flags configured")
endif()

# 🏰 Create the Ultimate Lighthouse Beacon executable
add_executable(ultimate_lighthouse_beacon 
    ${CMAKE_CURRENT_SOURCE_DIR}/ultimate_lighthouse_system.cpp)

# 🚀 Configure target properties
target_compile_features(ultimate_lighthouse_beacon PRIVATE cxx_std_20)
target_compile_options(ultimate_lighthouse_beacon PRIVATE ${OPTIMIZATION_FLAGS})

# 🔥 Link libraries
target_link_libraries(ultimate_lighthouse_beacon 
    PRIVATE 
    jsonifier::Jsonifier
    ${PLATFORM_LIBS})

# Include directories
if(PLATFORM_INCLUDE_DIRS)
    target_include_directories(ultimate_lighthouse_beacon PRIVATE ${PLATFORM_INCLUDE_DIRS})
endif()

# 🎯 Create beacon listener executable  
add_executable(ultimate_beacon_listener
    ${CMAKE_CURRENT_SOURCE_DIR}/ultimate_lighthouse_system.cpp)

target_compile_features(ultimate_beacon_listener PRIVATE cxx_std_20)
target_compile_options(ultimate_beacon_listener PRIVATE ${OPTIMIZATION_FLAGS})
target_compile_definitions(ultimate_beacon_listener PRIVATE LISTENER_MODE=1)

target_link_libraries(ultimate_beacon_listener 
    PRIVATE 
    jsonifier::Jsonifier
    ${PLATFORM_LIBS})

if(PLATFORM_INCLUDE_DIRS)
    target_include_directories(ultimate_beacon_listener PRIVATE ${PLATFORM_INCLUDE_DIRS})
endif()

# 🔬 Create performance benchmark executable
add_executable(ultimate_json_benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/ultimate_lighthouse_system.cpp)

target_compile_features(ultimate_json_benchmark PRIVATE cxx_std_20)
target_compile_options(ultimate_json_benchmark PRIVATE ${OPTIMIZATION_FLAGS})
target_compile_definitions(ultimate_json_benchmark PRIVATE BENCHMARK_MODE=1)

target_link_libraries(ultimate_json_benchmark 
    PRIVATE 
    jsonifier::Jsonifier
    ${PLATFORM_LIBS})

if(PLATFORM_INCLUDE_DIRS)
    target_include_directories(ultimate_json_benchmark PRIVATE ${PLATFORM_INCLUDE_DIRS})
endif()

# 🚀 Build information display
message(STATUS "")
message(STATUS "🏰 ═══════════════════════════════════════════════════════════════")
message(STATUS "   ULTIMATE LIGHTHOUSE BEACON SYSTEM BUILD CONFIGURATION")
message(STATUS "🏰 ═══════════════════════════════════════════════════════════════")
message(STATUS "🚀 Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "🔥 C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "⚡ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "💎 Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "🎯 CPU Count: ${CPU_COUNT}")
message(STATUS "🔥 CPU Instructions: ${JSONIFIER_CPU_INSTRUCTIONS}")

if(HAS_AVX512)
    message(STATUS "🚀 SIMD Level: AVX-512 (MAXIMUM POWER!)")
elseif(HAS_AVX2)
    message(STATUS "⚡ SIMD Level: AVX2 (HIGH PERFORMANCE)")
elseif(HAS_AVX)
    message(STATUS "🔥 SIMD Level: AVX (ACCELERATED)")
else()
    message(STATUS "📊 SIMD Level: Standard (Still Fast!)")
endif()

message(STATUS "🏰 ═══════════════════════════════════════════════════════════════")
message(STATUS "")

# 🎯 Custom build targets for convenience
add_custom_target(run_lighthouse
    COMMAND $<TARGET_FILE:ultimate_lighthouse_beacon>
    DEPENDS ultimate_lighthouse_beacon
    COMMENT "🏰 Running Ultimate Lighthouse Beacon System..."
    VERBATIM)

add_custom_target(run_listener
    COMMAND $<TARGET_FILE:ultimate_beacon_listener> --listener
    DEPENDS ultimate_beacon_listener
    COMMENT "🎧 Running Ultimate Beacon Listener..."
    VERBATIM)

add_custom_target(run_benchmark
    COMMAND $<TARGET_FILE:ultimate_json_benchmark> --benchmark
    DEPENDS ultimate_json_benchmark
    COMMENT "🔬 Running Ultimate JSON Performance Benchmark..."
    VERBATIM)

# 🚀 Installation configuration
install(TARGETS 
    ultimate_lighthouse_beacon 
    ultimate_beacon_listener 
    ultimate_json_benchmark
    RUNTIME DESTINATION bin
    COMPONENT runtime)

# 📝 Package configuration
set(CPACK_PACKAGE_NAME "UltimateLighthouseBeacon")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Fastest JSON-Powered Network Monitor")
set(CPACK_PACKAGE_VENDOR "RYO Modular & whispr.dev")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "RYO Modular <contact@ryomodular.com>")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcurl4")
endif()

include(CPack)

# 🔥 Display final build instructions
message(STATUS "🏰 BUILD INSTRUCTIONS:")
message(STATUS "   Build all targets:     cmake --build . --config Release")
message(STATUS "   Run lighthouse:        cmake --build . --target run_lighthouse")
message(STATUS "   Run listener:          cmake --build . --target run_listener") 
message(STATUS "   Run benchmark:         cmake --build . --target run_benchmark")
message(STATUS "   Install:               cmake --install .")
message(STATUS "   Create package:        cpack")
message(STATUS "")
message(STATUS "🚀 Ready to build the fastest lighthouse system ever created!")
message(STATUS "🏰 ═══════════════════════════════════════════════════════════════")

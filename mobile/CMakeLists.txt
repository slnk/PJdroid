# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Name your project here
project(PJAPI)

# Our source code directories
set (pj_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/main/pjproject)

# Resulting pjproject static libraries will go here
set (generated_libs_dir ${CMAKE_CURRENT_SOURCE_DIR}/build/generated/pjproject)
set (pjinstalled_libs_dir ${generated_libs_dir}/usr/local/lib)
set (pjinstalled_incs_dir ${generated_libs_dir}/usr/local/include)

set (TARGET_ABI armeabi-v7a)

# we create pjproject generated directories
file(MAKE_DIRECTORY ${generated_libs_dir})
file(MAKE_DIRECTORY ${generated_libs_dir}/usr)
file(MAKE_DIRECTORY ${generated_libs_dir}/usr/local)
file(MAKE_DIRECTORY ${generated_libs_dir}/usr/local/lib)

# Resulting pjApi.so will go here
set (final_output_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi)
# we create the final output dir for pjapi.so
file(MAKE_DIRECTORY ${final_output_dir})


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             pjapi

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# Find external libs to link
find_library(log_lib log)
find_library(openssles_lib OpenSLES)
find_library(android_lib android)

# Set include directories for pjapi library
# we use the include files directly from the source -- we could use the generated ones,
# but if we did so we would also have to include the include files in the BYPRODUCTS lists
target_include_directories ( pjapi PUBLIC
                 ${pj_dir}/pjsip/include
                 ${pj_dir}/pjlib-util/include
                 ${pj_dir}/pjmedia/include
                 ${pj_dir}/pjlib/include
                 ${pj_dir}/pjnath/include
                 ${pj_dir}/third_party/g7221/common
                 ${pj_dir}/third_party/resample/include
                 ${pj_dir}/third_party/gsm/inc
                 ${pj_dir}/third_party/ilbc
                 ${pj_dir}/third_party/speex/include
                 ${pj_dir}/third_party/srtp/include
             )

add_custom_target ( pjproject
                    COMMAND ${CMAKE_COMMAND} -E env
                    "ANDROID_NDK_ROOT=${ANDROID_NDK}"
                    "TARGET_ABI=${TARGET_ABI}" ./configure-android --use-ndk-cflags
                    COMMAND make dep
                    COMMAND make
                    WORKING_DIRECTORY ${pj_dir}
        )

add_custom_target( pjinstall
                    COMMAND ${CMAKE_COMMAND} -E env "DESTDIR=${generated_libs_dir}" make install > ${generated_libs_dir}/install_log.txt
                    WORKING_DIRECTORY ${pj_dir}
                    BYPRODUCTS ${pjinstalled_libs_dir}/libpj-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjsua-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjsip-ua-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjsip-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjsip-simple-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjlib-util-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjnath-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjmedia-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjmedia-codec-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjmedia-audiodev-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libg7221codec-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libresample-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libsrtp-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libwebrtc-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjsua2-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libpjmedia-videodev-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libyuv-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libgsmcodec-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libilbccodec-arm-unknown-linux-androideabi.a
                               ${pjinstalled_libs_dir}/libspeex-arm-unknown-linux-androideabi.a
                    )

add_dependencies (pjinstall pjproject)
add_dependencies (pjapi pjinstall)


# PJ SIP LIBRARIES

add_library(pjLib STATIC IMPORTED)
set_property(TARGET pjLib PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpj-arm-unknown-linux-androideabi.a)

add_library(pjSua STATIC IMPORTED)
set_property(TARGET pjSua PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjsua-arm-unknown-linux-androideabi.a)

add_library(pjSip-Ua STATIC IMPORTED)
set_property(TARGET pjSip-Ua PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjsip-ua-arm-unknown-linux-androideabi.a)

add_library(pjSip STATIC IMPORTED)
set_property(TARGET pjSip PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjsip-arm-unknown-linux-androideabi.a)

add_library(pjSimple STATIC IMPORTED)
set_property(TARGET pjSimple PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjsip-simple-arm-unknown-linux-androideabi.a)

add_library(pjUtil STATIC IMPORTED)
set_property(TARGET pjUtil PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjlib-util-arm-unknown-linux-androideabi.a)

add_library(pjNath STATIC IMPORTED)
set_property(TARGET pjNath PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjnath-arm-unknown-linux-androideabi.a)

add_library(pjMedia STATIC IMPORTED)
set_property(TARGET pjMedia PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjmedia-arm-unknown-linux-androideabi.a)

add_library(pjMediaCodec STATIC IMPORTED)
set_property(TARGET pjMediaCodec PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjmedia-codec-arm-unknown-linux-androideabi.a)

add_library(pjMediaAudioDev STATIC IMPORTED)
set_property(TARGET pjMediaAudioDev PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjmedia-audiodev-arm-unknown-linux-androideabi.a)

add_library(pjThird7221 STATIC IMPORTED)
set_property(TARGET pjThird7221 PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libg7221codec-arm-unknown-linux-androideabi.a)

add_library(pjThirdResample STATIC IMPORTED)
set_property(TARGET pjThirdResample PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libresample-arm-unknown-linux-androideabi.a)

add_library(pjThirdSrtp STATIC IMPORTED)
set_property(TARGET pjThirdSrtp PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libsrtp-arm-unknown-linux-androideabi.a)

add_library(pjWebRtc STATIC IMPORTED)
set_property(TARGET pjWebRtc PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libwebrtc-arm-unknown-linux-androideabi.a)

add_library(pjSua2 STATIC IMPORTED)
set_property(TARGET pjSua2 PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjsua2-arm-unknown-linux-androideabi.a)

add_library(pjMediaVideo STATIC IMPORTED)
set_property(TARGET pjMediaVideo PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libpjmedia-videodev-arm-unknown-linux-androideabi.a)

add_library(pjYuv STATIC IMPORTED)
set_property(TARGET pjYuv PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libyuv-arm-unknown-linux-androideabi.a)

add_library(pjGsm STATIC IMPORTED)
set_property(TARGET pjGsm PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libgsmcodec-arm-unknown-linux-androideabi.a)

add_library(pjIlbc STATIC IMPORTED)
set_property(TARGET pjIlbc PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libilbccodec-arm-unknown-linux-androideabi.a)

add_library(pjSpeex STATIC IMPORTED)
set_property(TARGET pjSpeex PROPERTY IMPORTED_LOCATION ${pjinstalled_libs_dir}/libspeex-arm-unknown-linux-androideabi.a)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       pjapi

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

# LINK ALL OF OUR LIBRARIES TOGETHER

# Add in our libraries external to this project
# Since the library manager used here only searches a listed library in one pass
# the library ordering here is critical.  Note that some libraries are searched
# twice by including them in the list more that once.
# OpenSSL is another case.  ssl and crypto must appear before and after the
# SECOND instance of pjLib.
target_link_libraries(  pjapi pjSua pjSua2 pjLib pjSip pjSip-Ua pjSimple pjNath pjUtil pjMedia
                        pjMediaCodec pjMediaAudioDev pjMediaVideo pjYuv pjThird7221 pjThirdResample
                        pjThirdSrtp pjWebRtc pjGsm pjIlbc pjSpeex log OpenSLES android
                )

add_custom_command (TARGET pjapi POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/build/intermediates/cmake/debug/obj/armeabi-v7a/libpjapi.so
${final_output_dir}/libpjapi.so)

# Move our created library to the output directory
#install (TARGETS pjapi DESTINATION ${final_output_dir})



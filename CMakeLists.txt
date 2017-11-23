cmake_minimum_required(VERSION 3.1)

project(SMD)

add_executable(SMD
    data_functions.cpp
    main.cpp
    )

target_compile_features(SMD
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    )

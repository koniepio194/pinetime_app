# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/../pinetime/cmake/boilerplate.cmake)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(pinetime_app)

# use this approach
#FILE(GLOB app_sources src/*.c)
#target_sources(app PRIVATE ${app_sources})

target_sources(app PRIVATE src/main.c src/display.c src/date_time.c)

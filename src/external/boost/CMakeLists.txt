# Copyright 2021 Arnaud Becheler
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# Include Boost as an imported target: 1.73 important for compilation with gcc 10 and C++20
find_package(Boost 1.79 REQUIRED COMPONENTS unit_test_framework filesystem serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) # Find Boost headers only
ENDIF()

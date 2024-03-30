# synergy -- mouse and keyboard sharing utility
# Copyright (C) 2012-2016 Symless Ltd.
# Copyright (C) 2009 Nick Bolton
# 
# This package is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# found in the file LICENSE that should have accompanied this file.
# 
# This package is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

set(sources
    synergy_flutter.cpp
    DummyClientProxy.cpp
    DummyClientProxy.h
    MyStream.cpp
    MyStream.h
    my_args.h
)

list(APPEND sources ${arch_sources})
list(APPEND headers ${arch_headers})

if (SYNERGY_ADD_HEADERS)
    list(APPEND sources ${headers})
endif()

add_executable(synergy_flutter ${sources})
target_link_libraries(synergy_flutter
    arch base client common io mt net ipc platform server synlib ${libs})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    install (TARGETS synergy_flutter DESTINATION ${SYNERGY_BUNDLE_BINARY_DIR})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install (TARGETS synergy_flutter DESTINATION bin)
endif()





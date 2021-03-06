#
# Copyright (c) 2015-2016 CNRS
# Copyright (c) 2015 Wandercraft, 86 rue de Paris 91400 Orsay, France.
#
# This file is part of Pinocchio
# pinocchio is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
# pinocchio is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Lesser Public License for more details. You should have
# received a copy of the GNU Lesser General Public License along with
# pinocchio If not, see
# <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(cmake/base.cmake)
INCLUDE(cmake/boost.cmake)
INCLUDE(cmake/cpack.cmake)
INCLUDE(cmake/eigen.cmake)
INCLUDE(cmake/python.cmake)

SET(PROJECT_NAME pinocchio)
SET(PROJECT_DESCRIPTION "Rigid multi body dynamics algorithms")
SET(PROJECT_URL "http://github.com/stac-of-tasks/pinocchio")

SET(DOXYGEN_USE_MATHJAX YES)

IF(APPLE)
  SET(CMAKE_MACOSX_RPATH 1)
ENDIF(APPLE)

# Disable -Werror on Unix for now.
SET(CXX_DISABLE_WERROR True)
SET(CMAKE_VERBOSE_MAKEFILE True)

SETUP_PROJECT()

IF(WIN32)
  SET(LINK copy_if_different)
ELSE(WIN32)
  SET(LINK create_symlink)
ENDIF(WIN32)

# --- OPTIONS ----------------------------------------
OPTION (BUILD_BENCHMARK "Build the benchmarks" OFF)
OPTION (BUILD_UNIT_TESTS "Build the unitary tests" ON)
OPTION (BUILD_UTILS "Build the utils" OFF)
OPTION (INITIALIZE_WITH_NAN "Initialize Eigen entries with NaN" OFF)
OPTION (BUILD_TESTS_WITH_HPP "Build geom tests and benchmarks with hpp to do comparisons" OFF)
OPTION (BUILD_PYTHON_INTERFACE "Build the python binding" ON)

IF (INITIALIZE_WITH_NAN)
  MESSAGE (STATUS "Initialize with NaN all the Eigen entries")
  ADD_DEFINITIONS(-DEIGEN_INITIALIZE_MATRICES_BY_NAN)
ENDIF (INITIALIZE_WITH_NAN)

# ----------------------------------------------------
# --- DEPENDANCIES -----------------------------------
# ----------------------------------------------------
ADD_REQUIRED_DEPENDENCY("eigen3 >= 3.0.5")
ADD_OPTIONAL_DEPENDENCY("eigenpy >= 1.3.0")
ADD_OPTIONAL_DEPENDENCY("metapod >= 1.0.7")
ADD_OPTIONAL_DEPENDENCY("urdfdom >= 0.2.10")
ADD_OPTIONAL_DEPENDENCY("lua5.1")
ADD_OPTIONAL_DEPENDENCY("hpp-fcl >= 0.4.1")
IF(HPP_FCL_FOUND AND URDFDOM_FOUND)
  ADD_REQUIRED_DEPENDENCY("assimp >= 3.0")
ENDIF(HPP_FCL_FOUND AND URDFDOM_FOUND)

SET(BOOST_COMPONENTS filesystem unit_test_framework system)
SEARCH_FOR_BOOST()
# Path to boost headers
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# ----------------------------------------------------
# --- INCLUDE ----------------------------------------
# ----------------------------------------------------


SET(${PROJECT_NAME}_MATH_HEADERS
  math/fwd.hpp
  math/sincos.hpp
  math/quaternion.hpp
  )

SET(${PROJECT_NAME}_TOOLS_HEADERS
  tools/timer.hpp
  tools/string-generator.hpp
  tools/file-explorer.hpp
  )

SET(${PROJECT_NAME}_SPATIAL_HEADERS
  spatial/symmetric3.hpp
  spatial/se3.hpp
  spatial/motion.hpp
  spatial/force.hpp
  spatial/inertia.hpp
  spatial/fwd.hpp
  spatial/skew.hpp
  spatial/act-on-set.hpp
  spatial/explog.hpp
  spatial/frame.hpp
  )

SET(${PROJECT_NAME}_MULTIBODY_JOINT_HEADERS
  multibody/joint/joint-base.hpp
  multibody/joint/joint-dense.hpp
  multibody/joint/joint-revolute.hpp
  multibody/joint/joint-revolute-unaligned.hpp
  multibody/joint/joint-spherical.hpp
  multibody/joint/joint-spherical-ZYX.hpp
  multibody/joint/joint-prismatic.hpp
  multibody/joint/joint-prismatic-unaligned.hpp
  multibody/joint/joint-planar.hpp
  multibody/joint/joint-translation.hpp
  multibody/joint/joint-free-flyer.hpp
  multibody/joint/joint-variant.hpp
  multibody/joint/joint-generic.hpp
  multibody/joint/joint-accessor.hpp
  multibody/joint/joint-basic-visitors.hpp
  multibody/joint/joint-basic-visitors.hxx
  ) 

SET(${PROJECT_NAME}_MULTIBODY_PARSER_HEADERS
  multibody/parser/sample-models.hpp
  multibody/parser/utils.hpp
  )

SET(${PROJECT_NAME}_MULTIBODY_HEADERS
  multibody/fwd.hpp
  multibody/constraint.hpp
  multibody/force-set.hpp
  multibody/joint.hpp
  multibody/model.hpp
  multibody/model.hxx
  multibody/visitor.hpp
  multibody/parser/srdf.hpp
  )

SET(${PROJECT_NAME}_ALGORITHM_HEADERS
  algorithm/aba.hpp
  algorithm/aba.hxx
  algorithm/rnea.hpp
  algorithm/rnea.hxx
  algorithm/crba.hpp
  algorithm/crba.hxx
  algorithm/jacobian.hpp
  algorithm/jacobian.hxx
  algorithm/cholesky.hpp
  algorithm/cholesky.hxx
  algorithm/kinematics.hpp
  algorithm/kinematics.hxx
  algorithm/dynamics.hpp
  algorithm/center-of-mass.hpp
  algorithm/center-of-mass.hxx
  algorithm/joint-configuration.hpp
  algorithm/energy.hpp
  algorithm/operational-frames.hpp
  algorithm/compute-all-terms.hpp
  )

IF(${BUILD_PYTHON_INTERFACE} STREQUAL "ON")
SET(${PROJECT_NAME}_PYTHON_HEADERS
  python/eigen_container.hpp
  python/handler.hpp
  python/python.hpp
  python/se3.hpp
  python/force.hpp
  python/motion.hpp
  python/inertia.hpp
  python/joint-dense.hpp
  python/joints-models.hpp
  python/joints-variant.hpp
  python/frame.hpp
  python/model.hpp
  python/data.hpp
  python/algorithms.hpp
  python/parsers.hpp
  python/explog.hpp
  )
ENDIF(${BUILD_PYTHON_INTERFACE} STREQUAL "ON")

IF(HPP_FCL_FOUND)
  LIST(APPEND ${PROJECT_NAME}_PYTHON_HEADERS
      python/geometry-object.hpp
      python/geometry-model.hpp
      python/geometry-data.hpp
    )
ENDIF(HPP_FCL_FOUND)

IF(URDFDOM_FOUND)
  LIST(APPEND ${PROJECT_NAME}_MULTIBODY_PARSER_HEADERS
    multibody/parser/urdf.hpp
    multibody/parser/urdf.hxx
    )
  
  IF(HPP_FCL_FOUND )
    LIST(APPEND ${PROJECT_NAME}_MULTIBODY_PARSER_HEADERS
      multibody/parser/from-collada-to-fcl.hpp
      multibody/parser/urdf-with-geometry.hpp
      multibody/parser/urdf-with-geometry.hxx
      )
  ENDIF(HPP_FCL_FOUND)

  ADD_DEFINITIONS(-DWITH_URDFDOM)
ENDIF(URDFDOM_FOUND)

IF(HPP_FCL_FOUND)
  LIST(APPEND ${PROJECT_NAME}_SPATIAL_HEADERS
    spatial/fcl-pinocchio-conversions.hpp
    )
  LIST(APPEND ${PROJECT_NAME}_MULTIBODY_HEADERS
    multibody/geometry.hpp
    multibody/geometry.hxx
    )
  LIST(APPEND ${PROJECT_NAME}_ALGORITHM_HEADERS
    algorithm/collisions.hpp
    )
ENDIF(HPP_FCL_FOUND)

IF(LUA5_1_FOUND)
  LIST(APPEND ${PROJECT_NAME}_MULTIBODY_PARSER_HEADERS
    multibody/parser/lua.hpp
    multibody/parser/lua/lua_tables.hpp
    )

  ADD_DEFINITIONS(-DWITH_LUA)
ENDIF(LUA5_1_FOUND)


SET(HEADERS
  ${${PROJECT_NAME}_MATH_HEADERS}
  ${${PROJECT_NAME}_TOOLS_HEADERS}
  ${${PROJECT_NAME}_SPATIAL_HEADERS}
  ${${PROJECT_NAME}_MULTIBODY_JOINT_HEADERS}
  ${${PROJECT_NAME}_MULTIBODY_PARSER_HEADERS}
  ${${PROJECT_NAME}_MULTIBODY_HEADERS}
  ${${PROJECT_NAME}_ALGORITHM_HEADERS}
  ${${PROJECT_NAME}_PYTHON_HEADERS}
  exception.hpp
  assert.hpp
 )
LIST(REMOVE_DUPLICATES HEADERS)

PKG_CONFIG_APPEND_LIBS (${PROJECT_NAME})
PKG_CONFIG_APPEND_BOOST_LIBS(${BOOST_COMPONENTS})

MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/math")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/spatial")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/multibody")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/multibody/joint")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/multibody/parser")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/multibody/parser/lua")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/tools")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/algorithm")
MAKE_DIRECTORY("${${PROJECT_NAME}_BINARY_DIR}/include/pinocchio/python")


FOREACH(header ${HEADERS})
  GET_FILENAME_COMPONENT(headerName ${header} NAME)
  GET_FILENAME_COMPONENT(headerPath ${header} PATH)
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E ${LINK}
    ${${PROJECT_NAME}_SOURCE_DIR}/src/${header}
    ${${PROJECT_NAME}_BINARY_DIR}/include/${PROJECT_NAME}/${header})
  INSTALL(FILES ${${PROJECT_NAME}_SOURCE_DIR}/src/${header}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}/${headerPath}
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ OWNER_WRITE)
ENDFOREACH(header)

ADD_SUBDIRECTORY(src)
# --- EXECUTABLES --------------------------------------------------------------
# --- EXECUTABLES --------------------------------------------------------------
# --- EXECUTABLES --------------------------------------------------------------
ADD_SUBDIRECTORY(unittest)
ADD_SUBDIRECTORY (benchmark)

IF (BUILD_UTILS)
  ADD_SUBDIRECTORY (utils)
ENDIF (BUILD_UTILS)

SETUP_PROJECT_FINALIZE()

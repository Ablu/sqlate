project(sqlatelib)

cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

option(SQL_ENABLE_NETWORK_WATCHER "Enable the watcher threads for SQL queries over the network. The default is disabled." FALSE)

if (SQL_ENABLE_NETWORK_WATCHER)
  add_definitions(-DSQL_ENABLE_NETWORK_WATCHER)
endif ()

if (ANDROID)
  find_host_package(Qt4 REQUIRED)
  include(Android)
else (ANDROID)
  find_package(Qt4 REQUIRED)
endif()

find_package(Boost 1.40 REQUIRED)

#the user of the library might have defined a larger size
set(MPL_LIMIT_DEFINED "FALSE")
get_directory_property( DirDefs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
foreach(define ${DirDefs})
  if (${define} MATCHES "BOOST_MPL_LIMIT_VECTOR_SIZE=.*")
    set(MPL_LIMIT_DEFINED "TRUE")
  endif()
endforeach()
if (NOT ${MPL_LIMIT_DEFINED})
  add_definitions( -DBOOST_MPL_LIMIT_VECTOR_SIZE=50)
endif()

add_definitions( -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS )
add_definitions( -DQT_STRICT_ITERATORS )
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DQT_NO_CAST_FROM_BYTEARRAY )
add_definitions( -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

enable_testing()

set(LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")

set(LIB_INSTALL_DIR lib${LIB_SUFFIX})
set(PLUGIN_INSTALL_DIR plugins)
set(BIN_INSTALL_DIR bin)
set(INCLUDE_INSTALL_DIR include)

include_directories(${QT_INCLUDES})

set(SQLATE_SRCS
  kdthreadrunner.cpp
  SqlCondition.cpp
  SqlQuery.cpp
  SqlQueryBuilderBase.cpp
  SqlConditionalQueryBuilderBase.cpp
  SqlDeleteQueryBuilder.cpp
  SqlSelectQueryBuilder.cpp
  SqlInsertQueryBuilder.cpp
  SqlUpdateQueryBuilder.cpp
  SqlCreateTable.cpp
  SqlSchema.cpp
  SqlTransaction.cpp
  SqlMonitor.cpp
  SqlQueryManager.cpp
  SqlQueryWatcher.cpp
  SqlUtils.cpp
  PostgresSchema.cpp
  SqlQueryCache.cpp
)

install(FILES
  PostgresSchema.h
  SchemaUpdater.h
  SqlConditionalQueryBuilderBase.h
  SqlCondition.h
  SqlCreateRule.h
  SqlCreateTable.h
  SqlDeleteQueryBuilder.h
  SqlExceptions.h
  SqlGlobal.h
  SqlGrantPermission.h
  SqlGraphviz.h
  Sql.h
  SqlInsertQueryBuilder.h
  SqlInternals_p.h
  SqlMonitor.h
  SqlQueryBuilderBase.h
  SqlQueryCache.h
  SqlQuery.h
  SqlQueryManager.h
  SqlQueryWatcher.h
  SqlSchema.h
  SqlSchema_p.h
  SqlSelect.h
  SqlSelectQueryBuilder.h
  SqlTransaction.h
  SqlUpdateQueryBuilder.h
  SqlUtils.h
  ${CMAKE_CURRENT_BINARY_DIR}/sqlate_export.h
  DESTINATION ${INCLUDE_INSTALL_DIR})

qt4_add_resources(sql_resources SqlResources.qrc )

include(GenerateExportHeader)

add_library(sqlate SHARED ${SQLATE_SRCS} ${sql_resources})
generate_export_header(sqlate)
set_property(TARGET sqlate PROPERTY EXPORT_NAME Sqlate)
target_link_libraries(sqlate
  LINK_PUBLIC
    Qt4::QtSql
    Qt4::QtCore
)

add_library(sqlate_schemaupdate SHARED SchemaUpdater.cpp)
generate_export_header(sqlate_schemaupdate)
set_property(TARGET sqlate_schemaupdate PROPERTY EXPORT_NAME SqlateSchemaUpdate)
target_link_libraries(sqlate_schemaupdate
  LINK_PUBLIC
    sqlate
    Qt4::QtGui
)

set(SQLATE_LIBRARY_VERSION 0.1.0)
set(SQLATE_LIBRARY_SO_VERSION 0)

if (NOT ANDROID)
  set_target_properties(sqlate sqlate_schemaupdate PROPERTIES
          VERSION ${SQLATE_LIBRARY_VERSION}
          SOVERSION ${SQLATE_LIBRARY_SO_VERSION}
  )
endif()


if(NOT ANDROID)
  install(TARGETS sqlate sqlate_schemaupdate EXPORT sqlateTargets
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  )
  install(EXPORT sqlateTargets
    NAMESPACE ::
    FILE SqlateConfig.cmake
    DESTINATION lib/cmake/Sqlate
  )

  set_property(TARGET sqlate sqlate_schemaupdate APPEND PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES
      $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INCLUDE_INSTALL_DIR}>
  )
  set_property(TARGET sqlate sqlate_schemaupdate APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS
      BOOST_MPL_LIMIT_VECTOR_SIZE=50
      BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
  )

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file("${CMAKE_BINARY_DIR}/SqlateConfigVersion.cmake"
    VERSION ${SQLATE_LIBRARY_VERSION}
    COMPATIBILITY AnyNewerVersion
  )
  install(FILES
    "${CMAKE_BINARY_DIR}/SqlateConfigVersion.cmake"
    DESTINATION lib/cmake/Sqlate
  )
  export(TARGETS sqlate sqlate_schemaupdate
    NAMESPACE ::
    FILE SqlateConfig.cmake
  )
endif()

add_subdirectory(tests)

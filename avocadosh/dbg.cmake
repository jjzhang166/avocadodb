# -*- mode: CMAKE; -*-
# these are the install targets for the client package.
# we can't use RUNTIME DESTINATION here.

install_debinfo(
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/strip"
  "${CMAKE_PROJECT_NAME}"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/"
  "${BIN_AVOCADOBENCH}")

install_debinfo(
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/strip"
  "${CMAKE_PROJECT_NAME}"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}"
  "${BIN_AVOCADODUMP}")

install_debinfo(
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/strip"
  "${CMAKE_PROJECT_NAME}/${CMAKE_INSTALL_BINDIR}"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}"
  "${BIN_AVOCADOIMP}")

install_debinfo(
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/strip"
  "${CMAKE_PROJECT_NAME}/${CMAKE_INSTALL_BINDIR}"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}"
  "${BIN_AVOCADORESTORE}")

install_debinfo(
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/strip"
  "${CMAKE_PROJECT_NAME}/${CMAKE_INSTALL_BINDIR}"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}"
  "${BIN_AVOCADOEXPORT}")

install_debinfo(
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/strip"
  "${CMAKE_PROJECT_NAME}/${CMAKE_INSTALL_BINDIR}"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}"
  "${BIN_AVOCADOSH}")

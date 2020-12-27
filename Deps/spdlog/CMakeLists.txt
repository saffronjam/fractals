cmake_minimum_required(VERSION 3.1.0)
project(Spdlog)

file(GLOB INCLUDES "include")

set (Spdlog_INCLUDES "${INCLUDES}" PARENT_SCOPE)
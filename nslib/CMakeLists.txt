cmake_minimum_required(VERSION 3.11)

add_library(nslib STATIC
	"src/ns/lexer.cpp"
	"src/ns/generator.cpp"
 "src/ns/loader.h" "src/ns/loader.cpp")
target_include_directories(nslib PUBLIC "src/")


add_library(ns INTERFACE)
target_include_directories(ns INTERFACE "src/")

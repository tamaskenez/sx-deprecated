FILE(GLOB_RECURSE hdrs *.h)
add_library(sx sx.cpp ${hdrs})

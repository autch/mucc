cmake_minimum_required(VERSION 3.20)
project(mucc LANGUAGES C)

add_executable(mucc
  codegen.c
  mmlcv.c
  mucc.c
  strsub.c
  hash.c
  msrcv.c
  reserved.c

  codegen.h
  hash.h
  mucc.h
  reserved.h
)

if (EMSCRIPTEN)
## https://gitlab.kitware.com/cmake/cmake/issues/15826
set_target_properties(mucc PROPERTIES LINK_FLAGS
  "--memory-init-file 0 \
   -O2 \
   --closure 1 \
   -s EXPORTED_FUNCTIONS='[\"_conv\"]' \
   -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\", \"FS\"]' \
   -s MODULARIZE=1"
)
set_target_properties(mucc PROPERTIES COMPILE_FLAGS
  "--memory-init-file 0 \
   -O2 \
   --closure 1 \
   -s EXPORTED_FUNCTIONS='[\"_conv\"]' \
   -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\", \"FS\"]' \
   -s MODULARIZE=1"
)
endif(EMSCRIPTEN)

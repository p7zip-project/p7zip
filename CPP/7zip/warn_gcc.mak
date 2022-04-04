CFLAGS_WARN_GCC_4_5 = \

CFLAGS_WARN_GCC_6 = \
  -Waddress \
  -Waggressive-loop-optimizations \
  -Wattributes \
  -Wbool-compare \
  -Wcast-align \
  -Wcomment \
  -Wdiv-by-zero \
  -Wduplicated-cond \
  -Wformat-contains-nul \
  -Winit-self \
  -Wint-to-pointer-cast \
  -Wunused \

#  -Wno-strict-aliasing

CFLAGS_WARN_GCC_9 = \
  -Waddress \
  -Waddress-of-packed-member \
  -Waggressive-loop-optimizations \
  -Wattributes \
  -Wbool-compare \
  -Wbool-operation \
  -Wcomment \
  -Wdangling-else \
  -Wdiv-by-zero \
  -Wduplicated-branches \
  -Wduplicated-cond \
  -Wformat-contains-nul \
  -Wimplicit-fallthrough=3 \
  -Winit-self \
  -Wint-in-bool-context \
  -Wint-to-pointer-cast \
  -Wunused \

#  -Wcast-align \
#  -Wcast-align=strict \
#  -Wunused-macros \
#  -Wconversion \
#  -Wno-sign-conversion \

CFLAGS_WARN_GCC_10 = $(CFLAGS_WARN_GCC_9) \
  -Wmaybe-uninitialized \
  -Wmisleading-indentation \

CFLAGS_WARN_GCC_PPMD_UNALIGNED = \
  -Wno-strict-aliasing \


CFLAGS_WARN = $(CFLAGS_WARN_GCC_9) \

#  $(CFLAGS_WARN_GCC_PPMD_UNALIGNED)

  

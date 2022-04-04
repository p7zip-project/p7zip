CFLAGS_WARN_CLANG_3_8_UNIQ = \
  -Wno-reserved-id-macro \
  -Wno-old-style-cast \
  -Wno-c++11-long-long \
  -Wno-unused-macros \

CFLAGS_WARN_CLANG_3_8 = \
  $(CFLAGS_WARN_CLANG_3_8_UNIQ) \
  -Weverything \
  -Wno-extra-semi \
  -Wno-sign-conversion \
  -Wno-language-extension-token \
  -Wno-global-constructors \
  -Wno-non-virtual-dtor \
  -Wno-switch-enum \
  -Wno-covered-switch-default \
  -Wno-cast-qual \
  -Wno-padded \
  -Wno-exit-time-destructors \
  -Wno-weak-vtables \

CFLAGS_WARN_CLANG_12= $(CFLAGS_WARN_CLANG_3_8) \
  -Wno-extra-semi-stmt \
  -Wno-zero-as-null-pointer-constant \
  -Wno-deprecated-dynamic-exception-spec \
  -Wno-c++98-compat-pedantic \
  -Wno-atomic-implicit-seq-cst \
  -Wconversion \
  -Wno-sign-conversion \

CFLAGS_WARN_CLANG_ADDITIONAL = \
  -Wno-suggest-override \
  -Wno-suggest-destructor-override \
  -Wno-documentation \
  -Wno-documentation-unknown-command \
  -Wno-used-but-marked-unused \
  -Wno-undef \
  -Wno-implicit-fallthrough \
  -Wno-empty-translation-unit \
  -Wno-vla \
  -Wno-bad-function-cast \
  -Wno-missing-prototypes \
  -Wno-cast-align \
  -Wno-shorten-64-to-32 \
  -Wno-implicit-int-conversion \
  -Wno-conditional-uninitialized \
  -Wno-missing-variable-declarations \
  -Wno-newline-eof \

CFLAGS_WARN_1 = \
  -Wno-deprecated-copy-dtor \




CFLAGS_WARN = $(CFLAGS_WARN_CLANG_12) $(CFLAGS_WARN_CLANG_ADDITIONAL) $(CFLAGS_WARN_1)

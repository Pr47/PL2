#!/bin/sh

which clang &> /dev/null
if [ "$?" == "0" ]; then
  clang pl2.c \
    -Weverything \
    -Wno-zero-length-array \
    -Wno-format-nonliteral \
    -Wno-cast-qual \
    -Wno-padded \
    -fPIC -shared -ldl -o libpl2.so
  clang main.c \
    -Weverything \
    -Wno-zero-length-array \
    -Wno-padded \
    -L. -lpl2 \
    -o pl2
  clang pldbg.c \
    -Weverything \
    -Wno-zero-length-array \
    -Wno-padded \
    -L. -lpl2 \
    -fPIC -shared -o libpldbg.so
  clang echo.c \
    -Weverything \
    -Wno-zero-length-array \
    -Wno-padded \
    -L. -lpl2 \
    -fPIC -shared -o libecho.so
  clang ezecho.c \
    -Wall -Wextra \
    -fPIC -shared -o libezecho.so
else
  cc pl2.c -Wall -Wextra -fPIC -shared -ldl -o libpl2.so
  cc main.c -Wall -Wextra -L. -lpl2 -o pl2
  cc pldbg.c -Wall -Wextra -fPIC -shared -L. -lpl2  -o libpldbg.so
  cc echo.c -Wall -Wextra -fPIC -shared -L. -lpl2 -o libecho.so
  cc ezecho.c -Wall -Wextra -fPIC -shared -o libezecho.so
fi


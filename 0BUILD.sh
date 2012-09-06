#!/bin/sh
export ACLOCAL='aclocal -I ../autoconf'
autoreconf --install
./configure --prefix $PREFIX
make clean all install
cp src/*.sp?t $PREFIX/lib/ocaml/site-lib/camlimages/
(cd examples/liv; make opt; /bin/cp liv.opt $PREFIX/bin)

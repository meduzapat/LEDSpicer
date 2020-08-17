#!/bin/bash
aclocal
[ -d config ] || mkdir config
[ -d m4 ] || mkdir m4
autoreconf --force --install -I config -I m4
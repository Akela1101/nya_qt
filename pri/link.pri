# Нужно задать переменные libPath и libs
include(include.pri)

PRE_TARGETDEPS += $$libPath
LIBS += $$_L($$libPath) $$_l_d($$libs)

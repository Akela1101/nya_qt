# Нужно задать переменные libPath и libs
include(include.pri)

PRE_TARGETDEPS += $$libPath
LIBS += $$_L($$libPath) $$_l_d($$libs)

*-g++ {
	LIBS += -Wl,--end-group # --start-group in common.pri
}

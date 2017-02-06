contains(TEMPLATE, app) {
# app
	DESTDIR = bin/
}
else {
# lib
	DESTDIR = ./

	!CONFIG(dll) {
		CONFIG -= shared
		CONFIG += static
	}

	# add d to debug lib
	CONFIG(debug, debug|release) {
		TARGET = $${TARGET}d
	}
}

CONFIG(debug, debug|release) {
# debug
	DEFINES += _DEBUG NDEBUG
	build_mode = debug
}
else {
# release
	build_mode = release
}
OBJECTS_DIR = build/$${build_mode}/obj
MOC_DIR = build/$${build_mode}/moc
UI_DIR = build/$${build_mode}/ui_h
RCC_DIR = build/$${build_mode}/rcc


*-g++ {
	QMAKE_CXXFLAGS *= \
		-std=c++0x \
		-fopenmp \
		-U__STRICT_ANSI__ \
		-MMD \ # add some headers
		-Wno-unused-local-typedefs \
		-Wno-sign-compare \     # for u numbers
		-Wno-old-style-cast \   # () instead of casts
		-pipe # maybe faster but uses more RAM

	QMAKE_LFLAGS *= -fopenmp

	win32 {
		QMAKE_CXXFLAGS *= \
			-fno-strict-aliasing \  # silent pointer dereferencing
			-fpermissive \ # disables some danger errors!
			-Wno-type-limits # disables: "comparison is always false"

		QMAKE_LFLAGS *= \
			--enable-runtime-pseudo-reloc # for dll ( http://www.mingw.org/wiki/sampleDLL )
	}

	# static & start group of libs
	LIBS += -static-libgcc -static -Wl,--start-group
}
win32 {
	# boost
	boostRoot = $$(BOOST_ROOT)
	isEmpty(boostRoot) {
		!build_pass:error(No environment variable `BOOST_ROOT`. E.g.: BOOST_ROOT=c:\\boost)
	} else {
		INCLUDEPATH += $$boostRoot
		LIBS += -L$${boostRoot}/stage/lib
	}

	# openssl
	sslRoot = $$(SSL_ROOT)
	isEmpty(sslRoot) {
		!build_pass:error(No environment variable `SSL_ROOT`. E.g.: SSL_ROOT=c:\\openssl)
	} else {
		INCLUDEPATH += $${sslRoot}/include
		libPath += $${sslRoot}/lib

		# Static build must include ssl! (-openssl-linked)
		# define QT_STATIC = 1
		isEmpty(QT_STATIC) {
			LIBS += -lcrypto
		}
	}

	# backtrace
	LIBS += -ldbghelp
}
else: *-g++ {
	# backtrace
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE += -O1
	QMAKE_LFLAGS *= -rdynamic
}

# --- Functions ---
# concatenate */src
defineReplace(_src) {
	path =
	for(arg, ARGS) {
		path += $${arg}/src
	}
	return($$path)
}
# concatenate -L*
defineReplace(_L) {
	path =
	for(arg, ARGS) {
		path += -L$${arg}
	}
	return($$path)
}
# concatenate -l*[d]
defineReplace(_l_d) {
	path =
	for(arg, ARGS) {
		CONFIG(debug, debug|release) {
			path = -l$${arg}d $$path
		}
		else {
			path = -l$${arg} $$path
		}
	}
	return($$path)
}

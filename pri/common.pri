# Приложение.
contains(TEMPLATE, app) {
	DESTDIR = bin/
}
else {
	DESTDIR = ./
}

# Библиотека.
contains(TEMPLATE, lib) {	
	!CONFIG(dll) : CONFIG += static

	# Добавить _d к имени отладочной библиотеки.
	CONFIG(debug, debug|release) {
		TARGET = $${TARGET}_d
	}
}

# Дополнительные #define-ы
CONFIG(debug, debug|release) {
	DEFINES += _DEBUG NDEBUG
}

# Задать папки для сборки
CONFIG(debug, debug|release) {
	build_mode = debug
}
else {
	build_mode = release
}
OBJECTS_DIR = build/$${build_mode}/obj
MOC_DIR = build/$${build_mode}/moc
UI_HEADERS_DIR = build/$${build_mode}/ui_h
RCC_DIR = build/$${build_mode}/rcc

# GCC
QMAKE_CXXFLAGS *= \
	-std=c++0x \ # Собирать новым сиплюсплюсом ввиду его особой няшности.
	-fopenmp \ # Open MP
	-U__STRICT_ANSI__ \ # На всякий случай включить макрос для ansi.
	-MMD \ # Добавить несистемные хедеры в зависимости.
	-Wno-unused-local-typedefs # отрубление ворнинга.

QMAKE_LFLAGS *= -fopenmp
LIBS *= -static-libgcc -static

DEFINES += QXT_STATIC


# Мои функции
defineReplace(_src) {
	path =
	for(arg, ARGS) {
		path += $${arg}/src
	}
	return($$path)
}

defineReplace(_L) {
	path =
	for(arg, ARGS) {
		path += -L$${arg}
	}
	return($$path)
}

# Библиотеки подключаются в обратном порядке,
# чтобы ld мог найти зависимости. Подробнее:
# http://www.delorie.com/djgpp/v2faq/faq8_10.html
#
# Если вдруг случится, что зависимость кольцевая,
# надо юзать: -Wl,--start-group,-lLib1,-lLib2,--end-group
# как и написано в этом факе.
defineReplace(_l_d) {
	path =
	for(arg, ARGS) {
		CONFIG(debug, debug|release) {
			path = -l$${arg}_d $$path
		}
		else {
			path = -l$${arg} $$path
		}
	}
	return($$path)
}

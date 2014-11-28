QT = core
TEMPLATE = lib

include(pri/vars.pri)
libPath = $${pRoot}/qxtcore
include($$includePri)


HEADERS += \
    src/ApplicationNya.hpp \
    src/ConfigNya.hpp \
    src/DirNya.hpp \
    src/Nya.hpp \
    src/SingletonNya.hpp \
    src/TimeNya.hpp

SOURCES += \
    src/ApplicationNya.cpp \
    src/ConfigNya.cpp \
    src/DirNya.cpp \
    src/TimeNya.cpp

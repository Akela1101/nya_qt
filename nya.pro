QT = core
TEMPLATE = lib

include(pri/vars.pri)
libPath = $${d_root}/qxtcore
include($$includePri)


HEADERS += \
    src/ApplicationNya.hpp \
    src/ConfigNya.hpp \
    src/DirNya.hpp \
    src/Nya.hpp \
    src/SingletonNya.hpp \
    src/TimeNya.hpp \
    src/EnumNya.hpp \
    src/SwitchNya.hpp \
    src/XmlNya.hpp

SOURCES += \
    src/ApplicationNya.cpp \
    src/ConfigNya.cpp \
    src/DirNya.cpp \
    src/TimeNya.cpp \
    src/XmlNya.cpp

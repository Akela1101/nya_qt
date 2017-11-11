# Нужно задать переменную libPath
include(common.pri)

INCLUDEPATH += src $$_src($$libPath)
DEPENDPATH *= $${INCLUDEPATH}

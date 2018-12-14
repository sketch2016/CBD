#-------------------------------------------------
#
# Project created by QtCreator 2018-11-07T14:18:42
#
#-------------------------------------------------

QT       += core gui serialbus serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CBD
TEMPLATE = app

QMAKE_PRE_LINK += mkdir -p $$OUT_PWD/config_file &\
mkdir -p $$OUT_PWD/config_file/distort1 &\
mkdir -p $$OUT_PWD/config_file/distort2 &\
cp -f $$PWD/config_file/distort1/camMtx.xml $$OUT_PWD/config_file/distort1/camMtx.xml &\
cp -f $$PWD/config_file/distort1/distMtx.xml $$OUT_PWD/config_file/distort1/distMtx.xml &\
cp -f $$PWD/config_file/distort1/perspMtx.xml $$OUT_PWD/config_file/distort1/perspMtx.xml &\
cp -f $$PWD/config_file/distort2/camMtx.xml $$OUT_PWD/config_file/distort2/camMtx.xml &\
cp -f $$PWD/config_file/distort2/distMtx.xml $$OUT_PWD/config_file/distort2/distMtx.xml &\
cp -f $$PWD/config_file/distort2/perspMtx.xml $$OUT_PWD/config_file/distort2/perspMtx.xml &\
cp -f $$PWD/config.ini $$OUT_PWD/config.ini &\
cp -f $$PWD/logblacklist.ini $$OUT_PWD/logblacklist.ini
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        ui/mainwindow.cpp \
    util/logger.cpp \
    hardware/icamera.cpp \
    util/socketutils.cpp \
    hardware/poecamera.cpp \
    ui/dialog.cpp \
    communication/communication.cpp \
    util/imageutil.cpp \
    config/configservice.cpp \
    manager/servicemanager.cpp \
    storage/storageservice.cpp \
    hardware/illuminantcontroller.cpp \
    hardware/optoelectroniccontroller.cpp \
    hardware/serialport.cpp \
    controller/controller.cpp \
    ui/imagebroswer.cpp \
    algorithm/algorithmwrapper.cpp \
    algorithm/imageprocessor.cpp \
    algorithm/stationoneprocessor.cpp \
    algorithm/baseprocessor.cpp \
    algorithm/stationtwoprocessor.cpp \
    #algorithm/testimageprocess.cpp


HEADERS += \
        include/ui/mainwindow.h \
        include/ui/dialog.h \
    include/util/logger.h \
    include/defines.h \
    include/hardware/icamera.h \
    include/hardware/poecamera.h \
    include/util/socketutils.h \
    include/util/autolock.h \
    include/communication/communication.h \
    include/util/imageutil.h \
    include/config/configservice.h \
    include/manager/servicemanager.h \
    include/storage/storageservice.h \
    include/hardware/cframeinfo.h \
    include/hardware/illuminatcontroller.h \
    include/hardware/optoelectroniccontroller.h \
    include/hardware/serialport.h \
    include/controller/controller.h \
    include/ui/imagebroswer.h \
    include/algorithm/algorithmwrapper.hpp \
    include/algorithm/imageprocessor.hpp \
    include/algorithm/stationoneprocessor.hpp \
    include/algorithm/baseprocessor.hpp \
    include/algorithm/stationtwoprocessor.hpp \
    #include/algorithm/testimageprocess.h


FORMS += \
        forms/mainwindow.ui \
    forms/dialog.ui \
    forms/imagebroswer.ui

INCLUDEPATH += /usr/local/include \
    /usr/local/include/opencv \
    /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_aruco.so \
/usr/local/lib/libopencv_bgsegm.so \
/usr/local/lib/libopencv_bioinspired.so \
/usr/local/lib/libopencv_calib3d.so \
/usr/local/lib/libopencv_ccalib.so \
/usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_datasets.so \
/usr/local/lib/libopencv_dnn.so \
/usr/local/lib/libopencv_dpm.so \
/usr/local/lib/libopencv_face.so \
/usr/local/lib/libopencv_features2d.so \
/usr/local/lib/libopencv_flann.so \
/usr/local/lib/libopencv_freetype.so \
/usr/local/lib/libopencv_fuzzy.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_imgcodecs.so \
/usr/local/lib/libopencv_img_hash.so \
/usr/local/lib/libopencv_imgproc.so \
/usr/local/lib/libopencv_line_descriptor.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_objdetect.so \
/usr/local/lib/libopencv_optflow.so \
/usr/local/lib/libopencv_phase_unwrapping.so \
/usr/local/lib/libopencv_photo.so \
/usr/local/lib/libopencv_plot.so \
/usr/local/lib/libopencv_reg.so \
/usr/local/lib/libopencv_rgbd.so \
/usr/local/lib/libopencv_saliency.so \
/usr/local/lib/libopencv_shape.so \
/usr/local/lib/libopencv_stereo.so \
/usr/local/lib/libopencv_stitching.so \
/usr/local/lib/libopencv_structured_light.so \
/usr/local/lib/libopencv_superres.so \
/usr/local/lib/libopencv_surface_matching.so \
/usr/local/lib/libopencv_text.so \
/usr/local/lib/libopencv_tracking.so \
/usr/local/lib/libopencv_videoio.so \
/usr/local/lib/libopencv_video.so \
/usr/local/lib/libopencv_videostab.so \
/usr/local/lib/libopencv_xfeatures2d.so \
/usr/local/lib/libopencv_ximgproc.so \
/usr/local/lib/libopencv_xobjdetect.so \
/usr/local/lib/libopencv_xphoto.so \

RESOURCES += \
    resource.qrc

LIBS += -L/usr/lib/gcc/x86_64-linux-gnu/5  -lgomp
LIBS += -L/opt/DahuaTech/MVViewer/lib -lMVSDK
LIBS += -L/opt/DahuaTech/MVViewer/lib -lImageConvert
LIBS += -L/opt/DahuaTech/MVViewer/lib/GenICam/bin/Linux64_x64 -lGCBase_gcc421_v3_0 -lGenApi_gcc421_v3_0 -lLog_gcc421_v3_0 -llog4cpp_gcc421_v3_0 -lNodeMapData_gcc421_v3_0 -lXmlParser_gcc421_v3_0 -lMathParser_gcc421_v3_0 -lrt -lpthread
LIBS += -lpthread

INCLUDEPATH += /opt/DahuaTech/MVViewer/share/C++/QtSample/include


add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QT_INCLUDES} ${KDE4_INCLUDES})
include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${QTOAUTH_INCLUDE_DIR}
                    ${QCA2_INCLUDE_DIR}
                    ${PHONON_INCLUDE_DIR}
                    )

set(moefm_SRCS app.cpp main.cpp logindialog.cpp
    requesttokenjob.cpp accesstokenjob.cpp networkaccessmanager.cpp reply.cpp
    requestdatajob.cpp mainwindow.cpp controller.cpp themeiconprovider.cpp
    )

SET(moefm_RCCS qml.qrc)

QT4_ADD_RESOURCES(moefm_RC_SRCS ${moefm_RCCS})
add_executable(kmoefm ${moefm_SRCS} ${moefm_RC_SRCS})
set_target_properties(kmoefm PROPERTIES AUTOMOC TRUE)
target_link_libraries(kmoefm
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${QT_QTNETWORK_LIBRARY}
                      ${QT_QTWEBKIT_LIBRARY}
                      ${QTOAUTH_LIBRARY}
                      ${QT_QTDECLARATIVE_LIBRARY}
                      ${QJSON_LIBRARY}
                      ${PHONON_LIBRARY}
                      ${QCA2_LIBRARIES})
install(TARGETS kmoefm RUNTIME DESTINATION bin)

install(PROGRAMS kmoefm.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

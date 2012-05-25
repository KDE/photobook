find_package(KDE4Internal REQUIRED)

set(photobook_SRCS
    Importer/Database.cpp
    Importer.cpp
    ImageRequest.cpp
    CategorizedSourcesModel.cpp
    ImporterModel.cpp
    ImageModel.cpp
    ThumbnailProvider.cpp
    KIconProvider.cpp
    ImageProvider.cpp
    qmlapplicationviewer.cpp
    main.cpp
)

kde4_add_executable(photobook
    ${photobook_SRCS}
)

target_link_libraries(photobook
    ${KDE4_KDEUI_LIBS}
    ${KDE4_SOLID_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${XAPIAN_LIBRARIES}
    ${KEXIV2_LIBRARIES}
)

set_target_properties(photobook PROPERTIES INSTALL_RPATH ${LIB_INSTALL_DIR}/photobook)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/photobook DESTINATION ${BIN_INSTALL_DIR} RENAME photobook)

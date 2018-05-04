cmake_minimum_required(VERSION 2.8)
project(schema CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(lib/ddui)

list(APPEND ASSETS
    ${ddui_ASSETS}
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/SFRegular.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/PTMono.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/data.txt
)

add_executable(schema MACOSX_BUNDLE WIN32 ${SOURCES} ${ASSETS})

if(APPLE)

    set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.icns)
    set_source_files_properties(${ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    
    set_target_properties(schema PROPERTIES
        MACOSX_BUNDLE_ICON_FILE icon.icns
        MACOSX_BUNDLE_INFO_PLIST ${ddui_MACOSX_BUNDLE}
        RESOURCE ${ICON}
    )
    
    set_source_files_properties(
        ${ASSETS}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources/assets
    )
    set(MACOSX_BUNDLE_BUNDLE_NAME "Schema")
        
else()

    file(COPY ${ASSETS} DESTINATION "assets/")

endif()

target_link_libraries(schema ddui)

cmake_minimum_required(VERSION 3.21)
project(ReversiGUI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6の検出
find_package(Qt6 COMPONENTS 
    Core 
    Gui 
    Widgets
    Network
    REQUIRED
)

# ソースファイル
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/boardwidget.cpp
    src/boardwidget.h
    src/gamemanager.cpp
    src/gamemanager.h
    src/aiengine.h
    src/minmaxengine.cpp
    src/minmaxengine.h
    src/simplehintengine.cpp
    src/simplehintengine.h
    ../osero_6x6/board.cpp
    ../osero_6x6/config/app_config.cpp
    ../osero_6x6/evaluator.cpp
    ../osero_6x6/hint/gemini_client.cpp
    ../osero_6x6/hint/hint_formatter.cpp
    ../osero_6x6/hint/llm_client_factory.cpp
    ../osero_6x6/hint/llm_hint_engine.cpp
    ../osero_6x6/hint/local_hint_engine.cpp
    ../osero_6x6/hint/minmax_hint_evaluator.cpp
    ../osero_6x6/hint/openai_client.cpp
    ../osero_6x6/hint/prompt_builder.cpp
    resources/resources.qrc
)

# 実行可能ファイル
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Qtライブラリをリンク
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Include paths
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../osero_6x6
)

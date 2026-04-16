cmake_minimum_required(VERSION 3.15)

project(BeatBattleVST VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set macOS deployment target to avoid compatibility issues
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version")
endif()

# Add JUCE subdirectory (you'll need to clone JUCE or use FetchContent)
# Option 1: If JUCE is in a parent directory
# add_subdirectory(../JUCE ./JUCE)

# Option 2: Fetch JUCE automatically
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
)
FetchContent_MakeAvailable(JUCE)

# WebSocket functionality is now handled by the embedded website

juce_add_plugin(BeatBattleVST
    COMPANY_NAME "BeatBattle"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Btbl
    PLUGIN_CODE Btb1
    FORMATS VST3 Standalone
    PRODUCT_NAME "BeatBattle VST"
)

juce_generate_juce_header(BeatBattleVST)

target_sources(BeatBattleVST
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor_WebView.cpp
)

target_compile_definitions(BeatBattleVST
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(BeatBattleVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

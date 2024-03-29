cmake_minimum_required(VERSION 3.15)

project(JOMSVIKING VERSION 0.0.1)


set(CMAKE_CXX_STANDARD 17)
add_subdirectory(Lib)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

juce_add_plugin(Jomsviking
	COMPANY_NAME "Tauri Invictus"
	COMPANY_COPYRIGHT "GPLv3"
	COMPANY_WEBSITE "arsdotfm.wordpress.com"
	COMPANY_EMAIL "embryon.id@gmail.com"
	PLUGIN_MANUFACTURER_CODE Manu
	PLUGIN_CODE Tjax
	FORMATS VST3
	VST3_CATEGORIES Dynamics Fx
)

juce_generate_juce_header(Jomsviking)

target_compile_definitions(Jomsviking
  PUBLIC
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
)

add_subdirectory(Source)
include_directories(Source)

juce_add_binary_data(Jomsviking_BinaryData
  SOURCES
    "Resource/Jomsviking Layout.png"
)

target_link_libraries(Jomsviking
  PRIVATE
    Jomsviking_BinaryData
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    ff_meters
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

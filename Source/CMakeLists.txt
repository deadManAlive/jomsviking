add_subdirectory(dsp)
add_subdirectory(util)

target_sources(Jomsviking
    PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
)
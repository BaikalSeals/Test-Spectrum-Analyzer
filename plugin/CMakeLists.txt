cmake_minimum_required(VERSION 3.22)
# Version specification required by JUCE
project(BaikalSpectrumAnalyzer VERSION 0.1.0)


juce_add_plugin(${PROJECT_NAME}
  COMPANY_NAME "Baikal Sound"
  ICON_BIG "Assets/SpectralIcon.png"
  ICON_SMALL "Assets/SpectralIcon.png"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEED_MIDI_OUTPUT FALSE
  PLUGIN_MANUFACTURER_CODE BAKL
  PLUGIN_CODE BEQT 
  FORMATS VST3
  MICROPHONE_PERMISSION_ENABLED TRUE
  PRODUCT_NAME "Baikal Spectrum Analyzer"
)

juce_generate_juce_header(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PRIVATE 
    source/PluginEditor.cpp
    source/PluginProcessor.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE 
  include/
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags

)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    JUCE_WEB_BROWSER=0 
    JUCE_USE_CURL=0 
    JUCE_VST3_CAN_REPLACE_VST2=0 
)

if(MSVC)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE 
      _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
    )
endif()

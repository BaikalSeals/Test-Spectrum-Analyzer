#pragma once

#include <JuceHeader.h>

class TTCOKnob : public Slider{
    public: 

        TTCOKnob(): Slider(SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextEntryBoxPosition::TextBoxLeft){
            setMouseCursor(MouseCursor::PointingHandCursor);
        }

        void setLook(TTCOLook& a){
            setLookAndFeel(&a);
        }

        ~TTCOKnob(){
            setLookAndFeel(nullptr); 
        }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTCOKnob)
}; 
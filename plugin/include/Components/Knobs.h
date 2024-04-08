#pragma once

#include <JuceHeader.h>

class TTCOKnob : public Slider{
    public: 
        TTCOKnob(): Slider(SliderStyle::RotaryHorizontalVerticalDrag, TextEntryBoxPosition::NoTextBox){
            setMouseCursor(MouseCursor::PointingHandCursor);
        }

        void setLook(TTCOLook& a){
            setLookAndFeel(&a);
        }

        ~TTCOKnob(){
            setLookAndFeel(nullptr); 
        }
}; 
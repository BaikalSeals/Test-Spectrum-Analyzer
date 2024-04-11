#pragma once

#include <JuceHeader.h>
//#include "../../Assets/BinaryData.h"

//Allows for the creation of different rotary designs into one class

class TTCOLook : public LookAndFeel_V4{
    
    public: 

        TTCOLook(){
        }

        //Overriding drawRotarySlider to use an image to draw rotary sliders
        void drawRotarySlider(Graphics& g, int x, int y, int width, int height, 
        float sliderPosProportional, float , float , Slider& )override{

            //Number of frames total in the image
            const int frames = 90; 
            //Determines the specific frame given the slider position
            const int frameId = static_cast<int>(ceil(sliderPosProportional*(static_cast<float>(frames) - 1.f))); 
            int sourceHeight = ((RotaryImage.getHeight())/frames);
            int sourceWidth = RotaryImage.getWidth();
            int sourceY = sourceHeight*frameId; 
            

            //Draws the image. Draws by fitting a specific part of the image into the given location of
            //a set of bounds. First numbers x,y,width and height are the location drawn to. 
            g.drawImage(RotaryImage, x, y, width, (int)(height*0.8), 0 ,sourceY, sourceWidth, sourceHeight);
            }

        //Sets the image used for the draw rotary function
        void setLook(int x){
            //Sets image to the blue knob with grill
            if(x==1){
                RotaryImage = ImageCache::getFromMemory(
                BinaryData::TTCOKnobV2_B_png, BinaryData::TTCOKnobV2_B_pngSize); 
            } else {
            //Currently, any other number is set to Lime
                RotaryImage = ImageCache::getFromMemory(
                BinaryData::TTCOKnobV2_G_png, BinaryData::TTCOKnobV2_G_pngSize); 
            }
        }

        //Allows for text in textbox to be set a specific way
        void drawLabel(Graphics& g, Label& label)override{
            //First getting original font
            Font originalFont = label.getFont(); 
            //Create new font with specific size to fit textbox better
            Font newFont(originalFont.getTypefaceName(), 10, Font::plain); 

            label.setFont(newFont);
            g.setColour(label.findColour(Label::textColourId)); 
            g.setFont(label.getFont()); 
            //Sets the text area to a specific size 
            Rectangle<int> textArea(label.getBorderSize().subtractedFrom(label.getLocalBounds()));
            //Draws the text and sets conditions to be met in order for the text to be truncated
            g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         jmax(1, (int)(textArea.getHeight() / label.getFont().getHeight())),
                         0);
        }

        //Adding text box to rotary slider
        Slider::SliderLayout getSliderLayout (Slider& slider)override{
            Slider::SliderLayout layout; 
            layout.sliderBounds = slider.getLocalBounds(); 
            int removedHeight; 
            removedHeight = (int)(layout.sliderBounds.getHeight()*0.7); 
            int removedWidth = (int)(layout.sliderBounds.getWidth()*0.1); 
            layout.textBoxBounds = slider.getLocalBounds();
            //Truncated the textbox bounds in order to fit it in the right location
            layout.textBoxBounds.removeFromTop(removedHeight); 
            layout.textBoxBounds.removeFromLeft(removedWidth); 
            layout.textBoxBounds.removeFromRight(removedWidth); 

            return layout; 
        }

    private:
        Image RotaryImage; 


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TTCOLook)
}; 

#pragma once

#include <JuceHeader.h>
//#include "../../Assets/BinaryData.h"

//Allows for the creation of different rotary designs into one class

class TTCOLook : public LookAndFeel_V4{
    //DONT FORGET TO WRITE PUBLIC NEXT TIME IDIOT
    public: 

        TTCOLook(){
        }

        void drawRotarySlider(Graphics& g, int x, int y, int width, int height, 
        float sliderPosProportional, float , float , Slider& )override{

            const auto frames = 90; 
            const auto frameId = static_cast<int>(ceil(sliderPosProportional*(static_cast<float>(frames) - 1.f))); 
            int sourceHeight = ((RotaryImage.getHeight())/frames);
            int sourceWidth = RotaryImage.getWidth();
            int sourceY = sourceHeight*frameId; 

            g.drawImage(RotaryImage, x, y, width, height, 0 ,sourceY, sourceWidth, sourceHeight);
            }


            void setLook(int x){
                if(x==1){
                    RotaryImage = ImageCache::getFromMemory(
                    BinaryData::TTCOKnobV2_B_png, BinaryData::TTCOKnobV2_B_pngSize); 
                } else {
                //Currently, any other number is set to Lime
                    RotaryImage = ImageCache::getFromMemory(
                    BinaryData::TTCOKnobV2_G_png, BinaryData::TTCOKnobV2_G_pngSize); 
                }
            }


    private:
        Image RotaryImage; 

}; 

#pragma once

#include <JuceHeader.h>


namespace Gui 
{

  class VerticalMeter : public Component{
    public:
      //This function creates the visuals for the level VerticalMeter 
      void paint(Graphics& g) override{
        //First it finds and sets the bounds of the meter
        auto bounds = getLocalBounds().toFloat(); 
        //Next we set the background color to a muted white 
        //g.setColour(Colours::white.withBrightness(0.4f)); 
        g.setColour(Colours::black.withBrightness(0.1f)); 
        //Now we fill the rectangle with that muted white as well as round off the corners by 5.0 
        g.fillRoundedRectangle(bounds, 5.f); 
       
        //Now creating a True peak area in the volume meter
        //It will be more transparent and behind the main volume meter 
        ColourGradient truePeakGradient; 
        truePeakGradient = ColourGradient::vertical(
            Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(255), 0.2f), 
            Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.45f),
            bounds);

        truePeakGradient.addColour(0.25,Colour(static_cast<juce::uint8>(102), static_cast<juce::uint8>(255), static_cast<juce::uint8>(204), 0.45f)); 

        g.setGradientFill(truePeakGradient); 

        const auto scaledX1 = jmap(peak, -60.f, +6.f, 0.f, static_cast<float>(getHeight())); 
        g.fillRoundedRectangle(bounds.removeFromBottom(scaledX1), 5.0f); 
        //Now we have created the background of the level meter but we need the foreground 
        //Here we create a colorgradient and then we set that color gradient object to a gradient object with specific
        //values 
        ColourGradient SealTeal;
        SealTeal = ColourGradient::vertical(
            Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(255), 0.4f), 
            Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.9f),
            bounds);
        
        //Now we add a color to the gradient object we have
        SealTeal.addColour(0.25,Colour(static_cast<juce::uint8>(102), static_cast<juce::uint8>(255), static_cast<juce::uint8>(204), 0.9f)); 
        //Now we set g to set the gradient fill to the gradient object we created 
        g.setGradientFill(SealTeal);

        //We can now map from -60 to 6 to the width of the rounded rectangle
        //Here we need to also make sure all variables are floats since the jmap function
        //is a templated function and therefore all variables need to be the same type
        const auto scaledX2 = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
        //Now that we know how much the foreground rectangle should be (width scaledX) then we 
        //call removeFromLeft to remove up until scaledX is reached and that is the value of 
        //how much we fill. Now with that variable checked we again just tell it to fill a 
        //rounded rectangle by the amount needed to be filled and round off the edges by 5.0
        g.fillRoundedRectangle(bounds.removeFromBottom(scaledX2), 5.0f); 

      }

      void setPeak(const float value){
        peak = value; 
      }

      void setLevel(const float value){
        level = value; 
      }

    private: 
      float level = -60.f;
      float peak = -60.f; 
    }; 
}

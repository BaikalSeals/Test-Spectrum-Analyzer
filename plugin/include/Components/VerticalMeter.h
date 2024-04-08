#pragma once

#include <JuceHeader.h>


namespace Gui 
{
  //This vertical meter gives info about volume level and true peak level 
  //True peak is put behind volume and is more transparent as is typically seen in plugins 
  class VerticalMeter : public Component{
    public:
      //This function creates the visuals for the level VerticalMeter 
      void paint(Graphics& g) override{
        //First it finds and sets the bounds of the meter
        auto bounds = getLocalBounds().toFloat(); 
        //A circular rectangle will be created first 
        //The meters will overlay this rectangle in order to create a boarder 
        g.setColour(Colours::black); 
        g.fillRoundedRectangle(bounds, 5.f); 

        //Now these scales are created that mark out the volume levels 
        const auto scaled0 = jmap(0.f, -60.f, 6.f, 2.f, static_cast<float>(getHeight()-2)); 
        const auto scaled12 = jmap(-12.f, -60.f, 6.f, 2.f, static_cast<float>(getHeight()-2)); 
        const auto scaled24 = jmap(-24.f, -60.f, 6.f, 2.f, static_cast<float>(getHeight()-2)); 
        const auto scaled36 = jmap(-36.f, -60.f, 6.f, 2.f, static_cast<float>(getHeight()-2)); 
        const auto scaled48 = jmap(-48.f, -60.f, 6.f, 2.f, static_cast<float>(getHeight()-2)); 
        //These scales generate the movement for the volume/peak meters
        const auto scaledX1 = jmap(peak, -60.f, 6.f, 2.f, static_cast<float>(getHeight()-2)); 
        const auto scaledX2 = jmap(level, -60.f, 6.f, 2.f, static_cast<float>(getHeight()-2)); 

        const auto scalecolor = jmap(-6.f, -60.f, 6.f, 0.f, 1.f); 

        float drawWidth = static_cast<float>(getWidth()-2); 
        float drawHeight = static_cast<float>(getHeight()-3); 

        //Creation of an shape in order to create the meters 
        //This first meter is the peak meter and we create this first since it will be 
        //behind the volume meter
        juce::Path pmeter; 
        pmeter.startNewSubPath(2,drawHeight); 
        pmeter.lineTo(2,static_cast<float>(getHeight()-scaledX1)); 
        pmeter.lineTo(drawWidth,static_cast<float>(getHeight()-scaledX1)); 
        pmeter.lineTo(drawWidth,drawHeight); 
        pmeter.closeSubPath(); 
        //fill must be created in order to set the fill type as a gradient 
        //This allows us to use fillPath for the gradient 
        auto fill = FillType(); 

        //Setting up the gradient color and in what direction it goes
        ColourGradient sealTealGradient; 
        sealTealGradient = ColourGradient::vertical(
            Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.3f), 3,
            Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.2f), drawHeight
          );
        //Adds middle color (teal)
        sealTealGradient.addColour((1-scalecolor),Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(255), static_cast<juce::uint8>(222), 0.2f)); 

        //Fill type is set to the gradient created
        fill.setGradient(sealTealGradient); 
        //Set the fill type to the object fill
        g.setFillType(fill); 
        //Fill the path (color (in this case gradient) is already set)
        g.fillPath(pmeter); 


        //This section is for the volume, same steps and process
        Path vmeter; 
        vmeter.startNewSubPath(2,drawHeight); 
        vmeter.lineTo(2,static_cast<float>(getHeight()-scaledX2)); 
        vmeter.lineTo(drawWidth,static_cast<float>(getHeight()-scaledX2)); 
        vmeter.lineTo(drawWidth,drawHeight); 
        vmeter.closeSubPath(); 

        sealTealGradient = ColourGradient::vertical(
            Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.6f), 3,
            Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.5f), drawHeight
          );
        sealTealGradient.addColour((1-scalecolor),Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(255), static_cast<juce::uint8>(222), 0.5f)); 

        fill.setGradient(sealTealGradient); 

        g.setFillType(fill); 

        g.fillPath(vmeter); 

        //Now creating the markings of the meter that list out the dbs 
        g.setColour(Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0),0.7f)); 
        Path zeroLine; 
        zeroLine.startNewSubPath(2,(getHeight()-scaled0));
        zeroLine.lineTo(drawWidth,(getHeight()-scaled0)); 
        g.strokePath(zeroLine, PathStrokeType(1.5f)); 

        zeroLine.startNewSubPath(2,(getHeight()-scaled12));
        zeroLine.lineTo(drawWidth,(getHeight()-scaled12)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

        zeroLine.startNewSubPath(2,(getHeight()-scaled24));
        zeroLine.lineTo(drawWidth,(getHeight()-scaled24)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

        zeroLine.startNewSubPath(2,(getHeight()-scaled36));
        zeroLine.lineTo(drawWidth,(getHeight()-scaled36)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

        zeroLine.startNewSubPath(2,(getHeight()-scaled48));
        zeroLine.lineTo(drawWidth,(getHeight()-scaled48)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

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

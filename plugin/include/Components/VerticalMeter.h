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

        //These variables are set to quickly and easily change the amount the vertical meter
        //fills the inside of the rectangle
        //Sets width between meter edge and meter display by 2
        float drawWidth = static_cast<float>(getWidth()-widthFromEdge); 
        float drawStartWidth = static_cast<float>(getWidth()-drawWidth); 
        //Sets height between meter edge and meter display by 6
        float drawHeight = static_cast<float>(getHeight()-heightFromEdge); 
        float drawStartHeight = static_cast<float>(getHeight()-drawHeight); 

        //Peak and level need to be set to be between -60 and 6 in order for proper
        //mapping. 
        (peak<-60.f) ? peak=-60.f : peak; 
        (level<-60.f) ? level=-60.f : level; 
        (peak>6.f) ? peak=6.f : peak; 
        (level>6.f) ? level=6.f : level; 


        //Now these scales are created that mark out the volume levels 
        //Scale needs to be between 0 and total length of display
        const auto scaled0 = jmap(0.f, -60.f, 6.f, 0.f, drawHeight-drawStartHeight); 
        const auto scaled12 = jmap(-12.f, -60.f, 6.f, 0.f, drawHeight-drawStartHeight); 
        const auto scaled24 = jmap(-24.f, -60.f, 6.f, 0.f, drawHeight-drawStartHeight); 
        const auto scaled36 = jmap(-36.f, -60.f, 6.f, 0.f, drawHeight-drawStartHeight); 
        const auto scaled48 = jmap(-48.f, -60.f, 6.f, 0.f, drawHeight-drawStartHeight); 

        //These scales generate the movement for the volume/peak meters
        const auto scaledX1 = jmap(peak, -60.f, 6.f, 0.f, drawHeight-drawStartHeight); 
        const auto scaledX2 = jmap(level, -60.f, 6.f, 0.f, drawHeight-drawStartHeight); 

        //sets the teal color to be positioned at the zero line (technically sets it at -6)
        //but due to the gradient it looks more like teal ends at the zero line 
        const auto scalecolor = jmap(-6.f, -60.f, 6.f, 0.f, 1.f); 

        //Creation of an shape in order to create the meters 
        //This first meter is the peak meter and we create this first since it will be 
        //behind the volume meter
        juce::Path pmeter; 
        pmeter.startNewSubPath(drawStartWidth,drawHeight); 
        pmeter.lineTo(drawStartWidth,drawHeight-scaledX1); 
        pmeter.lineTo(drawWidth,drawHeight-scaledX1); 
        pmeter.lineTo(drawWidth,drawHeight); 
        pmeter.closeSubPath(); 

        //fill must be created in order to set the fill type as a gradient 
        //This allows us to use fillPath for the gradient 
        auto fill = FillType(); 

        //Setting up the gradient color and in what direction it goes
        ColourGradient sealTealGradient; 
        sealTealGradient = ColourGradient::vertical(
            Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.3f), drawStartHeight,
            Colour(static_cast<juce::uint8>(187), static_cast<juce::uint8>(255), static_cast<juce::uint8>(0), 0.2f), drawHeight
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
        vmeter.startNewSubPath(drawStartWidth,drawHeight); 
        vmeter.lineTo(drawStartWidth,drawHeight-scaledX2); 
        vmeter.lineTo(drawWidth,drawHeight-scaledX2); 
        vmeter.lineTo(drawWidth,drawHeight); 
        vmeter.closeSubPath(); 

        sealTealGradient = ColourGradient::vertical(
            Colour(static_cast<juce::uint8>(255), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), 0.6f), drawStartHeight,
            Colour(static_cast<juce::uint8>(187), static_cast<juce::uint8>(255), static_cast<juce::uint8>(0), 0.5f), drawHeight 
          );
        sealTealGradient.addColour((1-scalecolor),Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(255), static_cast<juce::uint8>(222), 0.5f)); 

        fill.setGradient(sealTealGradient); 

        g.setFillType(fill); 

        g.fillPath(vmeter); 

        //Now creating the markings of the meter that list out the dbs 
        g.setColour(Colour(static_cast<juce::uint8>(0), static_cast<juce::uint8>(0), static_cast<juce::uint8>(0),0.7f)); 
        Path zeroLine; 
        zeroLine.startNewSubPath(drawStartWidth,(drawHeight-scaled0));
        zeroLine.lineTo(drawWidth,(drawHeight-scaled0)); 
        g.strokePath(zeroLine, PathStrokeType(1.5f)); 

        zeroLine.startNewSubPath(drawStartWidth,(drawHeight-scaled12));
        zeroLine.lineTo(drawWidth,(drawHeight-scaled12)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

        zeroLine.startNewSubPath(drawStartWidth,(drawHeight-scaled24));
        zeroLine.lineTo(drawWidth,(drawHeight-scaled24)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

        zeroLine.startNewSubPath(drawStartWidth,(drawHeight-scaled36));
        zeroLine.lineTo(drawWidth,(drawHeight-scaled36)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

        zeroLine.startNewSubPath(drawStartWidth,(drawHeight-scaled48));
        zeroLine.lineTo(drawWidth,(drawHeight-scaled48)); 
        g.strokePath(zeroLine, PathStrokeType(0.5f)); 

      }

      //Volume meter is created with a background rectangle outline.
      //This lets you set a width and height between the background and meter.
      void setWidthAndHeightFromMeter(float x, float y){
        widthFromEdge = x; 
        heightFromEdge = y; 
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
      float widthFromEdge = 2; 
      float heightFromEdge = 6; 
    }; 
}

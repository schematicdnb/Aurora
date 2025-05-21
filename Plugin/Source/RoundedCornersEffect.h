/*
  =============================================================================

    RoundedCornersEffect.h
    Created: 5 Mar 2025 4:32:49pm
    Credit to: 'dpsproservices' on JUCE forums

  =============================================================================
*/

#include <JuceHeader.h>

class RoundedCornersEffect : public juce::ImageEffectFilter
{
public:
    RoundedCornersEffect();

    explicit RoundedCornersEffect
    (
        const float& radius,
        const bool& roundTopLeftCorner = true,
        const bool& roundTopRightCorner = true,
        const bool& roundBottomLeftCorner = true,
        const bool& roundBottomRightCorner = true
    );

    ~RoundedCornersEffect() override;

    void setCornerRadius
    (
        const float& radius,
        const bool& roundTopLeftCorner = true,
        const bool& roundTopRightCorner = true,
        const bool& roundBottomLeftCorner = true,
        const bool& roundBottomRightCorner = true
    );

    void setClipReductionPath (const juce::Path& clipPath);

    void applyEffect
    (
        juce::Image& sourceImage,
        juce::Graphics& destContext,
        float scaleFactor,
        float alpha
     ) override;
private:
    float cornerRadius;

    bool topLeftCorner;
    bool topRightCorner;
    bool bottomLeftCorner;
    bool bottomRightCorner;

    juce::Path clipReductionPath;
    juce::Path scaledClipReductionPath;

    bool isClipReductionPathProvided;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RoundedCornersEffect)
};

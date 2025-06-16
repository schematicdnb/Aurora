/*
  ==============================================================================

    UpdateNotifier.h
    Created: 23 May 2025 9:06:49am
    Author:  Dan Dubois

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"

class UpdateNotifier : public Component {
public:
    UpdateNotifier(AuroraAudioProcessor&);
    void paint(Graphics &g) override;
    void resized() override;
    void checkForUpdates();
    bool isUpdateAvailable();
    bool isVersionStringSafe(const String& version);
    bool isNewerVersionSafe(const String& current, const String& latest);
    String sanitizeNotesForDisplay(const String& notes);
    
private:
    
    Label title;
    Label currentVersionLabel;
    Label latestVersionLabel;
    Label changelog;
    
    TextButton updateButton;
    TextButton remindButton;
    
    bool updateAvailable = false;
            
    String pluginName;
    String currentVersion;
    String latestVersion;
    String notes;
    
    AuroraAudioProcessor& ap;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdateNotifier)
};

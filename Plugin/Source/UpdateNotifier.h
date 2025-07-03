/*
  ==============================================================================

    UpdateNotifier.h
    Created: 23 May 2025 9:06:49am
    Author:  Dan Dubois

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
//#include "PluginProcessor.h"


class UpdateNotifier : public Component {
public:
    UpdateNotifier();
    void paint(Graphics &g) override;
    void resized() override;
    void checkForUpdates();
    bool isUpdateAvailable();
    
    // thread safe update dismissal
    void dismissUpdate();
    bool isUpdateDismissed();
    void setInfo();
    
    
private:

    bool isVersionStringSafe(const String& version);
    bool isNewerVersionSafe(const String& current, const String& latest);
    String sanitizeNotesForDisplay(const String& notes);
    
    Label title;
    Label currentVersionLabel;
    Label latestVersionLabel;
    Label changelog;
    
    TextButton updateButton;
    TextButton remindButton;
            
    String pluginName;
    String currentVersion;
    
    // thread safe shared data
    static CriticalSection cs;
    static Atomic<bool> updateAvailable;
    static Atomic<bool> updatesDismissed;
    static String latestVersion;
    static String notes;
    
    void setLatestVersion(const String& version);
    String getLatestVersion();
    void setNotes(const String& newNotes);
    String getNotes();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdateNotifier)
};

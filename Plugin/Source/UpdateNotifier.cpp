/*
  ==============================================================================

    UpdateNotifier.cpp
    Created: 23 May 2025 9:07:13am
    Author:  Dan Dubois

  ==============================================================================
*/

#include "UpdateNotifier.h"

UpdateNotifier::UpdateNotifier(AuroraAudioProcessor& p) : ap(p) {
    // Title
    FontOptions titleFontOptions = FontOptions(24.0f, Font::bold);
    title.setText("Update Available", NotificationType::dontSendNotification);
    title.setFont(titleFontOptions);
    title.setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(title);
    
    // Version labels
    latestVersionLabel.setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(latestVersionLabel);
    
    currentVersionLabel.setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(currentVersionLabel);
    
    // Changelog
    changelog.setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(changelog);

    // Buttons
    updateButton.setButtonText("Update");
    updateButton.setColour(TextButton::buttonColourId, Colour(32,32,32));
    updateButton.onClick = [this]() {
        URL download = URL("https://www.schematicsound.com/?mb_intent=view_products");
        download.launchInDefaultBrowser();
        ap.dismissUpdate();
        this->setVisible(false);
    };
    addAndMakeVisible(updateButton);
    
    remindButton.setColour(TextButton::buttonColourId, Colour(232, 232, 232));
    remindButton.setColour(TextButton::textColourOffId, Colours::black);
    remindButton.setButtonText("Remind Me");
    remindButton.onClick = [this]() {
        ap.dismissUpdate();
        this->setVisible(false);
    };
    addAndMakeVisible(remindButton);
}

void UpdateNotifier::paint(Graphics &g) {
    g.fillAll(Colour(232, 232, 232).withAlpha(static_cast<float>(0.95f)));
}

void UpdateNotifier::resized() {
    
    int buttonWidth = 100;
    int buttonHeight = 30;
    int spacing = 10;
    int leftColumnWidth = 2 * buttonWidth + spacing;
    
    auto leftColumnHeight = 130;
    
    // Left Column
    title.setBounds(getWidth() / 2 - leftColumnWidth, getHeight() / 2 - leftColumnHeight / 2, leftColumnWidth, buttonHeight);
    latestVersionLabel.setBounds(title.getX(), title.getY() + title.getHeight(), leftColumnWidth, buttonHeight);
    currentVersionLabel.setBounds(title.getX(), latestVersionLabel.getY() + 20, leftColumnWidth, buttonHeight);
    updateButton.setBounds(title.getX(), currentVersionLabel.getY() + currentVersionLabel.getHeight() + 10, buttonWidth, buttonHeight);
    remindButton.setBounds(title.getX() + buttonWidth + spacing, updateButton.getY(), buttonWidth, buttonHeight);
    
    // Right Column
    changelog.setBounds(getWidth()/2 + 10, title.getY(), getWidth()/2 - 20, leftColumnHeight);
}

bool UpdateNotifier::isUpdateAvailable() {
    return updateAvailable;
}

void UpdateNotifier::checkForUpdates() {
    pluginName = ProjectInfo::projectName;
    currentVersion = String(ProjectInfo::versionString);
    currentVersionLabel.setText("Current Version: " + currentVersion, NotificationType::dontSendNotification);
    
    String versionURL = "https://www.schematicsound.com/plugin-versions.php";
    String cacheBypass = String(Time::getCurrentTime().toMilliseconds());
    URL requestURL(versionURL + "?cb=" + cacheBypass);
    
    auto response = JSON::parse(requestURL.readEntireTextStream());
    
    if (response.isObject() && response.hasProperty(pluginName)) {
        
        auto info = response.getProperty(pluginName, "Update Check Failed.");
        latestVersion = info.getProperty("version", "0").toString();
        latestVersionLabel.setText("Latest Version: " + latestVersion, NotificationType::dontSendNotification);
    
        if (currentVersion.compare(latestVersion)) {
            auto notes = info.getProperty("notes", "No info available.").toString();
            changelog.setText(notes, NotificationType::dontSendNotification);
            updateAvailable = true;
        } else {
            updateAvailable = false;
        }
    }
}

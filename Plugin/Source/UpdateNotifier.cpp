/*
  ==============================================================================

    UpdateNotifier.cpp
    Created: 23 May 2025 9:07:13am
    Author:  Dan Dubois

  ==============================================================================
*/

#include "UpdateNotifier.h"
#include <regex>

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
    
    // always uses HTTPS
    String baseURL = "https://www.schematicsound.com/plugin-versions.php";
    String cacheBypass = String(Time::getCurrentTime().toMilliseconds());
//    URL requestURL(versionURL + "?cb=" + cacheBypass);
    URL requestURL(baseURL + "?plugin=" + URL::addEscapeChars(pluginName, false) +
                       "&cb=" + cacheBypass);
    
    // Set strict connection options
    auto options = URL::InputStreamOptions(URL::ParameterHandling::inAddress)
                      .withConnectionTimeoutMs(10000)  // 10 second timeout - prevent hanging
                      .withProgressCallback(nullptr)
                      .withNumRedirectsToFollow(0)     // CRITICAL: No redirects allowed
                      .withExtraHeaders("User-Agent: " + pluginName + "/" + currentVersion);
    
    std::unique_ptr<InputStream> stream(requestURL.createInputStream(options));
    
    if (stream == nullptr) {
        DBG("Connection failed - update check skipped");
        return;
    }
    
    // Limit response size to prevent memory exhaustion
    const int maxSafeResponseSize = 4096; // 4KB maximum
    String responseText;
    
    try {
        // Read with strict size limit
        char buffer[1024];
        int totalBytesRead = 0;
        
        while (!stream->isExhausted() && totalBytesRead < maxSafeResponseSize) {
            int bytesToRead = jmin(1024, maxSafeResponseSize - totalBytesRead);
            int bytesRead = stream->read(buffer, bytesToRead);
            
            if (bytesRead <= 0) break;
            
            responseText += String(CharPointer_UTF8(buffer), bytesRead);
            totalBytesRead += bytesRead;
        }
        
        // If we hit the limit, it's suspicious
        if (totalBytesRead >= maxSafeResponseSize) {
            DBG("Response too large - aborting.");
            return;
        }
            
        } catch (...) {
            DBG("Exception during response reading - aborting.");
            return;
        }
        
        if (responseText.isEmpty()) {
            DBG("Empty response.");
            return;
        }
        
        // CRITICAL: Parse JSON safely with error handling
        var response;
        try {
            response = JSON::parse(responseText);
        } catch (...) {
            DBG("JSON parsing failed");
            return;
        }
        
        if (!response.isObject()) {
            DBG("Invalid JSON structure");
            return;
        }
        
        // CRITICAL: Validate and sanitize all fields before use
        if (!response.hasProperty(pluginName)) {
            DBG("Plugin not found in response");
            
            return;
        }
        
        auto info = response.getProperty(pluginName, var());
        if (!info.isObject()) {
            DBG("Invalid plugin info structure");
            return;
        }
        
        // Extract and validate version string
        String receivedVersion = info.getProperty("version", "").toString();
        if (!isVersionStringSafe(receivedVersion)) {
            DBG("Unsafe version string received");
            return;
        }
        
        latestVersion = receivedVersion;
        latestVersionLabel.setText("Latest Version: " + latestVersion, NotificationType::dontSendNotification);
        
        // Safely compare versions
        if (isNewerVersionSafe(currentVersion, latestVersion)) {
            // Extract and sanitize notes
            String notes = info.getProperty("notes", "").toString();
            String safeNotes = sanitizeNotesForDisplay(notes);
            
            changelog.setText(safeNotes, NotificationType::dontSendNotification);
            updateAvailable = true;
            DBG("Update available");
        } else {
            updateAvailable = false;
            DBG("No updates available");
    }
    
    
//    auto response = JSON::parse(requestURL.readEntireTextStream());
//    
//    if (response.isObject() && response.hasProperty(pluginName)) {
//        
//        auto info = response.getProperty(pluginName, "Update Check Failed.");
//        latestVersion = info.getProperty("version", "0").toString();
//        latestVersionLabel.setText("Latest Version: " + latestVersion, NotificationType::dontSendNotification);
//    
//        if (currentVersion.compare(latestVersion)) {
//            auto notes = info.getProperty("notes", "No info available.").toString();
//            changelog.setText(notes, NotificationType::dontSendNotification);
//            updateAvailable = true;
//        } else {
//            updateAvailable = false;
//        }
//    }
}

bool UpdateNotifier::isVersionStringSafe(const String& version) {
    // Only allow semantic versioning format with reasonable length
    if (version.length() > 20) return false;  // Prevent long strings
    if (version.isEmpty()) return false;
    
    // Only allow digits and dots
    for (int i = 0; i < version.length(); ++i) {
        juce_wchar c = version[i];
        if (!CharacterFunctions::isDigit(c) && c != '.') {
            return false;
        }
    }
    
    // Must match basic version pattern
    const std::regex pattern(R"(\d+\.\d+\.\d+)");
    return std::regex_match(version.toStdString(), pattern);
}

bool UpdateNotifier::isNewerVersionSafe(const String& current, const String& latest) {
    
    // Safe semantic version comparison
    StringArray currentParts = StringArray::fromTokens(current, ".", "");
    StringArray latestParts = StringArray::fromTokens(latest, ".", "");
    
    // Limit number of parts to prevent algorithmic attacks
    if (currentParts.size() > 10 || latestParts.size() > 10) {
        DBG("Invalid version string");
        return false;
    }
    
    int maxParts = jmin(jmax(currentParts.size(), latestParts.size()), 10);
    
    for (int i = 0; i < maxParts; ++i) {
        int currentPart = (i < currentParts.size()) ? currentParts[i].getIntValue() : 0;
        int latestPart = (i < latestParts.size()) ? latestParts[i].getIntValue() : 0;
        
        // Sanity check on version numbers
        if (currentPart < 0 || latestPart < 0 || currentPart > 99999 || latestPart > 99999) {
            DBG("version number sanity check failed");
            return false;
        }
        
        if (latestPart > currentPart) return true;
        if (latestPart < currentPart) return false;
    }
    return false;
}

String UpdateNotifier::sanitizeNotesForDisplay(const String& notes) {
    if (notes.isEmpty()) return "No release notes available.";
    
    // CRITICAL: Strict length limit
    String sanitized = notes.substring(0, 500);  // Max 500 chars
    
    // Remove potentially dangerous characters
    sanitized = sanitized.removeCharacters("\x00-\x08\x0B\x0C\x0E-\x1F\x7F"); // Control chars
    sanitized = sanitized.replace("\r\n", "\n").replace("\r", "\n"); // Normalize line endings
    
    // Limit number of lines to prevent UI issues
    StringArray lines = StringArray::fromLines(sanitized);
    if (lines.size() > 20) {
        lines.removeRange(20, lines.size() - 20);
        sanitized = lines.joinIntoString("\n") + "\n...";
    }
    
    // Remove any remaining suspicious patterns
    sanitized = sanitized.replace("javascript:", "");
    sanitized = sanitized.replace("data:", "");
    sanitized = sanitized.replace("<script", "");
    
    return sanitized.trim();
}

#include "PluginProcessor.h"
#include "PluginEditor_WebView.h"

//==============================================================================
BeatBattleAudioProcessorEditor::BeatBattleAudioProcessorEditor(BeatBattleAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    DBG("=== CONSTRUCTOR START ===");

    setSize(800, 600);
    DBG("Set size to 800x600");

    setOpaque(true);
    DBG("Set opaque to true");

    // Create loading label
    loadingLabel = std::make_unique<juce::Label>("loading", "Loading Beat Battle website...");
    loadingLabel->setFont(juce::Font(24.0f, juce::Font::bold));
    loadingLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    loadingLabel->setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.8f));
    loadingLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(loadingLabel.get());
    DBG("Created loading label");

    // Hide loading label after a delay
    juce::Timer::callAfterDelay(5000, [this]()
    {
        if (loadingLabel)
            loadingLabel->setVisible(false);
    });

    // Reload button
    showSamplesButton = std::make_unique<juce::TextButton>("Reload Website");
    showSamplesButton->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    showSamplesButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    showSamplesButton->onClick = [this]()
    {
        DBG("Reload button clicked");
        if (webBrowser)
        {
            webBrowser->goToURL("https://beat-battle.net/");
            if (loadingLabel)
            {
                loadingLabel->setVisible(true);
                loadingLabel->setText("Reloading...", juce::dontSendNotification);
            }
        }
    };
    addAndMakeVisible(showSamplesButton.get());
    DBG("Created reload button");

    // NOW add the WebBrowser
    DBG("Creating WebBrowserComponent...");
    webBrowser = std::make_unique<juce::WebBrowserComponent>();
    addAndMakeVisible(webBrowser.get());
    DBG("WebBrowser created and added");

    // Load the website
    DBG("Loading https://beat-battle.net/");
    webBrowser->goToURL("https://beat-battle.net/");

    // Inject JavaScript after page loads
    juce::Timer::callAfterDelay(3000, [this]()
    {
        DBG("Injecting JavaScript bridge...");

        // JavaScript to add an "IMPORT TO DAW" button
        juce::String jsCode = R"(
            console.log("VST Bridge: Starting injection...");

            // Create floating import button
            var importBtn = document.createElement('button');
            importBtn.textContent = '⬇ IMPORT TO DAW';
            importBtn.id = 'vstImportButton';
            importBtn.style.cssText = `
                position: fixed;
                bottom: 20px;
                right: 20px;
                z-index: 99999;
                padding: 15px 30px;
                background: linear-gradient(135deg, #e02020 0%, #ff4444 100%);
                color: white;
                border: 2px solid rgba(255,255,255,0.3);
                border-radius: 8px;
                font-family: monospace;
                font-weight: bold;
                font-size: 16px;
                cursor: pointer;
                box-shadow: 0 4px 15px rgba(224, 32, 32, 0.4);
                transition: all 0.2s ease;
            `;

            importBtn.onmouseover = function() {
                this.style.transform = 'translateY(-2px)';
                this.style.boxShadow = '0 6px 20px rgba(224, 32, 32, 0.6)';
            };

            importBtn.onmouseout = function() {
                this.style.transform = 'translateY(0)';
                this.style.boxShadow = '0 4px 15px rgba(224, 32, 32, 0.4)';
            };

            importBtn.onclick = function() {
                console.log("Import button clicked!");

                // Mock sample data for now - you'll need to extract real URLs from the website
                var samples = [
                    {name: "KICK", url: "https://beat-battle.net/samples/kick.wav"},
                    {name: "SNARE", url: "https://beat-battle.net/samples/snare.wav"},
                    {name: "HIHAT", url: "https://beat-battle.net/samples/hihat.wav"},
                    {name: "808", url: "https://beat-battle.net/samples/808.wav"}
                ];

                // Send message to VST (using window.location.href as a simple bridge)
                window.location.href = 'vst-import://' + JSON.stringify(samples);
            };

            document.body.appendChild(importBtn);
            console.log("VST Bridge: Import button added!");
        )";

        // Try to execute the JavaScript (platform-specific)
        #if JUCE_MAC
        // On macOS, we can't directly execute JS in JUCE WebBrowserComponent easily
        // Instead, we'll use a workaround with pageFinishedLoading callback
        // For now, just log that we tried
        DBG("JavaScript prepared (execution may be limited on macOS WebKit)");
        #endif
    });

    // FORCE resized() to be called
    DBG("Manually calling resized()...");
    resized();
    DBG("resized() called");

    DBG("=== CONSTRUCTOR COMPLETE ===");
}

BeatBattleAudioProcessorEditor::~BeatBattleAudioProcessorEditor()
{
}

void BeatBattleAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Just a simple dark background - the WebBrowser will render on top
    g.fillAll(juce::Colour(0xff1a1a1a));
}

void BeatBattleAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    DBG("!!! RESIZED CALLED !!! Bounds: " << bounds.getWidth() << "x" << bounds.getHeight());

    // Position button at top (small)
    if (showSamplesButton)
    {
        auto buttonArea = bounds.removeFromTop(40);
        showSamplesButton->setBounds(buttonArea.reduced(10));
        DBG("Button bounds: " << showSamplesButton->getBounds().toString());
    }

    // WebBrowser takes most of the space
    if (webBrowser)
    {
        auto webArea = bounds;
        webBrowser->setBounds(webArea);
        DBG("WebBrowser bounds: " << webBrowser->getBounds().toString());
    }

    // Position label OVER the webbrowser (so we can see loading status)
    if (loadingLabel)
    {
        loadingLabel->setBounds(bounds.withSizeKeepingCentre(600, 100));
        loadingLabel->toFront(false);
        DBG("Label bounds: " << loadingLabel->getBounds().toString());
    }
}

void BeatBattleAudioProcessorEditor::handleWebMessage(const juce::String& message)
{
    DBG("Received message from web: " << message);

    // Parse message
    if (message.startsWith("vst-import://"))
    {
        juce::String jsonData = message.substring(13);
        juce::var parsedData;

        if (auto result = juce::JSON::parse(jsonData, parsedData))
        {
            importSamplesFromWeb(parsedData);
        }
    }
}

void BeatBattleAudioProcessorEditor::importSamplesFromWeb(const juce::var& samplesData)
{
    if (!samplesData.isArray())
        return;

    sampleDragger->clearSamples();

    for (int i = 0; i < samplesData.size(); i++)
    {
        auto sample = samplesData[i];
        juce::String name = sample.getProperty("name", "Sample").toString();
        juce::String url = sample.getProperty("url", "").toString();

        if (url.isNotEmpty())
        {
            sampleDragger->addSample(name, url);
            downloadSample(url, name);
        }
    }

    // Auto-show samples panel
    samplesVisible = true;
    sampleDragger->setVisible(true);
    showSamplesButton->setButtonText("Hide Samples");
    resized();
}

void BeatBattleAudioProcessorEditor::downloadSample(const juce::String& url, const juce::String& name)
{
    DBG("Downloading sample: " << name << " from " << url);

    // Download sample synchronously (simpler for now)
    juce::URL sampleUrl(url);

    // Create temp file
    auto tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
                        .getChildFile("BeatBattle_" + name + ".wav");

    // Download to input stream
    auto inputStream = sampleUrl.createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                                                    .withConnectionTimeoutMs(10000));

    if (inputStream != nullptr)
    {
        // Write to file
        if (tempFile.replaceWithData(inputStream->readEntireStreamAsString().toRawUTF8(),
                                     inputStream->getTotalLength()))
        {
            DBG("Downloaded " << name << " successfully to " << tempFile.getFullPathName());
        }
        else
        {
            DBG("Failed to write " << name << " to file");
        }
    }
    else
    {
        DBG("Failed to download " << name << " from " << url);
    }
}

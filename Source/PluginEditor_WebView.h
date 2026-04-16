#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Component for drag-and-drop sample import to DAW
class SampleDragger : public juce::Component,
                      public juce::FileDragAndDropTarget
{
public:
    SampleDragger()
    {
        setSize(600, 400);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.8f));
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);

        if (samples.isEmpty())
        {
            g.drawText("No samples loaded yet.\nUse 'Import from VST' on the website.",
                      getLocalBounds(), juce::Justification::centred);
        }
        else
        {
            g.drawText("Drag samples to your DAW tracks:",
                      getLocalBounds().removeFromTop(30),
                      juce::Justification::centred);

            auto bounds = getLocalBounds().reduced(20);
            bounds.removeFromTop(40);

            for (const auto& sample : samples)
            {
                auto rowBounds = bounds.removeFromTop(25);
                g.drawText(sample.name, rowBounds, juce::Justification::centredLeft);
            }
        }
    }

    bool isInterestedInFileDrag(const juce::StringArray&) override { return false; }
    void filesDropped(const juce::StringArray&, int, int) override {}

    struct Sample
    {
        juce::String name;
        juce::String url;
        juce::File tempFile;
    };

    void addSample(const juce::String& name, const juce::String& url)
    {
        Sample sample;
        sample.name = name;
        sample.url = url;
        samples.add(sample);
        repaint();
    }

    void clearSamples()
    {
        samples.clear();
        repaint();
    }

    const juce::Array<Sample>& getSamples() const { return samples; }

private:
    juce::Array<Sample> samples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleDragger)
};

//==============================================================================
class BeatBattleAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    BeatBattleAudioProcessorEditor(BeatBattleAudioProcessor&);
    ~BeatBattleAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BeatBattleAudioProcessor& audioProcessor;

    // Web view component
    std::unique_ptr<juce::WebBrowserComponent> webBrowser;

    // Sample dragger (shown when samples are imported)
    std::unique_ptr<SampleDragger> sampleDragger;
    std::unique_ptr<juce::TextButton> closeDraggerButton;
    std::unique_ptr<juce::TextButton> showSamplesButton;
    std::unique_ptr<juce::Label> loadingLabel;

    bool samplesVisible = false;

    // JavaScript bridge callbacks
    void handleWebMessage(const juce::String& message);
    void importSamplesFromWeb(const juce::var& samplesData);
    void downloadSample(const juce::String& url, const juce::String& name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatBattleAudioProcessorEditor)
};

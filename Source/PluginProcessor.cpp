#include "PluginProcessor.h"
#include "PluginEditor_WebView.h"

//==============================================================================
BeatBattleAudioProcessor::BeatBattleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       state("BeatBattleState")
#endif
{
    // Initialize state tree
    state.setProperty("isAuthenticated", false, nullptr);
    state.setProperty("username", "", nullptr);
    state.setProperty("authToken", "", nullptr);
    state.setProperty("currentLobby", "", nullptr);
    state.setProperty("gamePhase", "idle", nullptr); // idle, lobby, cooking, listening, voting, results

    state.addListener(this);
}

BeatBattleAudioProcessor::~BeatBattleAudioProcessor()
{
    state.removeListener(this);
}

//==============================================================================
const juce::String BeatBattleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BeatBattleAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BeatBattleAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BeatBattleAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BeatBattleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BeatBattleAudioProcessor::getNumPrograms()
{
    return 1;
}

int BeatBattleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BeatBattleAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BeatBattleAudioProcessor::getProgramName (int index)
{
    return {};
}

void BeatBattleAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BeatBattleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Reserved for future audio processing setup
}

void BeatBattleAudioProcessor::releaseResources()
{
    // Reserved for future audio processing cleanup
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BeatBattleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BeatBattleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // No audio processing - just pass through (website handles all game logic)
}

//==============================================================================
bool BeatBattleAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BeatBattleAudioProcessor::createEditor()
{
    return new BeatBattleAudioProcessorEditor (*this);
}

//==============================================================================
void BeatBattleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save state to binary
    auto xml = state.createXml();
    if (xml != nullptr)
        copyXmlToBinary (*xml, destData);
}

void BeatBattleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore state from binary
    auto xmlState = getXmlFromBinary (data, sizeInBytes);

    if (xmlState != nullptr && xmlState->hasTagName(state.getType()))
    {
        auto newState = juce::ValueTree::fromXml(*xmlState);
        if (newState.isValid())
        {
            state.removeListener(this);
            state = newState;
            state.addListener(this);
        }
    }
}

void BeatBattleAudioProcessor::valueTreePropertyChanged(juce::ValueTree& tree,
                                                         const juce::Identifier& property)
{
    // Handle state changes here
    if (property == juce::Identifier("isAuthenticated"))
    {
        bool isAuth = tree.getProperty("isAuthenticated");
        DBG("Authentication status changed: " + juce::String(isAuth ? "true" : "false"));
        // Website handles all authentication and websocket connections
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BeatBattleAudioProcessor();
}

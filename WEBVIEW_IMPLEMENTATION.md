# WebView Implementation Guide

## Overview

The VST plugin now embeds the Beat Battle website directly using JUCE's `WebBrowserComponent`. This approach provides:

- **Perfect visual fidelity** - It's literally the same website
- **Automatic updates** - Changes to the website automatically reflect in the VST
- **Simplified development** - No need to recreate UI in JUCE
- **Native VST features** - JavaScript bridge for importing samples to DAW

## Architecture

```
┌─────────────────────────────────────┐
│         VST Plugin Window           │
├─────────────────────────────────────┤
│  WebBrowserComponent                │
│  ┌───────────────────────────────┐  │
│  │ https://beatbattle.onrender.com │  │
│  │                               │  │
│  │  [Website renders here]       │  │
│  │                               │  │
│  │  [IMPORT TO DAW] ← Button     │  │
│  └───────────────────────────────┘  │
├─────────────────────────────────────┤
│  Sample Import Panel (toggleable)   │
│  - Drag samples to DAW tracks       │
│  - Downloaded samples cached        │
└─────────────────────────────────────┘
```

## Key Features

### 1. Embedded Website
- Loads `https://beatbattle.onrender.com` directly
- Full website functionality available in VST
- Uses platform-native web rendering (WKWebView on macOS, WebView2 on Windows)

### 2. JavaScript Bridge
JavaScript is injected into the page to add a `vstBridge` object:

```javascript
window.vstBridge = {
    importSamples: function(samplesArray) {
        // samplesArray: [{name: "Kick", url: "https://..."}, ...]
        window.location.href = 'vst-import://' + JSON.stringify(samplesArray);
    }
};
```

### 3. "Import to DAW" Button
A floating button is added to the website that:
- Appears in bottom-right corner
- Styled with Beat Battle theme (red #e02020)
- Calls `vstBridge.importSamples()` with current samples
- Triggers download and DAW import flow

### 4. Sample Import Flow

```
User clicks "IMPORT TO DAW"
        ↓
JavaScript calls vstBridge.importSamples()
        ↓
VST receives sample data (names + URLs)
        ↓
VST downloads samples to temp files
        ↓
VST shows sample panel (split view)
        ↓
User drags samples to DAW tracks
```

## Files

### Core Files
- `Source/PluginEditor_WebView.h` - Header with WebBrowser and SampleDragger
- `Source/PluginEditor_WebView.cpp` - Implementation with JS bridge
- `CMakeLists.txt` - Updated to enable web browser (`JUCE_WEB_BROWSER=1`)

### Components

#### SampleDragger
- Manages downloaded samples
- Provides drag-and-drop to DAW
- Shows list of available samples

#### BeatBattleAudioProcessorEditor (WebView version)
- Embeds website with `WebBrowserComponent`
- Injects JavaScript bridge
- Handles sample import messages
- Downloads and caches samples

## Usage

### For Users
1. Open the VST plugin in your DAW
2. Website loads automatically (requires internet)
3. Use the website normally (login, join lobby, cook, etc.)
4. When you have samples ready, click "IMPORT TO DAW" button
5. Click "Show Samples" to see imported samples
6. Drag samples from panel to DAW tracks

### For Developers

#### Modifying the JavaScript Bridge
Edit the JavaScript injection in `PluginEditor_WebView.cpp` line ~30:

```cpp
juce::String jsCode = R"(
    window.vstBridge = {
        importSamples: function(samplesArray) {
            window.location.href = 'vst-import://' + JSON.stringify(samplesArray);
        }
    };
    // Add more bridge functions here
)";
```

#### Handling Additional Messages
Add handlers in `handleWebMessage()`:

```cpp
void BeatBattleAudioProcessorEditor::handleWebMessage(const juce::String& message)
{
    if (message.startsWith("vst-import://"))
    {
        // Handle sample import
    }
    else if (message.startsWith("vst-custom://"))
    {
        // Handle custom message
    }
}
```

## Next Steps

### Immediate Enhancements
1. **Extract real sample URLs from website**
   - Currently uses mock data
   - Need to query website's sample state

2. **Implement drag-and-drop**
   - Make samples draggable to DAW
   - Support multiple sample formats

3. **Add offline mode**
   - Bundle website files locally
   - Load from disk if no internet

### Advanced Features
1. **Bidirectional sync**
   - Send DAW state to website
   - Update UI based on plugin state

2. **MIDI mapping**
   - Map samples to MIDI notes
   - Trigger samples from MIDI keyboard

3. **Sample caching**
   - Persist downloaded samples
   - Avoid re-downloading

## Comparison: WebView vs Native UI

| Feature | WebView | Native UI |
|---------|---------|-----------|
| **Visual Fidelity** | Perfect (same website) | Manual recreation |
| **Development Speed** | Very fast | Slow |
| **Update Frequency** | Automatic | Manual |
| **Performance** | Good (web overhead) | Excellent |
| **Offline Support** | Requires bundling | Native |
| **VST Integration** | JS bridge needed | Direct |
| **File Size** | Smaller (no UI assets) | Larger |

## Troubleshooting

### Website not loading
- Check internet connection
- Verify `https://beatbattle.onrender.com` is accessible
- Check firewall/proxy settings

### Import button not appearing
- JavaScript may not have injected properly
- Check console (if available) for errors
- Increase injection delay (currently 2000ms)

### Samples not downloading
- Check sample URLs are valid
- Verify network permissions
- Check temp directory is writable

### WebView crashes
- Update to latest JUCE version
- Check platform-specific web view requirements
- Ensure JUCE_WEB_BROWSER=1 is set

## Building

```bash
cd /Users/sbowerma/Code/beatbattle_vst
mkdir -p build && cd build
cmake ..
cmake --build . --config Release
```

The VST3 will be installed to:
- macOS: `~/Library/Audio/Plug-Ins/VST3/BeatBattle VST.vst3`
- Windows: `C:\Program Files\Common Files\VST3\BeatBattle VST.vst3`

## Credits

- **Website**: https://beatbattle.onrender.com
- **Framework**: JUCE 8.0.4
- **Web View**: Platform-native (WKWebView/WebView2)

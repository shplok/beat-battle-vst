# Beat Battle VST

A VST3 plugin that embeds the Beat Battle website (https://beat-battle.net/) directly into your DAW.

## Features

- **Embedded Website**: Full Beat Battle website runs inside the plugin
- **No Recreation Needed**: Website UI is mirrored exactly - any website updates automatically appear in the plugin
- **Lightweight**: ~20KB of code - the website handles all game logic
- **Cross-Platform**: Works on macOS and Windows

## Architecture

```
┌─────────────────────────────────────┐
│         Beat Battle VST             │
├─────────────────────────────────────┤
│  WebBrowserComponent                │
│  ┌───────────────────────────────┐  │
│  │ https://beat-battle.net/      │  │
│  │  (Full website embedded)      │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

**Key Implementation Detail**: The plugin calls `resized()` manually in the constructor to ensure components are laid out properly, as JUCE doesn't automatically trigger layout on initial creation.

## Building

### Prerequisites

- CMake 3.15+
- C++17 compiler (Xcode on macOS, Visual Studio on Windows)
- Git

### Build Steps

```bash
# Clone the repository
git clone https://github.com/shplok/beat-battle-vst
cd beat-battle-vst

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release

# The VST3 will be installed to:
# - macOS: ~/Library/Audio/Plug-Ins/VST3/BeatBattle VST.vst3
# - Windows: C:\Program Files\Common Files\VST3\BeatBattle VST.vst3
```

## Usage

1. **Launch**: Open the plugin in your DAW
2. **Website Loads**: Beat Battle website loads automatically
3. **Play**: Use the website normally - login, join battles, cook, vote, etc.
4. **Reload**: Click "Reload Website" button if needed

## Project Structure

```
Source/
├── PluginProcessor.cpp/h       # Audio plugin core (pass-through)
└── PluginEditor_WebView.cpp/h  # WebView UI implementation

WEBVIEW_IMPLEMENTATION.md        # Technical details
README.md                        # This file
CMakeLists.txt                   # Build configuration
```

## Tech Stack

- **JUCE 8.0.4**: Cross-platform audio framework
- **WebBrowserComponent**: Native web view (WKWebView on macOS, WebView2 on Windows)
- **C++17**: Modern C++ features

## Future Enhancements

- JavaScript bridge for VST ↔ website communication
- "Import to DAW" button to download samples from website into DAW
- Drag-and-drop sample integration
- DAW transport sync

## How It Works

The plugin uses JUCE's `WebBrowserComponent` to embed the live website. The key fix that makes it work:

```cpp
// In constructor:
webBrowser = std::make_unique<juce::WebBrowserComponent>();
addAndMakeVisible(webBrowser.get());
webBrowser->goToURL("https://beat-battle.net/");

// CRITICAL: Manually call resized() to layout components
resized();  // Without this, components have 0x0 bounds and don't render
```

See `WEBVIEW_IMPLEMENTATION.md` for full technical details.

## Website

- Production: https://beat-battle.net/
- The website handles all game logic, authentication, lobbies, samples, voting, etc.

## License

[Your License Here]

## Credits

Built with [JUCE](https://juce.com/) framework

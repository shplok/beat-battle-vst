# Beat Battle VST

A VST3 plugin for the Beat Battle platform that enables real-time multiplayer beat battles directly from your DAW.

## Features

- **Real-time Multiplayer**: Join lobbies and compete with other producers
- **WebSocket Integration**: Seamless communication with Beat Battle servers
- **Sample Distribution**: Automatically receive battle samples
- **Beat Submission**: Export and submit your beats directly from the plugin
- **Live Voting**: Participate in voting and see results in real-time

## Architecture

```
┌─────────────────────────────────────┐
│         Beat Battle VST             │
├─────────────────────────────────────┤
│  - AuthManager (Login/Auth)         │
│  - LobbyManager (Browse/Join)       │
│  - SampleManager (Download/Playback)│
│  - WebSocketManager (Real-time)     │
└─────────────────────────────────────┘
            ↕ WebSocket
┌─────────────────────────────────────┐
│   Beat Battle Backend (FastAPI)    │
│   https://beatbattle.onrender.com   │
└─────────────────────────────────────┘
```

## Building

### Prerequisites

- CMake 3.15+
- C++17 compiler (Xcode on macOS, Visual Studio on Windows)
- Git

### Build Steps

```bash
# Clone the repository
git clone <your-repo>
cd beatbattle_vst

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# The VST3 will be in:
# - macOS: ~/Library/Audio/Plug-Ins/VST3/BeatBattle VST.vst3
# - Windows: C:\Program Files\Common Files\VST3\BeatBattle VST.vst3
```

## Usage

1. **Launch**: Open the plugin in your DAW
2. **Login**: Enter your Beat Battle credentials
3. **Browse Lobbies**: View available game rooms or create your own
4. **Join Battle**: Enter a lobby and wait for the game to start
5. **Cook**: Create your beat using the provided samples
6. **Submit**: Export your beat when time's up
7. **Vote**: Listen and vote on other submissions
8. **Results**: See who won!

## Game Modes

- **Solo**: Practice mode with random samples
- **Multiplayer**: 
  - Quick Match: Join a random lobby
  - Custom Lobby: Create with specific settings
  - Private: Join via code

## Configuration

Edit `Source/WebSocketManager.cpp` to change the backend URL:

```cpp
juce::String WebSocketManager::getWebSocketURL() const
{
    // Production
    return "wss://beatbattle.onrender.com/ws";
    
    // Local dev
    // return "ws://localhost:8000/ws";
}
```

## Tech Stack

- **JUCE**: Cross-platform audio framework
- **IXWebSocket**: WebSocket client library
- **C++17**: Modern C++ features

## Development Roadmap

- [x] Basic project structure
- [x] WebSocket integration
- [x] Authentication system
- [ ] Lobby browser UI
- [ ] Sample download/playback
- [ ] Beat submission system
- [ ] Voting interface
- [ ] Results display
- [ ] Audio effects (stretch goal)

## Backend Integration

This plugin requires the Beat Battle backend to be running. The backend handles:
- User authentication
- Lobby management
- Sample distribution
- Beat collection and voting
- Real-time game state synchronization

Backend repo: [Link when available]

## License

[Your License Here]

## Credits

Built with [JUCE](https://juce.com/) framework

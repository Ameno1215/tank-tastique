#ifndef NETWORK_PROTOCOL_HPP
#define NETWORK_PROTOCOL_HPP

#include <array>
#include <cstring>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace network {

constexpr int kMaxPlayers = 6;

struct ConnectionRequest {
    std::string ip;
    std::string pseudo;
};

struct ConnectionConfig {
    int playerId = -1;
    int port = 3000;
    int mode = 1;
};

struct ConnectionReady {
    int clientPort = 0;
};

struct LobbyState {
    int nbJoueurs = 0;
    std::array<std::string, kMaxPlayers> pseudos{};
    std::array<int, kMaxPlayers> equipes{};
};

struct InputState {
    int playerId = 0;
    bool z = false;
    bool q = false;
    bool s = false;
    bool d = false;
    bool x = false;
    int mouseX = 0;
    int mouseY = 0;
    bool clicked = false;
};

struct TankChoice {
    int playerId = 0;
    int tankType = 0;
};

struct TankState {
    int playerId = 0;
    float x = 0.f;
    float y = 0.f;
    float baseRotation = 0.f;
    float turretRotation = 0.f;
    int ultiState = 0;
};

inline std::string makeConnectionRequest(const ConnectionRequest& request) {
    if (!request.ip.empty()) {
        return "C " + request.ip + " N: " + request.pseudo;
    }

    return "C " + request.pseudo;
}

inline std::optional<ConnectionRequest> parseConnectionRequest(const std::string& message) {
    if (message.rfind("C ", 0) != 0) {
        return std::nullopt;
    }

    ConnectionRequest request;
    const std::size_t pseudoPos = message.find(" N: ");
    if (pseudoPos != std::string::npos) {
        request.ip = message.substr(2, pseudoPos - 2);
        request.pseudo = message.substr(pseudoPos + 4);
    } else {
        request.pseudo = message.substr(2);
    }

    if (request.pseudo.empty()) {
        return std::nullopt;
    }

    return request;
}

inline std::string makeConnectionConfig(const ConnectionConfig& config) {
    std::ostringstream oss;
    oss << "C " << config.playerId << ' ' << config.port << ' ' << config.mode;
    return oss.str();
}

inline std::optional<ConnectionConfig> parseConnectionConfig(const std::string& message) {
    if (message.empty() || message[0] != 'C') {
        return std::nullopt;
    }

    ConnectionConfig config;
    std::istringstream iss(message);
    char type = '\0';
    if (!(iss >> type)) {
        return std::nullopt;
    }

    if (!(iss >> config.playerId >> config.port >> config.mode)) {
        iss.clear();
        iss.str(message);

        if (!(iss >> type >> config.port >> config.mode)) {
            return std::nullopt;
        }

        if (config.port >= 3001 && config.port < 3001 + kMaxPlayers) {
            config.playerId = config.port - 3001;
        } else {
            config.playerId = 0;
        }
    }

    if (config.playerId < 0 || config.playerId >= kMaxPlayers || config.port <= 0) {
        return std::nullopt;
    }

    return config;
}

inline std::string makeConnectionReady(const ConnectionReady& ready) {
    std::ostringstream oss;
    oss << "T " << ready.clientPort;
    return oss.str();
}

inline std::optional<ConnectionReady> parseConnectionReady(const std::string& message) {
    if (message.empty() || message[0] != 'T') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    ConnectionReady ready;
    char type = '\0';

    if (!(iss >> type >> ready.clientPort)) {
        return std::nullopt;
    }

    if (ready.clientPort <= 0) {
        return std::nullopt;
    }

    return ready;
}

inline std::string makeLobbyState(const LobbyState& state, int mode) {
    std::ostringstream oss;
    oss << "P " << state.nbJoueurs;

    for (int i = 0; i < state.nbJoueurs && i < kMaxPlayers; ++i) {
        oss << ' ' << state.pseudos[i];
        if (mode == 2) {
            oss << ' ' << state.equipes[i];
        }
    }

    return oss.str();
}

inline std::optional<LobbyState> parseLobbyState(const std::string& message, int mode) {
    if (message.empty() || message[0] != 'P') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    LobbyState state;
    char type = '\0';

    if (!(iss >> type >> state.nbJoueurs)) {
        return std::nullopt;
    }

    for (int i = 0; i < state.nbJoueurs && i < kMaxPlayers; ++i) {
        if (!(iss >> state.pseudos[i])) {
            return std::nullopt;
        }

        if (mode == 2 && !(iss >> state.equipes[i])) {
            return std::nullopt;
        }
    }

    return state;
}

inline std::string makeInputState(const InputState& input) {
    std::ostringstream oss;
    oss << "A "
        << input.playerId << ' '
        << static_cast<int>(input.z) << ' '
        << static_cast<int>(input.q) << ' '
        << static_cast<int>(input.s) << ' '
        << static_cast<int>(input.d) << ' '
        << static_cast<int>(input.x) << ' '
        << input.mouseX << ' '
        << input.mouseY << ' '
        << static_cast<int>(input.clicked);
    return oss.str();
}

inline std::optional<InputState> parseInputState(const std::string& message) {
    if (message.empty() || message[0] != 'A') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    InputState input;
    char type = '\0';
    int z = 0;
    int q = 0;
    int s = 0;
    int d = 0;
    int x = 0;
    int clicked = 0;

    if (!(iss >> type >> input.playerId >> z >> q >> s >> d >> x >> input.mouseX >> input.mouseY >> clicked)) {
        return std::nullopt;
    }

    input.z = z != 0;
    input.q = q != 0;
    input.s = s != 0;
    input.d = d != 0;
    input.x = x != 0;
    input.clicked = clicked != 0;
    return input;
}

inline std::string makeTankChoice(const TankChoice& choice) {
    std::ostringstream oss;
    oss << "K " << choice.playerId << ' ' << choice.tankType;
    return oss.str();
}

inline std::optional<TankChoice> parseTankChoice(const std::string& message) {
    if (message.empty() || message[0] != 'K') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    TankChoice choice;
    char type = '\0';

    if (!(iss >> type >> choice.playerId >> choice.tankType)) {
        return std::nullopt;
    }

    return choice;
}

inline std::string makeTankState(const TankState& state) {
    std::ostringstream oss;
    oss << "T " << state.playerId << ' '
        << state.x << ' '
        << state.y << ' '
        << state.baseRotation << ' '
        << state.turretRotation << ' '
        << state.ultiState;
    return oss.str();
}

inline std::optional<TankState> parseTankState(const std::string& message) {
    if (message.empty() || message[0] != 'T') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    TankState state;
    char type = '\0';

    if (!(iss >> type >> state.playerId >> state.x >> state.y >> state.baseRotation >> state.turretRotation >> state.ultiState)) {
        return std::nullopt;
    }

    return state;
}

inline std::string makeTankList(const std::vector<std::pair<int, int>>& tankTypes) {
    std::ostringstream oss;
    oss << "B";

    for (const auto& [playerId, tankType] : tankTypes) {
        oss << ' ' << playerId << ' ' << tankType;
    }

    return oss.str();
}

inline std::optional<std::vector<std::pair<int, int>>> parseTankList(const std::string& message) {
    if (message.empty() || message[0] != 'B') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    char type = '\0';
    iss >> type;

    std::vector<std::pair<int, int>> tankTypes;
    int playerId = 0;
    int tankType = 0;

    while (iss >> playerId >> tankType) {
        tankTypes.emplace_back(playerId, tankType);
    }

    return tankTypes;
}

inline std::string makeTankChoiceProgress(int count) {
    std::ostringstream oss;
    oss << "N " << count;
    return oss.str();
}

inline std::optional<int> parseTankChoiceProgress(const std::string& message) {
    if (message.empty() || message[0] != 'N') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    char type = '\0';
    int count = 0;

    if (!(iss >> type >> count)) {
        return std::nullopt;
    }

    return count;
}

inline std::string makeReadySignal() {
    return "W 1";
}

inline bool isReadySignal(const std::string& message) {
    return message.rfind("W", 0) == 0;
}

inline std::string makeHealthState(const std::array<int, kMaxPlayers>& hp) {
    std::ostringstream oss;
    oss << "V";

    for (int value : hp) {
        oss << ' ' << value;
    }

    return oss.str();
}

inline std::optional<std::array<int, kMaxPlayers>> parseHealthState(const std::string& message) {
    if (message.empty() || message[0] != 'V') {
        return std::nullopt;
    }

    std::istringstream iss(message);
    std::array<int, kMaxPlayers> hp{};
    char type = '\0';

    if (!(iss >> type)) {
        return std::nullopt;
    }

    for (int i = 0; i < kMaxPlayers; ++i) {
        if (!(iss >> hp[i])) {
            return std::nullopt;
        }
    }

    return hp;
}

inline std::string makeStatsPacket(const float stats[kMaxPlayers][4]) {
    std::string packet(1 + sizeof(float) * kMaxPlayers * 4, '\0');
    packet[0] = 'Z';
    std::memcpy(packet.data() + 1, stats, sizeof(float) * kMaxPlayers * 4);
    return packet;
}

inline bool parseStatsPacket(const char* buffer, std::size_t size, float stats[kMaxPlayers][4]) {
    const std::size_t expectedSize = 1 + sizeof(float) * kMaxPlayers * 4;

    if (size != expectedSize || buffer == nullptr || buffer[0] != 'Z') {
        return false;
    }

    std::memcpy(stats, buffer + 1, sizeof(float) * kMaxPlayers * 4);
    return true;
}

}  // namespace network

#endif

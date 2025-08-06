#pragma once

#include "Core/Module.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <queue>

namespace Daisy {

enum class NetworkMode {
    Server,
    Client,
    Dedicated
};

struct NetworkMessage {
    uint32_t type;
    std::vector<uint8_t> data;
    uint32_t senderId = 0;
    bool reliable = true;
};

class DaisyNet : public Module {
public:
    DaisyNet();
    virtual ~DaisyNet() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    bool StartServer(int port);
    bool ConnectToServer(const std::string& address, int port);
    void Disconnect();
    
    void SendMessage(const NetworkMessage& message);
    void BroadcastMessage(const NetworkMessage& message);
    
    void SetMessageHandler(std::function<void(const NetworkMessage&)> handler);
    
    bool IsServer() const { return m_networkMode == NetworkMode::Server; }
    bool IsClient() const { return m_networkMode == NetworkMode::Client; }
    bool IsConnected() const { return m_connected; }
    
    void EnableModSupport(bool enable) { m_modSupportEnabled = enable; }
    void SetMaxClients(int max) { m_maxClients = max; }
    
private:
    void ProcessIncomingMessages();
    void ProcessOutgoingMessages();
    void HandleClientConnection(uint32_t clientId);
    void HandleClientDisconnection(uint32_t clientId);
    
    NetworkMode m_networkMode = NetworkMode::Client;
    bool m_connected = false;
    bool m_modSupportEnabled = true;
    int m_maxClients = 1000;
    
    std::vector<uint32_t> m_connectedClients;
    std::function<void(const NetworkMessage&)> m_messageHandler;
    
    std::vector<NetworkMessage> m_incomingMessages;
    std::vector<NetworkMessage> m_outgoingMessages;
};

}
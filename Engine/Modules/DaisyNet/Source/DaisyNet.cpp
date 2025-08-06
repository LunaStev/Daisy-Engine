#include "DaisyNet.h"
#include "Core/Logger.h"
#include <algorithm>

namespace Daisy {

DaisyNet::DaisyNet() : Module("DaisyNet") {
}

bool DaisyNet::Initialize() {
    DAISY_INFO("Initializing Daisy Network Engine");
    
    m_initialized = true;
    DAISY_INFO("Daisy Network Engine initialized successfully");
    return true;
}

void DaisyNet::Update(float deltaTime) {
    if (!m_initialized) return;
    
    ProcessIncomingMessages();
    ProcessOutgoingMessages();
}

void DaisyNet::Shutdown() {
    if (!m_initialized) return;
    
    DAISY_INFO("Shutting down Daisy Network Engine");
    
    if (m_connected) {
        Disconnect();
    }
    
    m_initialized = false;
    DAISY_INFO("Daisy Network Engine shut down successfully");
}

bool DaisyNet::StartServer(int port) {
    DAISY_INFO("Starting server on port {}", port);
    
    m_networkMode = NetworkMode::Server;
    m_connected = true;
    
    return true;
}

bool DaisyNet::ConnectToServer(const std::string& address, int port) {
    DAISY_INFO("Connecting to server at {}:{}", address, port);
    
    m_networkMode = NetworkMode::Client;
    m_connected = true;
    
    return true;
}

void DaisyNet::Disconnect() {
    DAISY_INFO("Disconnecting from network");
    
    m_connected = false;
    m_connectedClients.clear();
    m_incomingMessages.clear();
    m_outgoingMessages.clear();
}

void DaisyNet::SendMessage(const NetworkMessage& message) {
    if (!m_connected) return;
    
    m_outgoingMessages.push_back(message);
}

void DaisyNet::BroadcastMessage(const NetworkMessage& message) {
    if (!m_connected || m_networkMode != NetworkMode::Server) return;
    
    // Broadcast to all connected clients
    for (uint32_t clientId : m_connectedClients) {
        NetworkMessage clientMessage = message;
        clientMessage.senderId = clientId;
        m_outgoingMessages.push_back(clientMessage);
    }
}

void DaisyNet::SetMessageHandler(std::function<void(const NetworkMessage&)> handler) {
    m_messageHandler = handler;
}

void DaisyNet::ProcessIncomingMessages() {
    for (const auto& message : m_incomingMessages) {
        if (m_messageHandler) {
            m_messageHandler(message);
        }
    }
    m_incomingMessages.clear();
}

void DaisyNet::ProcessOutgoingMessages() {
    // Network send implementation would be here
    m_outgoingMessages.clear();
}

void DaisyNet::HandleClientConnection(uint32_t clientId) {
    m_connectedClients.push_back(clientId);
    DAISY_INFO("Client {} connected", clientId);
}

void DaisyNet::HandleClientDisconnection(uint32_t clientId) {
    auto it = std::find(m_connectedClients.begin(), m_connectedClients.end(), clientId);
    if (it != m_connectedClients.end()) {
        m_connectedClients.erase(it);
        DAISY_INFO("Client {} disconnected", clientId);
    }
}

}
#pragma once

#include "Types.h"

#include <string>
#include <vector>

namespace chip {
namespace castingsdk {
namespace player {

using namespace memory;
using namespace endpoint;

enum ConnectionError
{
    NO_ERROR          = 0,
    FAILED_TO_CONNECT = 1
};

using ConnectCallback    = std::function<void(ConnectionError)>;
using DisconnectCallback = std::function<void(void)>;

class Attributes
{
public:
    std::string id;
    std::string name;
    std::string product_id;
    std::string vendor_id;
    std::string type;
};

class CastingPlayer : public std::enable_shared_from_this<CastingPlayer>
{
private:
    std::vector<Strong<Endpoint>> endpoints;
    bool connected = false;
    Attributes attributes;

public:
    std::string GetId() const { return attributes.id; }

    std::string GetName() const { return attributes.name; }

    std::string GetProductId() const { return attributes.product_id; }

    std::string GetVendorId() const { return attributes.vendor_id; }

    std::string GetType() const { return attributes.type; }

public:
    void RegisterEndpoint(const Strong<Endpoint> endpoint) { endpoints.push_back(endpoint); }

    const std::vector<Strong<Endpoint>> GetEndpoints() const { return endpoints; }

public:
    bool IsConnected() const { return connected; }

    void Connect(const long timeout, ConnectCallback onCompleted);
    void Disconnect(const long timeout, DisconnectCallback onCompleted);
};

}; // namespace player
}; // namespace castingsdk
}; // namespace chip

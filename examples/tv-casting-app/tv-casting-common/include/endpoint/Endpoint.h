#pragma once

#include "Types.h"

#include <iostream>
#include <map>
#include <memory>
#include <type_traits>

namespace chip {
namespace castingsdk {
namespace endpoint {

using namespace memory;
using namespace player;
using namespace cluster;

class Attributes
{
public:
    std::string id;
    std::string name;
    std::string product_id;
    std::string vendor_id;
    std::string type;
};

class Endpoint : public std::enable_shared_from_this<Endpoint>
{

private:
    Weak<CastingPlayer> player;

    std::map<cluster::ClusterId, Strong<BaseCluster>> clusters;
    Attributes attributes;

public:
    Endpoint(Weak<CastingPlayer> player, const Attributes & attributes)
    {
        this->player     = player;
        this->attributes = attributes;
    }

    ~Endpoint() {}

    Endpoint()                       = delete;
    Endpoint(Endpoint & other)       = delete;
    void operator=(const Endpoint &) = delete;

protected:
    Strong<CastingPlayer> GetPlayer() const { return player.lock(); }

public:
    std::string GetId() const { return attributes.id; }

    std::string GetName() const { return attributes.name; }

    std::string GetProductId() const { return attributes.product_id; }

    std::string GetVendorId() const { return attributes.vendor_id; }

    std::string GetType() const { return attributes.type; }

public:
    template <typename T>
    void RegisterCluster(const cluster::ClusterId clusterId)
    {
        static_assert(std::is_base_of<cluster::BaseCluster, T>::value, "T must be derived from BaseCluster");
        auto cluster        = std::make_shared<T>(shared_from_this());
        clusters[clusterId] = cluster;
    }

    template <typename T>
    Strong<T> GetCluster()
    {
        static_assert(std::is_base_of<cluster::BaseCluster, T>::value, "T must be derived from BaseCluster");
        for (const auto & pair : clusters)
        {
            auto cluster = std::dynamic_pointer_cast<T>(pair.second);
            if (cluster)
            {
                return cluster;
            }
        }
        return nullptr;
    }
};

}; // namespace endpoint
}; // namespace castingsdk
}; // namespace chip

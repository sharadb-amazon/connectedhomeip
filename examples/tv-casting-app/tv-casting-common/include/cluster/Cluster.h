#pragma once

#include "Types.h"

#include <iostream>
#include <map>
#include <memory>
#include <type_traits>

namespace chip {
namespace castingsdk {
namespace cluster {

using namespace memory;
using namespace endpoint;

// Base cluster class
class BaseCluster
{
private:
protected:
    Weak<Endpoint> endpoint;

public:
    BaseCluster(Weak<Endpoint> endpoint) { this->endpoint = endpoint; }

    virtual ~BaseCluster() {}

    BaseCluster()                       = delete;
    BaseCluster(BaseCluster & other)    = delete;
    void operator=(const BaseCluster &) = delete;

protected:
    Weak<Endpoint> GetEndpoint() const { return endpoint.lock(); }
};

}; // namespace cluster
}; // namespace castingsdk
}; // namespace chip

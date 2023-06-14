#pragma once

#include <cstdint>
#include <memory>

namespace chip {
namespace castingsdk {

namespace memory {

template <typename T>
using Weak = std::weak_ptr<T>;

template <typename T>
using Strong = std::shared_ptr<T>;

template <typename T>
using Optional = std::optional<T>;

} // namespace memory

namespace endpoint {

class Endpoint;

};

namespace cluster {

class BaseCluster;

using ClusterId = uint32_t;
using CommandId = uint32_t;

}; // namespace cluster

namespace attribute {
using AttributeId = uint32_t;
}

namespace player {

class CastingPlayer;

};

}; // namespace castingsdk
}; // namespace chip

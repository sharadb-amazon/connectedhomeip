#pragma once

#include "Types.h"

namespace chip {
namespace castingsdk {
namespace attribute {

using namespace memory;
using namespace cluster;

enum ReadAttributeError
{
    READ_ATTRIBUTE_NO_ERROR
};

enum WriteAttributeError
{
    WRITE_ATTRIBUTE_NO_ERROR
};

template <typename ValueType>
using ReadAttributeCallback = std::function<void(Optional<ValueType> before, ValueType after, ReadAttributeError)>;

using WriteAttributeCallback = std::function<void(WriteAttributeError)>;

template <typename ValueType>
class Attribute
{
private:
    Weak<BaseCluster> cluster;
    ValueType value;

public:
    Attribute(Weak<BaseCluster> cluster) { this->cluster = cluster; }

    ~Attribute() {}

    Attribute()                       = delete;
    Attribute(Attribute & other)      = delete;
    void operator=(const Attribute &) = delete;

protected:
    Strong<BaseCluster> GetCluster() const { return cluster.lock(); }

public:
    ValueType GetValue();
    void Read(ReadAttributeCallback<ValueType> onRead);
    void Write(ValueType value, WriteAttributeCallback onWrite);
    bool SubscribeAttribute(AttributeId attributeId, ReadAttributeCallback<ValueType> callback);
    bool UnsubscribeAttribute(AttributeId attributeId, ReadAttributeCallback<ValueType> callback);
};

}; // namespace attribute
}; // namespace castingsdk
}; // namespace chip

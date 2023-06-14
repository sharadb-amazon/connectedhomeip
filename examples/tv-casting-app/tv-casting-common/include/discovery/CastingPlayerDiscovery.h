#pragma once

#include "Types.h"

#include <vector>

namespace chip {
namespace castingsdk {
namespace discovery {

using namespace player;

class Delegate
{
public:
    virtual void HandleOnAdded(std::vector<Strong<CastingPlayer>> players);
    virtual void HandleOnUpdated(std::vector<Strong<CastingPlayer>> players);
    virtual void HandleOnRemoved(std::vector<Strong<CastingPlayer>> players);
};

class CastingPlayerDiscovery
{

private:
    CastingPlayerDiscovery();

    CastingPlayerDiscovery(CastingPlayerDiscovery & other) = delete;
    void operator=(const CastingPlayerDiscovery &)         = delete;

public:
    static CastingPlayerDiscovery * GetInstance();

    void StartDiscovery();

    void StopDiscovery();

    void SetDelegate(Delegate delegate);

    std::vector<Strong<CastingPlayer>> GetCastingPlayers();
};

}; // namespace discovery
}; // namespace castingsdk
}; // namespace chip

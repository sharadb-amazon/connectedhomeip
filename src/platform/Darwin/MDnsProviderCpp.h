//
//  MDnsProviderCpp.h
//  MDNS-SD
//
//  Created by Hilal, Rawad on 2/15/23.
//

#ifndef MDnsProviderCpp_hpp
#define MDnsProviderCpp_hpp

#include <stdio.h>
#include <stdint.h>

namespace chip {
namespace Dnssd {

/**
  Reference to mDns provider.
 */
typedef struct MDnsProvider MDnsProvider;

/**
 Defines the state of the mDNS provider.
 */
typedef enum _MDnsProviderState
{
    /** The provider is not running. */
    NOT_RUNNING,
    /** The provider is initializing. */
    STARTING,
    /** The provider is running. */
    RUNNING,
    /** The provider failed. */
    FAILED
}MDnsProviderState;

/**
 * The handler that will be invoked when the state of the provider changes.
 * @param provider The provider instance.
 * @param state The provider state.
 * @param error_code The error code associated to the failed state. 0 if no error.
 * @param context The user-provided context object that is passed to DnsProviderSetStateHandler.
 */
typedef void (*MDnsProviderStateHandler)
(
 MDnsProvider* provider,
 MDnsProviderState state,
 int error_code,
 void* context
 );

/**
 * The various flags for the browse event.
 * kDnsProviderBrowseFlagsAdd The endpoint was added since the last browse event.
 * kDnsProviderBrowseFlagsRemove The endpoint was removed since the last browse event.
 * kDnsProviderBrowseFlagsUpdate The endpoint was updated since the last browse event.
 * kDnsProviderBrowseFlagsBatchComplete When set, the provider has no more immediate changes to
 *        report to the user.
 */
enum _MDnsProviderBrowseFlags {
    kMDnsProviderBrowseFlagsAdd              = 1,
    kMDnsProviderBrowseFlagsRemove           = 1 << 1,
    kMDnsProviderBrowseFlagsUpdate           = 1 << 2,
    kMDnsProviderBrowseFlagsBatchComplete    = 1 << 3
};
typedef int MDnsProviderBrowseFlags;

/**
 * The handler that will be invoked when the provider finds an endpoint.
 * @param provider The provider instance.
 * @param flags The flags of the browse event. @see DnsProviderBrowseFlags.
 * @param name Service name string for the endpoint.
 * @param type Service type string for the endpoint.
 * @param domain Domain string for the endpoint.
 * @param interface_id Interface id of the endpoint.
 * @param context The user-provided context object that is passed to DnsProviderSetStateHandler.
 */
typedef void (*MDnsProviderBrowseHandler)
(
 MDnsProvider* provider,
 const MDnsProviderBrowseFlags flags,
 const char* name,
 const char* type,
 const char* domain,
 uint32_t interface_id,
 void* context
 );

/**
 * Create a mDNS provider.
 * @param service_type The service type. Ex. _matterd._udp
 * @param domain The domain. Ex. `local.`.
 * @return The mDNS provider instance.
 */
MDnsProvider* MDnsProviderCreate(const char* service_type, const char* domain);

/**
 * Release the mDNS provider from memory.
 * @param provider The mDNS provider.
 */
void MDnsProviderDelete(MDnsProvider* provider);

/**
 * Set the mDNS provider state handler.
 * @param provider The mDNS provider.
 * @param handler The handler.
 * @param context The user context that will be provided as part of the callback.
 */
void MDnsProviderSetStateHandler(MDnsProvider* provider, MDnsProviderStateHandler handler, void* context);

/**
 * Set the mDNS provider browse handler.
 * @param provider The mDNS provider.
 * @param handler The handler.
 * @param context The user context that will be provided as part of the callback.
 */
void MDnsProviderSetBrowseHandler(MDnsProvider* provider, MDnsProviderBrowseHandler handler, void* context);

/**
 * Start browsing.
 * @param provider The mDNS provider.
 */
void MDnsProviderStartBrowsing(MDnsProvider* provider);

/**
 * Stop browsing.
 * @param provider The mDNS provider.
 */
void MDnsProviderStopBrowsing(MDnsProvider* provider);

}
}
#endif /* MDnsProviderCpp_hpp */

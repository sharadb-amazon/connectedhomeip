/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef MDnsProvider_h
#define MDnsProvider_h

#include <lib/support/CHIPMem.h>

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

/**
 * In C++, you can take advantage of the MDnsProviderRef which automatically manages memory and encapsulates the MDnsProvider.
 *
 * When all references to the object are lost, the class will automatically release the underlying MDnsProvider object and terminate the browsing process.
 */
class MDnsProviderRef
{
private:
    const int kInitRefCount = 1;
    using CounterType = uint32_t;
    
    static void* operator new(size_t) = delete;
    static void* operator new[](size_t) = delete;
    static void  operator delete(void*)  = delete;
    static void  operator delete[](void*)  = delete;
    
public:
    
    explicit MDnsProviderRef(MDnsProvider* ptr = nullptr)
    {
        _provider = ptr;
        _counter = chip::Platform::New<CounterType>();
        *_counter = kInitRefCount;
    }
    
    MDnsProviderRef(const char* service_type, const char* domain)
    {
        _provider = MDnsProviderCreate(service_type, domain);
        _counter = chip::Platform::New<CounterType>();
        *_counter = kInitRefCount;
    }
    
    MDnsProviderRef(const MDnsProviderRef& ref)
    {
        _provider = ref._provider;
        _counter = ref._counter;
        if (ref._provider != nullptr)
        {
            (*_counter)++;
        }
    }
    
    MDnsProviderRef(MDnsProviderRef&& ref) noexcept
    {
        _cleanup();
        
        _provider = ref._provider;
        _counter = ref._counter;
        ref._provider = nullptr;
        ref._counter = nullptr;
    }
    
    ~MDnsProviderRef()
    {
        _cleanup();
        _provider = nullptr;
        _counter = nullptr;
    }
    
    MDnsProvider* get()
    {
        return _provider;
    }
    
    MDnsProvider& operator*()
    {
      return *_provider;
    }
 
    MDnsProvider* operator->()
    {
      return _provider;
    }
    
    MDnsProviderRef& operator= (const MDnsProviderRef& ref)
    {
        if (this != &ref)
        {
            _cleanup();
            _provider = ref._provider;
            _counter = ref._counter;
            (*_counter)++;
        }
        return *this;
    }
    
    MDnsProviderRef& operator= (MDnsProviderRef && ref) noexcept
    {
        if (this != &ref)
        {
            _cleanup();
            
            _provider = ref._provider;
            _counter = ref._counter;
            
            ref._provider = nullptr;
            ref._counter = nullptr;
        }
        return *this;
    }
    
private:
    void _cleanup()
    {
        if (_counter != nullptr && --(*_counter) == 0)
        {
            if (_provider != nullptr)
            {
                MDnsProviderDelete(_provider);
                _provider = nullptr;
            }
            chip::Platform::Delete(_counter);
            _counter = nullptr;
        }
    }
    
private:
    MDnsProvider* _provider = nullptr;
    CounterType* _counter = nullptr;
};

}
}

#endif /* MDnsProvider_h */

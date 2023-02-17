//
//  MDnsProviderCpp.c
//  MDNS-SD
//
//  Created by Hilal, Rawad on 2/15/23.
//

#include "MDnsProviderCpp.h"
#include <Network/Network.h>
#include <lib/support/CHIPMem.h>

namespace chip {
namespace Dnssd {
    
typedef struct MDnsProvider
{
    nw_browser_t                browser;
    MDnsProviderState           state;
    nw_browse_descriptor_t      browse_descriptor;
    nw_parameters_t             browse_parameters;
    MDnsProviderStateHandler    state_handler;
    void*                       state_handler_context;
    MDnsProviderBrowseHandler   browse_handler;
    void*                       browse_handler_context;
}MDnsProvider;
    
void _MDnsProviderInitializeBrowser(MDnsProvider* provider);

MDnsProvider* MDnsProviderCreate(const char* service_type, const char* domain)
{
    MDnsProvider* provider = chip::Platform::New<MDnsProvider>();
    
    provider->browser = NULL;
    provider->state = NOT_RUNNING;
    provider->browse_descriptor = nw_browse_descriptor_create_bonjour_service(service_type, domain);
    provider->browse_parameters = nw_parameters_create();
    provider->state_handler = NULL;
    provider->state_handler_context = NULL;
    provider->browse_handler = NULL;
    provider->browse_handler_context = NULL;
    
    return provider;
}

void MDnsProviderDelete(MDnsProvider* provider)
{
    assert(provider != NULL);
    
    provider->state_handler = NULL;
    provider->browse_handler = NULL;
    
    if (provider->browser != NULL)
    {
        MDnsProviderStopBrowsing(provider);
    }
    
    nw_release(provider->browse_descriptor);
    nw_release(provider->browse_parameters);
    
    chip::Platform::Delete(provider);
}

void MDnsProviderSetStateHandler(MDnsProvider* provider, MDnsProviderStateHandler handler, void* context)
{
    provider->state_handler = handler;
    provider->state_handler_context = context;
}

void MDnsProviderSetBrowseHandler(MDnsProvider* provider, MDnsProviderBrowseHandler handler, void* context)
{
    provider->browse_handler = handler;
    provider->browse_handler_context = context;
}

void MDnsProviderStartBrowsing(MDnsProvider* provider)
{
    if (provider->state == RUNNING || provider->state == STARTING)
    {
        return;
    }
    
    provider->state = STARTING;
    
    _MDnsProviderInitializeBrowser(provider);
    nw_browser_start(provider->browser);
}

void MDnsProviderStopBrowsing(MDnsProvider* provider)
{
    if (provider->state != RUNNING && provider->state != STARTING)
    {
        return;
    }
    
    if (provider->browser == NULL)
    {
        return;
    }
    
    nw_browser_set_state_changed_handler(provider->browser, NULL);
    nw_browser_set_browse_results_changed_handler(provider->browser, NULL);
    nw_browser_cancel(provider->browser);
    nw_release(provider->browser);
    
    provider->state = NOT_RUNNING;
    provider->browser = NULL;
}

void _MDnsProviderInitializeBrowser(MDnsProvider* provider)
{
    provider->browser = nw_browser_create(provider->browse_descriptor, provider->browse_parameters);
    nw_retain(provider->browser);
    
    nw_browser_set_queue(provider->browser, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
    nw_browser_set_state_changed_handler(provider->browser, ^(nw_browser_state_t state, nw_error_t  _Nullable error) {
        int error_code = 0;
        MDnsProviderState dns_provider_state = NOT_RUNNING;
        switch (state) {
            case nw_browser_state_ready:
            case nw_browser_state_waiting:
                dns_provider_state = RUNNING;
                break;
            case nw_browser_state_failed:
            case nw_browser_state_invalid:
                dns_provider_state = FAILED;
                nw_release(provider->browser);
                break;
            case nw_browser_state_cancelled:
                dns_provider_state = NOT_RUNNING;
                nw_release(provider->browser);
                break;
            default:
                break;
        }
        provider->state = dns_provider_state;
        if (error != NULL)
        {
            error_code = nw_error_get_error_code(error);
        }
        
        if (provider->state_handler != NULL)
        {
            provider->state_handler(provider, dns_provider_state, error_code, provider->state_handler_context);
        }
    });
    
    nw_browser_set_browse_results_changed_handler(provider->browser, ^(nw_browse_result_t  _Nonnull old_result, nw_browse_result_t  _Nonnull new_result, bool batch_complete) {
        
        MDnsProviderBrowseFlags flags = 0;
        nw_browse_result_t result = nil;
        if (old_result && new_result)
        {
            result = new_result;
            flags = flags | kMDnsProviderBrowseFlagsUpdate;
        }
        else if (old_result)
        {
            result = old_result;
            flags = flags | kMDnsProviderBrowseFlagsRemove;
        }
        else if (new_result)
        {
            result = new_result;
            flags = flags | kMDnsProviderBrowseFlagsAdd;
        }
        
        nw_endpoint_t endpoint = nw_browse_result_copy_endpoint(result);
        const char *name = nw_endpoint_get_bonjour_service_name(endpoint);
        const char *type = nw_endpoint_get_bonjour_service_type(endpoint);
        const char *domain = nw_endpoint_get_bonjour_service_domain(endpoint);
        
        __block size_t interface_count = nw_browse_result_get_interfaces_count(result);
        
        nw_browse_result_enumerate_interfaces(result, ^bool(nw_interface_t  _Nonnull interface) {
            MDnsProviderBrowseFlags final_flags = flags;
            
            bool is_last_interface = (--interface_count == 0);
            if (batch_complete && is_last_interface)
            {
                final_flags = final_flags | kMDnsProviderBrowseFlagsBatchComplete;
            }
            
            uint32_t interface_id = nw_interface_get_index(interface);
            
            if (provider->browse_handler != NULL)
            {
                provider->browse_handler(
                    provider, final_flags, name, type, domain, interface_id, provider->browse_handler_context);
            }
            return true;
        });
    });
}
    
}
}

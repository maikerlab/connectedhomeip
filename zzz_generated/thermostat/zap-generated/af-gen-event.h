/**
 *
 *    Copyright (c) 2020 Project CHIP Authors
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

/**
 *
 *    Copyright (c) 2020 Silicon Labs
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
// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef __AF_GEN_EVENT__
#define __AF_GEN_EVENT__

// Code used to configure the cluster event mechanism
#define EMBER_AF_GENERATED_EVENT_CODE                                                                                              \
    EmberEventControl emberAfBarrierControlClusterServerTickCallbackControl1;                                                      \
    EmberEventControl emberAfIasZoneClusterServerTickCallbackControl1;                                                             \
    extern EmberEventControl emberAfPluginIasZoneServerManageQueueEventControl;                                                    \
    extern void emberAfPluginColorControlServerTempTransitionEventHandler(void);                                                   \
    extern void emberAfPluginColorControlServerXyTransitionEventHandler(void);                                                     \
    extern void emberAfPluginDoorLockServerLockoutEventHandler(void);                                                              \
    extern void emberAfPluginDoorLockServerRelockEventHandler(void);                                                               \
    extern void emberAfPluginIasZoneServerManageQueueEventHandler(void);                                                           \
    static void clusterTickWrapper(EmberEventControl * control, EmberAfTickFunction callback, chip::EndpointId endpoint)           \
    {                                                                                                                              \
        /* emberAfPushEndpointNetworkIndex(endpoint); */                                                                           \
        emberEventControlSetInactive(control);                                                                                     \
        (*callback)(endpoint);                                                                                                     \
        /* emberAfPopNetworkIndex(); */                                                                                            \
    }                                                                                                                              \
    void emberAfBarrierControlClusterServerTickCallbackWrapperFunction1(void)                                                      \
    {                                                                                                                              \
        clusterTickWrapper(&emberAfBarrierControlClusterServerTickCallbackControl1,                                                \
                           emberAfBarrierControlClusterServerTickCallback, 1);                                                     \
    }                                                                                                                              \
    void emberAfIasZoneClusterServerTickCallbackWrapperFunction1(void)                                                             \
    {                                                                                                                              \
        clusterTickWrapper(&emberAfIasZoneClusterServerTickCallbackControl1, emberAfIasZoneClusterServerTickCallback, 1);          \
    }

// EmberEventData structs used to populate the EmberEventData table
#define EMBER_AF_GENERATED_EVENTS                                                                                                  \
    { &emberAfBarrierControlClusterServerTickCallbackControl1, emberAfBarrierControlClusterServerTickCallbackWrapperFunction1 },

#define EMBER_AF_GENERATED_EVENT_STRINGS                                                                                           \
    "Barrier Control Cluster Server EP 1", "Color Control Cluster Server Plugin HueSatTransition",                                 \
        "Color Control Cluster Server Plugin TempTransition", "Color Control Cluster Server Plugin XyTransition",                  \
        "Door Lock Server Cluster Plugin Lockout", "Door Lock Server Cluster Plugin Relock", "IAS Zone Server Plugin ManageQueue",

// The length of the event context table used to track and retrieve cluster events
#define EMBER_AF_EVENT_CONTEXT_LENGTH 1

// EmberAfEventContext structs used to populate the EmberAfEventContext table
#define EMBER_AF_GENERATED_EVENT_CONTEXT                                                                                           \
    { 0x1, 0x103, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfBarrierControlClusterServerTickCallbackControl1 },
#endif // __AF_GEN_EVENT__

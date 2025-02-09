# AMS - Ability Manager Service

## Introduction

`Ability Manager Service` plays an important role in OpenHarmony. This module provides functions such as managing ability stacks, managing application processes, and controlling ability slice lifecycle.

## Features

- Ability stack management
- Application process management
- Ability slice lifecycle control
- AMS tools

## Directions

```
.
├── BUILD.gn
├── include
│   ├── ability_connect_mission.h
│   ├── ability_connect_record.h
│   ├── ability_connect_trans_param.h
│   ├── ability_inner_feature.h
│   ├── ability_list.h                          # Ability list
│   ├── ability_message_id.h                    # Message ID of the AMS task
│   ├── ability_mgr_context.h
│   ├── ability_mgr_feature.h                   # AMS feature in Service Manager
│   ├── ability_mgr_handler.h                   # AMS message handler
│   ├── ability_mgr_service.h                   # Registered AMS in the Service Manager
│   ├── ability_mission_record.h                # Ability mission record
│   ├── ability_mission_stack.h                 # Ability mission record stack
│   ├── ability_record.h                        # Ability record including ability info for GT
│   ├── ability_record_manager.h                # Ability record manager for GT
│   ├── ability_stack.h                         # AMS stack for GT
│   ├── ability_stack_manager.h                 # Ability stack manager
│   ├── ability_worker.h                        # Ability task manager
│   ├── app_manager.h                           # Application process record manager
│   ├── app_record.h                            # Application process record
│   ├── client
│   │   ├── ability_dump_client.h
│   │   ├── ability_thread_client.h
│   │   ├── app_spawn_client.h
│   │   ├── bundlems_client.h
│   │   └── wms_client.h
│   ├── js_app_host.h
│   ├── page_ability_record.h
│   ├── task
│   │   ├── ability_activate_task.h
│   │   ├── ability_attach_task.h
│   │   ├── ability_background_task.h
│   │   ├── ability_connect_done_task.h
│   │   ├── ability_connect_task.h
│   │   ├── ability_disconnect_done_task.h
│   │   ├── ability_disconnect_task.h
│   │   ├── ability_dump_task.h
│   │   ├── ability_inactivate_task.h
│   │   ├── ability_start_task.h
│   │   ├── ability_stop_task.h
│   │   ├── ability_task.h
│   │   ├── ability_terminate_service_task.h
│   │   ├── ability_terminate_task.h
│   │   ├── app_restart_task.h
│   │   └── app_terminate_task.h
│   └── util
│       ├── abilityms_helper.h
│       ├── abilityms_log.h
│       └── abilityms_status.h
├── README.md
├── src
│   ├── ability_connect_mission.cpp
│   ├── ability_inner_feature.cpp
│   ├── ability_list.cpp
│   ├── ability_mgr_context.cpp
│   ├── ability_mgr_feature.cpp
│   ├── ability_mgr_handler.cpp
│   ├── ability_mgr_service.cpp
│   ├── ability_mission_record.cpp
│   ├── ability_mission_stack.cpp
│   ├── ability_record.cpp
│   ├── ability_record_manager.cpp
│   ├── ability_stack.cpp
│   ├── ability_stack_manager.cpp
│   ├── ability_worker.cpp
│   ├── app_manager.cpp
│   ├── app_record.cpp
│   ├── client
│   │   ├── ability_dump_client.cpp
│   │   ├── ability_thread_client.cpp
│   │   ├── app_spawn_client.cpp
│   │   ├── bundlems_client.cpp
│   │   └── wms_client.cpp
│   ├── js_app_host.cpp
│   ├── page_ability_record.cpp
│   ├── task
│   │   ├── ability_activate_task.cpp
│   │   ├── ability_attach_task.cpp
│   │   ├── ability_background_task.cpp
│   │   ├── ability_connect_done_task.cpp
│   │   ├── ability_connect_task.cpp
│   │   ├── ability_disconnect_done_task.cpp
│   │   ├── ability_disconnect_task.cpp
│   │   ├── ability_dump_task.cpp
│   │   ├── ability_inactivate_task.cpp
│   │   ├── ability_start_task.cpp
│   │   ├── ability_stop_task.cpp
│   │   ├── ability_terminate_service_task.cpp
│   │   ├── ability_terminate_task.cpp
│   │   ├── app_restart_task.cpp
│   │   └── app_terminate_task.cpp
│   └── util
│       ├── abilityms_helper.cpp
│       └── abilityms_status.cpp
└── tools
    ├── BUILD.gn
    ├── include
    │   └── ability_tool.h
    └── src
        ├── ability_tool.cpp
        └── main.cpp

```

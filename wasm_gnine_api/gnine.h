/*****************************************************************************
 *
 *  PROJECT:     GninE v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     GninE C & CPP api
 *  GITHUB:      https://github.com/GninE-MP/GinnE/blob/main/wasm_gnine_api/gnine.h
 *
 *  GninE is available from http://www.gnine.com/
 *
 *****************************************************************************/

#pragma once

#ifndef GNINE_HEADER
#define GNINE_HEADER

#include <stdarg.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define GNINE_INLINE __attribute__((always_inline)) inline

#ifdef __cplusplus

#define GNINE_API extern "C"
#define GNINE_EXPORT extern "C"
#define GNINE_EXPORT_NAME(name) extern "C" __attribute__((export_name(#name)))
#define GNINE_CPP
#define GNINE_NAMESPACE Gnine
#define STD_NAMESPACE std

#include <functional>
#include <string>
#include <vector>
#include <cmath>
#include <array>
#include <variant>

#else

#define GNINE_API
#define GNINE_EXPORT
#define GNINE_EXPORT_NAME(name) __attribute__((export_name(#name)))

#endif

#ifndef NULL
#define NULL (0)
#endif

#define GNINE_DECONST(type) *(type*)&

#define GNINE_MAIN GNINE_EXPORT_NAME(ModuleMain)

#define GNINE_NULL NULL
#define GNINE_NULL_PTR nullptr

#define GNINE_IMPORT(name, args, ret) __attribute__((import_module("env"), import_name(#name))) GNINE_API ret name args
#define GNINE_IMPORT_NAME(name, functionName, args, ret) __attribute__((import_module("env"), import_name(#name))) GNINE_API ret functionName args
#define GNINE_API_IMPORT(name, args, ret) GNINE_IMPORT(gnine_##name, args, ret)
#define GNINE_API_IMPORT_NAME(name, functionName, args, ret) GNINE_IMPORT_NAME(gnine_##name, functionName, args, ret)

#ifdef GNINE_CPP
#define GNINE_LUA_IMPORT(function, ...) GNINE_IMPORT(function, (__VA_ARGS__, GNINE_PTR returnValue = NULL, GNINE_I_PTR max_return_size = 0xffffffff), GNINE_I_PTR)
#define GNINE_LUA_IMPORT_NO_ARGS(function) GNINE_IMPORT(function, (GNINE_PTR returnValue = NULL, GNINE_I_PTR max_return_size = 0xffffffff), GNINE_I_PTR)
#else
#define GNINE_LUA_IMPORT(function, ...) GNINE_IMPORT(function, (__VA_ARGS__, GNINE_PTR returnValue, GNINE_I_PTR max_return_size), GNINE_I_PTR)
#define GNINE_LUA_IMPORT_NO_ARGS(function) GNINE_IMPORT(function, (GNINE_PTR returnValue, GNINE_I_PTR max_return_size), GNINE_I_PTR)
#endif

#define GNINE_C_X_TO_STRING(data) #data
#define GNINE_C_TO_STRING(macro_data) GNINE_C_X_TO_STRING(macro_data)

#define GNINE_SCRIPT_ID_ARGUMENT_INDEX (2)
#define GNINE_GET_SCRIPT_ID(argv) ((int)atoi(argv[GNINE_SCRIPT_ID_ARGUMENT_INDEX]))

#define GNINE_CALLABLE_REF_BYTE_HEADER "crbh1"

#define GNINE_PLAYER_AC_INFO(size) struct { GNINE_I8 DetectedAC[size]; GNINE_UI32 d3d9Size; GNINE_I8 d3d9MD5[size]; GNINE_I8 d3d9SHA256[size]; }

#define gnine_get_player_ac_info(player, info, max_strings_size) gnine_get_player_ac_info_r(player, (info)->DetectedAC, &((info)->d3d9Size), (info)->d3d9MD5, (info)->d3d9SHA256, max_strings_size)

typedef char GNINE_I8;
typedef unsigned char GNINE_UI8;
typedef GNINE_UI8 GNINE_BYTE;
typedef short GNINE_I16;
typedef unsigned short GNINE_UI16;
typedef int GNINE_I32;
typedef unsigned int GNINE_UI32;
typedef long long GNINE_I64;
typedef unsigned long long GNINE_UI64;
typedef float GNINE_F32;
typedef double GNINE_F64;
typedef uintptr_t GNINE_I_PTR;

typedef const GNINE_I8* GNINE_STRING;
typedef GNINE_I8* GNINE_XSTRING;

typedef GNINE_F64 GNINE_LUA_NUMBER;

typedef void* GNINE_PTR;

#ifdef GNINE_ARCHITECTURE_X64
typedef GNINE_I64 GNINE_USERDATA_T;
typedef GNINE_I64 GNINE_SHARED_POINTER_ADDRESS;

#define GNINE_MEMORY_POINTER_ADDRESS_STRING "%016X"

#else
typedef GNINE_I32 GNINE_USERDATA_T;
typedef GNINE_I32 GNINE_SHARED_POINTER_ADDRESS;

#define GNINE_MEMORY_POINTER_ADDRESS_STRING "%08X"

#endif

#define GNINE_REMOTE_REQUEST_DEFAULT_QUEUE_NAME "default"

typedef GNINE_USERDATA_T* GNINE_USERDATA;

typedef GNINE_USERDATA GNINE_ELEMENT;
typedef GNINE_USERDATA GNINE_RESOURCE;
typedef GNINE_USERDATA GNINE_ACCOUNT;
typedef GNINE_USERDATA GNINE_BAN;
typedef GNINE_USERDATA GNINE_WORKER;
typedef GNINE_USERDATA GNINE_MUTEX;
typedef GNINE_USERDATA GNINE_TIMER;
typedef GNINE_USERDATA GNINE_REMOTE_REQUEST;
//typedef GNINE_USERDATA GNINE_THREAD;

typedef GNINE_ELEMENT GNINE_COL_SHAPE;
typedef GNINE_ELEMENT GNINE_MARKER;

typedef GNINE_ELEMENT GNINE_PLAYER;
typedef GNINE_ELEMENT GNINE_TEAM;

typedef GNINE_ELEMENT GNINE_WATER;

typedef GNINE_UI8 GNINE_CALLABLE_REF[
    sizeof(GNINE_CALLABLE_REF_BYTE_HEADER) - 1 + // header -> identifier
    sizeof(GNINE_USERDATA_T)*2 +                 // 'resource' and 'reference'
    sizeof(GNINE_UI8)                            // is wasm function or not [true or false]
];

typedef GNINE_USERDATA GNINE_ARGUMENTS;

enum GNINE_ARGUMENT_TYPE : GNINE_BYTE {
    GNINE_ARGUMENT_TYPE_UNKNOWN = 0,
    GNINE_ARGUMENT_TYPE_NULL = '0',
    GNINE_ARGUMENT_TYPE_BOOLEAN = 'b',
    GNINE_ARGUMENT_TYPE_NUMBER = 'n',
    GNINE_ARGUMENT_TYPE_STRING = 's',
    GNINE_ARGUMENT_TYPE_CALLABLE = 'c',
    GNINE_ARGUMENT_TYPE_USERDATA = 'u',
    GNINE_ARGUMENT_TYPE_LIST = 'l'
};

struct GNINE_REAL_TIME {
    GNINE_I32 second;
    GNINE_I32 minute;
    GNINE_I32 hour;
    GNINE_I32 monthday;
    GNINE_I32 month;
    GNINE_I32 year;
    GNINE_I32 weekday;
    GNINE_I32 yearday;
    GNINE_I32 isdst;
    GNINE_I64 timestamp;
};

struct GNINE_COLOR {
    GNINE_BYTE b;
    GNINE_BYTE g;
    GNINE_BYTE r;
    GNINE_BYTE a;
};

struct GNINE_VECTOR2 {
    GNINE_F32 x;
    GNINE_F32 y;
};

struct GNINE_VECTOR3 {
    GNINE_F32 x;
    GNINE_F32 y;
    GNINE_F32 z;
};

typedef struct GNINE_VECTOR3 GNINE_VECTOR;

struct GNINE_MATRIX {
    struct GNINE_VECTOR3 right;
    struct GNINE_VECTOR3 front;
    struct GNINE_VECTOR3 up;
    struct GNINE_VECTOR3 pos;
};

struct GNINE_PROCESS_MEMORY_STATS {
    GNINE_UI32 virtualMemorySize;
    GNINE_UI32 residentMemorySize;
    GNINE_UI32 sharedMemorySize;
    GNINE_UI32 privateMemorySize;
};

typedef GNINE_BYTE GNINE_WORKER_STATE_TYPE;
enum GNINE_WORKER_STATE : GNINE_WORKER_STATE_TYPE {
    GNINE_WORKER_STATE_OFF = 0,
    GNINE_WORKER_STATE_STARTING = 1,
    GNINE_WORKER_STATE_RUNNING = 2,
    GNINE_WORKER_STATE_WAITING = 3,
    GNINE_WORKER_STATE_TERMINATED = 4,
    GNINE_WORKER_STATE_FINISHED = 5
};

typedef void (*GNINE_WORKER_HANDLER)(GNINE_SHARED_POINTER_ADDRESS data);

struct GNINE_TIMER_DETAILS {
    GNINE_UI32 timeLeft;
    GNINE_UI32 repeats;
    GNINE_UI32 delay;
};

struct GNINE_CAMERA_MATRIX {
    GNINE_VECTOR3 position;
    GNINE_VECTOR3 lookAt;
    GNINE_F32     roll;
    GNINE_F32     fov;
};

/*
    header & formFields code example:
    {
        GNINE_STRING headers[4] = { // 4 strings for 2 headers
            "Content-Type", "application/json",
            "User", "Gnine"
        };
        GNINE_STRING formFields[6] = {
            "name", "Gnine",
            "version", "1.0.0",
            "age", "1"
        };
        
        GNINE_REMOTE_REQUEST_OPTIONS options;
        options.headers = headers;
        options.headersCount = 2; // size of the `headers` array which is `4` devided by 2
        options.formFields = formFields;
        options.formFieldsCount = 3;
    }
*/
struct GNINE_REMOTE_REQUEST_OPTIONS {
    GNINE_UI32    connectionAttempts;
    GNINE_UI32    connectTimeout;
    GNINE_STRING  method;
    GNINE_STRING  queueName;
    GNINE_STRING  postData;
    bool          postIsBinary;
    GNINE_UI32    maxRedirects;
    GNINE_STRING  username;
    GNINE_STRING  password;
    GNINE_STRING* headers;    //this is a list which contains 2 strings for each header. first one is `header_name` and second one is `header_value`.
    GNINE_UI32    headersCount;
    GNINE_STRING* formFields;
    GNINE_UI32    formFieldsCount;
};

struct GNINE_REMOTE_REQUEST_INFO {
    bool           isFetch;
    GNINE_STRING   url;
    GNINE_STRING   queue;
    GNINE_RESOURCE resource;
    GNINE_UI64     start;
    GNINE_STRING   postData;
    GNINE_UI32     postDataSize;
    GNINE_STRING*  headers;       // this headers are like `GNINE_REMOTE_REQUEST_OPTIONS` headers. that means you can select header name with `headers[0]` and header value `header[1]`
    GNINE_UI32     headersCount;  // if we had 2 items in `headers` that means this `headersCount` will equal to `1`
    GNINE_STRING   method;
    GNINE_UI32     connectionAttempts;
    GNINE_UI32     connectionTimeout;
    GNINE_UI32     bytesReceived;
    GNINE_UI32     bytesTotal;
    GNINE_UI32     currentAttempt;
};

GNINE_API_IMPORT(print_data, (GNINE_STRING data), void);

GNINE_API_IMPORT(get_shared_pointer_address, (GNINE_PTR ptr), GNINE_SHARED_POINTER_ADDRESS);
GNINE_API_IMPORT(get_pointer_from_shared_pointer, (GNINE_SHARED_POINTER_ADDRESS ptr), GNINE_PTR);
GNINE_API_IMPORT(does_shared_pointer_address_belong_to_this_module, (GNINE_SHARED_POINTER_ADDRESS shared_ptr), bool);
GNINE_API_IMPORT(does_shared_pointer_address_belong_to_this_resource, (GNINE_SHARED_POINTER_ADDRESS shared_ptr), bool);
GNINE_API_IMPORT(read_shared_pointer_address, (GNINE_SHARED_POINTER_ADDRESS shared_ptr, GNINE_BYTE* data, GNINE_I_PTR size), GNINE_I_PTR);
GNINE_API_IMPORT(write_shared_pointer_address, (GNINE_SHARED_POINTER_ADDRESS shared_ptr, const GNINE_BYTE* data, GNINE_I_PTR size), GNINE_I_PTR);
GNINE_API_IMPORT(get_max_shared_pointer_size, (GNINE_SHARED_POINTER_ADDRESS shared_ptr), GNINE_I_PTR);
GNINE_API_IMPORT(get_memory_free_space, (), GNINE_I_PTR);

GNINE_API_IMPORT(get_tick_count, (), GNINE_I64);
GNINE_API_IMPORT(get_real_time, (struct GNINE_REAL_TIME* out_time, GNINE_I64 seconds, bool local_time), bool);
GNINE_API_IMPORT(get_userdata_type, (GNINE_USERDATA userdata, GNINE_XSTRING out_string, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_color_from_string, (GNINE_STRING colorString, struct GNINE_COLOR* out_color), bool);

GNINE_API_IMPORT(get_distance_between_points_2D, (struct GNINE_VECTOR2* point_1, struct GNINE_VECTOR2* point_2), GNINE_F32);
GNINE_API_IMPORT(get_distance_between_points_3D, (struct GNINE_VECTOR3* point_1, struct GNINE_VECTOR3* point_2), GNINE_F32);
GNINE_API_IMPORT(get_easing_value, (GNINE_F32 progress, GNINE_STRING easing_type, GNINE_F32 easing_period, GNINE_F32 easing_amplitude, GNINE_F32 easing_overshoot), GNINE_F32);
GNINE_API_IMPORT(interpolate_between, (struct GNINE_VECTOR3* point_1, struct GNINE_VECTOR3* point_2, struct GNINE_VECTOR3* out_point, GNINE_F32 progress, GNINE_STRING easingType, GNINE_F32 easing_period, GNINE_F32 easing_amplitude, GNINE_F32 easing_overshoot), bool);

GNINE_API_IMPORT(debug_sleep, (GNINE_I32 time_ms), bool);

GNINE_API_IMPORT(to_color, (GNINE_I32 r, GNINE_I32 g, GNINE_I32 b, GNINE_I32 a), GNINE_I32);
GNINE_API_IMPORT(get_process_memory_stats, (struct GNINE_PROCESS_MEMORY_STATS* out_status), bool);

GNINE_API_IMPORT(call_callable, (GNINE_CALLABLE_REF function, GNINE_ARGUMENTS args, GNINE_XSTRING out_error, GNINE_I_PTR max_error_size), GNINE_ARGUMENTS);
GNINE_API_IMPORT(is_valid_callable, (GNINE_CALLABLE_REF function), bool);
GNINE_API_IMPORT(get_callable_declaration, (GNINE_CALLABLE_REF function, GNINE_STRING function_name, GNINE_XSTRING out_string, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(c_function_to_callable, (GNINE_PTR function , GNINE_CALLABLE_REF callableRef), bool);
GNINE_API_IMPORT(get_callable_resource, (GNINE_CALLABLE_REF callableRef), GNINE_USERDATA);
GNINE_API_IMPORT(get_callable_reference, (GNINE_CALLABLE_REF callableRef), GNINE_USERDATA);
GNINE_API_IMPORT(is_callable_wasm_function, (GNINE_CALLABLE_REF callableRef), bool);
GNINE_API_IMPORT(free_callable, (GNINE_CALLABLE_REF callableRef), bool);

GNINE_API_IMPORT(args_create, (), GNINE_ARGUMENTS);
GNINE_API_IMPORT(args_delete, (GNINE_ARGUMENTS args), void);
GNINE_API_IMPORT(args_push_null, (GNINE_ARGUMENTS args), void);
GNINE_API_IMPORT(args_push_boolean, (GNINE_ARGUMENTS args, bool arg), void);
GNINE_API_IMPORT(args_push_number, (GNINE_ARGUMENTS args, GNINE_LUA_NUMBER arg), void);
GNINE_API_IMPORT(args_push_string, (GNINE_ARGUMENTS args, GNINE_STRING arg), void);
GNINE_API_IMPORT(args_push_callable, (GNINE_ARGUMENTS args, GNINE_CALLABLE_REF callable), void);
GNINE_API_IMPORT(args_push_userdata, (GNINE_ARGUMENTS args, GNINE_USERDATA arg), void);
GNINE_API_IMPORT(args_push_list, (GNINE_ARGUMENTS args, GNINE_ARGUMENTS arg), void);
GNINE_API_IMPORT(args_pop, (GNINE_ARGUMENTS args, GNINE_I32 count), void);
GNINE_API_IMPORT(args_get_arg_type, (GNINE_ARGUMENTS args, GNINE_I32 index), GNINE_BYTE);
GNINE_API_IMPORT(args_get_arg_as_boolean, (GNINE_ARGUMENTS args, GNINE_I32 index), bool);
GNINE_API_IMPORT(args_get_arg_as_number, (GNINE_ARGUMENTS args, GNINE_I32 index), GNINE_LUA_NUMBER);
GNINE_API_IMPORT(args_get_arg_as_string, (GNINE_ARGUMENTS args, GNINE_I32 index, GNINE_XSTRING out_string, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(args_get_arg_as_callable, (GNINE_ARGUMENTS args, GNINE_I32 index, GNINE_CALLABLE_REF callable), bool);
GNINE_API_IMPORT(args_get_arg_as_userdata, (GNINE_ARGUMENTS args, GNINE_I32 index), GNINE_USERDATA);
GNINE_API_IMPORT(args_get_arg_as_list, (GNINE_ARGUMENTS args, GNINE_I32 index), GNINE_ARGUMENTS);
GNINE_API_IMPORT(args_get_arg_count, (GNINE_ARGUMENTS args), GNINE_UI32);
GNINE_API_IMPORT(args_clone, (GNINE_ARGUMENTS args), GNINE_ARGUMENTS);

GNINE_API_IMPORT_NAME(execute_lua_code, __gnine_execute_lua_code, (GNINE_STRING code, GNINE_ARGUMENTS arguments, GNINE_ARGUMENTS* results, GNINE_XSTRING out_error, GNINE_I_PTR max_error_size), GNINE_I_PTR);

GNINE_API_IMPORT(get_c_function_numeric_position, (GNINE_PTR function), GNINE_UI32);

GNINE_API_IMPORT(create_element, (GNINE_STRING type_name, GNINE_STRING id), GNINE_ELEMENT);
GNINE_API_IMPORT(destroy_element, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(clone_element, (GNINE_ELEMENT element, struct GNINE_VECTOR3* position, bool cloneChildren), GNINE_ELEMENT);

GNINE_API_IMPORT(is_element, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(is_element_within_col_shape, (GNINE_ELEMENT element, GNINE_COL_SHAPE col_shape), bool);
GNINE_API_IMPORT(is_element_within_marker, (GNINE_ELEMENT element, GNINE_MARKER marker), bool);
GNINE_API_IMPORT(is_element_in_water, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(is_element_frozen, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(is_element_low_lod, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(set_element_call_propagation_enabled, (GNINE_ELEMENT element, bool enabled), bool);
GNINE_API_IMPORT(is_element_call_propagation_enabled, (GNINE_ELEMENT element), bool);

GNINE_API_IMPORT(get_element_by_id, (GNINE_STRING id, GNINE_UI32 index), GNINE_ELEMENT);
GNINE_API_IMPORT(get_element_by_index, (GNINE_STRING type, GNINE_UI32 index), GNINE_ELEMENT);
GNINE_API_IMPORT(get_element_children, (GNINE_ELEMENT element, GNINE_STRING type, GNINE_ELEMENT* out_children, GNINE_UI32 max_size), GNINE_UI32);
GNINE_API_IMPORT(get_element_child, (GNINE_ELEMENT parent, GNINE_UI32 index), GNINE_ELEMENT);
GNINE_API_IMPORT(get_element_children_count, (GNINE_ELEMENT element), GNINE_UI32);
GNINE_API_IMPORT(get_element_id, (GNINE_ELEMENT element, GNINE_XSTRING out_id, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_element_parent, (GNINE_ELEMENT element), GNINE_ELEMENT);
GNINE_API_IMPORT(get_element_matrix, (GNINE_ELEMENT element, struct GNINE_MATRIX* out_matrix, bool badSyntax), bool);
GNINE_API_IMPORT(get_element_position, (GNINE_ELEMENT element, struct GNINE_VECTOR3* out_position), bool);
GNINE_API_IMPORT(get_element_rotation, (GNINE_ELEMENT element, struct GNINE_VECTOR3* out_rotation, GNINE_STRING rotationOrder), bool);
GNINE_API_IMPORT(get_element_velocity, (GNINE_ELEMENT element, struct GNINE_VECTOR3* out_velocity), bool);
GNINE_API_IMPORT(get_element_turn_velocity, (GNINE_ELEMENT element, struct GNINE_VECTOR3* out_turn_velocity), bool);
GNINE_API_IMPORT(get_elements_by_type, (GNINE_STRING type, GNINE_ELEMENT start_at, GNINE_ELEMENT* out_elements, GNINE_UI32 max_size), GNINE_UI32);
GNINE_API_IMPORT(get_element_type, (GNINE_ELEMENT element, GNINE_XSTRING out_string, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_element_interior, (GNINE_ELEMENT element), GNINE_I32);
GNINE_API_IMPORT(get_elements_within_col_shape, (GNINE_COL_SHAPE col_shape, GNINE_STRING element_type, GNINE_ELEMENT* out_elements, GNINE_UI32 max_size), GNINE_UI32);
GNINE_API_IMPORT(get_elements_within_range, (struct GNINE_VECTOR3* position, GNINE_F32 radius, GNINE_ELEMENT* out_elements, GNINE_UI32 max_size, GNINE_STRING elementType, GNINE_I32 interior, GNINE_I32 dimension), GNINE_UI32);
GNINE_API_IMPORT(get_element_dimension, (GNINE_ELEMENT element), GNINE_I32);
GNINE_API_IMPORT(get_element_zone_name, (GNINE_ELEMENT element, GNINE_XSTRING out_string, GNINE_I_PTR max_size, bool cities_only), GNINE_I_PTR);
GNINE_API_IMPORT(get_element_col_shape, (GNINE_ELEMENT element), GNINE_COL_SHAPE);
GNINE_API_IMPORT(get_element_alpha, (GNINE_ELEMENT element), GNINE_UI32);
GNINE_API_IMPORT(is_element_double_sided, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(get_element_health, (GNINE_ELEMENT element), GNINE_F32);
GNINE_API_IMPORT(get_element_model, (GNINE_ELEMENT element), GNINE_UI32);
GNINE_API_IMPORT(get_element_syncer, (GNINE_ELEMENT element), GNINE_ELEMENT);
GNINE_API_IMPORT(get_element_collisions_enabled, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(get_low_lod_element, (GNINE_ELEMENT element), GNINE_ELEMENT);

GNINE_API_IMPORT(attach_elements, (GNINE_ELEMENT element, GNINE_ELEMENT attach_to, struct GNINE_VECTOR3* position_offset, struct GNINE_VECTOR3* rotation_offset), bool);
GNINE_API_IMPORT(detach_elements, (GNINE_ELEMENT element, GNINE_ELEMENT target_element), bool);
GNINE_API_IMPORT(is_element_attached, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(get_attached_elements, (GNINE_ELEMENT element, GNINE_ELEMENT* out_elements, GNINE_UI32 max_size), GNINE_UI32);
GNINE_API_IMPORT(get_element_attached_to, (GNINE_ELEMENT element), GNINE_ELEMENT);
GNINE_API_IMPORT(set_element_attached_offsets, (GNINE_ELEMENT element, struct GNINE_VECTOR3* position_offset, struct GNINE_VECTOR3* rotation_offset), bool);
GNINE_API_IMPORT(get_element_attached_offsets, (GNINE_ELEMENT element, struct GNINE_VECTOR3* out_position_offset, struct GNINE_VECTOR3* out_rotation_offset), bool);

GNINE_API_IMPORT(get_element_data, (GNINE_ELEMENT element, GNINE_STRING key, bool inherit), GNINE_ARGUMENTS);
GNINE_API_IMPORT(get_all_element_data, (GNINE_ELEMENT element, GNINE_STRING* data_list_out, GNINE_UI32 max_item_size), GNINE_UI32);
GNINE_API_IMPORT(has_element_data, (GNINE_ELEMENT element, GNINE_STRING key, bool inherit), bool);
GNINE_API_IMPORT(set_element_data, (GNINE_ELEMENT element, GNINE_STRING key, GNINE_ARGUMENTS args, bool sync_mode), bool);
GNINE_API_IMPORT(remove_element_data, (GNINE_ELEMENT element, GNINE_STRING key), bool);
GNINE_API_IMPORT(add_element_data_subscriber, (GNINE_ELEMENT element, GNINE_STRING key, GNINE_PLAYER player), bool);
GNINE_API_IMPORT(remove_element_data_subscriber, (GNINE_ELEMENT element, GNINE_STRING key, GNINE_PLAYER player), bool);
GNINE_API_IMPORT(has_element_data_subscriber, (GNINE_ELEMENT element, GNINE_STRING key, GNINE_PLAYER player), bool);

GNINE_API_IMPORT(set_element_id, (GNINE_ELEMENT element, GNINE_STRING id), bool);
GNINE_API_IMPORT(set_element_parent, (GNINE_ELEMENT element, GNINE_ELEMENT parent), bool);
GNINE_API_IMPORT(set_element_matrix, (GNINE_ELEMENT element, struct GNINE_MATRIX* matrix), bool);
GNINE_API_IMPORT(set_element_position, (GNINE_ELEMENT element, struct GNINE_VECTOR3* position, bool wrap), bool);
GNINE_API_IMPORT(set_element_rotation, (GNINE_ELEMENT element, struct GNINE_VECTOR3* rotation, GNINE_STRING rotation_order, bool fix_ped_rotaion), bool);
GNINE_API_IMPORT(set_element_velocity, (GNINE_ELEMENT element, struct GNINE_VECTOR3* velocity), bool);
GNINE_API_IMPORT(set_element_turn_velocity, (GNINE_ELEMENT element, struct GNINE_VECTOR3* velocity), bool);
GNINE_API_IMPORT(set_element_visible_to, (GNINE_ELEMENT element, GNINE_ELEMENT to_element, bool visible), bool);
GNINE_API_IMPORT(clear_element_visible_to, (GNINE_ELEMENT element), bool);
GNINE_API_IMPORT(is_element_visible_to, (GNINE_ELEMENT element, GNINE_ELEMENT to_element), bool);
GNINE_API_IMPORT(set_element_interior, (GNINE_ELEMENT element, GNINE_I32 interior, bool set_position, struct GNINE_VECTOR3* position), bool);
GNINE_API_IMPORT(set_element_dimension, (GNINE_ELEMENT element, GNINE_I32 dimension), bool);
GNINE_API_IMPORT(set_element_alpha, (GNINE_ELEMENT element, GNINE_I32 alpha), bool);
GNINE_API_IMPORT(set_element_double_sided, (GNINE_ELEMENT element, bool enabled), bool);
GNINE_API_IMPORT(set_element_health, (GNINE_ELEMENT element, GNINE_F32 health), bool);
GNINE_API_IMPORT(set_element_model, (GNINE_ELEMENT element, GNINE_I32 model), bool);
GNINE_API_IMPORT(set_element_syncer, (GNINE_ELEMENT element, GNINE_PLAYER player, bool enabled, bool persist), bool);
GNINE_API_IMPORT(set_element_collisions_enabled, (GNINE_ELEMENT element, bool enabled), bool);
GNINE_API_IMPORT(set_element_frozen, (GNINE_ELEMENT element, bool freeze_status), bool);
GNINE_API_IMPORT(set_low_lod_element, (GNINE_ELEMENT element, GNINE_ELEMENT target), bool);

GNINE_API_IMPORT(get_player_count, (), GNINE_UI32);
GNINE_API_IMPORT(set_player_ammo, (GNINE_PLAYER player, GNINE_UI32 ammo, GNINE_UI32 slot, GNINE_UI32 ammo_in_clip), bool);
GNINE_API_IMPORT(get_player_from_name, (GNINE_STRING name), GNINE_PLAYER);
GNINE_API_IMPORT(get_player_money, (GNINE_PLAYER player), GNINE_I32);
GNINE_API_IMPORT(get_player_ping, (GNINE_PLAYER player), GNINE_UI32);
GNINE_API_IMPORT(get_random_player, (), GNINE_PLAYER);
GNINE_API_IMPORT(is_player_muted, (GNINE_PLAYER player), bool);
GNINE_API_IMPORT(get_player_team, (GNINE_PLAYER player), GNINE_TEAM);
GNINE_API_IMPORT(can_player_use_function, (GNINE_PLAYER player, GNINE_STRING function_name), bool);
GNINE_API_IMPORT(get_player_wanted_level, (GNINE_PLAYER player), GNINE_UI32);
GNINE_API_IMPORT(get_alive_players, (GNINE_PLAYER* player_list, GNINE_UI32 max_item_count), GNINE_UI32);
GNINE_API_IMPORT(get_dead_players, (GNINE_PLAYER* player_list, GNINE_UI32 max_item_count), GNINE_UI32);
GNINE_API_IMPORT(get_player_idle_time, (GNINE_PLAYER player), GNINE_I64);
GNINE_API_IMPORT(is_player_scoreboard_forced, (GNINE_PLAYER player), bool);
GNINE_API_IMPORT(is_player_map_forced, (GNINE_PLAYER player), bool);
GNINE_API_IMPORT(get_player_nametag_text, (GNINE_PLAYER player, GNINE_XSTRING out_text, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_player_nametag_color, (GNINE_PLAYER player, struct GNINE_COLOR* color), bool);
GNINE_API_IMPORT(is_player_nametag_showing, (GNINE_PLAYER player), bool);
GNINE_API_IMPORT(get_player_serial, (GNINE_PLAYER player, GNINE_UI32 index, GNINE_XSTRING out_serial, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_player_community_id, (GNINE_PLAYER player, GNINE_XSTRING out_id, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_player_user_name, (GNINE_PLAYER player, GNINE_XSTRING out_user_name, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_player_blur_level, (GNINE_PLAYER player), GNINE_I32);
GNINE_API_IMPORT(get_player_name, (GNINE_PLAYER player, GNINE_XSTRING out_name, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_player_ip, (GNINE_PLAYER player, GNINE_XSTRING out_ip, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_player_account, (GNINE_PLAYER player), GNINE_ACCOUNT);
GNINE_API_IMPORT(get_player_version, (GNINE_PLAYER player, GNINE_XSTRING out_version, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_player_ac_info_r, (GNINE_PLAYER player, GNINE_XSTRING out_DetectedAC, GNINE_UI32* out_d3d9Size, GNINE_XSTRING out_d3d9MD5, GNINE_XSTRING out_d3d9SHA256, GNINE_I_PTR max_strings_size), bool);
GNINE_API_IMPORT(get_player_script_debug_level, (GNINE_PLAYER player), GNINE_UI32);

GNINE_API_IMPORT(set_player_money, (GNINE_PLAYER player, GNINE_I32 money, bool instant), bool);
GNINE_API_IMPORT(give_player_money, (GNINE_PLAYER player, GNINE_I32 money), bool);
GNINE_API_IMPORT(take_player_money, (GNINE_PLAYER player, GNINE_I32 money), bool);
GNINE_API_IMPORT(spawn_player, (GNINE_PLAYER player, struct GNINE_VECTOR3* position, GNINE_F32 rotation, GNINE_I32 model, GNINE_I32 interior, GNINE_I32 dimension, GNINE_TEAM team), bool);
GNINE_API_IMPORT(show_player_hud_component, (GNINE_PLAYER player, GNINE_STRING component, bool show), bool);
GNINE_API_IMPORT(set_player_wanted_level, (GNINE_PLAYER player, GNINE_UI32 level), bool);
GNINE_API_IMPORT(force_player_map, (GNINE_PLAYER player, bool force), bool);
GNINE_API_IMPORT(set_player_nametag_text, (GNINE_PLAYER player, GNINE_STRING nametag_text), bool);
GNINE_API_IMPORT(set_player_nametag_color, (GNINE_PLAYER player, struct GNINE_COLOR* nametag_color), bool);
GNINE_API_IMPORT(set_player_nametag_showing, (GNINE_PLAYER player, bool show), bool);
GNINE_API_IMPORT(set_player_muted, (GNINE_PLAYER player , bool muted), bool);
GNINE_API_IMPORT(set_player_blur_level, (GNINE_PLAYER player, GNINE_I32 blur_level), bool);
GNINE_API_IMPORT(redirect_player, (GNINE_PLAYER player, GNINE_STRING host, GNINE_I32 port, GNINE_STRING password), bool);
GNINE_API_IMPORT(set_player_name, (GNINE_PLAYER player, GNINE_STRING name), bool);
GNINE_API_IMPORT(detonate_satchels, (GNINE_PLAYER player), bool);
GNINE_API_IMPORT(take_player_screen_shot, (GNINE_PLAYER player, GNINE_UI32 width, GNINE_UI32 height, GNINE_STRING tag, GNINE_UI32 quality, GNINE_UI32 max_bandwidth, GNINE_UI32 max_packet_size), bool);
GNINE_API_IMPORT(set_player_script_debug_level, (GNINE_PLAYER player, GNINE_UI32 level), bool);

GNINE_API_IMPORT(get_player_announce_value, (GNINE_PLAYER player, GNINE_STRING key, GNINE_XSTRING out_value, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(set_player_announce_value, (GNINE_PLAYER player, GNINE_STRING key, GNINE_STRING value), bool);
GNINE_API_IMPORT(resend_player_mod_info, (GNINE_PLAYER player), bool);
GNINE_API_IMPORT(resend_player_ac_info, (GNINE_PLAYER player), bool);

GNINE_API_IMPORT(bind_key, (GNINE_PLAYER player, GNINE_STRING key, GNINE_STRING hit_state, GNINE_CALLABLE_REF callable, GNINE_ARGUMENTS arguments), bool);
GNINE_API_IMPORT(bind_key_on_command, (GNINE_PLAYER player, GNINE_STRING key, GNINE_STRING hit_state, GNINE_STRING command, GNINE_STRING arguments), bool);
GNINE_API_IMPORT(unbind_key, (GNINE_PLAYER player, GNINE_STRING key, GNINE_STRING hit_state, GNINE_CALLABLE_REF callable), bool);
GNINE_API_IMPORT(unbind_key_on_command, (GNINE_PLAYER player, GNINE_STRING key, GNINE_STRING hit_state, GNINE_STRING command), bool);
GNINE_API_IMPORT(is_key_bound, (GNINE_PLAYER player, GNINE_STRING key, GNINE_STRING hit_state, GNINE_CALLABLE_REF callable), bool);
GNINE_API_IMPORT(get_functions_bound_to_key, (GNINE_PLAYER player, GNINE_STRING key, GNINE_STRING hit_state, GNINE_CALLABLE_REF* callables, GNINE_UI32 max_item_count), GNINE_UI32);
GNINE_API_IMPORT(get_key_bound_to_function, (GNINE_PLAYER player, GNINE_CALLABLE_REF function, GNINE_XSTRING out_key, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_control_state, (GNINE_PLAYER player, GNINE_STRING control), bool);
GNINE_API_IMPORT(is_control_enabled, (GNINE_PLAYER player, GNINE_STRING control), bool);

GNINE_API_IMPORT(set_control_state, (GNINE_PLAYER player, GNINE_STRING control, bool state), bool);
GNINE_API_IMPORT(toggle_control, (GNINE_PLAYER player, GNINE_STRING control, bool enabled), bool);
GNINE_API_IMPORT(toggle_all_controls, (GNINE_PLAYER player, bool enabled, bool gta_controls, bool gnine_controls), bool);

GNINE_API_IMPORT(play_sound_front_end, (GNINE_PLAYER player, GNINE_UI32 sound), bool);
GNINE_API_IMPORT(play_mission_audio, (GNINE_PLAYER player, struct GNINE_VECTOR3* position, GNINE_UI32 slot), bool);
GNINE_API_IMPORT(preload_mission_audio, (GNINE_PLAYER player, GNINE_UI32 sound, GNINE_UI32 slot), bool);

GNINE_API_IMPORT(is_cursor_showing, (GNINE_PLAYER player), bool);
GNINE_API_IMPORT(show_cursor, (GNINE_PLAYER player, bool show, bool toggle_controls), bool);

GNINE_API_IMPORT(show_chat, (GNINE_PLAYER player, bool show, bool input_blocked), bool);

GNINE_API_IMPORT(kick_player, (GNINE_PLAYER player, GNINE_PLAYER responsible, GNINE_STRING reason), bool);
GNINE_API_IMPORT(ban_player, (GNINE_PLAYER player, bool ip, bool username, bool serial, GNINE_ELEMENT responsible_player, GNINE_STRING reason, GNINE_I64 seconds), GNINE_BAN);

GNINE_API_IMPORT(create_team, (GNINE_STRING name, struct GNINE_COLOR* color), GNINE_TEAM);

GNINE_API_IMPORT(get_team_from_name, (GNINE_STRING name), GNINE_TEAM);
GNINE_API_IMPORT(get_team_name, (GNINE_TEAM team, GNINE_XSTRING out_name, GNINE_I_PTR max_size), GNINE_I_PTR);
GNINE_API_IMPORT(get_team_color, (GNINE_TEAM team, struct GNINE_COLOR* color), bool);
GNINE_API_IMPORT(get_team_friendly_fire, (GNINE_TEAM team), bool);
GNINE_API_IMPORT(get_players_in_team, (GNINE_TEAM team, GNINE_PLAYER* out_players, GNINE_UI32 max_item_size), GNINE_UI32);
GNINE_API_IMPORT(count_players_in_team, (GNINE_TEAM team), GNINE_UI32);

GNINE_API_IMPORT(set_player_team, (GNINE_PLAYER player, GNINE_TEAM team), bool);
GNINE_API_IMPORT(set_team_name, (GNINE_TEAM team, GNINE_STRING name), bool);
GNINE_API_IMPORT(set_team_color, (GNINE_TEAM team, struct GNINE_COLOR* color), bool);
GNINE_API_IMPORT(set_team_friendly_fire, (GNINE_TEAM team, bool friend_fire), bool);

GNINE_API_IMPORT(create_worker, (GNINE_WORKER_HANDLER function, GNINE_PTR data_to_send_to_worker), GNINE_WORKER);
GNINE_API_IMPORT(terminate_worker, (GNINE_WORKER worker), bool);
GNINE_API_IMPORT(run_worker, (GNINE_WORKER worker), bool);
GNINE_API_IMPORT(worker_join, (GNINE_WORKER worker), bool);
GNINE_API_IMPORT(get_current_worker, (), GNINE_WORKER);
GNINE_API_IMPORT(get_main_worker, (), GNINE_WORKER);
GNINE_API_IMPORT(sleep_worker, (GNINE_UI32 milli_seconds), void);
GNINE_API_IMPORT(get_worker_state, (GNINE_WORKER worker), GNINE_WORKER_STATE_TYPE);
GNINE_API_IMPORT(is_worker, (GNINE_WORKER worker), bool);

GNINE_API_IMPORT(create_mutex, (), GNINE_MUTEX);
GNINE_API_IMPORT(destroy_mutex, (GNINE_MUTEX mutex), bool);
GNINE_API_IMPORT(lock_mutex, (GNINE_MUTEX mutex), bool);
GNINE_API_IMPORT(unlock_mutex, (GNINE_MUTEX mutex), bool);
GNINE_API_IMPORT(is_mutex, (GNINE_MUTEX mutex), bool);

GNINE_API_IMPORT(set_timer, (GNINE_CALLABLE_REF function, GNINE_UI32 time_interval, GNINE_UI32 time_to_execute, GNINE_ARGUMENTS arguments), GNINE_TIMER);
GNINE_API_IMPORT(kill_timer, (GNINE_TIMER timer), bool);
GNINE_API_IMPORT(reset_timer, (GNINE_TIMER timer), bool);
GNINE_API_IMPORT(get_timers, (GNINE_F64 time, GNINE_TIMER* out_timers, GNINE_UI32 max_item_count), GNINE_UI32);
GNINE_API_IMPORT(is_timer, (GNINE_TIMER timer), bool);
GNINE_API_IMPORT(get_timer_details, (GNINE_TIMER timer, struct GNINE_TIMER_DETAILS* out_timer_details), bool);

GNINE_API_IMPORT(get_camera_matrix, (GNINE_PLAYER player, struct GNINE_CAMERA_MATRIX* out_camera_matrix), bool);
GNINE_API_IMPORT(get_camera_target, (GNINE_PLAYER player), GNINE_ELEMENT);
GNINE_API_IMPORT(get_camera_interior, (GNINE_PLAYER player), GNINE_I32);

GNINE_API_IMPORT(set_camera_matrix, (GNINE_PLAYER player, struct GNINE_CAMERA_MATRIX* camera_matrix), bool);
GNINE_API_IMPORT(set_camera_target, (GNINE_PLAYER player, GNINE_ELEMENT target), bool);
GNINE_API_IMPORT(set_camera_interior, (GNINE_PLAYER player, GNINE_I32 interior), bool);
GNINE_API_IMPORT(fade_camera, (GNINE_PLAYER player, bool fade_in, float time_to_fade, GNINE_COLOR* screen_color), bool);

GNINE_API_IMPORT(call_remote, (GNINE_STRING url, GNINE_STRING queue_name, GNINE_UI32 connection_attempts, GNINE_UI32 connection_timeout, GNINE_CALLABLE_REF callback_function, GNINE_ARGUMENTS arguments), GNINE_REMOTE_REQUEST);
GNINE_API_IMPORT(fetch_remote, (GNINE_STRING url, struct GNINE_REMOTE_REQUEST_OPTIONS* options, GNINE_CALLABLE_REF callback_function, GNINE_ARGUMENTS arguments), GNINE_REMOTE_REQUEST);
GNINE_API_IMPORT(get_remote_requests, (GNINE_RESOURCE resource, GNINE_REMOTE_REQUEST* out_remote_request, GNINE_UI32 max_item_count), GNINE_UI32);
GNINE_API_IMPORT(get_remote_request_info, (GNINE_REMOTE_REQUEST request, struct GNINE_REMOTE_REQUEST_INFO* out_info, GNINE_I32 post_data_length, bool include_headers), bool);
GNINE_API_IMPORT(cleanup_remote_request_info, (struct GNINE_REMOTE_REQUEST_INFO* info), bool);
GNINE_API_IMPORT(abort_remote_request, (GNINE_REMOTE_REQUEST request), bool);

GNINE_API_IMPORT(is_voice_enabled, (), bool);
GNINE_API_IMPORT(set_player_voice_broadcast_to, (GNINE_PLAYER player, GNINE_PLAYER* target_players, GNINE_UI32 target_players_count), bool);
GNINE_API_IMPORT(set_player_voice_ignore_from, (GNINE_PLAYER player, GNINE_PLAYER* target_players, GNINE_UI32 target_players_count), bool);

GNINE_API_IMPORT(create_water, (GNINE_VECTOR3* v1, GNINE_VECTOR3* v2, GNINE_VECTOR3* v3, GNINE_VECTOR3* v4, bool shallow), GNINE_WATER);
GNINE_API_IMPORT(set_water_level, (GNINE_WATER water, GNINE_F32 level, bool include_world_non_sea_level, bool include_all_water_elements, bool include_world_sea_level, bool include_outside_world_level), bool);
GNINE_API_IMPORT(reset_water_level, (), bool);
GNINE_API_IMPORT(get_water_vertex_position, (GNINE_WATER water, GNINE_I32 vertex_index, GNINE_VECTOR3* out_position), bool);
GNINE_API_IMPORT(set_water_vertex_position, (GNINE_WATER water, GNINE_I32 vertex_index, GNINE_VECTOR3* position), bool);
GNINE_API_IMPORT(get_water_color, (GNINE_COLOR* out_color), bool);
GNINE_API_IMPORT(set_water_color, (GNINE_COLOR* color), bool);
GNINE_API_IMPORT(reset_water_color, (), bool);

/*
    Gnine still can't use shared memory for web assembly modules.
    This means we can't use threads normaly like we do in real C & CPP applications.

    You can use `GNINE_WORKER`s with `gnine_create_worker` that allows you to do multi threading programming with other methods.

*/
//GNINE_API_IMPORT(create_thread, (GNINE_PTR function), GNINE_THREAD);
//GNINE_API_IMPORT(terminate_thread, (GNINE_THREAD thread), bool);

/*
    API FUNCTION: gnine_print
    PARAMS:
        1- format: string format which guides arguments
        2- ...: arguments to pass in format
    USAGE EXAMPLE:
        gnine_print("this is a number : %d", 3193);
*/
#ifdef GNINE_CPP
GNINE_API GNINE_INLINE void gnine_print(GNINE_STRING format, ...) {
#else
GNINE_API void gnine_print(GNINE_STRING format, ...) {
#endif
    GNINE_I8 buffer[0xfff];

    va_list args;
    va_start(args, format);

    vsprintf(buffer, format, args);

    va_end(args);

    gnine_print_data(buffer);
}

#ifdef GNINE_CPP
GNINE_API GNINE_INLINE GNINE_ARGUMENTS gnine_execute_lua_code(GNINE_STRING code, GNINE_ARGUMENTS arguments, GNINE_XSTRING out_error, GNINE_I_PTR max_error_size, bool void_state) {
#else
GNINE_API GNINE_ARGUMENTS gnine_execute_lua_code(GNINE_STRING code, GNINE_ARGUMENTS arguments, GNINE_XSTRING out_error, GNINE_I_PTR max_error_size, bool void_state) {
#endif
    GNINE_ARGUMENTS results = NULL;

    if (void_state) {
        __gnine_execute_lua_code(code, arguments, NULL, out_error, max_error_size);

        return NULL;
    }

    __gnine_execute_lua_code(code, arguments, &results, out_error, max_error_size);

    return results;
}

#ifndef GNINE_CPP
#define GNINE_LUA(code, args) gnine_execute_lua_code(#code, args, NULL, 0, false)
#define GNINE_LUA_VOID(code, args) gnine_execute_lua_code(#code, args, NULL, 0, true)
#endif

#ifdef GNINE_CPP

#include <exception>
#include <optional>
#include <map>
#include <unordered_map>

/*
    MACRO: GNINE_LAMBDA
    PARAMS:
        1- result: function result
        2- args: function arguments which must be inside paranthesis! [for example: (int, float)]
    USAGE EXAMPLE:
        GNINE_LAMBDA(float, (int x, int y)) { return (float)(x + y); }
*/
#define GNINE_LAMBDA(result, args) (result (*)args)[]args -> result

namespace GNINE_NAMESPACE {
    using Int8 = GNINE_I8;
    using UInt8 = GNINE_UI8;
    using Byte = GNINE_BYTE;
    using Int32 = GNINE_I32;
    using UInt32 = GNINE_UI32;
    using Int64 = GNINE_I64;
    using UInt64 = GNINE_UI64;
    using Float32 = GNINE_F32;
    using Float64 = GNINE_F64;

    using IntPtr = intptr_t;
    using UIntPtr = GNINE_I_PTR;

    using MemoryPointer = GNINE_PTR;
    using SharedMemoryPointer = GNINE_SHARED_POINTER_ADDRESS;

    using CString = GNINE_STRING;
    using CXString = GNINE_XSTRING;
    using String = STD_NAMESPACE::string;
    using StringList = STD_NAMESPACE::vector<String>;
    
    template<typename ValueType = String>
    using StringMap = STD_NAMESPACE::map<String, ValueType>;
    template<typename ValueType = String>
    using StringUnorderedMap = STD_NAMESPACE::unordered_map<String, ValueType>;

    using LuaNumber = GNINE_LUA_NUMBER;

    using UserdataT = GNINE_USERDATA_T;
    using Userdata = GNINE_USERDATA;

    using ElementId = GNINE_ELEMENT;
    using ResourceId = GNINE_RESOURCE;
    using AccountId = GNINE_ACCOUNT;
    using BanId = GNINE_BAN;
    using ColShapeId = GNINE_COL_SHAPE;
    using MarkerId = GNINE_MARKER;
    using PlayerId = GNINE_PLAYER;
    using TeamId = GNINE_TEAM;
    using WaterId = GNINE_WATER;

    using RemoteRequestId = GNINE_REMOTE_REQUEST;

    using WorkerId = GNINE_WORKER;
    using WorkerMutexId = GNINE_MUTEX;

    using TimerId = GNINE_TIMER;

    using RealTimeData = GNINE_REAL_TIME;

    using Color = GNINE_COLOR;

    using ProcessMemoryStats = GNINE_PROCESS_MEMORY_STATS;

    using CameraMatrix = GNINE_CAMERA_MATRIX;

    static constexpr Float32 FLOAT_EPSILON = 0.0001f;

    template<typename TypeX, typename TypeY>
    inline constexpr bool IsSameType() {
        return String(typeid(TypeX).name()) == String(typeid(TypeY).name());
    }

    template<typename With, typename Value>
    inline constexpr bool InstanceOf(Value value) {
        return IsSameType<Value, With>();
    }

    class Exception : public STD_NAMESPACE::exception {
        public:
            Exception(String message) : m_strMessage(message) {}

            const char* what() const throw() {
                return m_strMessage.c_str();
            }

        private:
            String m_strMessage;
    };
    
    class Callable {
        public:
            class Arguments {
                public:
                    Arguments() {
                        Drop();
                        m_pArgs = gnine_args_create();
                    }

                    Arguments(GNINE_ARGUMENTS args) {
                        Drop();

                        if (!args) {
                            return;
                        }

                        m_pArgs = gnine_args_clone(args);
                    }

                    Arguments(Arguments& args) {
                        Drop();
                        
                        if (!args) {
                            return;
                        }

                        m_pArgs = gnine_args_clone(args);
                    }

                    ~Arguments() {
                        if (m_pArgs) {
                            gnine_args_delete(m_pArgs);

                            m_pArgs = NULL;
                        }
                    }

                    void Drop() {
                        m_pArgs = NULL;
                    }

                    void Push() {
                        gnine_args_push_null(m_pArgs);
                    }

                    void PushNull() {
                        Push();
                    }

                    void Push(bool arg) {
                        gnine_args_push_boolean(m_pArgs, arg);
                    }

                    void Push(LuaNumber arg) {
                        gnine_args_push_number(m_pArgs, arg);
                    }

                    void Push(Int32 arg) {
                        return Push((LuaNumber)arg);
                    }
                    
                    void Push(UInt32 arg) {
                        return Push((LuaNumber)arg);
                    }
                    
                    void Push(Int64 arg) {
                        return Push((LuaNumber)arg);
                    }
                    
                    void Push(UInt64 arg) {
                        return Push((LuaNumber)arg);
                    }

                    void Push(Float32 arg) {
                        return Push((LuaNumber)arg);
                    }

                    void Push(const char* arg) {
                        gnine_args_push_string(m_pArgs, arg);
                    }

                    void Push(String arg) {
                        gnine_args_push_string(m_pArgs, arg.c_str());
                    }

                    void Push(GNINE_CALLABLE_REF arg) {
                        gnine_args_push_callable(m_pArgs, arg);
                    }
                    
                    void PushCallable(GNINE_CALLABLE_REF arg) {
                        gnine_args_push_callable(m_pArgs, arg);
                    }

                    void Push(Userdata arg) {
                        gnine_args_push_userdata(m_pArgs, arg);
                    }

                    void Push(Arguments arg) {
                        gnine_args_push_list(m_pArgs, (GNINE_ARGUMENTS)arg);
                    }

                    template<typename Type>
                    void Push(Type* arg) {
                        gnine_args_push_number(m_pArgs, (LuaNumber)(IntPtr)arg);
                    }

                    UInt32 Count() const {
                        return gnine_args_get_arg_count(m_pArgs);
                    }

                    Byte GetType(Int32 index) const {
                        return gnine_args_get_arg_type(m_pArgs, index);
                    }

                    bool IsNull(Int32 index) const {
                        return GetType(index) == GNINE_ARGUMENT_TYPE_NULL;
                    }

                    bool GetAsBoolean(Int32 index) const {
                        return gnine_args_get_arg_as_boolean(m_pArgs, index);
                    }

                    LuaNumber GetAsNumber(Int32 index) const {
                        return gnine_args_get_arg_as_number(m_pArgs, index);
                    }

                    String GetAsString(Int32 index, UIntPtr length = 1024) const {
                        char* str = (char*)malloc(length);

                        UIntPtr size = gnine_args_get_arg_as_string(m_pArgs, index, str, length);

                        String data(str, size);

                        free((void*)str);

                        return data;
                    }

                    Callable GetAsCallable(Int32 index) const {
                        Callable callable;
                        
                        gnine_args_get_arg_as_callable(m_pArgs, index, callable.m_rFunctionRef);
                        
                        return callable;
                    }

                    Userdata GetAsUserdata(Int32 index) const {
                        return gnine_args_get_arg_as_userdata(m_pArgs, index);
                    }

                    Arguments GetAsList(Int32 index) const {
                        Arguments list(NULL);

                        GNINE_ARGUMENTS l = gnine_args_get_arg_as_list(m_pArgs, index);

                        list = Arguments(l);

                        return list;
                    }

                    bool operator==(Arguments target) const {
                        return m_pArgs == (GNINE_ARGUMENTS)target;
                    }

                    Arguments& operator=(GNINE_ARGUMENTS target) {
                        if (m_pArgs) {
                            gnine_args_delete(m_pArgs);
                        }

                        if (!target) {
                            Drop();
                            return *this;
                        }

                        m_pArgs = gnine_args_clone(target);

                        return *this;
                    }

                    Arguments& operator=(Arguments& target) {
                        if (m_pArgs) {
                            gnine_args_delete(m_pArgs);
                        }

                        if (!target) {
                            Drop();
                            return *this;
                        }

                        m_pArgs = gnine_args_clone((GNINE_ARGUMENTS)target);

                        return *this;
                    }

                    operator GNINE_ARGUMENTS() const {
                        return m_pArgs;
                    }

                    operator bool() const {
                        return m_pArgs ? true : false;
                    }
                private:
                    GNINE_ARGUMENTS m_pArgs;
            };

            using Argument = STD_NAMESPACE::variant<
                nullptr_t,
                bool,
                LuaNumber,
                CString,
                String,
                Callable,
                Userdata,
                Arguments*,
                Int32,
                Int64,
                UInt32,
                UInt64,
                Float32,
                MemoryPointer
            >;

            GNINE_CALLABLE_REF m_rFunctionRef;

            Callable() {
                m_bAutoCleanup = true;
                m_FunctionArguments = NULL;
                memset((MemoryPointer)m_rFunctionRef, 0, sizeof(GNINE_CALLABLE_REF));
            }
            
            Callable(GNINE_CALLABLE_REF ref) {
                m_bAutoCleanup = true;
                m_FunctionArguments = NULL;
                memcpy((MemoryPointer)m_rFunctionRef, (MemoryPointer)ref, sizeof(GNINE_CALLABLE_REF));
            }

            template<typename Function>
            Callable(Function function) {
                m_bAutoCleanup = true;
                m_FunctionArguments = NULL;
                gnine_c_function_to_callable((MemoryPointer)function, m_rFunctionRef);
            }

            Callable(const Callable& callable) {
                TakeOwnership(*(Callable*)(&callable));
            }

            ~Callable() {
                if (!m_bAutoCleanup) {
                    return;
                }

                Free();
            }

            void TakeOwnership(Callable& callable) {
                if (*this) {
                    if (m_bAutoCleanup) {
                        Free();
                    }
                }

                memcpy((MemoryPointer)m_rFunctionRef, (MemoryPointer)callable.m_rFunctionRef, sizeof(GNINE_CALLABLE_REF));

                m_bAutoCleanup = callable.m_bAutoCleanup;

                callable.DisableAutoCleanup();
            }

            Callable& operator=(GNINE_CALLABLE_REF ref) {
                if (*this) {
                    if (m_bAutoCleanup) {
                        Free();
                    }
                }

                memcpy((MemoryPointer)m_rFunctionRef, (MemoryPointer)ref, sizeof(GNINE_CALLABLE_REF));

                return *this;
            }

            Callable& operator=(const Callable& callback) {
                TakeOwnership(*(Callable*)(&callback));

                return *this;
            }

            template<typename Function>
            Callable& operator=(Function function) {
                if (*this) {
                    if (m_bAutoCleanup) {
                        Free();
                    }
                }

                gnine_c_function_to_callable(function, m_rFunctionRef);

                return *this;
            }

            bool operator==(const Callable& target) const {
                return strncmp((GNINE_STRING)m_rFunctionRef, (GNINE_STRING)target.m_rFunctionRef, sizeof(m_rFunctionRef)) == 0;
            }

            String GetDeclaration(String functionName = "callable") const {
                char dec[0x3e8];
                IntPtr size = gnine_get_callable_declaration((GNINE_UI8*)m_rFunctionRef, functionName.c_str(), dec, sizeof(dec));

                return String(dec, size);
            }
            
            Userdata GetReference() {
                return gnine_get_callable_reference(m_rFunctionRef);
            }

            ResourceId GetResource() {
                return gnine_get_callable_resource(m_rFunctionRef);
            }

            bool IsWasmFunction() {
                return gnine_is_callable_wasm_function(m_rFunctionRef);
            }
            
            void Free() {
                gnine_free_callable(m_rFunctionRef);
            }

            void DisableAutoCleanup(bool state = true) {
                m_bAutoCleanup = !state;
            }

            bool IsAutoCleanupEnabled() {
                return m_bAutoCleanup;
            }

            bool Invoke(Arguments* args = NULL, Arguments* results = NULL, String* errorMessage = NULL) {
                if (!*this) {
                    return false;
                }

                GNINE_ARGUMENTS funcArgs = NULL;

                if (args && *args) {
                    funcArgs = (GNINE_ARGUMENTS)*args;
                }

                char errorM[1024];
                memset((MemoryPointer)errorM, 0, sizeof(errorM));
                GNINE_ARGUMENTS funcResults = gnine_call_callable(m_rFunctionRef, funcArgs, errorM, sizeof(errorM));

                size_t length = strlen(errorM);

                if (length > 0) {
                    *errorMessage = String(errorM, length);
                    return false;
                }

                if (results) {
                    *results = funcResults;
                }

                return true;
            }

            Arguments operator()() {
                Arguments results;

                String errorMessage = "";

                bool success = Invoke(NULL, &results, &errorMessage);

                if (!success) {
                    throw Exception(errorMessage);
                }
                
                return results;
            }

            template<typename Arg>
            Arguments operator()(Arg arg) {
                AddFunctionArgument((Argument)arg);

                Arguments results;

                String errorMessage = "";

                bool success = Invoke(m_FunctionArguments, &results, &errorMessage);

                delete m_FunctionArguments;
                m_FunctionArguments = NULL;

                if (!success) {
                    throw Exception(errorMessage);
                }
                
                return results;
            }

            template<typename Arg, typename... Args>
            Arguments operator()(Arg arg, Args... args) {
                AddFunctionArgument((Argument)arg);
                
                return operator()(args...);
            }

            operator bool() const {
                return gnine_is_valid_callable((GNINE_UI8*)m_rFunctionRef);
            }

            operator String() const {
                return GetDeclaration();
            }
            
            operator GNINE_UI8*() const {
                return (GNINE_UI8*)m_rFunctionRef;
            }

            static void AddCallableArgumentToArguments(Arguments* args, Argument arg) {
                if (!args || !*args) {
                    return;
                }

                if (STD_NAMESPACE::holds_alternative<nullptr_t>(arg)) {
                    args->Push();
                }else if (STD_NAMESPACE::holds_alternative<bool>(arg)) {
                    args->Push(STD_NAMESPACE::get<bool>(arg));
                }else if (STD_NAMESPACE::holds_alternative<LuaNumber>(arg)) {
                    args->Push(STD_NAMESPACE::get<LuaNumber>(arg));
                }else if (STD_NAMESPACE::holds_alternative<CString>(arg)) {
                    args->Push(String(STD_NAMESPACE::get<CString>(arg)));
                }else if (STD_NAMESPACE::holds_alternative<String>(arg)) {
                    args->Push(STD_NAMESPACE::get<String>(arg));
                }else if (STD_NAMESPACE::holds_alternative<Callable>(arg)) {
                    args->Push(STD_NAMESPACE::get<Callable>(arg).m_rFunctionRef);
                }else if (STD_NAMESPACE::holds_alternative<Userdata>(arg)) {
                    args->Push(STD_NAMESPACE::get<Userdata>(arg));
                }else if (STD_NAMESPACE::holds_alternative<Arguments*>(arg)) {
                    args->Push(*STD_NAMESPACE::get<Arguments*>(arg));
                }else if (STD_NAMESPACE::holds_alternative<Int32>(arg)) {
                    args->Push(STD_NAMESPACE::get<Int32>(arg));
                }else if (STD_NAMESPACE::holds_alternative<Int64>(arg)) {
                    args->Push(STD_NAMESPACE::get<Int64>(arg));
                }else if (STD_NAMESPACE::holds_alternative<UInt32>(arg)) {
                    args->Push(STD_NAMESPACE::get<UInt32>(arg));
                }else if (STD_NAMESPACE::holds_alternative<UInt64>(arg)) {
                    args->Push(STD_NAMESPACE::get<UInt64>(arg));
                }else if (STD_NAMESPACE::holds_alternative<Float32>(arg)) {
                    args->Push(STD_NAMESPACE::get<Float32>(arg));
                }else if (STD_NAMESPACE::holds_alternative<MemoryPointer>(arg)) {
                    args->Push((LuaNumber)(IntPtr)STD_NAMESPACE::get<MemoryPointer>(arg));
                }
            }

            private:
                Arguments* m_FunctionArguments;

                bool m_bAutoCleanup;

                void AddFunctionArgument(Argument arg) {
                    if (!m_FunctionArguments) {
                        m_FunctionArguments = new Arguments();
                    }

                    AddCallableArgumentToArguments(m_FunctionArguments, arg);
                }
    };

    class Vector4 {
        public:
            float fX = 0.0f;
            float fY = 0.0f;
            float fZ = 0.0f;
            float fW = 0.0f;

            constexpr Vector4() noexcept = default;

            constexpr Vector4(const Vector4&) noexcept = default;

            constexpr Vector4& operator=(const Vector4&) noexcept = default;

            constexpr Vector4(float x, float y, float z, float w) noexcept : fX(x), fY(y), fZ(z), fW(w) {}

            // Warning, this function is returning the wrong value(fW is missing), its kept because nothing uses it, only
            // CLuaVector4DDefs.
            constexpr float DotProduct(const Vector4& other) const noexcept { return fX * other.fX + fY * other.fY + fZ * other.fZ; }

            float Length() const noexcept { return sqrt(fX * fX + fY * fY + fZ * fZ + fW * fW); }

            // LengthSquared returns Length() without sqrt applied (i.e. returns x*x* + y*y + z*z + w*w).
            // This can be useful if you only want to compare lengths.
            constexpr float LengthSquared() const noexcept { return (fX * fX) + (fY * fY) + (fZ * fZ) + (fW * fW); }

            void Normalize() noexcept
            {
                const float fLength = Length();
                if (fLength > 0.0f)
                {
                    fX /= fLength;
                    fY /= fLength;
                    fZ /= fLength;
                    fW /= fLength;
                }
            }

            constexpr Vector4 operator*(const Vector4& vecRight) const { return Vector4(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ, fW * vecRight.fW); }

            constexpr Vector4 operator*(const float fRight) const noexcept { return Vector4(fX * fRight, fY * fRight, fZ * fRight, fW * fRight); }

            constexpr Vector4 operator+(const Vector4& vecRight) const noexcept
            {
                return Vector4(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ, fW + vecRight.fW);
            }

            constexpr Vector4 operator-(const Vector4& vecRight) const noexcept
            {
                return Vector4(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ, fW - vecRight.fW);
            }

            constexpr Vector4 operator/(const Vector4& vecRight) const noexcept
            {
                return Vector4(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ, fW / vecRight.fW);
            }

            constexpr Vector4 operator/(const float fRight) const noexcept { return Vector4(fX / fRight, fY / fRight, fZ / fRight, fW / fRight); }

            constexpr void operator+=(const float fRight) noexcept
            {
                fX += fRight;
                fY += fRight;
                fZ += fRight;
                fW += fRight;
            }

            constexpr void operator+=(const Vector4& vecRight) noexcept
            {
                fX += vecRight.fX;
                fY += vecRight.fY;
                fZ += vecRight.fZ;
                fW += vecRight.fW;
            }

            constexpr void operator-=(const float fRight) noexcept
            {
                fX -= fRight;
                fY -= fRight;
                fZ -= fRight;
                fW -= fRight;
            }

            constexpr void operator-=(const Vector4& vecRight) noexcept
            {
                fX -= vecRight.fX;
                fY -= vecRight.fY;
                fZ -= vecRight.fZ;
                fW -= vecRight.fW;
            }

            constexpr void operator*=(const float fRight) noexcept
            {
                fX *= fRight;
                fY *= fRight;
                fZ *= fRight;
                fW *= fRight;
            }

            constexpr void operator/=(const float fRight) noexcept
            {
                fX /= fRight;
                fY /= fRight;
                fZ /= fRight;
                fW /= fRight;
            }

            constexpr void operator/=(const Vector4& vecRight) noexcept
            {
                fX /= vecRight.fX;
                fY /= vecRight.fY;
                fZ /= vecRight.fZ;
                fW /= vecRight.fW;
            }

            bool operator==(const Vector4& param) const noexcept
            {
                return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON) &&
                        (fabs(fW - param.fW) < FLOAT_EPSILON));
            }

            bool operator!=(const Vector4& param) const noexcept { return !(*this == param); }

            operator String() {
                char data[100];
                memset((MemoryPointer)data, 0, sizeof(data));

                Int32 size = sprintf(data, "vector4(x: %g, y: %g, z: %g, w: %g)", fX, fY, fZ, fW);

                return String(data, size);
            }
    };
    
    class Vector3 {
        private:
            static constexpr float FLOAT_EPSILON = 0.0001f;

        public:
            float fX = 0.0f;
            float fY = 0.0f;
            float fZ = 0.0f;

            constexpr Vector3() = default;

            constexpr Vector3(float x, float y, float z) : fX(x), fY(y), fZ(z) {}

            constexpr Vector3(const Vector4& vec) noexcept : fX(vec.fX), fY(vec.fY), fZ(vec.fZ) {}
            
            constexpr Vector3(const GNINE_VECTOR3& vec) : fX(vec.x), fY(vec.y), fZ(vec.z) {}

            constexpr Vector3& operator=(const Vector4& vec) noexcept
            {
                fX = vec.fX;
                fY = vec.fY;
                fZ = vec.fZ;
                return *this;
            }

            constexpr Vector3 Clone() const { return *this; }

            // Normalize returns the normalized length of the vector.
            float Normalize()
            {
                const float t = Length();
                if (t > FLOAT_EPSILON)
                {
                    fX /= t;
                    fY /= t;
                    fZ /= t;

                    return t;
                }
                else
                    return 0;
            }

            float Length() const { return sqrt((fX * fX) + (fY * fY) + (fZ * fZ)); }

            // LengthSquared returns Length() without sqrt applied (i.e. returns x*x* + y*y + z*z).
            // This can be useful if you only want to compare lengths.
            float LengthSquared() const { return (fX * fX) + (fY * fY) + (fZ * fZ); }

            float DotProduct(const Vector3* param) const { return fX * param->fX + fY * param->fY + fZ * param->fZ; }

            void CrossProduct(const Vector3* param)
            {
                const float _fX = fX, _fY = fY, _fZ = fZ;
                fX = _fY * param->fZ - param->fY * _fZ;
                fY = _fZ * param->fX - param->fZ * _fX;
                fZ = _fX * param->fY - param->fX * _fY;
            }

            // Convert (direction) to rotation
            Vector3 ToRotation() const
            {
                Vector3 vecRotation;
                vecRotation.fZ = atan2(fY, fX);
                Vector3 vecTemp(STD_NAMESPACE::hypotf(fX, fY), fZ, 0);
                vecTemp.Normalize();
                constexpr float pi = 3.14159265358979323846f;
                vecRotation.fY = atan2(vecTemp.fX, vecTemp.fY) - pi / 2;
                return vecRotation;
            }

            // Return a perpendicular direction
            Vector3 GetOtherAxis() const
            {
                Vector3 vecResult;
                if (STD_NAMESPACE::abs(fX) > STD_NAMESPACE::abs(fY))
                    vecResult = Vector3(fZ, 0, -fX);
                else
                    vecResult = Vector3(0, -fZ, fY);

                vecResult.Normalize();
                return vecResult;
            }

            // Intersections code based on https://github.com/juj/MathGeoLib/blob/master/src/Geometry/Plane.h
            bool IntesectsLinePlane(const Vector3& vecRay, const Vector3& vecNormal, const Vector3& vecPosition, float* fOutDist) const noexcept
            {
                const float fDenom = vecNormal.DotProduct(&vecRay);

                if (fabs(fDenom) > 1e-4f)
                {
                    *fOutDist = (vecNormal.DotProduct(&vecPosition) - vecNormal.DotProduct(this)) / fDenom;
                    return true;
                }

                if (fDenom != 0.0f)
                {
                    *fOutDist = (vecNormal.DotProduct(&vecPosition) - vecNormal.DotProduct(this)) / fDenom;
                    return fabs(*fOutDist) < 1e-4f;
                }

                *fOutDist = 0.0f;
                return fabs(vecNormal.DotProduct(this) - vecNormal.DotProduct(&vecPosition)) < 1e-3f;
                ;
            }

            bool IntersectsSegmentPlane(const Vector3& vecSegment, const Vector3& vecNormal, const Vector3& vecPosition, Vector3* outVec) const noexcept
            {
                float   fDist;
                Vector3 vecRay = vecSegment;
                vecRay.Normalize();
                bool bIntersects = IntesectsLinePlane(vecRay, vecNormal, vecPosition, &fDist);

                const float fSegLength = vecSegment.Length();

                *outVec = *this + vecRay * fDist;
                return bIntersects && fDist >= 0 && (fDist <= fSegLength);
            }

            // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
            bool IntersectsSegmentTriangle(const Vector3& dir, const Vector3& vecVert1, const Vector3& vecVert2, const Vector3& vecVert3,
                                        Vector3* outVec, Vector3* outHitBary = nullptr) const noexcept
            {
                constexpr float fEpsilon = 1e-6f;

                Vector3 vecEdge1, vecEdge2, h, s;
                float   a, f, u, v;

                Vector3 vecRay = dir;
                vecRay.Normalize();
                h = vecRay;

                vecEdge1 = vecVert2 - vecVert1;
                vecEdge2 = vecVert3 - vecVert1;

                h.CrossProduct(&vecEdge2);
                a = vecEdge1.DotProduct(&h);

                if (a > -fEpsilon && a < fEpsilon)
                {
                    return false;
                }

                f = 1.0f / a;
                s = *this - vecVert1;
                u = f * s.DotProduct(&h);
                if (u < 0.0f || u > 1.0f)
                {
                    return false;
                }

                Vector3 sCrossE1 = s;
                sCrossE1.CrossProduct(&vecEdge1);
                v = f * vecRay.DotProduct(&sCrossE1);
                if (v < 0.0f || u + v > 1.0f)
                {
                    return false;
                }

                float t = f * vecEdge2.DotProduct(&sCrossE1);
                if (t > fEpsilon && t <= dir.Length())
                {
                    *outVec = *this + vecRay * t;
                    if (outHitBary) { // Calculate all barycentric coords if necessary
                        *outHitBary = { 1.f - u - v, u, v }; // For vertices A, B, C [I assume?]
                    }
                    return true;
                }

                return false;
            }

            constexpr Vector3 operator+(const Vector3& vecRight) const noexcept { return Vector3(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ); }

            constexpr Vector3 operator-(const Vector3& vecRight) const noexcept { return Vector3(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ); }

            constexpr Vector3 operator-() const noexcept { return Vector3(-fX, -fY, -fZ); }

            constexpr Vector3 operator*(const Vector3& vecRight) const noexcept { return Vector3(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ); }

            constexpr Vector3 operator*(const float fRight) const noexcept { return Vector3(fX * fRight, fY * fRight, fZ * fRight); }

            constexpr Vector3 operator/(const Vector3& vecRight) const noexcept { return Vector3(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ); }

            constexpr Vector3 operator/(const float fRight) const noexcept { return Vector3(fX / fRight, fY / fRight, fZ / fRight); }

            constexpr void operator+=(const float fRight) noexcept
            {
                fX += fRight;
                fY += fRight;
                fZ += fRight;
            }

            constexpr void operator+=(const Vector3& vecRight) noexcept
            {
                fX += vecRight.fX;
                fY += vecRight.fY;
                fZ += vecRight.fZ;
            }

            constexpr void operator-=(const float fRight) noexcept
            {
                fX -= fRight;
                fY -= fRight;
                fZ -= fRight;
            }

            constexpr void operator-=(const Vector3& vecRight) noexcept
            {
                fX -= vecRight.fX;
                fY -= vecRight.fY;
                fZ -= vecRight.fZ;
            }

            constexpr void operator*=(const float fRight) noexcept
            {
                fX *= fRight;
                fY *= fRight;
                fZ *= fRight;
            }

            constexpr void operator*=(const Vector3& vecRight) noexcept
            {
                fX *= vecRight.fX;
                fY *= vecRight.fY;
                fZ *= vecRight.fZ;
            }

            constexpr void operator/=(const float fRight) noexcept
            {
                fX /= fRight;
                fY /= fRight;
                fZ /= fRight;
            }

            constexpr void operator/=(const Vector3& vecRight) noexcept
            {
                fX /= vecRight.fX;
                fY /= vecRight.fY;
                fZ /= vecRight.fZ;
            }

            bool operator==(const Vector3& param) const noexcept
            {
                return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON));
            }

            bool operator!=(const Vector3& param) const noexcept { return !(*this == param); }

            float operator[](size_t i) const noexcept { return ((float*)this)[i]; }

            operator GNINE_VECTOR3() {
                GNINE_VECTOR3 vec;
                vec.x = fX;
                vec.y = fY;
                vec.z = fZ;

                return vec;
            }

            operator Vector4() {
                return Vector4(fX, fY, fZ, 0.0f);
            }

            operator String() {
                char data[100];
                memset((MemoryPointer)data, 0, sizeof(data));

                Int32 size = sprintf(data, "vector3(x: %g, y: %g, z: %g)", fX, fY, fZ);

                return String(data, size);
            }
    };

    class Vector2 {        
        public:
            float fX = 0.0f;
            float fY = 0.0f;

            constexpr Vector2() noexcept = default;

            constexpr Vector2(float x, float y) noexcept : fX(x), fY(y) {}

            constexpr Vector2(const Vector3& vec) noexcept : fX(vec.fX), fY(vec.fY) {}

            constexpr Vector2(const Vector4& vec) noexcept : fX(vec.fX), fY(vec.fY) {}

            constexpr Vector2(const GNINE_VECTOR2& vec) : fX(vec.x), fY(vec.y) {}

            constexpr Vector2& operator=(const Vector3& vec) noexcept
            {
                fX = vec.fX;
                fY = vec.fY;
                return *this;
            }

            constexpr Vector2& operator=(const Vector4& vec) noexcept
            {
                fX = vec.fX;
                fY = vec.fY;
                return *this;
            }

            constexpr float DotProduct(Vector2& other) const { return fX * other.fX + fY * other.fY; }

            float Length() const { return STD_NAMESPACE::hypotf(fX, fY); }

            // LengthSquared returns Length() without sqrt applied (i.e. returns x*x* + y*y).
            // This can be useful if you only want to compare lengths.
            constexpr float LengthSquared() const noexcept { return (fX * fX) + (fY * fY); }

            void Normalize() noexcept
            {
                const float fLength = Length();
                if (fLength > 0.0f)
                {
                    fX /= fLength;
                    fY /= fLength;
                }
            }

            constexpr Vector2 operator*(float fRight) const noexcept { return Vector2(fX * fRight, fY * fRight); }

            constexpr Vector2 operator/(float fRight) const noexcept { return Vector2(fX / fRight, fY / fRight); }

            constexpr Vector2 operator+(const Vector2& vecRight) const noexcept { return Vector2(fX + vecRight.fX, fY + vecRight.fY); }

            constexpr Vector2 operator-(const Vector2& vecRight) const noexcept { return Vector2(fX - vecRight.fX, fY - vecRight.fY); }

            constexpr Vector2 operator*(const Vector2& vecRight) const noexcept { return Vector2(fX * vecRight.fX, fY * vecRight.fY); }

            constexpr Vector2 operator/(const Vector2& vecRight) const noexcept { return Vector2(fX / vecRight.fX, fY / vecRight.fY); }

            constexpr void operator+=(float fRight) noexcept
            {
                fX += fRight;
                fY += fRight;
            }

            constexpr void operator+=(const Vector2& vecRight) noexcept
            {
                fX += vecRight.fX;
                fY += vecRight.fY;
            }

            constexpr void operator-=(float fRight) noexcept
            {
                fX -= fRight;
                fY -= fRight;
            }

            constexpr void operator-=(const Vector2& vecRight) noexcept
            {
                fX -= vecRight.fX;
                fY -= vecRight.fY;
            }

            constexpr void operator*=(float fRight) noexcept
            {
                fX *= fRight;
                fY *= fRight;
            }

            constexpr void operator*=(const Vector2& vecRight) noexcept
            {
                fX *= vecRight.fX;
                fY *= vecRight.fY;
            }

            constexpr void operator/=(float fRight) noexcept
            {
                fX /= fRight;
                fY /= fRight;
            }

            constexpr void operator/=(const Vector2& vecRight) noexcept
            {
                fX /= vecRight.fX;
                fY /= vecRight.fY;
            }

            bool operator==(const Vector2& param) const noexcept { return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON)); }

            bool operator!=(const Vector2& param) const noexcept { return !(*this == param); }

            operator GNINE_VECTOR2() {
                GNINE_VECTOR2 vec;
                vec.x = fX;
                vec.y = fY;

                return vec;
            }

            operator Vector3() {
                return Vector3(fX, fY, 0.0f);
            }

            operator Vector4() {
                return Vector4(fX, fY, 0, 0.0f);
            }
            
            operator String() {
                char data[100];
                memset((MemoryPointer)data, 0, sizeof(data));

                Int32 size = sprintf(data, "vector2(x: %g, y: %g)", fX, fY);

                return String(data, size);
            }
    };
            
    class Matrix {
        public:
            Matrix()
            {
                // Load the identity matrix
                vRight = Vector3(1.0f, 0.0f, 0.0f);
                vFront = Vector3(0.0f, 1.0f, 0.0f);
                vUp = Vector3(0.0f, 0.0f, 1.0f);
                vPos = Vector3(0.0f, 0.0f, 0.0f);
            }

            Matrix(const Vector3& vecPosition)
            {
                vRight = Vector3(1.0f, 0.0f, 0.0f);
                vFront = Vector3(0.0f, 1.0f, 0.0f);
                vUp = Vector3(0.0f, 0.0f, 1.0f);
                SetPosition(vecPosition);
            }

            Matrix(const Vector3& vecPosition, const Vector3& vecRotation)
            {
                // Initialize as identity, so rotation isn't broken by no scale set.
                vRight = Vector3(1.0f, 0.0f, 0.0f);
                vFront = Vector3(0.0f, 1.0f, 0.0f);
                vUp = Vector3(0.0f, 0.0f, 1.0f);
                SetRotation(vecRotation);
                SetPosition(vecPosition);
            }

            Matrix(const Vector3& vecPosition, const Vector3& vecRotation, const Vector3& vecScale)
            {
                // Initialize with scale set
                vRight = Vector3(vecScale.fX, 0.0f, 0.0f);
                vFront = Vector3(0.0f, vecScale.fY, 0.0f);
                vUp = Vector3(0.0f, 0.0f, vecScale.fZ);
                SetRotation(vecRotation);
                SetPosition(vecPosition);
            }

            Matrix(const GNINE_MATRIX& matData) {
                vRight = matData.right;
                vFront = matData.front;
                vUp = matData.up;
                vPos = matData.pos;
            }

            Matrix operator+(const Matrix& other) const
            {
                Matrix matResult;
                matResult.vRight = vRight + other.vRight;
                matResult.vFront = vFront + other.vFront;
                matResult.vUp = vUp + other.vUp;
                matResult.vPos = vPos + other.vPos;
                return matResult;
            }

            Matrix operator-(const Matrix& other) const
            {
                Matrix matResult;
                matResult.vRight = vRight - other.vRight;
                matResult.vFront = vFront - other.vFront;
                matResult.vUp = vUp - other.vUp;
                matResult.vPos = vPos - other.vPos;
                return matResult;
            }

            Matrix operator*(const Matrix& other) const
            {
                Matrix matResult;
                matResult.vRight.fX = vRight.fX * other.vRight.fX + vRight.fY * other.vFront.fX + vRight.fZ * other.vUp.fX;
                matResult.vRight.fY = vRight.fX * other.vRight.fY + vRight.fY * other.vFront.fY + vRight.fZ * other.vUp.fY;
                matResult.vRight.fZ = vRight.fX * other.vRight.fZ + vRight.fY * other.vFront.fZ + vRight.fZ * other.vUp.fZ;

                matResult.vFront.fX = vFront.fX * other.vRight.fX + vFront.fY * other.vFront.fX + vFront.fZ * other.vUp.fX;
                matResult.vFront.fY = vFront.fX * other.vRight.fY + vFront.fY * other.vFront.fY + vFront.fZ * other.vUp.fY;
                matResult.vFront.fZ = vFront.fX * other.vRight.fZ + vFront.fY * other.vFront.fZ + vFront.fZ * other.vUp.fZ;

                matResult.vUp.fX = vUp.fX * other.vRight.fX + vUp.fY * other.vFront.fX + vUp.fZ * other.vUp.fX;
                matResult.vUp.fY = vUp.fX * other.vRight.fY + vUp.fY * other.vFront.fY + vUp.fZ * other.vUp.fY;
                matResult.vUp.fZ = vUp.fX * other.vRight.fZ + vUp.fY * other.vFront.fZ + vUp.fZ * other.vUp.fZ;

                matResult.vPos.fX = vPos.fX * other.vRight.fX + vPos.fY * other.vFront.fX + vPos.fZ * other.vUp.fX + other.vPos.fX;
                matResult.vPos.fY = vPos.fX * other.vRight.fY + vPos.fY * other.vFront.fY + vPos.fZ * other.vUp.fY + other.vPos.fY;
                matResult.vPos.fZ = vPos.fX * other.vRight.fZ + vPos.fY * other.vFront.fZ + vPos.fZ * other.vUp.fZ + other.vPos.fZ;

                return matResult;
            }

            Matrix operator/(Matrix other) const
            {
                other.Invert();
                return (*this) * other;
            }

            Vector3 operator*(const Vector3& vec) const
            {
                return Vector3(vRight.fX * vec.fX + vFront.fX * vec.fY + vUp.fX * vec.fZ, vRight.fY * vec.fX + vFront.fY * vec.fY + vUp.fY * vec.fZ,
                            vRight.fZ * vec.fX + vFront.fZ * vec.fY + vUp.fZ * vec.fZ);
            }

            Matrix Inverse() const
            {
                float fDeterminant = vRight.fX * (vFront.fY * vUp.fZ - vUp.fY * vFront.fZ) - vFront.fX * (vRight.fY * vUp.fZ - vUp.fY * vRight.fZ) +
                                    vUp.fX * (vRight.fY * vFront.fZ - vFront.fY * vRight.fZ);

                if (fDeterminant == 0.0f)
                    return Matrix();

                float fRcp = 1.0f / fDeterminant;

                Matrix matResult;
                matResult.vRight.fX = fRcp * (vFront.fY * vUp.fZ - vUp.fY * vFront.fZ);
                matResult.vRight.fY = -fRcp * (vRight.fY * vUp.fZ - vUp.fY * vRight.fZ);
                matResult.vRight.fZ = fRcp * (vRight.fY * vFront.fZ - vFront.fY * vRight.fZ);

                matResult.vFront.fX = -fRcp * (vFront.fX * vUp.fZ - vUp.fX * vFront.fZ);
                matResult.vFront.fY = fRcp * (vRight.fX * vUp.fZ - vUp.fX * vRight.fZ);
                matResult.vFront.fZ = -fRcp * (vRight.fX * vFront.fZ - vFront.fX * vRight.fZ);

                matResult.vUp.fX = fRcp * (vFront.fX * vUp.fY - vUp.fX * vFront.fY);
                matResult.vUp.fY = -fRcp * (vRight.fX * vUp.fY - vUp.fX * vRight.fY);
                matResult.vUp.fZ = fRcp * (vRight.fX * vFront.fY - vFront.fX * vRight.fY);

                matResult.vPos.fX = -fRcp * (vFront.fX * (vUp.fY * vPos.fZ - vUp.fZ * vPos.fY) - vUp.fX * (vFront.fY * vPos.fZ - vFront.fZ * vPos.fY) +
                                            vPos.fX * (vFront.fY * vUp.fZ - vFront.fZ * vUp.fY));

                matResult.vPos.fY = fRcp * (vRight.fX * (vUp.fY * vPos.fZ - vUp.fZ * vPos.fY) - vUp.fX * (vRight.fY * vPos.fZ - vRight.fZ * vPos.fY) +
                                            vPos.fX * (vRight.fY * vUp.fZ - vRight.fZ * vUp.fY));

                matResult.vPos.fZ = -fRcp * (vRight.fX * (vFront.fY * vPos.fZ - vFront.fZ * vPos.fY) - vFront.fX * (vRight.fY * vPos.fZ - vRight.fZ * vPos.fY) +
                                            vPos.fX * (vRight.fY * vFront.fZ - vRight.fZ * vFront.fY));
                return matResult;
            }

            Matrix Clone() const
            {
                Matrix matResult;
                matResult.vRight = vRight.Clone();
                matResult.vFront = vFront.Clone();
                matResult.vUp = vUp.Clone();
                matResult.vPos = vPos.Clone();

                return matResult;
            }

            void Invert()
            {
                // Transpose the rotation matrix and negate the position
                Vector3 vOldRight = vRight, vOldFront = vFront, vOldUp = vUp;
                vRight = Vector3(vOldRight.fX, vOldFront.fX, vOldUp.fX);
                vFront = Vector3(vOldRight.fY, vOldFront.fY, vOldUp.fY);
                vUp = Vector3(vOldRight.fZ, vOldFront.fZ, vOldUp.fZ);
                vPos *= -1.0f;
            }
            Vector3 TransformVector(const Vector3& vec) const
            {
                return Vector3(vec.fX * vRight.fX + vec.fY * vFront.fX + vec.fZ * vUp.fX + vPos.fX, vec.fX * vRight.fY + vec.fY * vFront.fY + vec.fZ * vUp.fY + vPos.fY,
                            vec.fX * vRight.fZ + vec.fY * vFront.fZ + vec.fZ * vUp.fZ + vPos.fZ);
            }

            //
            // Ensure matrix component axes are normalized and orthogonal to each other.
            // Axis A direction is preserved, B is adjusted, the other is overwritten.
            //
            void OrthoNormalize(const UInt32 A, const UInt32 B)
            {
                // Deduce 3rd axis index
                const UInt32 C = 3 - A - B;

                // Ensure indices are in range and unique
                assert(A < 3 && B < 3 && C < 3 && A != B && C != A && C != B);

                Vector3& vPriAxis = GetAxis(A);
                Vector3& vSecAxis = GetAxis(B);
                Vector3& vLastAxis = GetAxis(C);

                if (vPriAxis.Normalize() == 0)
                    vPriAxis = Vector3(0, 0, 1);
                vSecAxis.Normalize();

                // Ensure vSecAxis is perpendicular to vPriAxis
                float fDot = vPriAxis.DotProduct(&vSecAxis);
                vSecAxis += vPriAxis * -fDot;
                if (vSecAxis.Normalize() == 0)
                    vSecAxis = vPriAxis.GetOtherAxis();

                // Do correct crossproduct calculation for the other axis
                if (((C + 1) % 3) != A)
                {
                    vLastAxis = vSecAxis;
                    vLastAxis.CrossProduct(&vPriAxis);
                }
                else
                {
                    vLastAxis = vPriAxis;
                    vLastAxis.CrossProduct(&vSecAxis);
                }
            }

            Matrix GetRotationMatrix() const
            {
                // Operate only on rotation, ignore scale.
                Matrix matClone = Clone();
                Vector3 vecScale = GetScale();
                matClone.vRight /= vecScale.fX;
                matClone.vFront /= vecScale.fY;
                matClone.vUp /= vecScale.fZ;
                matClone.vPos = Vector3(0, 0, 0);
                return matClone;
            }

            // Get matrix rotation as angles
            // Inverted to match GninEized rotations for vehicles and players (and objects on the server)
            // Should produce the same results as ( Vector3(0,0,0) - ConvertToEulerAngles() )
            Vector3 GetRotation() const
            {
                // Operate only on rotation, ignore scale.
                Matrix matRot = GetRotationMatrix();

                float fRotY = atan2(matRot.vRight.fZ, sqrtf((matRot.vRight.fX * matRot.vRight.fX) + (matRot.vRight.fY * matRot.vRight.fY)));
                float fRotZ = atan2(matRot.vRight.fY, matRot.vRight.fX);

                float fSinZ = -sin(fRotZ);
                float fCosZ = cos(fRotZ);
                float fRotX = atan2(matRot.vUp.fX * fSinZ + matRot.vUp.fY * fCosZ, matRot.vFront.fX * fSinZ + matRot.vFront.fY * fCosZ);
                return Vector3(-fRotX, -fRotY, fRotZ);
            }

            // Set matrix rotational part
            // Inverted to match GninEized rotations for vehicles and players (and objects on the server)
            // Should produce the same results as ( Vector3(0,0,0) - ConvertFromEulerAngles() )
            void SetRotation(const Vector3& vecRotation)
            {
                float fCosX = cos(-vecRotation.fX);
                float fCosY = cos(-vecRotation.fY);
                float fCosZ = cos(vecRotation.fZ);
                float fSinX = sin(-vecRotation.fX);
                float fSinY = sin(-vecRotation.fY);
                float fSinZ = sin(vecRotation.fZ);

                // Keep current scale even after rotation.
                Vector3 vecScale = GetScale();

                vRight.fX = fCosY * fCosZ;
                vRight.fY = fCosY * fSinZ;
                vRight.fZ = fSinY;
                vRight *= vecScale.fX;

                vFront.fX = fSinX * fSinY * fCosZ - fCosX * fSinZ;
                vFront.fY = fSinX * fSinY * fSinZ + fCosX * fCosZ;
                vFront.fZ = -fSinX * fCosY;
                vFront *= vecScale.fY;

                vUp.fX = -(fCosX * fSinY * fCosZ + fSinX * fSinZ);
                vUp.fY = fCosZ * fSinX - fCosX * fSinY * fSinZ;
                vUp.fZ = fCosX * fCosY;
                vUp *= vecScale.fZ;
            }

            // Get matrix translational part
            const Vector3& GetPosition() const { return vPos; }

            // Set matrix translational part
            void SetPosition(const Vector3& vecPosition) { vPos = vecPosition; }

            Vector3 GetScale() const { return Vector3(vRight.Length(), vFront.Length(), vUp.Length()); }

            void SetScale(const Vector3& vecScale)
            {
                Matrix matRot = GetRotationMatrix();
                vRight = matRot.vRight * vecScale.fX;
                vFront = matRot.vFront * vecScale.fY;
                vUp = matRot.vUp * vecScale.fZ;
            }
            //
            // Get reference to component axis by index
            //
            Vector3& GetAxis(const UInt32 uiIndex)
            {
                if (uiIndex == AXIS_UP)
                    return vUp;
                if (uiIndex == AXIS_FRONT)
                    return vFront;
                assert(uiIndex == AXIS_RIGHT);
                return vRight;
            }

            void GetBuffer(float* array)
            {
                array[0] = vRight.fX;
                array[1] = vRight.fY;
                array[2] = vRight.fZ;
                array[3] = 0.0f;

                array[4] = vFront.fX;
                array[5] = vFront.fY;
                array[6] = vFront.fZ;
                array[7] = 0.0f;

                array[8] = vUp.fX;
                array[9] = vUp.fY;
                array[10] = vUp.fZ;
                array[11] = 0.0f;

                array[12] = vPos.fX;
                array[13] = vPos.fY;
                array[14] = vPos.fZ;
                array[15] = 1.0f;
            }

            STD_NAMESPACE::array<STD_NAMESPACE::array<float, 4>, 4> To4x4Array() const noexcept
            {
                return {vRight.fX, vRight.fY, vRight.fZ, 0.0f, vFront.fX, vFront.fY, vFront.fZ, 0.0f, vUp.fX, vUp.fY, vUp.fZ, 0.0f, vPos.fX, vPos.fY, vPos.fZ, 1.0f};
            }

            operator String() {
                char data[250];
                memset((MemoryPointer)data, 0, sizeof(data));

                Int32 size = sprintf(
                    data,
                    "matrix(right: %s, front: %s, up : %s, pos: %s)",
                    ((String)vRight).c_str(),
                    ((String)vFront).c_str(),
                    ((String)vUp).c_str(),
                    ((String)vPos).c_str()
                );

                return String(data, size);
            }

            operator GNINE_MATRIX() {
                GNINE_MATRIX mat;

                GNINE_VECTOR3 v;
                v.x = vRight.fX;
                v.y = vRight.fY;
                v.z = vRight.fZ;

                mat.right = v;
                
                v.x = vFront.fX;
                v.y = vFront.fY;
                v.z = vFront.fZ;

                mat.front = v;
                
                v.x = vUp.fX;
                v.y = vUp.fY;
                v.z = vUp.fZ;

                mat.up = v;
                
                v.x = vPos.fX;
                v.y = vPos.fY;
                v.z = vPos.fZ;

                mat.pos = v;

                return mat;
            }

            enum EMatrixAxes
            {
                AXIS_RIGHT,
                AXIS_FRONT,
                AXIS_UP,
            };

            Vector3 vRight;
            Vector3 vFront;
            Vector3 vUp;
            Vector3 vPos;
    };

    class Element;
    class ColShape;
    class Marker;
    class Player;
    class Team;

    using ElementList = STD_NAMESPACE::vector<Element>;
    using ElementDataKeyList = STD_NAMESPACE::vector<String>;
    using PlayerList = STD_NAMESPACE::vector<Player>;
    using CallableList = STD_NAMESPACE::vector<Callable>;

    class Element {
        public:
            Element() {
                Drop();
            }

            Element(ElementId elementId) {
                Drop();

                SetId(elementId);
            }

            ~Element() = default;

            bool SetLowLOD(Element element) {
                return gnine_set_low_lod_element(m_pElementId, element);
            }

            bool SetFrozenStatus(bool frozenStatus) {
                return gnine_set_element_frozen(m_pElementId, frozenStatus);
            }

            bool SetCollisionsEnabled(bool enabled) {
                return gnine_set_element_collisions_enabled(m_pElementId, enabled);
            }

            bool SetModel(Int32 model) {
                return gnine_set_element_model(m_pElementId, model);
            }

            bool SetHealth(Float32 health) {
                return gnine_set_element_health(m_pElementId, health);
            }

            bool SetDoubleSidedEnabled(bool enabled) {
                return gnine_set_element_double_sided(m_pElementId, enabled);
            }

            bool SetAlpha(Int32 alpha) {
                return gnine_set_element_alpha(m_pElementId, alpha);
            }

            bool SetDimension(Int32 dimension) {
                return gnine_set_element_dimension(m_pElementId, dimension);
            }

            bool SetInterior(Int32 interior, bool setPosition = false, Vector3 position = Vector3(0.0f, 0.0f, 0.0f)) {
                GNINE_VECTOR3 pos = position;

                return gnine_set_element_interior(m_pElementId, interior, setPosition, &pos);
            }

            bool IsVisibleTo(Element to) const {
                return gnine_is_element_visible_to(m_pElementId, to);
            }

            bool ClearVisibleTo() {
                return gnine_clear_element_visible_to(m_pElementId);
            }

            bool SetVisibleTo(Element to, bool visible) {
                return gnine_set_element_visible_to(m_pElementId, to, visible);
            }

            bool SetTurnVelocity(Vector3 turnVelocity) {
                GNINE_VECTOR3 vel = turnVelocity;

                return gnine_set_element_turn_velocity(m_pElementId, &vel);
            }

            bool SetVelocity(Vector3 velocity) {
                GNINE_VECTOR3 vel = velocity;

                return gnine_set_element_velocity(m_pElementId, &vel);
            }

            bool SetRotation(Vector3 rotation, String rotationOrder = "default", bool fixPedRotation = false) {
                GNINE_VECTOR3 rot = rotation;

                return gnine_set_element_rotation(m_pElementId, &rot, rotationOrder.c_str(), fixPedRotation);
            }

            bool SetPosition(Vector3 position, bool wrap = true) {
                GNINE_VECTOR3 pos = position;

                return gnine_set_element_position(m_pElementId, &pos, wrap);
            }

            bool SetMatrix(Matrix matrix) {
                GNINE_MATRIX matData = matrix;

                return gnine_set_element_matrix(m_pElementId, &matData);
            }

            bool SetParent(Element parent) {
                return gnine_set_element_parent(m_pElementId, parent);
            }

            bool SetUniqueId(String id) {
                return gnine_set_element_id(m_pElementId, id.c_str());
            }

            bool RemoveData(String key) {
                return gnine_remove_element_data(m_pElementId, key.c_str());
            }

            bool HasData(String key, bool inherit = true) const {
                return gnine_has_element_data(m_pElementId, key.c_str(), inherit);
            }

            ElementDataKeyList GetAllData(UInt32 maxItemSize = 500) const {
                ElementDataKeyList list;

                CString datas[maxItemSize];
                memset((MemoryPointer)datas, 0, sizeof(datas));

                UInt32 itemCount = gnine_get_all_element_data(m_pElementId, datas, maxItemSize);

                for (UInt32 i = 0; i < itemCount; i++) {
                    CString data = datas[i];

                    list.push_back(data);

                    free((void*)data);
                }

                return list;
            }

            Callable::Arguments GetDataDirect(String key, bool inherit = true) const {
                if (key.empty()) {
                    return Callable::Arguments(NULL);
                }

                return gnine_get_element_data(m_pElementId, key.c_str(), inherit);
            }

            Byte GetDataType(String key, bool inherit = true) const {
                if (key.empty()) {
                    return GNINE_ARGUMENT_TYPE_UNKNOWN;
                }

                Callable::Arguments data = GetDataDirect(key, inherit);

                if (!data || data.Count() < 1) {
                    return GNINE_ARGUMENT_TYPE_UNKNOWN;
                }

                return data.GetType(1);
            }

            bool GetData(String key, bool* value, bool inherit = true) const {
                if (key.empty() || !value) {
                    return false;
                }

                Callable::Arguments data = GetDataDirect(key, inherit);

                if (!data || data.Count() < 1) {
                    return false;
                }

                if (data.GetType(1) != GNINE_ARGUMENT_TYPE_BOOLEAN) {
                    return false;
                }

                *value = data.GetAsBoolean(1);

                return true;
            }

            bool GetData(String key, LuaNumber* value, bool inherit = true) const {
                if (key.empty() || !value) {
                    return false;
                }

                Callable::Arguments data = GetDataDirect(key, inherit);

                if (!data || data.Count() < 1) {
                    return false;
                }

                if (data.GetType(1) != GNINE_ARGUMENT_TYPE_NUMBER) {
                    return false;
                }

                *value = data.GetAsNumber(1);

                return true;
            }

            bool GetData(String key, String* value, UIntPtr valueLength = 1024, bool inherit = true) const {
                if (key.empty() || !value) {
                    return false;
                }

                Callable::Arguments data = GetDataDirect(key, inherit);

                if (!data || data.Count() < 1) {
                    return false;
                }

                if (data.GetType(1) != GNINE_ARGUMENT_TYPE_STRING) {
                    return false;
                }

                *value = data.GetAsString(1, valueLength);

                return true;
            }

            bool GetData(String key, Callable* value, bool inherit = true) const {
                if (key.empty() || !value) {
                    return false;
                }

                Callable::Arguments data = GetDataDirect(key, inherit);

                if (!data || data.Count() < 1) {
                    return false;
                }

                if (data.GetType(1) != GNINE_ARGUMENT_TYPE_CALLABLE) {
                    return false;
                }

                *value = data.GetAsCallable(1);

                return true;
            }

            bool GetData(String key, Userdata* value, bool inherit = true) const {
                if (key.empty() || !value) {
                    return false;
                }

                Callable::Arguments data = GetDataDirect(key, inherit);

                if (!data || data.Count() < 1) {
                    return false;
                }

                if (data.GetType(1) != GNINE_ARGUMENT_TYPE_USERDATA) {
                    return false;
                }

                *value = data.GetAsUserdata(1);

                return true;
            }

            bool GetData(String key, Callable::Arguments* value, bool inherit = true) const {
                if (key.empty() || !value) {
                    return false;
                }

                Callable::Arguments data = GetDataDirect(key, inherit);

                if (!data || data.Count() < 1) {
                    return false;
                }

                if (data.GetType(1) != GNINE_ARGUMENT_TYPE_NUMBER) {
                    return false;
                }

                *value = data.GetAsList(1);

                return true;
            }

            bool SetDataDirect(String key, Callable::Arguments* data, bool syncMode = true) {
                if (key.empty() || !data) {
                    return false;
                }

                return gnine_set_element_data(m_pElementId, key.c_str(), *data, syncMode);
            }

            bool SetData(String key, Callable::Arguments& value, bool syncMode = true) {
                if (key.empty()) {
                    return false;
                }

                Callable::Arguments data;
                data.Push(value);

                return SetDataDirect(key, &data, syncMode);
            }

            bool SetData(String key, Callable value, bool syncMode = true) {
                if (key.empty()) {
                    return false;
                }

                Callable::Arguments data;
                data.PushCallable(value.m_rFunctionRef);

                return SetDataDirect(key, &data, syncMode);
            }

            template<typename ValueType>
            bool SetData(String key, ValueType value, bool syncMode = true) {
                if (key.empty()) {
                    return false;
                }

                Callable::Arguments data;
                data.Push(value);

                return SetDataDirect(key, &data, syncMode);
            }

            bool SetAttachedOffsets(Vector3 positionOffset, Vector3 rotationOffset) {
                GNINE_VECTOR3 posOffset = positionOffset;
                GNINE_VECTOR3 rotOffset = rotationOffset;

                return gnine_set_element_attached_offsets(m_pElementId, &posOffset, &rotOffset);
            }

            Vector3 GetAttachedPositionOffset() const {
                GNINE_VECTOR3 posOffset;
                memset((MemoryPointer)&posOffset, 0, sizeof(posOffset));

                if (!gnine_get_element_attached_offsets(m_pElementId, &posOffset, NULL)) {
                    return Vector3();
                }

                return posOffset;
            }
            
            Vector3 GetAttachedRotationOffset() const {
                GNINE_VECTOR3 rotOffset;
                memset((MemoryPointer)&rotOffset, 0, sizeof(rotOffset));

                if (!gnine_get_element_attached_offsets(m_pElementId, NULL, &rotOffset)) {
                    return Vector3();
                }

                return rotOffset;
            }

            Element GetAttachedTo() const {
                return gnine_get_element_attached_to(m_pElementId);
            }

            ElementList GetAttachElements(UInt32 maxSize = 500) const {
                ElementList list;

                ElementId elements[maxSize];
                memset((MemoryPointer)elements, 0, sizeof(elements));

                UInt32 count = gnine_get_attached_elements(m_pElementId, elements, maxSize);

                for (UInt32 i = 0; i < count; i++) {
                    list.push_back(elements[i]);
                }

                return list;
            }

            bool IsAttached() const {
                return gnine_is_element_attached(m_pElementId);
            }

            bool AttachElement(Element target) {
                return gnine_attach_elements(target, m_pElementId, NULL, NULL);
            }

            bool AttachElement(Element target, Vector3 positionOffset = Vector3(), Vector3 rotationOffset = Vector3()) {
                GNINE_VECTOR3 pOffset = positionOffset;
                GNINE_VECTOR3 rOffset = rotationOffset;

                return gnine_attach_elements(target, m_pElementId, &pOffset, &rOffset);
            }

            bool AttachTo(Element to) {
                return gnine_attach_elements(m_pElementId, to, NULL, NULL);
            }
            
            bool AttachTo(Element to, Vector3 positionOffset = Vector3(), Vector3 rotationOffset = Vector3()) {
                GNINE_VECTOR3 pOffset = positionOffset;
                GNINE_VECTOR3 rOffset = rotationOffset;

                return gnine_attach_elements(m_pElementId, to, &pOffset, &rOffset);
            }

            bool DetachElement(Element element) {
                return gnine_detach_elements(element, m_pElementId);
            }

            bool Detach() {
                return gnine_detach_elements(m_pElementId, NULL);
            }

            bool DetachFrom(Element from) {
                return gnine_detach_elements(m_pElementId, from);
            }

            Element GetLowLODElement() const {
                return gnine_get_low_lod_element(m_pElementId);
            }

            bool GetCollisionsEnabled() const {
                return gnine_get_element_collisions_enabled(m_pElementId);
            }

            Element GetSyncer() const {
                return gnine_get_element_syncer(m_pElementId);
            }

            UInt32 GetModel() const {
                return gnine_get_element_model(m_pElementId);
            }

            Float32 GetHealth() const {
                return gnine_get_element_health(m_pElementId);
            }

            bool IsDoubleSided() const {
                return gnine_is_element_double_sided(m_pElementId);
            }

            UInt32 GetAlpha() const {
                return gnine_get_element_alpha(m_pElementId);
            }

            String GetZoneName(bool citiesOnly = false) const {
                Int8 zoneName[100];
                memset((MemoryPointer)zoneName, 0, sizeof(zoneName));

                UIntPtr size = gnine_get_element_zone_name(m_pElementId, zoneName, sizeof(zoneName), citiesOnly);

                return String(zoneName, size);
            }

            UInt32 GetDimension() const {
                return gnine_get_element_dimension(m_pElementId);
            }

            UInt32 GetInterior() const {
                return gnine_get_element_interior(m_pElementId);
            }

            String GetType() const {
                Int8 type[100];
                memset((MemoryPointer)type, 0, sizeof(type));

                UIntPtr size = gnine_get_element_type(m_pElementId, type, sizeof(type));

                return String(type, size);
            }

            Vector3 GetTurnVelocity() const {
                GNINE_VECTOR3 vector;
                memset((MemoryPointer)&vector, 0, sizeof(vector));

                if (!gnine_get_element_turn_velocity(m_pElementId, &vector)) {
                    return Vector3();
                }

                return vector;
            }

            Vector3 GetVelocity() const {
                GNINE_VECTOR3 vector;
                memset((MemoryPointer)&vector, 0, sizeof(vector));

                if (!gnine_get_element_velocity(m_pElementId, &vector)) {
                    return Vector3();
                }

                return vector;
            }

            Vector3 GetPosition() const {
                GNINE_VECTOR3 vector;
                memset((MemoryPointer)&vector, 0, sizeof(vector));

                if (!gnine_get_element_position(m_pElementId, &vector)) {
                    return Vector3();
                }

                return vector;
            }

            Vector3 GetRotation(String rotationOrder = "default") const {
                GNINE_VECTOR3 vector;
                memset((MemoryPointer)&vector, 0, sizeof(vector));

                if (!gnine_get_element_rotation(m_pElementId, &vector, rotationOrder.c_str())) {
                    return Vector3();
                }

                return vector;
            }

            Matrix GetMatrix(bool badSyntax = true) const {
                GNINE_MATRIX matrix;
                memset((MemoryPointer)&matrix, 0, sizeof(matrix));

                if (!gnine_get_element_matrix(m_pElementId, &matrix, badSyntax)) {
                    return Matrix();
                }

                return matrix;
            }

            Element GetParent() const {
                return gnine_get_element_parent(m_pElementId);
            }

            String GetUniqueId() const {
                Int8 id[100];
                memset((MemoryPointer)id, 0, sizeof(id));

                UIntPtr size = gnine_get_element_id(m_pElementId, id, sizeof(id));

                return String(id, size);
            }

            ElementList GetChildren(String elementType = "", UInt32 maxSize = 500) const {
                ElementList list;

                ElementId children[maxSize];
                memset((MemoryPointer)children, 0, sizeof(children));

                UInt32 count = gnine_get_element_children(m_pElementId, elementType.empty() ? NULL : elementType.c_str(), children, maxSize);

                for (UInt32 i = 0; i < count; i++) {
                    list.push_back(children[i]);
                }

                return STD_NAMESPACE::move(list);
            }

            Element GetChild(UInt32 index) const {
                return gnine_get_element_child(m_pElementId, index);
            }

            UInt32 GetChildrenCount() const {
                return gnine_get_element_children_count(m_pElementId);
            }

            bool SetCallPropagationEnabled(bool enabled) {
                return gnine_set_element_call_propagation_enabled(m_pElementId, enabled);
            }

            bool IsCallPropagationEnabled() const {
                return gnine_is_element_call_propagation_enabled(m_pElementId);
            }

            bool IsLowLOD() const {
                return gnine_is_element_low_lod(m_pElementId);
            }

            bool IsInWater() const {
                return gnine_is_element_in_water(m_pElementId);
            }

            bool IsFrozen() const {
                return gnine_is_element_frozen(m_pElementId);
            }

            Element Clone() const {
                return gnine_clone_element(m_pElementId, NULL, false);
            }

            Element Clone(Vector3 position, bool cloneChildren = false) const {
                GNINE_VECTOR3 posV = position;

                return gnine_clone_element(m_pElementId, &posV, cloneChildren);
            }

            bool Destroy() {
                return gnine_destroy_element(m_pElementId);
            }

            void Drop() {
                m_pElementId = NULL;
            }

            void SetId(ElementId elementId) {
                m_pElementId = elementId;
            }

            ElementId GetId() const {
                return m_pElementId;
            }

            bool IsElement() const {
                return gnine_is_element(m_pElementId);
            }

            Element& operator=(Element element) {
                SetId(element.GetId());
                return *this;
            }

            Element& operator=(ElementId element) {
                SetId(element);
                return *this;
            }

            bool operator==(Element element) const {
                return m_pElementId == element.m_pElementId;
            }

            bool operator==(ElementId element) const {
                return m_pElementId == element;
            }

            Element operator[](UInt32 index) const {
                return GetChild(index);
            }

            operator bool() const {
                return gnine_is_element(m_pElementId);
            }

            operator ElementId() const {
                return m_pElementId;
            }

            operator String() const {
                char buff[50];
                memset((MemoryPointer)buff, 0, sizeof(buff));
                
                Int32 size = sprintf(buff, "element:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (int)m_pElementId);

                return String(buff, size);
            }

            static Element CreateElement(String elementType, String elementId = "") {
                return gnine_create_element(elementType.c_str(), elementId.c_str());
            }

            static bool IsElementInvalid(Element element) {
                return element.IsElement();
            }

            static Element GetElementById(String id, UInt32 index) {
                if (id.empty()) {
                    return NULL;
                }

                return gnine_get_element_by_id(id.c_str(), index);
            }

            static Element GetElementByIndex(String elementType, UInt32 index) {
                if (elementType.empty()) {
                    return NULL;
                }

                return gnine_get_element_by_index(elementType.c_str(), index);
            }

            static ElementList GetElementsByType(String elementType, Element startAt = NULL, UInt32 maxSize = 500) {
                ElementList list;

                if (elementType.empty()) {
                    return list;
                }

                ElementId elements[maxSize];
                memset((MemoryPointer)elements, 0, sizeof(elements));

                UInt32 count = gnine_get_elements_by_type(elementType.c_str(), startAt, elements, maxSize);

                for (UInt32 i = 0; i < count; i++) {
                    list.push_back(elements[i]);
                }

                return STD_NAMESPACE::move(list);
            }

            static ElementList GetElementsWithinRange(Vector3 position, Float32 radius, String elementType = "", Int32 interior = -1, Int32 dimension = -1, UInt32 maxSize = 500) {
                ElementList list;

                GNINE_VECTOR3 posVector = position;

                ElementId elements[maxSize];
                memset((MemoryPointer)elements, 0, sizeof(elements));

                UInt32 count = gnine_get_elements_within_range(&posVector, radius, elements, maxSize, elementType.c_str(), interior, dimension);

                for (UInt32 i = 0; i < count; i++) {
                    list.push_back(elements[i]);
                }

                return list;
            }

        private:
            ElementId m_pElementId;
    };

    class ColShape : public Element {
        public:
            ColShape() {
                Drop();
            }

            ColShape(ColShapeId elementId) {
                Drop();

                SetId(elementId);
            }

            ~ColShape() = default;

            ElementList GetElementsInside(String elementType = "", UInt32 maxSize = 500) const {
                ElementList list;

                ElementId elements[maxSize];
                memset((MemoryPointer)elements, 0, sizeof(elements));

                UInt32 count = gnine_get_elements_within_col_shape(*this, elementType.empty() ? NULL : elementType.c_str(), elements, maxSize);

                for (UInt32 i = 0; i < count; i++) {
                    list.push_back(elements[i]);
                }

                return list;
            }

            bool IsElementInside(Element element) const {
                return gnine_is_element_within_col_shape(element, *this);
            }

            ColShape& operator=(Element element) {
                SetId(element);
                
                return *this;
            }

            static ColShape GetElementColShape(Element element) {
                return gnine_get_element_col_shape(element);
            }
    };

    class Marker : public Element {
        public:
            Marker() {
                Drop();
            }

            Marker(MarkerId markerId) {
                Drop();

                SetId(markerId);
            }

            ~Marker() = default;

            bool IsElementInside(Element element) const {
                return gnine_is_element_within_marker(element, *this);
            }

            Marker& operator=(Element element) {
                SetId(element);

                return *this;
            }
    };

    class Player : public Element {
        public:
            struct ACInfo {
                String DetectedAC;
                UInt32 d3d9Size;
                String d3d9MD5;
                String d3d9SHA256;
            };

            Player() {
                Drop();
            }

            Player(PlayerId id) {
                Drop();
                
                SetId(id);
            }

            ~Player() = default;

            Player& operator=(Player player) {
                SetId(player);

                return *this;
            }

            bool SetVoiceIgnoreFrom(PlayerList players) {
                if (players.empty()) {
                    return false;
                }

                STD_NAMESPACE::vector<GNINE_PLAYER> playerRefs;

                UInt32 count = players.size();

                for (UInt32 i = 0; i < count; i++) {
                    playerRefs[i] = (GNINE_PLAYER)players[i];
                }

                return gnine_set_player_voice_ignore_from(*this, playerRefs.data(), count);
            }

            bool SetVoiceIgnoreFrom(Player player) {
                PlayerList list;
                list.push_back(player);

                return SetVoiceIgnoreFrom(list);
            }

            bool SetVoiceIgnoreFrom() {
                return gnine_set_player_voice_ignore_from(*this, NULL, 0);
            }

            bool SetVoiceBroadcastTo(PlayerList players) {
                if (players.empty()) {
                    return false;
                }

                STD_NAMESPACE::vector<GNINE_PLAYER> playerRefs;

                UInt8 count = players.size();

                playerRefs.resize(count);

                for (UInt32 i = 0; i < count; i++) {
                    playerRefs[i] = (GNINE_PLAYER)players[i];
                }

                return gnine_set_player_voice_broadcast_to(*this, playerRefs.data(), count);
            }

            bool SetVoiceBroadcastTo(Player player) {
                PlayerList list;
                list.push_back(player);

                return SetVoiceBroadcastTo(list);
            }

            bool SetVoiceBroadcastTo() {
                return gnine_set_player_voice_broadcast_to(*this, NULL, 0);
            }

            bool FadeCamera(bool fadeIn, Float32 timeToFade = 1.0f, Color screenColor = { 0, 0, 0, 255 }) {
                return gnine_fade_camera(*this, fadeIn, timeToFade, &screenColor);
            }

            bool SetCameraInterior(Int32 interior) {
                return gnine_set_camera_interior(*this, interior);
            }

            bool SetCameraTarget(Element target) {
                return gnine_set_camera_target(*this, target);
            }

            bool SetCameraMatrix(CameraMatrix matrix) {
                return gnine_set_camera_matrix(*this, &matrix);
            }

            Int32 GetCameraInterior() {
                return gnine_get_camera_interior(*this);
            }

            Element GetCameraTarget() {
                return gnine_get_camera_target(*this);
            }

            CameraMatrix GetCameraMatrix() {
                CameraMatrix matrix;

                gnine_get_camera_matrix(*this, &matrix);

                return matrix;
            }

            bool SetTeam(TeamId team) {
                return gnine_set_player_team(*this, team);
            }

            bool Ban(bool ip = true, bool username = false, bool serial = false, Element responsiblePlayer = NULL, String reason = "", Int64 seconds = 0) {
                return gnine_ban_player(*this, ip, username, serial, responsiblePlayer, reason.c_str(), seconds);
            }

            bool Kick(Player responsible = NULL, String reason = "") {
                return gnine_kick_player(*this, responsible, reason.c_str());
            }

            bool ShowChat(bool show, bool inputBlocked) {
                return gnine_show_chat(*this, show, inputBlocked);
            }

            bool ShowCursor(bool show, bool toggleControls = true) {
                return gnine_show_cursor(*this, show, toggleControls);
            }

            bool IsCursorShowing() {
                return gnine_is_cursor_showing(*this);
            }

            bool PreloadMissionAudio(UInt32 sound, UInt32 slot) {
                return gnine_preload_mission_audio(*this, sound, slot);
            }

            bool PlayMissionAudio(Vector3 position, UInt32 slot) {
                GNINE_VECTOR3 pos = position;

                return gnine_play_mission_audio(*this, &pos, slot);
            }

            bool PlaySoundFrontEnd(UInt32 sound) {
                return gnine_play_sound_front_end(*this, sound);
            }

            bool ToggleAllControls(bool enabled, bool gtaControls = true, bool gnineControls = true) {
                return gnine_toggle_all_controls(*this, enabled, gtaControls, gnineControls);
            }

            bool ToggleControl(String control, bool enabled) {
                return gnine_toggle_control(*this, control.c_str(), enabled);
            }

            bool SetControlState(String control, bool state) {
                return gnine_set_control_state(*this, control.c_str(), state);
            }

            bool IsControlEnabled(String control) {
                return gnine_is_control_enabled(*this, control.c_str());
            }

            bool GetControlState(String control) {
                return gnine_get_control_state(*this, control.c_str());
            }

            String GetKeyBoundToFunction(Callable function, UInt32 maxSize = 1024) {
                Int8 key[maxSize];
                memset((MemoryPointer)key, 0, sizeof(key));

                UInt32 size = gnine_get_key_bound_to_function(*this, function, key, maxSize);

                return String(key, size);
            }

            CallableList GetFunctionsBoundToKey(String key, String hitState, UInt32 maxItemCount = 500) {
                CallableList list;

                GNINE_CALLABLE_REF callables[maxItemCount];
                memset((MemoryPointer)callables, 0, sizeof(callables));

                UInt32 count = gnine_get_functions_bound_to_key(*this, key.c_str(), hitState.c_str(), callables, maxItemCount);

                list.resize(count);

                for (UInt32 i = 0; i < count; i++) {
                    list[i] = callables[i];
                }

                return STD_NAMESPACE::move(list);
            }

            bool IsKeyBound(String key, String hitState, Callable function) {
                return gnine_is_key_bound(*this, key.c_str(), hitState.c_str(), function);
            }

            bool UnbindKey(String key, String hitState, String command) {
                return gnine_unbind_key_on_command(*this, key.c_str(), hitState.c_str(), command.c_str());
            }

            bool UnbindKey(String key, String hitState, Callable function) {
                return gnine_unbind_key(*this, key.c_str(), hitState.c_str(), function);
            }

            bool BindKey(String key, String hitState, String command, String commandArguments = "") {
                return gnine_bind_key_on_command(*this, key.c_str(), hitState.c_str(), command.c_str(), commandArguments.c_str());
            }

            bool BindKey(String key, String hitState, Callable function, Callable::Arguments extraArguments = Callable::Arguments(NULL)) {
                return gnine_bind_key(*this, key.c_str(), hitState.c_str(), function, extraArguments);
            }

            bool ResendACInfo() {
                return gnine_resend_player_ac_info(*this);
            }

            bool ResendModInfo() {
                return gnine_resend_player_mod_info(*this);
            }

            bool SetAnnounceValue(String key, String value) {
                return gnine_set_player_announce_value(*this, key.c_str(), value.c_str());
            }

            String GetAnnounceValue(String key, UInt32 maxSize = 1024) {
                Int8 value[maxSize];
                memset((MemoryPointer)value, 0, sizeof(value));

                UInt32 size = gnine_get_player_announce_value(*this, key.c_str(), value, maxSize);

                return String(value, size);
            }

            bool SetScriptDebugLevel(UInt32 level) {
                return gnine_set_player_script_debug_level(*this, level);
            }

            bool TakeScreenShot(UInt32 width, UInt32 height, String tag = "", UInt32 quality = 30, UInt32 maxBandwidth = 5000, UInt32 maxPacketSize = 500) {
                return gnine_take_player_screen_shot(*this, width, height, tag.c_str(), quality, maxBandwidth, maxPacketSize);
            }

            bool DetonateSatchels() {
                return gnine_detonate_satchels(*this);
            }

            bool SetName(String name) {
                return gnine_set_player_name(*this, name.c_str());
            }

            bool Redirect(String host, Int32 port, String password = "") {
                return gnine_redirect_player(*this, host.c_str(), port, password.c_str());
            }

            bool SetBlurLevel(Int32 level) {
                return gnine_set_player_blur_level(*this, level);
            }

            bool SetMuted(bool muted) {
                return gnine_set_player_muted(*this, muted);
            }

            bool SetNametagShowing(bool show) {
                return gnine_set_player_nametag_showing(*this, show);
            }

            bool SetNametagColor(Color color, bool removeOverride = false) {
                return gnine_set_player_nametag_color(*this, &color);
            }

            bool SetNametagText(String text) {
                return gnine_set_player_nametag_text(*this, text.c_str());
            }

            bool ForceMap(bool force) {
                return gnine_force_player_map(*this, force);
            }

            bool SetWantedLevel(UInt32 level) {
                return gnine_set_player_wanted_level(*this, level);
            }

            bool ShowHudComponent(String component, bool show) {
                return gnine_show_player_hud_component(*this, component.c_str(), show);
            }

            bool Spawn(Vector3 position, Float32 rotation = 0.0f, Int32 model = 0, Int32 interior = 0, Int32 dimension = 0, TeamId team = NULL) {
                GNINE_VECTOR3 pos = position;

                return gnine_spawn_player(*this, &pos, rotation, model, interior, dimension, team);
            }

            bool TakeMoney(Int32 money) {
                return gnine_take_player_money(*this, money);
            }

            bool GiveMoney(Int32 money) {
                return gnine_give_player_money(*this, money);
            }

            bool SetMoney(Int32 money, bool instant = false) {
                return gnine_set_player_money(*this, money, instant);
            }

            UInt32 GetScriptDebugLevel() {
                return gnine_get_player_script_debug_level(*this);
            }

            ACInfo GetACInfo(UInt32 maxStringsSize = 1024) {
                Int8   DetectedAC[maxStringsSize];
                UInt32 d3d9Size = 0;
                Int8   d3d9MD5[maxStringsSize];
                Int8   d3d9SHA256[maxStringsSize];

                memset((MemoryPointer)DetectedAC, 0, sizeof(DetectedAC));
                memset((MemoryPointer)d3d9MD5, 0, sizeof(d3d9MD5));
                memset((MemoryPointer)d3d9SHA256, 0, sizeof(d3d9SHA256));

                GNINE_PLAYER_AC_INFO(10) f;

                gnine_get_player_ac_info_r(*this, DetectedAC, &d3d9Size, d3d9MD5, d3d9SHA256, maxStringsSize);

                ACInfo info;
                info.DetectedAC = String(DetectedAC, strlen(DetectedAC));
                info.d3d9Size = d3d9Size;
                info.d3d9MD5 = String(d3d9MD5, strlen(d3d9MD5));
                info.d3d9SHA256 = String(d3d9SHA256, strlen(d3d9SHA256));

                return info;
            }

            String GetVersion(UInt32 maxSize = 1024) {
                Int8 version[maxSize];
                memset((MemoryPointer)version, 0, sizeof(version));

                UInt32 size = gnine_get_player_version(*this, version, maxSize);

                return String(version, size);
            }

            AccountId GetAccount() {
                return gnine_get_player_account(*this);
            }

            String GetIP(UInt32 maxSize = 1024) {
                Int8 ip[maxSize];
                memset((MemoryPointer)ip, 0, sizeof(ip));

                UInt32 size = gnine_get_player_ip(*this, ip, maxSize);

                return String(ip, size);
            }

            String GetName(UInt32 maxSize = 1024) {
                Int8 name[maxSize];
                memset((MemoryPointer)name, 0, sizeof(name));

                UInt32 size = gnine_get_player_name(*this, name, maxSize);

                return String(name, size);
            }

            Int32 GetBlurLevel() {
                return gnine_get_player_blur_level(*this);
            }

            String GetUsername(UInt32 maxSize = 1024) {
                Int8 username[maxSize];
                memset((MemoryPointer)username, 0, sizeof(username));

                UInt32 size = gnine_get_player_user_name(*this, username, maxSize);

                return String(username, size);
            }

            String GetCommunityId(UInt32 maxSize = 1024) {
                Int8 id[maxSize];
                memset((MemoryPointer)id, 0, sizeof(id));

                UInt32 size = gnine_get_player_community_id(*this, id, maxSize);

                return String(id, size);
            }

            String GetSerial(UInt32 index = 0, UInt32 maxSize = 1024) {
                Int8 serial[maxSize];
                memset((MemoryPointer)serial, 0, sizeof(serial));

                UInt32 size = gnine_get_player_serial(*this, index, serial, maxSize);

                return String(serial, size);
            }

            bool IsNametagShowing() {
                return gnine_is_player_nametag_showing(*this);
            }

            Color GetNametagColor() {
                Color color;

                gnine_get_player_nametag_color(*this, &color);

                return color;
            }

            String GetNametagText(UInt32 maxSize = 1024) {
                Int8 text[maxSize];
                memset((MemoryPointer)text, 0, sizeof(text));

                UInt32 size = gnine_get_player_nametag_text(*this, text, maxSize);

                return String(text, size);
            }

            bool IsMapForced() {
                return gnine_is_player_map_forced(*this);
            }

            bool IsScoreboardForced() {
                return gnine_is_player_scoreboard_forced(*this);
            }

            Int64 GetIdleTime() {
                return gnine_get_player_idle_time(*this);
            }

            UInt32 GetWantedLevel() {
                return gnine_get_player_wanted_level(*this);
            }

            bool CanUseFunction(String functionName) {
                return gnine_can_player_use_function(*this, functionName.c_str());
            }

            TeamId GetTeam() {
                return gnine_get_player_team(*this);
            }

            bool IsMuted() {
                return gnine_is_player_muted(*this);
            }

            UInt32 GetPing() {
                return gnine_get_player_ping(*this);
            }

            Int32 GetMoney() {
                return gnine_get_player_money(*this);
            }

            bool SetAmmo(UInt32 ammo, UInt32 slot = 0xff, UInt32 ammoInClip = 0) {
                return gnine_set_player_ammo(*this, ammo, slot, ammoInClip);
            }

            bool SetSyncElementEnabled(Element element, bool enabled = true, bool persist = false) {
                return gnine_set_element_syncer(element, *this, enabled, persist);
            }

            bool SubscribeElementData(Element element, String key) {
                return gnine_add_element_data_subscriber(element, key.c_str(), *this);
            }

            bool UnsubscribeElementData(Element element, String key) {
                return gnine_remove_element_data_subscriber(element, key.c_str(), *this);
            }

            bool DoesSubscribeElementData(Element element, String key) const {
                return gnine_has_element_data_subscriber(element, key.c_str(), *this);
            }

            static UInt32 GetPlayerCount() {
                return gnine_get_player_count();
            }

            static Player GetPlayerFromName(String name) {
                return gnine_get_player_from_name(name.c_str());
            }

            static Player GetRandomPlayer() {
                return gnine_get_random_player();
            }
            
            static PlayerList GetAlivePlayers(UInt32 maxItemCount = 500) {
                PlayerList list;

                PlayerId players[maxItemCount];
                memset((MemoryPointer)players, 0, sizeof(players));

                UInt32 count = gnine_get_alive_players(players, maxItemCount);

                list.resize(count);

                for (UInt32 i = 0; i < count ; i++) {
                    list[i] = players[i];
                }

                return list;
            }

            static PlayerList GetDeadPlayers(UInt32 maxItemCount = 500) {
                PlayerList list;

                PlayerId players[maxItemCount];
                memset((MemoryPointer)players, 0, sizeof(players));

                UInt32 count = gnine_get_dead_players(players, maxItemCount);

                list.resize(count);

                for (UInt32 i = 0; i < count ; i++) {
                    list[i] = players[i];
                }

                return list;
            }
    };

    class Team : public Element {
        public:
            Team() {
                Drop();
            }

            Team(TeamId id) {
                Drop();
                
                SetId(id);
            }

            ~Team() = default;

            Team& operator=(Team team) {
                SetId(team);

                return *this;
            }

            bool SetFriendlyFire(bool frinedlyFire) {
                return gnine_set_team_friendly_fire(*this, frinedlyFire);
            }

            bool SetColor(Color color) {
                return gnine_set_team_color(*this, &color);
            } 

            bool SetName(String name) {
                return gnine_set_team_name(*this, name.c_str());
            }

            UInt32 CountPlayers() {
                return gnine_count_players_in_team(*this); 
            }

            PlayerList GetPlayers(UInt32 maxItemSize = 500) {
                PlayerList list;

                PlayerId players[maxItemSize];
                memset((MemoryPointer)players, 0, sizeof(players));

                UInt32 count = gnine_get_players_in_team(*this, players, maxItemSize);

                list.resize(count);

                for (UInt32 i = 0; i < count; i++) {
                    list[i] = players[i];
                }

                return std::move(list);
            }

            bool GetFriendlyFire() {
                return gnine_get_team_friendly_fire(*this);
            }

            Color GetColor() {
                Color color;
                memset((MemoryPointer)&color, 0, sizeof(color));

                gnine_get_team_color(*this, &color);

                return color;
            }

            String GetName(UIntPtr maxSize = 1024) {
                Int8 name[maxSize];
                memset((MemoryPointer)name, 0, maxSize);

                UIntPtr size = gnine_get_team_name(*this, name, maxSize);

                return String(name, size);
            }

            static Team CreateTeam(String name, Color color = { .b = 178, .g = 221, .r = 235, .a = 255 }) {
                return gnine_create_team(name.c_str(), &color);
            }

            static Team GetTeamFromName(String name) {
                return gnine_get_team_from_name(name.c_str());
            }
    };

    class Water : public Element {
        public:
            Water() {
                Drop();
            }

            Water(WaterId id) {
                Drop();
                
                SetId(id);
            }

            ~Water() = default;

            Water& operator=(Water water) {
                SetId(water);

                return *this;
            }

            bool SetLevel(Float32 level) {
                return gnine_set_water_level(*this, level, true, true, true, false);
            }

            Vector3 GetVertexPosition(Int32 vertexIndex) {
                GNINE_VECTOR3 pos;
                memset((MemoryPointer)&pos, 0, sizeof(pos));

                gnine_get_water_vertex_position(*this, vertexIndex, &pos);

                return Vector3(pos.x, pos.y, pos.z);
            }

            bool SetVertexPosition(Int32 vertexIndex, Vector3 position) {
                GNINE_VECTOR3 pos = position;

                return gnine_set_water_vertex_position(*this, vertexIndex, &pos);
            }

            static Water CreateWater(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, bool shallow = false) {
                GNINE_VECTOR3 pv1 = v1;
                GNINE_VECTOR3 pv2 = v2;
                GNINE_VECTOR3 pv3 = v3;
                GNINE_VECTOR3 pv4 = v4;

                return gnine_create_water(&pv1, &pv2, &pv3, &pv4, shallow);
            }

            static bool SetWaterLevel(Float32 level, bool includeWorldNonSeaLevel = true, bool includeAllWaterElements = true, bool includeWorldSeaLevel = true, bool includeOutsideWorldLevel = false) {
                return gnine_set_water_level(NULL, level, includeWorldNonSeaLevel, includeAllWaterElements, includeWorldSeaLevel, includeOutsideWorldLevel);
            }

            static bool ResetWaterLevel() {
                return gnine_reset_water_level();
            }

            static Color GetWaterColor() {
                Color color;

                gnine_get_water_color(&color);

                return color;
            }

            static bool SetWaterColor(Color color) {
                return gnine_set_water_color(&color);
            }

            static bool ResetWaterColor() {
                return gnine_reset_water_color();
            }
    };

    class Worker {
        public:
            using WorkerHandler = GNINE_WORKER_HANDLER;

            enum class State : Byte {
                Off = GNINE_WORKER_STATE_OFF,
                Starting = GNINE_WORKER_STATE_STARTING,
                Running = GNINE_WORKER_STATE_RUNNING,
                Waiting = GNINE_WORKER_STATE_WAITING,
                Terminated = GNINE_WORKER_STATE_TERMINATED,
                Finished = GNINE_WORKER_STATE_FINISHED
            };

            class Mutex {
                public:
                    Mutex() {
                        Drop();

                        m_pMutexId = gnine_create_mutex();
                    }

                    Mutex(WorkerMutexId mutexId) {
                        Drop();

                        m_pMutexId = mutexId;
                    }

                    Mutex(const Mutex& mutex) {
                        TakeOwnership(*(Mutex*)(&mutex));
                    }

                    ~Mutex() {
                        if (m_bAutoCleanup) {
                            Destroy();
                        }

                        Drop();
                    }

                    void TakeOwnership(Mutex& mutex) {
                        if (*this) {
                            if (m_bAutoCleanup) {
                                Destroy();
                            }
                        }

                        m_pMutexId = mutex.m_pMutexId;
                        m_bAutoCleanup = mutex.m_bAutoCleanup;

                        mutex.DisableAutoCleanup();
                    }

                    void Drop() {
                        m_pMutexId = NULL;
                        m_bAutoCleanup = true;
                    }

                    bool Destroy() {
                        return gnine_destroy_mutex(m_pMutexId);
                    }

                    bool Lock() {
                        return gnine_lock_mutex(m_pMutexId);
                    }

                    bool Unlock() {
                        return gnine_unlock_mutex(m_pMutexId);
                    }

                    void DisableAutoCleanup(bool disabled = true) {
                        m_bAutoCleanup = !disabled;
                    }

                    bool IsAutoCleanupDisabled() const {
                        return !m_bAutoCleanup;
                    }

                    bool operator==(WorkerMutexId mutexId) {
                        return m_pMutexId = mutexId;
                    }

                    bool operator==(Mutex mutex) {
                        return m_pMutexId = mutex.m_pMutexId;
                    }

                    Mutex& operator=(WorkerMutexId mutexId) {
                        if (*this) {
                            if (m_bAutoCleanup) {
                                Destroy();
                            }
                        }

                        m_pMutexId = mutexId;

                        return *this;
                    }

                    Mutex& operator=(const Mutex& mutex) {
                        TakeOwnership(*(Mutex*)(&mutex));

                        return *this;
                    } 

                    operator WorkerMutexId() const {
                        return m_pMutexId;
                    }

                    operator String() {
                        char buff[50];
                        memset((MemoryPointer)buff, 0, sizeof(buff));
                        
                        Int32 size = sprintf(buff, "worker_mutex:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (int)m_pMutexId);

                        return String(buff, size);
                    }

                    operator bool() {
                        return gnine_is_mutex(m_pMutexId);
                    }

                private:
                    WorkerMutexId m_pMutexId;
                    
                    bool m_bAutoCleanup;
            };

            Worker() {
                Drop();
            }

            Worker(WorkerId workerId) {
                Drop();

                m_pWorkerId = workerId;
            }

            Worker(const Worker& worker) {
                TakeOwnership(*(Worker*)(&worker));
            }

            Worker(WorkerHandler function, MemoryPointer dataToSendToWorker) {
                Drop();

                if (!function) {
                    return;
                }

                m_pWorkerId = gnine_create_worker(function, dataToSendToWorker);
            }

            ~Worker() {
                if (m_bAutoTerminator) {
                    Terminate();
                }

                Drop();
            }

            void TakeOwnership(Worker& worker) {
                if (*this) {
                    if (m_bAutoTerminator) {
                        Terminate();
                    }
                }

                m_pWorkerId = worker.m_pWorkerId;
                m_bAutoTerminator = worker.m_bAutoTerminator;

                worker.DisableAutoTerminator();
            }

            void Drop() {
                m_pWorkerId = NULL;
                m_bAutoTerminator = true;
            }

            bool Run(bool waitUntilStartCompeletlly = true) {
                bool result = gnine_run_worker(m_pWorkerId);

                if (result && waitUntilStartCompeletlly) {
                    while ((Int32)GetState() < (Int32)State::Running) {
                        Sleep(1);
                    }
                }

                return result;
            }

            bool Terminate() {
                return gnine_terminate_worker(m_pWorkerId);
            }

            bool Join() {
                return gnine_worker_join(m_pWorkerId);
            }

            State GetState() const {
                if (!*this) {
                    return State::Off;
                }

                return (State)gnine_get_worker_state(m_pWorkerId);
            }

            void DisableAutoTerminator(bool disabled = true) {
                m_bAutoTerminator = !disabled;
            }

            bool IsAutoTerminatorDisabled() const {
                return !m_bAutoTerminator;
            }

            bool operator==(const WorkerId& workerId) const {
                return m_pWorkerId == workerId;
            }

            bool operator==(const Worker& worker) const {
                return m_pWorkerId == worker.m_pWorkerId;
            }

            Worker& operator=(WorkerId workerId) {
                if (m_bAutoTerminator) {
                    Terminate();
                }

                m_pWorkerId = workerId;

                return *this;
            }

            Worker& operator=(const Worker& worker) {
                TakeOwnership(*(Worker*)(&worker));

                return *this;
            }

            operator WorkerId() const {
                return m_pWorkerId;
            }

            operator String() const {
                char buff[50];
                memset((MemoryPointer)buff, 0, sizeof(buff));
                
                Int32 size = sprintf(buff, "worker:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (int)m_pWorkerId);

                return String(buff, size);
            }

            operator bool() const {
                return gnine_is_worker(m_pWorkerId);
            }

            static Worker GetCurrentWorker() {
                Worker worker = gnine_get_current_worker();

                worker.DisableAutoTerminator();

                return worker;
            }

            static Worker GetMainWorker() {
                Worker worker = gnine_get_main_worker();

                worker.DisableAutoTerminator();

                return worker;
            }

            static void Sleep(UInt32 milliSeconds) {
                gnine_sleep_worker(milliSeconds);
            }

        private:
            WorkerId m_pWorkerId;

            bool m_bAutoTerminator;
    };
    
    using TimerList = STD_NAMESPACE::vector<class Timer>;

    class Timer {
        public:
            using TimerDetails = GNINE_TIMER_DETAILS;

            Timer() {
                Drop();
            }

            Timer(TimerId timerId) {
                Drop();

                m_pTimerId = timerId;
            }

            Timer(const Timer& timer) {
                Drop();

                m_pTimerId = timer.m_pTimerId;
            }

            Timer(Callable function, UInt32 interval, UInt32 repeats = 1, Callable::Arguments arguments = NULL) {
                m_pTimerId = gnine_set_timer(function, interval, repeats, arguments);
            }

            template<typename... Args>
            Timer(Callable function, UInt32 interval, UInt32 repeats, Args... args) {
                if (!m_pTimerArguments) {
                    m_pTimerArguments = new Callable::Arguments();
                }

                AddArgument(args...);

                m_pTimerId = gnine_set_timer(function, interval, repeats, *m_pTimerArguments);

                delete m_pTimerArguments;
                m_pTimerArguments = NULL;
            }

            ~Timer() {
                Drop();
            }

            void Drop() {
                m_pTimerId = NULL;
            }

            void Kill() {
                gnine_kill_timer(m_pTimerId);
            }

            void Reset() {
                gnine_reset_timer(m_pTimerId);
            }

            TimerDetails GetDetails() const {
                TimerDetails details;

                gnine_get_timer_details(m_pTimerId, &details);

                return details;
            }

            bool operator==(const TimerId& timerId) {
                return m_pTimerId == timerId;
            }

            bool operator==(const Timer& timer) {
                return m_pTimerId == timer.m_pTimerId;
            }

            Timer& operator=(const TimerId& timerId) {
                m_pTimerId = timerId;
                return *this;
            }

            Timer& operator=(const Timer& timer) {
                m_pTimerId = timer.m_pTimerId;
                return *this;
            }

            operator TimerId() const {
                return m_pTimerId;
            }

            operator String() const {
                char buff[50];
                memset((MemoryPointer)buff, 0, sizeof(buff));
                
                Int32 size = sprintf(buff, "timer:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (int)m_pTimerId);

                return String(buff, size);
            }

            operator bool() const {
                return gnine_is_timer(m_pTimerId);
            }

            static TimerList GetTimers(Float64 time, UInt32 maxItemCount = 500) {
                TimerList list;

                TimerId timers[maxItemCount];
                memset((MemoryPointer)timers, 0, sizeof(timers));

                UInt32 count = gnine_get_timers(time, timers, maxItemCount);

                for (UInt32 i = 0; i < count; i++) list.push_back(timers[i]);

                return list;
            }

        private:
            TimerId m_pTimerId;

            Callable::Arguments* m_pTimerArguments;

            template<typename Arg>
            void AddArgument(Arg arg) {
                Callable::AddCallableArgumentToArguments(m_pTimerArguments, (Callable::Argument)arg);
            }

            template<typename Arg, typename... Args>
            void AddArgument(Arg arg, Args... args) {
                Callable::AddCallableArgumentToArguments(m_pTimerArguments, (Callable::Argument)arg);
                AddArgument(args...);
            }
    };
    
    // class Resource {};
    using Resource = ResourceId;

    struct RemoteRequestOptions {
        UInt32     connectionAttempts;
        UInt32     connectTimeout;
        String               method;
        String               queueName;
        String               postData;
        bool                 postIsBinary;
        UInt32               maxRedirects;
        String               username;
        String               password;
        StringUnorderedMap<> headers;
        StringUnorderedMap<> formFields;

        RemoteRequestOptions() {
            connectionAttempts = 10;
            connectTimeout = 10000;
            method = "";
            queueName = GNINE_REMOTE_REQUEST_DEFAULT_QUEUE_NAME;
            postData = "";
            postIsBinary = false;
            maxRedirects = 8;
            username = "";
            password = "";
            headers = StringUnorderedMap<>();
            formFields = StringUnorderedMap<>();
        }

        RemoteRequestOptions(GNINE_REMOTE_REQUEST_OPTIONS options) {
            Read(&options);
        }

        void Read(GNINE_REMOTE_REQUEST_OPTIONS* options) {
            if (!options) {
                return;
            }

            connectionAttempts = options->connectionAttempts;
            connectTimeout = options->connectTimeout;
            method = options->method;
            queueName = options->queueName;
            postData = options->postData;
            postIsBinary = options->postIsBinary;
            maxRedirects = options->maxRedirects;
            username = options->username;
            password = options->password;
            headers = StringUnorderedMap<>();
            formFields = StringUnorderedMap<>();

            if (options->headers && options->headersCount > 0) {
                for (UInt32 i = 0; i < (options->headersCount * 2); i += 2) {
                    headers[options->headers[i]] = options->headers[i + 1];
                }
            }

            if (options->formFields && options->formFieldsCount > 0) {
                for (UInt32 i = 0; i < (options->formFieldsCount * 2); i += 2) {
                    formFields[options->formFields[i]] = options->formFields[i + 1];
                }
            }
        }

        void Write(GNINE_REMOTE_REQUEST_OPTIONS* options) const {
            if (!options) {
                return;
            }

            memset((MemoryPointer)options, 0, sizeof(*options));

            options->connectionAttempts = connectionAttempts;
            options->connectTimeout = connectTimeout;
            options->method = method.c_str();
            options->queueName = queueName.c_str();
            options->postData = postData.c_str();
            options->postIsBinary = postIsBinary;
            options->maxRedirects = maxRedirects;
            options->username = username.c_str();
            options->password = password.c_str();
            
            UInt32 headersSize = headers.size();
            UInt32 formFieldsSize = formFields.max_size();

            if (headersSize > 0) {
                options->headers = (CString*)malloc(headersSize * (sizeof(CString) * 2));

                if (options->headers) {
                    UInt32 i = 0;

                    for (const auto& header : headers) {
                        options->headers[i] = header.first.c_str();
                        options->headers[i + 1] = header.second.c_str();

                        i += 2;
                    }

                    options->headersCount = headersSize;
                }
            }

            if (formFieldsSize > 0) {
                options->formFields = (CString*)malloc(formFieldsSize * (sizeof(CString) * 2));

                if (options->formFields) {
                    UInt32 i = 0;

                    for (const auto& form : formFields) {
                        options->formFields[i] = form.first.c_str();
                        options->formFields[i + 1] = form.second.c_str();

                        i += 2;
                    }

                    options->formFieldsCount = formFieldsSize;
                }
            }
        }

        void ClearWrite(GNINE_REMOTE_REQUEST_OPTIONS* options) const {
            if (!options) {
                return;
            }

            if (options->headers) {
                free(options->headers);
            }

            if (options->formFields) {
                free(options->formFields);
            }
        }
    };

    struct RemoteRequestInfo {
        bool                  isFetch;
        String                url;
        String                queue;
        Resource              resource;
        UInt64                start;
        String                postData;
        StringUnorderedMap<>  headers;       // this headers are like `GNINE_REMOTE_REQUEST_OPTIONS` headers. that means you can select header name with `headers[0]` and header value `header[1]`
        String                method;
        UInt32                connectionAttempts;
        UInt32                connectionTimeout;
        UInt32                bytesReceived;
        UInt32                bytesTotal;
        UInt32                currentAttempt;

        RemoteRequestInfo() {
            isFetch = false;
            url = "";
            queue = "";
            resource = NULL;
            start = 0;
            postData = "";
            headers = StringUnorderedMap<>();
            method = "";
            connectionAttempts = 0;
            connectionTimeout = 0;
            bytesReceived = 0;
            bytesTotal = 0;
            currentAttempt = 0;
        }

        RemoteRequestInfo(GNINE_REMOTE_REQUEST_INFO info) {
            Read(&info);
        }

        void Read(GNINE_REMOTE_REQUEST_INFO* info) {
            if (!info) {
                return;
            }

            isFetch = info->isFetch;
            url = info->url;
            queue = info->queue;
            resource = info->resource;
            start = info->start;
            postData = info->postData;
            headers = StringUnorderedMap<>();
            method = info->method;
            connectionAttempts = info->connectionAttempts;
            connectionTimeout = info->connectionTimeout;
            bytesReceived = info->bytesReceived;
            bytesTotal = info->bytesTotal;
            currentAttempt = info->currentAttempt;

            if (info->headers && info->headersCount > 0) {
                for (UInt32 i = 0; i < (info->headersCount * 2); i += 2) {
                    headers[info->headers[i]] = info->headers[i + 1];
                }
            }
        }
    };

    class RemoteRequest;
    using RemoteRequestList = STD_NAMESPACE::vector<RemoteRequest>;

    class RemoteRequest {
        public:
            RemoteRequest() {
                Drop();
            }
            
            RemoteRequest(RemoteRequestId id) {
                Drop();

                m_pRemoteRequestId = id;
            }

            RemoteRequest(RemoteRequestOptions options) {
                Drop();
                m_mpOptions = options;
            }

            RemoteRequest(const RemoteRequest& request) {
                Drop();

                m_pRemoteRequestId = request.m_pRemoteRequestId;
                m_mpOptions = request.m_mpOptions;
            }

            ~RemoteRequest() {
                Drop();
            }

            void Drop() {
                m_pRemoteRequestId = NULL;
                m_mpOptions = RemoteRequestOptions();
            }

            bool Abort() {
                return gnine_abort_remote_request(m_pRemoteRequestId);
            }

            RemoteRequestInfo GetInfo(UInt32 postDataLength = 0, bool includeHeaders = false) const {
                RemoteRequestInfo info;

                GNINE_REMOTE_REQUEST_INFO cInfo;

                if (!gnine_get_remote_request_info(m_pRemoteRequestId, &cInfo, postDataLength, includeHeaders)) {
                    return info;
                }

                info.Read(&cInfo);

                gnine_cleanup_remote_request_info(&cInfo);

                return info;
            }

            void SetOptions(RemoteRequestOptions options) {
                m_mpOptions = options;
            }

            RemoteRequestOptions GetOptions() const {
                return m_mpOptions;
            }

            bool Call(String url, Callable callbackFunction, String queueName = GNINE_REMOTE_REQUEST_DEFAULT_QUEUE_NAME, UInt32 connectionAttempts = 10, UInt32 connectionTimeout = 10000, Callable::Arguments arguments = NULL) {
                m_pRemoteRequestId = gnine_call_remote(url.c_str(), queueName.c_str(), connectionAttempts, connectionTimeout, callbackFunction, arguments);

                if (m_pRemoteRequestId) {
                    return true;
                }

                return false;
            }

            bool Fetch(String url, Callable callbackFunction, Callable::Arguments arguments = NULL) {
                GNINE_REMOTE_REQUEST_OPTIONS options;

                m_mpOptions.Write(&options);

                m_pRemoteRequestId = gnine_fetch_remote(url.c_str(), &options, callbackFunction, arguments);

                m_mpOptions.ClearWrite(&options);

                if (m_pRemoteRequestId) {
                    return true;
                }

                return false;
            }

            bool operator==(RemoteRequestId id) const {
                return m_pRemoteRequestId == id;
            }

            bool operator==(const RemoteRequest& request) const {
                return m_pRemoteRequestId == request.m_pRemoteRequestId;
            }

            RemoteRequest& operator=(RemoteRequestId id) {
                m_pRemoteRequestId = id;

                return *this;
            }

            RemoteRequest& operator=(const RemoteRequest& request) {
                m_pRemoteRequestId = request.m_pRemoteRequestId;
                m_mpOptions = request.m_mpOptions;

                return *this;
            }

            operator RemoteRequestId() const {
                return m_pRemoteRequestId;
            }

            operator String() const {
                char buff[50];
                memset((MemoryPointer)buff, 0, sizeof(buff));
                
                RemoteRequestInfo info = GetInfo();
                
                Int32 progress = 0;

                if (info.bytesTotal > 0) {
                    progress = (info.bytesReceived / info.bytesTotal) * 100;
                }
                
                Int32 size = sprintf(buff, "remote_request{%d}:" GNINE_MEMORY_POINTER_ADDRESS_STRING, progress, (int)m_pRemoteRequestId);

                return String(buff, size);
            }

            operator bool() const {
                return m_pRemoteRequestId == NULL ? false : true;
            }

            static RemoteRequestList GetRemoteRequests(Resource resource = NULL, UInt32 maxItemCount = 500) {
                RemoteRequestList list;

                RemoteRequestId remotes[maxItemCount];
                memset((MemoryPointer)remotes, 0, sizeof(remotes));

                UInt32 count = gnine_get_remote_requests(resource, remotes, maxItemCount);

                list.resize(count);

                for (UInt32 i = 0; i < count; i++) {
                    list[i] = remotes[i];
                }

                return list;
            }
        private:
            RemoteRequestId      m_pRemoteRequestId;
            RemoteRequestOptions m_mpOptions;
    };

    inline SharedMemoryPointer GetSharedPointerAddress(MemoryPointer ptr) {
        return gnine_get_shared_pointer_address(ptr);
    }

    inline MemoryPointer GetPointerFromSharedPointer(SharedMemoryPointer sharedPtr) {
        return gnine_get_pointer_from_shared_pointer(sharedPtr);
    }

    inline bool DoesSharedPointerAddressBelongToThisModule(SharedMemoryPointer sharedPtr) {
        return gnine_does_shared_pointer_address_belong_to_this_module(sharedPtr);
    }

    inline bool DoesSharedPointerAddressBelongToThisResource(SharedMemoryPointer sharedPtr) {
        return gnine_does_shared_pointer_address_belong_to_this_resource(sharedPtr);
    }

    inline UIntPtr ReadSharedPointerAddress(SharedMemoryPointer sharedPtr, Byte* data, UIntPtr size) {
        return gnine_read_shared_pointer_address(sharedPtr, data, size);
    }

    template<typename Type>
    inline UIntPtr ReadSharedPointerAddress(SharedMemoryPointer sharedPtr, Type* data) {
        return gnine_read_shared_pointer_address(sharedPtr, (Byte*)data, sizeof(Type));
    }

    inline UIntPtr WriteSharedPointerAddress(SharedMemoryPointer sharedPtr, Byte* data, UIntPtr size) {
        return gnine_write_shared_pointer_address(sharedPtr, data, size);
    }

    template<typename Type>
    inline UIntPtr WriteSharedPointerAddress(SharedMemoryPointer sharedPtr, Type* data) {
        return gnine_write_shared_pointer_address(sharedPtr, (Byte*)data, sizeof(Type));
    }

    inline UIntPtr GetMaxSharedPointerSize(SharedMemoryPointer sharedPtr) {
        return gnine_get_max_shared_pointer_size(sharedPtr);
    }
    
    inline UIntPtr GetTotalMemorySize() {
        MemoryPointer base = (MemoryPointer)1;

        return GetMaxSharedPointerSize(GetSharedPointerAddress(base) - 1);
    }

    inline UIntPtr GetMemoryFreeSpaceSize() {
        return gnine_get_memory_free_space();
    }

    inline UInt64 GetTickCount() {
        return gnine_get_tick_count();
    }

    inline RealTimeData GetRealTime(Int64 seconds = -1, bool localTime = true) {
        RealTimeData data;
        memset((MemoryPointer)&data, 0, sizeof(data));

        gnine_get_real_time(&data, seconds, localTime);

        return data;
    }

    inline String GetUserdataType(Userdata userdata) {
        Int8 type[100];
        memset((MemoryPointer)type, 0, sizeof(type));

        UIntPtr size = gnine_get_userdata_type(userdata, type, sizeof(type));

        return String(type, size);
    }

    inline Color GetColorFromString(String colorString) {
        Color cData;
        memset((MemoryPointer)&cData, 0, sizeof(cData));

        if (colorString.empty()) {
            return cData;
        }

        gnine_get_color_from_string(colorString.c_str(), &cData);

        return cData;
    }
    
    inline Float32 GetDistanceBetweenPoints2D(Vector2 point_1, Vector2 point_2) {
        GNINE_VECTOR2 p1 = point_1;
        GNINE_VECTOR2 p2 = point_2;

        return gnine_get_distance_between_points_2D(&p1, &p2);
    }

    inline Float32 GetDistanceBetweenPoints3D(Vector3 point_1, Vector3 point_2) {
        GNINE_VECTOR3 p1 = point_1;
        GNINE_VECTOR3 p2 = point_2;

        return gnine_get_distance_between_points_3D(&p1, &p2);
    }

    inline Float32 GetEasingValue(Float32 progress, String easingType, Float32 easingPeriod = 0.3f, Float32 easingAmplitude = 1.0f, Float32 easingOvershoot = 1.70158f) {
        return gnine_get_easing_value(progress, easingType.c_str(), easingPeriod, easingAmplitude, easingOvershoot);
    }

    inline Vector3 InterpolateBetween(Vector3 point_1, Vector3 point_2, Float32 progress, String easingType, Float32 easingPeriod = 0.3f, Float32 easingAmplitude = 1.0f, Float32 easingOvershoot = 1.70158f) {
        GNINE_VECTOR3 p1 = point_1;
        GNINE_VECTOR3 p2 = point_2;

        GNINE_VECTOR3 result;
        memset((MemoryPointer)&result, 0, sizeof(result));

        gnine_interpolate_between(&p1, &p2, &result, progress, easingType.c_str(), easingPeriod, easingAmplitude, easingOvershoot);

        return result;
    }

    inline bool DebugSleep(Int32 timeMs) {
        return gnine_debug_sleep(timeMs);
    }

    inline Int32 ToColor(Int32 r, Int32 g, Int32 b, Int32 a = 0xff) {
        return gnine_to_color(r, g, b, a);
    }

    inline ProcessMemoryStats GetProcessMemoryStats() {
        GNINE_PROCESS_MEMORY_STATS stats;
        memset((MemoryPointer)&stats, 0, sizeof(stats));

        gnine_get_process_memory_stats(&stats);

        return stats;
    }

    inline bool IsVoiceEnabled() {
        return gnine_is_voice_enabled();
    }

    template<typename ...Args>
    inline void Printf(String format, Args... args) {
        gnine_print(format.c_str(), args...);
    }
};

namespace STD_NAMESPACE {
    inline string to_string(const GNINE_NAMESPACE::MemoryPointer& ptr) {
        using namespace GNINE_NAMESPACE;

        char buffer[50];
        memset((MemoryPointer)buffer, 0, sizeof(buffer));

        Int32 size = sprintf(buffer, "memory:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (Int32)ptr);

        return string(buffer, size);
    }

    inline string to_string(const GNINE_NAMESPACE::Userdata& userdata) {
        using namespace GNINE_NAMESPACE;

        char buffer[50];
        memset((MemoryPointer)buffer, 0, sizeof(buffer));

        Int32 size = sprintf(buffer, "userdata:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (Int32)userdata);

        return string(buffer, size);
    }

    inline string to_string(const bool& boolean) {
        using namespace GNINE_NAMESPACE;

        return boolean ? "true" : "false";
    }

    inline string to_string(const string& str) {
        if (str.empty()) {
            return str;
        }

        using namespace GNINE_NAMESPACE;

        char buffer[str.size() + 10];
        memset((MemoryPointer)buffer, 0, sizeof(buffer));

        Int32 size = sprintf(buffer, "\"%s\"", str.c_str());

        return string(buffer, size);
    }

    template<typename Arg>
    inline string to_string(const Arg& arg) {
        return (string)arg;
    }
}

namespace GNINE_NAMESPACE {
    inline String __OUTPUT_DATA__ = String();

    template<typename Arg>
    inline void Print(const Arg& arg) {
        __OUTPUT_DATA__ += STD_NAMESPACE::to_string(arg);
        __OUTPUT_DATA__ += "    ";

        Printf(__OUTPUT_DATA__.c_str());

        __OUTPUT_DATA__ = String();
    }

    template<typename Arg, typename ...Args>
    inline void Print(const Arg& arg, Args... args) {
        __OUTPUT_DATA__ += STD_NAMESPACE::to_string(arg);
        __OUTPUT_DATA__ += "    ";

        Print(args...);
    }

    inline Callable::Arguments* __LUA_CODE_ARGS__ = NULL;
    
    template<typename Arg>
    inline void __ADD_LUA_CODE_ARG__(Arg arg) {
        Callable::AddCallableArgumentToArguments(__LUA_CODE_ARGS__, (Callable::Argument)arg);
    }
    
    template<typename Arg, typename... Args>
    inline void __ADD_LUA_CODE_ARG__(Arg arg, Args... args) {
        Callable::AddCallableArgumentToArguments(__LUA_CODE_ARGS__, (Callable::Argument)arg);

        __ADD_LUA_CODE_ARG__(args...);
    }
    
    inline Callable::Arguments ExecuteLuaCode(GNINE_STRING code) {
        GNINE_ARGUMENTS results = NULL;

        __gnine_execute_lua_code(code, NULL, &results, NULL, 0);

        return results;
    }

    template<typename... Args>
    inline Callable::Arguments ExecuteLuaCode(GNINE_STRING code, Args... args) {
        GNINE_ARGUMENTS results = NULL;

        if (!__LUA_CODE_ARGS__) {
            __LUA_CODE_ARGS__ = new Callable::Arguments();
        }

        __ADD_LUA_CODE_ARG__(args...);

        __gnine_execute_lua_code(code, *__LUA_CODE_ARGS__, &results, NULL, 0);

        delete __LUA_CODE_ARGS__;
        __LUA_CODE_ARGS__ = NULL;

        return results;
    }
    
    inline void ExecuteLuaCodeVoid(GNINE_STRING code) {
        __gnine_execute_lua_code(code, NULL, NULL, NULL, 0);
    }

    template<typename... Args>
    inline void ExecuteLuaCodeVoid(GNINE_STRING code, Args... args) {
        if (!__LUA_CODE_ARGS__) {
            __LUA_CODE_ARGS__ = new Callable::Arguments();
        }

        __ADD_LUA_CODE_ARG__(args...);

        __gnine_execute_lua_code(code, *__LUA_CODE_ARGS__, NULL, NULL, 0);

        delete __LUA_CODE_ARGS__;
        __LUA_CODE_ARGS__ = NULL;
    }
}

#define GNINE_LUA(code, ...) GNINE_NAMESPACE::ExecuteLuaCode(#code, __VA_ARGS__)
#define GNINE_LUA_VOID(code, ...) GNINE_NAMESPACE::ExecuteLuaCodeVoid(#code, __VA_ARGS__)
#define GNINE_LUA_NO_ARGS(code) GNINE_NAMESPACE::ExecuteLuaCode(#code)
#define GNINE_LUA_VOID_NO_ARGS(code) GNINE_NAMESPACE::ExecuteLuaCodeVoid(#code)

#endif

#endif
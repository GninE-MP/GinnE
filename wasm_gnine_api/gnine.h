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

#endif

#ifndef NULL
#define NULL (0)
#endif

#define GNINE_NULL 0
#define GNINE_NULL_PTR nullptr

#define GNINE_IMPORT(name, args, ret) __attribute__((import_module("env"), import_name(#name))) GNINE_API ret name args
#define GNINE_API_IMPORT(name, args, ret) GNINE_IMPORT(gnine_##name, args, ret)

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

typedef GNINE_USERDATA_T* GNINE_USERDATA;

typedef GNINE_USERDATA GNINE_ELEMENT;
typedef GNINE_USERDATA GNINE_RESOURCE;

typedef GNINE_ELEMENT GNINE_COL_SHAPE;
typedef GNINE_ELEMENT GNINE_MARKER;

typedef GNINE_ELEMENT GNINE_PLAYER;

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

/*
    API FUNCTION: gnine_print
    PARAMS:
        1- format: string format which guides arguments
        2- ...: arguments to pass in format
    USAGE EXAMPLE:
        gnine_print("this is a number : %d", 3193);
*/
GNINE_API GNINE_INLINE void gnine_print(GNINE_STRING format, ...) {
    GNINE_I8 buffer[0xfff];

    va_list args;
    va_start(args, format);

    vsprintf(buffer, format, args);

    va_end(args);

    gnine_print_data(buffer);
}

#ifdef GNINE_CPP

#include <exception>

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

    using LuaNumber = GNINE_LUA_NUMBER;

    using UserdataT = GNINE_USERDATA_T;
    using Userdata = GNINE_USERDATA;

    using ElementId = GNINE_ELEMENT;
    using ResourceId = GNINE_RESOURCE;
    using ColShapeId = GNINE_COL_SHAPE;
    using MarkerId = GNINE_MARKER;
    using PlayerId = GNINE_PLAYER;

    using RealTimeData = GNINE_REAL_TIME;

    using Color = GNINE_COLOR;

    using ProcessMemoryStats = GNINE_PROCESS_MEMORY_STATS;

    static constexpr Float32 FLOAT_EPSILON = 0.0001f;

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
                memset((MemoryPointer)m_rFunctionRef, 0, sizeof(GNINE_CALLABLE_REF));
            }
            
            Callable(GNINE_CALLABLE_REF ref) {
                memcpy((MemoryPointer)m_rFunctionRef, (MemoryPointer)ref, sizeof(GNINE_CALLABLE_REF));
            }

            template<typename Function>
            Callable(Function function) {
                gnine_c_function_to_callable((MemoryPointer)function, m_rFunctionRef);
            }

            Callable& operator=(GNINE_CALLABLE_REF ref) {
                memcpy((MemoryPointer)m_rFunctionRef, (MemoryPointer)ref, sizeof(GNINE_CALLABLE_REF));

                return *this;
            }

            Callable& operator=(Callable callback) {
                memcpy((MemoryPointer)m_rFunctionRef, (MemoryPointer)callback.m_rFunctionRef, sizeof(GNINE_CALLABLE_REF));

                return *this;
            }

            template<typename Function>
            Callable& operator=(Function function) {
                gnine_c_function_to_callable(function, m_rFunctionRef);

                return *this;
            }

            bool operator==(Callable target) const {
                return strncmp((GNINE_STRING)m_rFunctionRef, (GNINE_STRING)target.m_rFunctionRef, sizeof(m_rFunctionRef)) == 0;
            }

            String GetDeclaration(String functionName = "callable") const {
                char dec[0x3e8];
                IntPtr size = gnine_get_callable_declaration((GNINE_UI8*)m_rFunctionRef, functionName.c_str(), dec, sizeof(dec));

                return String(dec, size);
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
                return m_rFunctionRef[0] == 0 ? false : true;
            }

            operator String() const {
                return GetDeclaration();
            }

            private:
                Arguments* m_FunctionArguments;

                void AddFunctionArgument(Argument arg) {
                    if (!m_FunctionArguments) {
                        m_FunctionArguments = new Arguments();
                    }

                    if (STD_NAMESPACE::holds_alternative<nullptr_t>(arg)) {
                        m_FunctionArguments->Push();
                    }else if (STD_NAMESPACE::holds_alternative<bool>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<bool>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<LuaNumber>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<LuaNumber>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<CString>(arg)) {
                        m_FunctionArguments->Push(String(STD_NAMESPACE::get<CString>(arg)));
                    }else if (STD_NAMESPACE::holds_alternative<String>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<String>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<Callable>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<Callable>(arg).m_rFunctionRef);
                    }else if (STD_NAMESPACE::holds_alternative<Userdata>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<Userdata>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<Arguments*>(arg)) {
                        m_FunctionArguments->Push(*STD_NAMESPACE::get<Arguments*>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<Int32>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<Int32>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<Int64>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<Int64>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<UInt32>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<UInt32>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<UInt64>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<UInt64>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<Float32>(arg)) {
                        m_FunctionArguments->Push(STD_NAMESPACE::get<Float32>(arg));
                    }else if (STD_NAMESPACE::holds_alternative<MemoryPointer>(arg)) {
                        m_FunctionArguments->Push((LuaNumber)(IntPtr)STD_NAMESPACE::get<MemoryPointer>(arg));
                    }
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
                vec.z = fY;

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

    using ElementList = STD_NAMESPACE::vector<Element>;
    using ElementDataKeyList = STD_NAMESPACE::vector<String>;

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
                
                Int32 size = sprintf(buff, "element:%08X", (int)m_pElementId);

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

    inline UIntPtr WriteSharedPointerAddress(SharedMemoryPointer sharedPtr, Byte* data, UIntPtr size) {
        return gnine_write_shared_pointer_address(sharedPtr, data, size);
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

    template<typename ...Args>
    inline void Printf(String format, Args... args) {
        gnine_print(format.c_str(), args...);
    }
};

namespace STD_NAMESPACE {
    inline string to_string(GNINE_NAMESPACE::MemoryPointer ptr) {
        using namespace GNINE_NAMESPACE;

        char buffer[50];
        memset((MemoryPointer)buffer, 0, sizeof(buffer));

        Int32 size = sprintf(buffer, "memory:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (Int32)ptr);

        return string(buffer, size);
    }

    inline string to_string(GNINE_NAMESPACE::Userdata userdata) {
        using namespace GNINE_NAMESPACE;

        char buffer[50];
        memset((MemoryPointer)buffer, 0, sizeof(buffer));

        Int32 size = sprintf(buffer, "userdata:" GNINE_MEMORY_POINTER_ADDRESS_STRING, (Int32)userdata);

        return string(buffer, size);
    }

    inline string to_string(bool boolean) {
        using namespace GNINE_NAMESPACE;

        return boolean ? "true" : "false";
    }

    inline string to_string(string str) {
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
    inline string to_string(Arg arg) {
        return (string)arg;
    }
}

namespace GNINE_NAMESPACE {
    inline String __OUTPUT_DATA__ = String();

    template<typename Arg>
    inline void Print(Arg arg) {
        __OUTPUT_DATA__ += STD_NAMESPACE::to_string(arg);
        __OUTPUT_DATA__ += "    ";

        Printf(__OUTPUT_DATA__.c_str());

        __OUTPUT_DATA__ = String();
    }

    template<typename Arg, typename ...Args>
    inline void Print(Arg arg, Args... args) {
        __OUTPUT_DATA__ += STD_NAMESPACE::to_string(arg);
        __OUTPUT_DATA__ += "    ";

        Print(args...);
    }
}

#endif

#endif
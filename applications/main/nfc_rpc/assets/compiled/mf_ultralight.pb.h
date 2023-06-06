/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.8-dev */

#ifndef PB_PB_MFULTRALIGHT_MF_ULTRALIGHT_PB_H_INCLUDED
#define PB_PB_MFULTRALIGHT_MF_ULTRALIGHT_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _PB_MfUltralight_Error {
    PB_MfUltralight_Error_None = 0,
    PB_MfUltralight_Error_NotPresent = 1,
    PB_MfUltralight_Error_Protocol = 2,
    PB_MfUltralight_Error_Auth = 3,
    PB_MfUltralight_Error_Timeout = 4
} PB_MfUltralight_Error;

/* Struct definitions */
typedef struct _PB_MfUltralight_ReadPageRequest {
    uint16_t page;
} PB_MfUltralight_ReadPageRequest;

typedef PB_BYTES_ARRAY_T(4) PB_MfUltralight_ReadPageResponse_data_t;
typedef struct _PB_MfUltralight_ReadPageResponse {
    PB_MfUltralight_Error error;
    uint16_t page;
    PB_MfUltralight_ReadPageResponse_data_t data;
} PB_MfUltralight_ReadPageResponse;

typedef struct _PB_MfUltralight_ReadVersionRequest {
    char dummy_field;
} PB_MfUltralight_ReadVersionRequest;

typedef struct _PB_MfUltralight_ReadVersionResponse {
    PB_MfUltralight_Error error;
    uint32_t header;
    uint32_t vendor_id;
    uint32_t prod_type;
    uint32_t prod_subtype;
    uint32_t prod_ver_major;
    uint32_t prod_ver_minor;
    uint32_t storage_size;
    uint32_t protocol_type;
} PB_MfUltralight_ReadVersionResponse;

typedef PB_BYTES_ARRAY_T(4) PB_MfUltralight_WritePageRequest_data_t;
typedef struct _PB_MfUltralight_WritePageRequest {
    uint16_t page;
    PB_MfUltralight_WritePageRequest_data_t data;
} PB_MfUltralight_WritePageRequest;

typedef struct _PB_MfUltralight_WritePageResponse {
    PB_MfUltralight_Error error;
    bool result;
} PB_MfUltralight_WritePageResponse;

typedef struct _PB_MfUltralight_ReadSignatureRequest {
    char dummy_field;
} PB_MfUltralight_ReadSignatureRequest;

typedef PB_BYTES_ARRAY_T(32) PB_MfUltralight_ReadSignatureResponse_data_t;
typedef struct _PB_MfUltralight_ReadSignatureResponse {
    PB_MfUltralight_Error error;
    PB_MfUltralight_ReadSignatureResponse_data_t data;
} PB_MfUltralight_ReadSignatureResponse;

typedef struct _PB_MfUltralight_ReadCounterRequest {
    uint32_t counter_num;
} PB_MfUltralight_ReadCounterRequest;

typedef PB_BYTES_ARRAY_T(3) PB_MfUltralight_ReadCounterResponse_data_t;
typedef struct _PB_MfUltralight_ReadCounterResponse {
    PB_MfUltralight_Error error;
    uint32_t counter_num;
    PB_MfUltralight_ReadCounterResponse_data_t data;
} PB_MfUltralight_ReadCounterResponse;

typedef struct _PB_MfUltralight_ReadTearingFlagRequest {
    uint32_t flag_num;
} PB_MfUltralight_ReadTearingFlagRequest;

typedef PB_BYTES_ARRAY_T(1) PB_MfUltralight_ReadTearingFlagResponse_data_t;
typedef struct _PB_MfUltralight_ReadTearingFlagResponse {
    PB_MfUltralight_Error error;
    uint32_t flag_num;
    PB_MfUltralight_ReadTearingFlagResponse_data_t data;
} PB_MfUltralight_ReadTearingFlagResponse;

typedef PB_BYTES_ARRAY_T(2048) PB_MfUltralight_EmulateStartRequest_data_t;
typedef struct _PB_MfUltralight_EmulateStartRequest {
    PB_MfUltralight_EmulateStartRequest_data_t data;
} PB_MfUltralight_EmulateStartRequest;

typedef struct _PB_MfUltralight_EmulateStartResponse {
    PB_MfUltralight_Error error;
} PB_MfUltralight_EmulateStartResponse;

typedef struct _PB_MfUltralight_EmulateStopRequest {
    char dummy_field;
} PB_MfUltralight_EmulateStopRequest;

typedef struct _PB_MfUltralight_EmulateStopResponse {
    PB_MfUltralight_Error error;
} PB_MfUltralight_EmulateStopResponse;

#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _PB_MfUltralight_Error_MIN PB_MfUltralight_Error_None
#define _PB_MfUltralight_Error_MAX PB_MfUltralight_Error_Timeout
#define _PB_MfUltralight_Error_ARRAYSIZE \
    ((PB_MfUltralight_Error)(PB_MfUltralight_Error_Timeout + 1))

#define PB_MfUltralight_ReadPageResponse_error_ENUMTYPE PB_MfUltralight_Error

#define PB_MfUltralight_ReadVersionResponse_error_ENUMTYPE PB_MfUltralight_Error

#define PB_MfUltralight_WritePageResponse_error_ENUMTYPE PB_MfUltralight_Error

#define PB_MfUltralight_ReadSignatureResponse_error_ENUMTYPE PB_MfUltralight_Error

#define PB_MfUltralight_ReadCounterResponse_error_ENUMTYPE PB_MfUltralight_Error

#define PB_MfUltralight_ReadTearingFlagResponse_error_ENUMTYPE PB_MfUltralight_Error

#define PB_MfUltralight_EmulateStartResponse_error_ENUMTYPE PB_MfUltralight_Error

#define PB_MfUltralight_EmulateStopResponse_error_ENUMTYPE PB_MfUltralight_Error

/* Initializer values for message structs */
#define PB_MfUltralight_ReadPageRequest_init_default \
    { 0 }
#define PB_MfUltralight_ReadPageResponse_init_default \
    {                                                 \
        _PB_MfUltralight_Error_MIN, 0, {              \
            0, {                                      \
                0                                     \
            }                                         \
        }                                             \
    }
#define PB_MfUltralight_ReadVersionRequest_init_default \
    { 0 }
#define PB_MfUltralight_ReadVersionResponse_init_default \
    { _PB_MfUltralight_Error_MIN, 0, 0, 0, 0, 0, 0, 0, 0 }
#define PB_MfUltralight_WritePageRequest_init_default \
    {                                                 \
        0, {                                          \
            0, {                                      \
                0                                     \
            }                                         \
        }                                             \
    }
#define PB_MfUltralight_WritePageResponse_init_default \
    { _PB_MfUltralight_Error_MIN, 0 }
#define PB_MfUltralight_ReadSignatureRequest_init_default \
    { 0 }
#define PB_MfUltralight_ReadSignatureResponse_init_default \
    {                                                      \
        _PB_MfUltralight_Error_MIN, {                      \
            0, {                                           \
                0                                          \
            }                                              \
        }                                                  \
    }
#define PB_MfUltralight_ReadCounterRequest_init_default \
    { 0 }
#define PB_MfUltralight_ReadCounterResponse_init_default \
    {                                                    \
        _PB_MfUltralight_Error_MIN, 0, {                 \
            0, {                                         \
                0                                        \
            }                                            \
        }                                                \
    }
#define PB_MfUltralight_ReadTearingFlagRequest_init_default \
    { 0 }
#define PB_MfUltralight_ReadTearingFlagResponse_init_default \
    {                                                        \
        _PB_MfUltralight_Error_MIN, 0, {                     \
            0, {                                             \
                0                                            \
            }                                                \
        }                                                    \
    }
#define PB_MfUltralight_EmulateStartRequest_init_default \
    {                                                    \
        {                                                \
            0, {                                         \
                0                                        \
            }                                            \
        }                                                \
    }
#define PB_MfUltralight_EmulateStartResponse_init_default \
    { _PB_MfUltralight_Error_MIN }
#define PB_MfUltralight_EmulateStopRequest_init_default \
    { 0 }
#define PB_MfUltralight_EmulateStopResponse_init_default \
    { _PB_MfUltralight_Error_MIN }
#define PB_MfUltralight_ReadPageRequest_init_zero \
    { 0 }
#define PB_MfUltralight_ReadPageResponse_init_zero \
    {                                              \
        _PB_MfUltralight_Error_MIN, 0, {           \
            0, {                                   \
                0                                  \
            }                                      \
        }                                          \
    }
#define PB_MfUltralight_ReadVersionRequest_init_zero \
    { 0 }
#define PB_MfUltralight_ReadVersionResponse_init_zero \
    { _PB_MfUltralight_Error_MIN, 0, 0, 0, 0, 0, 0, 0, 0 }
#define PB_MfUltralight_WritePageRequest_init_zero \
    {                                              \
        0, {                                       \
            0, {                                   \
                0                                  \
            }                                      \
        }                                          \
    }
#define PB_MfUltralight_WritePageResponse_init_zero \
    { _PB_MfUltralight_Error_MIN, 0 }
#define PB_MfUltralight_ReadSignatureRequest_init_zero \
    { 0 }
#define PB_MfUltralight_ReadSignatureResponse_init_zero \
    {                                                   \
        _PB_MfUltralight_Error_MIN, {                   \
            0, {                                        \
                0                                       \
            }                                           \
        }                                               \
    }
#define PB_MfUltralight_ReadCounterRequest_init_zero \
    { 0 }
#define PB_MfUltralight_ReadCounterResponse_init_zero \
    {                                                 \
        _PB_MfUltralight_Error_MIN, 0, {              \
            0, {                                      \
                0                                     \
            }                                         \
        }                                             \
    }
#define PB_MfUltralight_ReadTearingFlagRequest_init_zero \
    { 0 }
#define PB_MfUltralight_ReadTearingFlagResponse_init_zero \
    {                                                     \
        _PB_MfUltralight_Error_MIN, 0, {                  \
            0, {                                          \
                0                                         \
            }                                             \
        }                                                 \
    }
#define PB_MfUltralight_EmulateStartRequest_init_zero \
    {                                                 \
        {                                             \
            0, {                                      \
                0                                     \
            }                                         \
        }                                             \
    }
#define PB_MfUltralight_EmulateStartResponse_init_zero \
    { _PB_MfUltralight_Error_MIN }
#define PB_MfUltralight_EmulateStopRequest_init_zero \
    { 0 }
#define PB_MfUltralight_EmulateStopResponse_init_zero \
    { _PB_MfUltralight_Error_MIN }

/* Field tags (for use in manual encoding/decoding) */
#define PB_MfUltralight_ReadPageRequest_page_tag 1
#define PB_MfUltralight_ReadPageResponse_error_tag 1
#define PB_MfUltralight_ReadPageResponse_page_tag 2
#define PB_MfUltralight_ReadPageResponse_data_tag 3
#define PB_MfUltralight_ReadVersionResponse_error_tag 1
#define PB_MfUltralight_ReadVersionResponse_header_tag 2
#define PB_MfUltralight_ReadVersionResponse_vendor_id_tag 3
#define PB_MfUltralight_ReadVersionResponse_prod_type_tag 4
#define PB_MfUltralight_ReadVersionResponse_prod_subtype_tag 5
#define PB_MfUltralight_ReadVersionResponse_prod_ver_major_tag 6
#define PB_MfUltralight_ReadVersionResponse_prod_ver_minor_tag 7
#define PB_MfUltralight_ReadVersionResponse_storage_size_tag 8
#define PB_MfUltralight_ReadVersionResponse_protocol_type_tag 9
#define PB_MfUltralight_WritePageRequest_page_tag 1
#define PB_MfUltralight_WritePageRequest_data_tag 2
#define PB_MfUltralight_WritePageResponse_error_tag 1
#define PB_MfUltralight_WritePageResponse_result_tag 2
#define PB_MfUltralight_ReadSignatureResponse_error_tag 1
#define PB_MfUltralight_ReadSignatureResponse_data_tag 2
#define PB_MfUltralight_ReadCounterRequest_counter_num_tag 1
#define PB_MfUltralight_ReadCounterResponse_error_tag 1
#define PB_MfUltralight_ReadCounterResponse_counter_num_tag 2
#define PB_MfUltralight_ReadCounterResponse_data_tag 3
#define PB_MfUltralight_ReadTearingFlagRequest_flag_num_tag 1
#define PB_MfUltralight_ReadTearingFlagResponse_error_tag 1
#define PB_MfUltralight_ReadTearingFlagResponse_flag_num_tag 2
#define PB_MfUltralight_ReadTearingFlagResponse_data_tag 3
#define PB_MfUltralight_EmulateStartRequest_data_tag 1
#define PB_MfUltralight_EmulateStartResponse_error_tag 1
#define PB_MfUltralight_EmulateStopResponse_error_tag 1

/* Struct field encoding specification for nanopb */
#define PB_MfUltralight_ReadPageRequest_FIELDLIST(X, a) X(a, STATIC, SINGULAR, UINT32, page, 1)
#define PB_MfUltralight_ReadPageRequest_CALLBACK NULL
#define PB_MfUltralight_ReadPageRequest_DEFAULT NULL

#define PB_MfUltralight_ReadPageResponse_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UENUM, error, 1)              \
    X(a, STATIC, SINGULAR, UINT32, page, 2)              \
    X(a, STATIC, SINGULAR, BYTES, data, 3)
#define PB_MfUltralight_ReadPageResponse_CALLBACK NULL
#define PB_MfUltralight_ReadPageResponse_DEFAULT NULL

#define PB_MfUltralight_ReadVersionRequest_FIELDLIST(X, a)

#define PB_MfUltralight_ReadVersionRequest_CALLBACK NULL
#define PB_MfUltralight_ReadVersionRequest_DEFAULT NULL

#define PB_MfUltralight_ReadVersionResponse_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UENUM, error, 1)                 \
    X(a, STATIC, SINGULAR, UINT32, header, 2)               \
    X(a, STATIC, SINGULAR, UINT32, vendor_id, 3)            \
    X(a, STATIC, SINGULAR, UINT32, prod_type, 4)            \
    X(a, STATIC, SINGULAR, UINT32, prod_subtype, 5)         \
    X(a, STATIC, SINGULAR, UINT32, prod_ver_major, 6)       \
    X(a, STATIC, SINGULAR, UINT32, prod_ver_minor, 7)       \
    X(a, STATIC, SINGULAR, UINT32, storage_size, 8)         \
    X(a, STATIC, SINGULAR, UINT32, protocol_type, 9)
#define PB_MfUltralight_ReadVersionResponse_CALLBACK NULL
#define PB_MfUltralight_ReadVersionResponse_DEFAULT NULL

#define PB_MfUltralight_WritePageRequest_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UINT32, page, 1)              \
    X(a, STATIC, SINGULAR, BYTES, data, 2)
#define PB_MfUltralight_WritePageRequest_CALLBACK NULL
#define PB_MfUltralight_WritePageRequest_DEFAULT NULL

#define PB_MfUltralight_WritePageResponse_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UENUM, error, 1)               \
    X(a, STATIC, SINGULAR, BOOL, result, 2)
#define PB_MfUltralight_WritePageResponse_CALLBACK NULL
#define PB_MfUltralight_WritePageResponse_DEFAULT NULL

#define PB_MfUltralight_ReadSignatureRequest_FIELDLIST(X, a)

#define PB_MfUltralight_ReadSignatureRequest_CALLBACK NULL
#define PB_MfUltralight_ReadSignatureRequest_DEFAULT NULL

#define PB_MfUltralight_ReadSignatureResponse_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UENUM, error, 1)                   \
    X(a, STATIC, SINGULAR, BYTES, data, 2)
#define PB_MfUltralight_ReadSignatureResponse_CALLBACK NULL
#define PB_MfUltralight_ReadSignatureResponse_DEFAULT NULL

#define PB_MfUltralight_ReadCounterRequest_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UINT32, counter_num, 1)
#define PB_MfUltralight_ReadCounterRequest_CALLBACK NULL
#define PB_MfUltralight_ReadCounterRequest_DEFAULT NULL

#define PB_MfUltralight_ReadCounterResponse_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UENUM, error, 1)                 \
    X(a, STATIC, SINGULAR, UINT32, counter_num, 2)          \
    X(a, STATIC, SINGULAR, BYTES, data, 3)
#define PB_MfUltralight_ReadCounterResponse_CALLBACK NULL
#define PB_MfUltralight_ReadCounterResponse_DEFAULT NULL

#define PB_MfUltralight_ReadTearingFlagRequest_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UINT32, flag_num, 1)
#define PB_MfUltralight_ReadTearingFlagRequest_CALLBACK NULL
#define PB_MfUltralight_ReadTearingFlagRequest_DEFAULT NULL

#define PB_MfUltralight_ReadTearingFlagResponse_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UENUM, error, 1)                     \
    X(a, STATIC, SINGULAR, UINT32, flag_num, 2)                 \
    X(a, STATIC, SINGULAR, BYTES, data, 3)
#define PB_MfUltralight_ReadTearingFlagResponse_CALLBACK NULL
#define PB_MfUltralight_ReadTearingFlagResponse_DEFAULT NULL

#define PB_MfUltralight_EmulateStartRequest_FIELDLIST(X, a) X(a, STATIC, SINGULAR, BYTES, data, 1)
#define PB_MfUltralight_EmulateStartRequest_CALLBACK NULL
#define PB_MfUltralight_EmulateStartRequest_DEFAULT NULL

#define PB_MfUltralight_EmulateStartResponse_FIELDLIST(X, a) \
    X(a, STATIC, SINGULAR, UENUM, error, 1)
#define PB_MfUltralight_EmulateStartResponse_CALLBACK NULL
#define PB_MfUltralight_EmulateStartResponse_DEFAULT NULL

#define PB_MfUltralight_EmulateStopRequest_FIELDLIST(X, a)

#define PB_MfUltralight_EmulateStopRequest_CALLBACK NULL
#define PB_MfUltralight_EmulateStopRequest_DEFAULT NULL

#define PB_MfUltralight_EmulateStopResponse_FIELDLIST(X, a) X(a, STATIC, SINGULAR, UENUM, error, 1)
#define PB_MfUltralight_EmulateStopResponse_CALLBACK NULL
#define PB_MfUltralight_EmulateStopResponse_DEFAULT NULL

extern const pb_msgdesc_t PB_MfUltralight_ReadPageRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadPageResponse_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadVersionRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadVersionResponse_msg;
extern const pb_msgdesc_t PB_MfUltralight_WritePageRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_WritePageResponse_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadSignatureRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadSignatureResponse_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadCounterRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadCounterResponse_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadTearingFlagRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_ReadTearingFlagResponse_msg;
extern const pb_msgdesc_t PB_MfUltralight_EmulateStartRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_EmulateStartResponse_msg;
extern const pb_msgdesc_t PB_MfUltralight_EmulateStopRequest_msg;
extern const pb_msgdesc_t PB_MfUltralight_EmulateStopResponse_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define PB_MfUltralight_ReadPageRequest_fields &PB_MfUltralight_ReadPageRequest_msg
#define PB_MfUltralight_ReadPageResponse_fields &PB_MfUltralight_ReadPageResponse_msg
#define PB_MfUltralight_ReadVersionRequest_fields &PB_MfUltralight_ReadVersionRequest_msg
#define PB_MfUltralight_ReadVersionResponse_fields &PB_MfUltralight_ReadVersionResponse_msg
#define PB_MfUltralight_WritePageRequest_fields &PB_MfUltralight_WritePageRequest_msg
#define PB_MfUltralight_WritePageResponse_fields &PB_MfUltralight_WritePageResponse_msg
#define PB_MfUltralight_ReadSignatureRequest_fields &PB_MfUltralight_ReadSignatureRequest_msg
#define PB_MfUltralight_ReadSignatureResponse_fields &PB_MfUltralight_ReadSignatureResponse_msg
#define PB_MfUltralight_ReadCounterRequest_fields &PB_MfUltralight_ReadCounterRequest_msg
#define PB_MfUltralight_ReadCounterResponse_fields &PB_MfUltralight_ReadCounterResponse_msg
#define PB_MfUltralight_ReadTearingFlagRequest_fields &PB_MfUltralight_ReadTearingFlagRequest_msg
#define PB_MfUltralight_ReadTearingFlagResponse_fields &PB_MfUltralight_ReadTearingFlagResponse_msg
#define PB_MfUltralight_EmulateStartRequest_fields &PB_MfUltralight_EmulateStartRequest_msg
#define PB_MfUltralight_EmulateStartResponse_fields &PB_MfUltralight_EmulateStartResponse_msg
#define PB_MfUltralight_EmulateStopRequest_fields &PB_MfUltralight_EmulateStopRequest_msg
#define PB_MfUltralight_EmulateStopResponse_fields &PB_MfUltralight_EmulateStopResponse_msg

/* Maximum encoded size of messages (where known) */
#define PB_MfUltralight_EmulateStartRequest_size 2051
#define PB_MfUltralight_EmulateStartResponse_size 2
#define PB_MfUltralight_EmulateStopRequest_size 0
#define PB_MfUltralight_EmulateStopResponse_size 2
#define PB_MfUltralight_ReadCounterRequest_size 6
#define PB_MfUltralight_ReadCounterResponse_size 13
#define PB_MfUltralight_ReadPageRequest_size 4
#define PB_MfUltralight_ReadPageResponse_size 12
#define PB_MfUltralight_ReadSignatureRequest_size 0
#define PB_MfUltralight_ReadSignatureResponse_size 36
#define PB_MfUltralight_ReadTearingFlagRequest_size 6
#define PB_MfUltralight_ReadTearingFlagResponse_size 11
#define PB_MfUltralight_ReadVersionRequest_size 0
#define PB_MfUltralight_ReadVersionResponse_size 50
#define PB_MfUltralight_WritePageRequest_size 10
#define PB_MfUltralight_WritePageResponse_size 4

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

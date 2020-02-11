/** @file sb_sw_context.h
 *  @brief private context structure for short Weierstrass curves
 */

/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * This file is part of Sweet B, a safe, compact, embeddable library for
 * elliptic curve cryptography.
 *
 * https://github.com/westerndigitalcorporation/sweet-b
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SB_SW_CONTEXT_H
#define SB_SW_CONTEXT_H

#include <sb_types.h>
#include <sb_hmac_drbg.h>
#include <sb_hkdf.h>

#ifdef SB_TEST
#define SB_CONTEXT_SIZE_ASSERT(context, size) \
_Static_assert(1, "obvious")
#else
#define SB_CONTEXT_SIZE_ASSERT(context, size) \
_Static_assert(sizeof(context) == size, #context " should be " #size " bytes long.")
#endif

/** @brief Size of the parameter buffer in SB_ELEM_BYTES number of elements.
 *  This value is set as large as it can be in order to keep the size of \ref
 *  sb_sw_context_t to 512 bytes.
 */
#define SB_SW_CONTEXT_PARAM_BUF_ELEMS 4

/** @struct sb_sw_context_params_t
 *  @brief Private context structure representing possibly-generated parameters.
 */
typedef struct sb_sw_context_params_t {
    sb_fe_t k, z;
} sb_sw_context_params_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_params_t, 64);

/** @struct sb_sw_context_param_gen_t
 *  @brief Private context structure for parameter generation.
 */
typedef struct sb_sw_context_param_gen_t {
    // At most one DRBG or HKDF state instance. Once Z candidates have
    // been generated in buf, the DRBG or HKDF instance is no longer
    // used, and the space may be reused for validity testing of Z
    // candidates.
    union {
        sb_hmac_drbg_state_t drbg;
        sb_hkdf_state_t hkdf;
        sb_fe_t z2; // Candidate Z value during Z generation
    };
    sb_byte_t buf[SB_SW_CONTEXT_PARAM_BUF_ELEMS * SB_ELEM_BYTES];
} sb_sw_context_param_gen_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_param_gen_t, 424);

/** @struct sb_sw_context_curve_arith_t
 *  @brief Private context structure for curve arithmetic operations.
 */
typedef struct sb_sw_context_curve_arith_t {
    sb_fe_pair_t p1, p2;
} sb_sw_context_curve_arith_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_curve_arith_t, 128);

/** @struct sb_sw_context_curve_arith_temporaries_t
 *  @brief Temporaries for curve arithmetic methods.
 */
typedef struct sb_sw_context_curve_arith_temporaries_t {
    sb_fe_t t5, t6, t7, t8;
} sb_sw_context_curve_arith_temporaries_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_curve_arith_temporaries_t, 128);

/** @struct sb_sw_context_sign_t
 *  @brief Private context structure for signing operations.
 */
typedef struct sb_sw_context_sign_t {
    sb_fe_t message;
    sb_fe_t priv;
} sb_sw_context_sign_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_sign_t, 64);

/** @struct sb_sw_context_mult_t
 *  @brief Private context structure for point-scalar multiplication operations.
 */
typedef struct sb_sw_context_mult_t {
    sb_fe_pair_t point;
} sb_sw_context_mult_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_mult_t, 64);

/** @struct sb_sw_context_verify_common_t
 *  @brief Private context structure common to all verification phases.
 */
typedef struct sb_sw_context_verify_common_t {
    sb_fe_t qr;
} sb_sw_context_verify_common_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_verify_common_t, 32);

/** @struct sb_sw_context_verify_early_t
 *  @brief Private context structure used early in signature verification.
 */
typedef struct sb_sw_context_verify_early_t {
    sb_fe_t message, qs;
} sb_sw_context_verify_early_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_verify_early_t, 64);

/** @struct sb_sw_context_verify_early_t
 *  @brief Private context structure used late in signature verification.
 */
typedef struct sb_sw_context_verify_late_t {
    sb_fe_t kg;
    sb_fe_pair_t pg;
} sb_sw_context_verify_late_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_verify_late_t, 96);

/** @struct sb_sw_context_verify_t
 *  @brief Private context structure used during signature verification.
 */
typedef struct sb_sw_context_verify_t {
    sb_sw_context_verify_common_t common;
    union {
        sb_sw_context_verify_early_t early;
        sb_sw_context_verify_late_t late;
    };
} sb_sw_context_verify_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_verify_t, 128);

typedef enum sb_sw_incremental_operation_value_t {
    SB_SW_INCREMENTAL_OPERATION_NONE = 0,
    SB_SW_INCREMENTAL_OPERATION_COMPUTE_PUBLIC_KEY,
    SB_SW_INCREMENTAL_OPERATION_SHARED_SECRET,
    SB_SW_INCREMENTAL_OPERATION_POINT_MULTIPLY,
    SB_SW_INCREMENTAL_OPERATION_SIGN_MESSAGE_DIGEST,
    SB_SW_INCREMENTAL_OPERATION_VERIFY_SIGNATURE
} sb_sw_incremental_operation_value_t;

typedef uint32_t sb_sw_incremental_operation_t;

typedef enum sb_sw_point_mult_op_stage_t {
    SB_SW_POINT_MULT_OP_STAGE_LADDER = 0,
    SB_SW_POINT_MULT_OP_STAGE_INV_Z,
    SB_SW_POINT_MULT_OP_DONE
} sb_sw_point_mult_op_stage_t;

typedef enum sb_sw_sign_op_stage_t {
    SB_SW_SIGN_OP_STAGE_INV = SB_SW_POINT_MULT_OP_DONE,
    SB_SW_SIGN_OP_STAGE_DONE
} sb_sw_sign_op_stage_t;

typedef enum sb_sw_verify_op_stage_t {
    SB_SW_VERIFY_OP_STAGE_INV_S = 0,
    SB_SW_VERIFY_OP_STAGE_INV_Z,
    SB_SW_VERIFY_OP_STAGE_LADDER,
    SB_SW_VERIFY_OP_STAGE_TEST,
    SB_SW_VERIFY_OP_DONE
} sb_sw_verify_op_stage_t;

typedef uint32_t sb_sw_op_stage_t;

typedef struct sb_sw_context_saved_state_t {
    sb_sw_incremental_operation_t operation;
    const struct sb_sw_curve_t* curve;
    sb_sw_op_stage_t stage;
    size_t i;
    union {
        struct {
            sb_word_t inv_k, k_one, swap;
        };
        struct {
            sb_word_t res;
        };
    };
} sb_sw_context_saved_state_t;

// There is no size assertion for sb_sw_context_saved_state_t as it contains
// native pointers, which may be of different widths on different platforms.

/** @struct sb_sw_context_param_use_t
 *  @brief Private context structure used during all curve operations.
 */
typedef struct sb_sw_context_param_use_t {
    // Stores the two point registers used in the Montgomery ladder and in
    // the dual scalar-point multiplication-addition used for verification.
    sb_sw_context_curve_arith_t curve_arith;

    // Stores the point to be multiplied against in ECDH (or the generator
    // during message signing and public key verification), or the public key
    // during signature verification.
    sb_sw_context_mult_t mult;

    union {
        // State saved across incremental operations.
        sb_sw_context_saved_state_t saved_state;

        // Basic temporaries used during all curve arithmetic.
        sb_sw_context_curve_arith_temporaries_t curve_temporaries;
    };

    union {
        // Temporaries used during message signing.
        sb_sw_context_sign_t sign;

        // Temporaries used during signature verification.
        sb_sw_context_verify_t verify;
    };
} sb_sw_context_param_use_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_param_use_t, 448);

/** @struct sb_sw_context_t
 *  @brief Private context structure for short Weierstrass curves. You are
 *  responsible for allocating this and passing it to curve functions. */
typedef struct sb_sw_context_t {
    // Possibly-generated parameters.
    sb_sw_context_params_t params;

    union {
        // State used during parameter generation.
        sb_sw_context_param_gen_t param_gen;

        // Parameter use during curve arithmetic.
        sb_sw_context_param_use_t param_use;
    };
} sb_sw_context_t;

SB_CONTEXT_SIZE_ASSERT(sb_sw_context_t, 512);

#endif

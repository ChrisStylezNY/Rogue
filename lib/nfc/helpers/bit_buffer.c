#include "bit_buffer.h"

#include <furi.h>

#define BITS_IN_BYTE (8)

struct BitBuffer {
    uint8_t* data;
    uint8_t* parity;
    size_t capacity_bytes;
    size_t size_bits;
};

/* TODO:
 * - Handle partial bytes
 * - Calculate parity
 * - Something else?
 */

BitBuffer* bit_buffer_alloc(size_t capacity_bytes) {
    furi_assert(capacity_bytes);

    BitBuffer* buf = malloc(sizeof(BitBuffer));

    buf->data = malloc(capacity_bytes);
    buf->parity = malloc((capacity_bytes + BITS_IN_BYTE - 1) / BITS_IN_BYTE);
    buf->capacity_bytes = capacity_bytes;
    buf->size_bits = 0;

    return buf;
}

void bit_buffer_free(BitBuffer* buf) {
    furi_assert(buf);

    free(buf->data);
    free(buf->parity);
    free(buf);
}

void bit_buffer_reset(BitBuffer* buf) {
    furi_assert(buf);

    buf->size_bits = 0;
}

void bit_buffer_copy(BitBuffer* buf, const BitBuffer* other) {
    furi_assert(buf);
    furi_assert(other);
    furi_assert(buf->capacity_bytes * BITS_IN_BYTE >= other->size_bits);

    memcpy(buf->data, other->data, bit_buffer_get_size_bytes(other));
    buf->size_bits = other->size_bits;
}

void bit_buffer_copy_right(BitBuffer* buf, const BitBuffer* other, size_t start_index) {
    furi_assert(buf);
    furi_assert(other);
    furi_assert(bit_buffer_get_size_bytes(other) > start_index);
    furi_assert(buf->capacity_bytes >= bit_buffer_get_size_bytes(other) - start_index);

    memcpy(buf->data, other->data + start_index, bit_buffer_get_size_bytes(other) - start_index);
    buf->size_bits = other->size_bits - start_index * BITS_IN_BYTE;
}

void bit_buffer_copy_bytes(BitBuffer* buf, const uint8_t* data, size_t size_bytes) {
    furi_assert(buf);
    furi_assert(buf->capacity_bytes >= size_bytes);

    memcpy(buf->data, data, size_bytes);
    buf->size_bits = size_bytes * BITS_IN_BYTE;
}

void bit_buffer_write_bytes(const BitBuffer* buf, void* dest, size_t size_bytes) {
    furi_assert(buf);
    furi_assert(dest);
    furi_assert(bit_buffer_get_size_bytes(buf) >= size_bytes);

    memcpy(dest, buf->data, size_bytes);
}

bool bit_buffer_has_partial_byte(const BitBuffer* buf) {
    furi_assert(buf);

    return (buf->size_bits % BITS_IN_BYTE) != 0;
}

bool bit_buffer_starts_with_byte(const BitBuffer* buf, uint8_t byte) {
    furi_assert(buf);

    return bit_buffer_get_size_bytes(buf) && (buf->data[0] == byte);
}

size_t bit_buffer_get_capacity_bytes(const BitBuffer* buf) {
    furi_assert(buf);

    return buf->capacity_bytes;
}

size_t bit_buffer_get_size(const BitBuffer* buf) {
    furi_assert(buf);

    return buf->size_bits;
}

size_t bit_buffer_get_size_bytes(const BitBuffer* buf) {
    furi_assert(buf);

    return (buf->size_bits / BITS_IN_BYTE) + (buf->size_bits % BITS_IN_BYTE ? 1 : 0);
}

uint8_t bit_buffer_get_byte(const BitBuffer* buf, size_t index) {
    furi_assert(buf);
    furi_assert(buf->capacity_bytes > index);

    return buf->data[index];
}

uint8_t* bit_buffer_get_data(const BitBuffer* buf) {
    furi_assert(buf);

    return buf->data;
}

void bit_buffer_set_byte(BitBuffer* buf, size_t index, uint8_t byte) {
    furi_assert(buf);
    furi_assert(buf->size_bits / BITS_IN_BYTE > index);

    buf->data[index] = byte;
}

void bit_buffer_append(BitBuffer* buf, const BitBuffer* other) {
    bit_buffer_append_right(buf, other, 0);
}

void bit_buffer_append_right(BitBuffer* buf, const BitBuffer* other, size_t start_index) {
    furi_assert(buf);
    furi_assert(other);

    const size_t size_bytes = bit_buffer_get_size_bytes(buf);
    const size_t other_size_bytes = bit_buffer_get_size_bytes(other) - start_index;

    furi_assert(buf->capacity_bytes >= size_bytes + other_size_bytes);

    memcpy(buf->data + size_bytes, other->data + start_index, other_size_bytes);
    buf->size_bits += other->size_bits - start_index * BITS_IN_BYTE;
}

void bit_buffer_set_size(BitBuffer* buf, size_t new_size) {
    furi_assert(buf);
    furi_assert(buf->capacity_bytes * BITS_IN_BYTE >= new_size);

    buf->size_bits = new_size;
}

void bit_buffer_set_size_bytes(BitBuffer* buf, size_t new_size_bytes) {
    furi_assert(buf);
    furi_assert(buf->capacity_bytes >= new_size_bytes);

    buf->size_bits = new_size_bytes * BITS_IN_BYTE;
}

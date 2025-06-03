#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>

void manage_bits_print_bits_inle_outbe(
    const void * const value,
    const size_t value_size_bytes)
{
    static const int _i = 1;
#define IS_BIGENDIAN() ( (*(char*)&_i) == 0 )

    const unsigned char *val = (char *)value;

    if (value == NULL
     || value_size_bytes == 0
     || value_size_bytes > sizeof(size_t)
     || IS_BIGENDIAN()) {
        printf("\tCan't print value!\n");
        exit(1);
    }

    for (ssize_t i = (value_size_bytes - 1); i >= 0; --i) {
        for (int j = (CHAR_BIT - 1); j >= 0; --j) {
            printf("%c", (val[i] & (1 << j)) ? '1' : '0');
        }
        printf(" ");
    }
    printf("\n");

    return;
}

void manage_bits_setv_bit(
    void *value, const size_t value_size_bytes,
    const size_t value_bit_position, const unsigned char value_bit)
{
    if (value == NULL
     || value_size_bytes == 0
     || value_size_bytes > sizeof(size_t)
     || value_bit_position >= (value_size_bytes * CHAR_BIT)
     || (value_bit != 0 && value_bit != 1)) {
        printf("\tCan't set bit!\n");
        exit(1);
    }
    
    *(size_t *)value = (*((size_t *)value) & ~((size_t)1 << value_bit_position))
                     | ((size_t)value_bit << value_bit_position);
    return;
}

void manage_bits_set_bit(
    void *value, const size_t value_size_bytes,
    const size_t value_bit_position)
{
    if (value == NULL
     || value_size_bytes == 0
     || value_size_bytes > sizeof(size_t)
     || value_bit_position >= (value_size_bytes * CHAR_BIT)) {
        printf("\tCan't set bit!\n");
        exit(1);
    }

    // ((uint8_t*)out)[n / CHAR_BIT] |= (1 << (n % CHAR_BIT));
    *(size_t *)value = *((size_t *)value) | ((size_t)1 << value_bit_position);
    return;
}

void manage_bits_clear_bit(
    void *value, const size_t value_size_bytes,
    const size_t value_bit_position)
{
    if (value == NULL
     || value_size_bytes == 0
     || value_size_bytes > sizeof(size_t)
     || value_bit_position >= (value_size_bytes * CHAR_BIT)) {
        printf("\tCan't set bit!\n");
        exit(1);
    }

    // ((uint8_t*)out)[n / CHAR_BIT] &= ~(1 << (n % CHAR_BIT));
    *(size_t *)value = *((size_t *)value) & ~((size_t)1 << value_bit_position);
    return;
}

void manage_bits_get_bit(
    const void * const value, const size_t value_size_bytes,
    const size_t value_bit_position, unsigned char *value_bit)
{
    if (value == NULL
     || value_size_bytes == 0
     || value_size_bytes > sizeof(size_t)
     || value_bit_position >= (value_size_bytes * CHAR_BIT)
     || value_bit == NULL) {
        printf("\tCan't get bit!\n");
        exit(1);
    }

    // return (((const uint8_t*)in)[n / CHAR_BIT] & (1 << (n % CHAR_BIT))) != 0;
    *value_bit = (*((size_t *)value) >> value_bit_position) & (size_t)1;
    return;
}

void manage_bits_toggle_bit(
    void *value, const size_t value_size_bytes,
    const size_t value_bit_position)
{
    if (value == NULL
     || value_size_bytes == 0
     || value_size_bytes > sizeof(size_t)
     || value_bit_position >= (value_size_bytes * CHAR_BIT)) {
        printf("\tCan't toggle/flip bit!\n");
        exit(1);
    }

    // ((uint8_t*)out)[n / CHAR_BIT] ^= (1 << (n % CHAR_BIT));
    *(size_t *)value = *((size_t *)value) ^ ((size_t)1 << value_bit_position);
    return;
}

void manage_bits_check()
{
    _Static_assert(sizeof(size_t) >= sizeof(void *), "Check Error!");
    _Static_assert(CHAR_BIT == 8, "Check Error!");
    return;
}

struct I_Bits_Manager {
    void (*check)();
    void (*print)(const void * const value,
                  const size_t value_size_bytes);
    void (*setv)(void *value, const size_t value_size_bytes,
                 const size_t value_bit_position, const unsigned char value_bit);
    void (*get)(const void * const value, const size_t value_size_bytes,
                const size_t value_bit_position, unsigned char *value_bit);
    void (*set)(void *value, const size_t value_size_bytes,
                const size_t value_bit_position);
    void (*clear)(void *value, const size_t value_size_bytes,
                  const size_t value_bit_position);
    void (*toggle)(void *value, const size_t value_size_bytes,
                   const size_t value_bit_position);
} static const manage_bits = {
    .check  = &manage_bits_check,
    .print  = &manage_bits_print_bits_inle_outbe,
    .setv   = &manage_bits_setv_bit,
    .get    = &manage_bits_get_bit,
    .set    = &manage_bits_set_bit,
    .clear  = &manage_bits_clear_bit,
    .toggle = &manage_bits_toggle_bit,
};

void manage_hamming_code_check()
{
    _Static_assert(sizeof(size_t) >= sizeof(void *), "Check Error!");
    _Static_assert(CHAR_BIT == 8, "Check Error!");
    return;
}

void manage_hamming_code_encode(
    const void * const in,
    void *out,
    const size_t in_count_bytes,
    const size_t out_count_bytes,
    size_t *encoded_bits,
    const struct I_Bits_Manager * const manage_bits)
{
    size_t i = 0;
    size_t j = 0;
    size_t s = 0;
    const size_t in_bits = in_count_bytes * CHAR_BIT;
    const size_t out_bits = out_count_bytes * CHAR_BIT;
    
    if (in == NULL || out == NULL || encoded_bits == NULL
     || manage_bits == NULL
     || manage_bits->set == NULL || manage_bits->get == NULL
     || in_count_bytes == 0 || in_count_bytes > sizeof(size_t)
     || out_count_bytes == 0 || out_count_bytes > sizeof(size_t)
     || out_count_bytes < (in_count_bytes + 1)) {
        printf("\tCan't encode!\n");
        exit(1);
    }

    for (i = j = 0; j < in_bits; ++i) {
        if (((i + 1) & i) != 0) {
            unsigned char val = 0;
            manage_bits->get(in, in_count_bytes, j, &val);
            if (val == 1) {
                s ^= i + 1;
            }
            manage_bits->setv(out, out_count_bytes, i, val);
            ++j;
        }
    }
    for (size_t k = 1; k < i; k <<= 1) {
        manage_bits->setv(out, out_count_bytes, k - 1, (s & k) != 0 ? 1 : 0);
    }
    
    *encoded_bits = i;
    return;
}

void manage_hamming_code_decode(
    const void * const in,
    void *out,
    const size_t in_count_bytes,
    const size_t out_count_bytes,
    const size_t encoded_bits,
    size_t *decoded_bits,
    size_t *syndrom,
    const struct I_Bits_Manager * const manage_bits)
{
    size_t i = 0;
    size_t j = 0;
    size_t s = 0;
    const size_t in_bits = in_count_bytes * CHAR_BIT;
    const size_t out_bits = out_count_bytes * CHAR_BIT;
    
    if (in == NULL || out == NULL || decoded_bits == NULL || syndrom == NULL
     || manage_bits == NULL
     || manage_bits->set == NULL || manage_bits->get == NULL
     || in_count_bytes == 0 || in_count_bytes > sizeof(size_t)
     || out_count_bytes == 0 || out_count_bytes > sizeof(size_t)
     || out_count_bytes < (in_count_bytes - 1)
     || encoded_bits > in_bits || encoded_bits > out_bits) {
        printf("\tCan't decode!\n");
        exit(1);
    }

    for (i = j = 0; i < encoded_bits; ++i) {
        unsigned char val = 0;
        manage_bits->get(in, in_count_bytes, i, &val);
        if (val != 0) {
            s ^= i + 1;
        }
        if (((i + 1) & i) != 0) {
            manage_bits->setv(out, out_count_bytes, j, val);
            ++j;
        }
    }

    *syndrom = s;
    *decoded_bits = j;
    return;
}

void manage_hamming_code_decode_and_fix(
    const void * const in,
    void *out,
    const size_t in_count_bytes,
    const size_t out_count_bytes,
    const size_t encoded_bits,
    size_t *decoded_bits,
    size_t *syndrom,
    const struct I_Bits_Manager * const manage_bits)
{
    size_t i = 0;
    size_t j = 0;
    size_t s = 0;
    const size_t in_bits = in_count_bytes * CHAR_BIT;
    const size_t out_bits = out_count_bytes * CHAR_BIT;
    
    if (in == NULL || out == NULL || decoded_bits == NULL || syndrom == NULL
     || manage_bits == NULL
     || manage_bits->set == NULL
     || manage_bits->get == NULL
     || in_count_bytes == 0 || in_count_bytes > sizeof(size_t)
     || out_count_bytes == 0 || out_count_bytes > sizeof(size_t)
     || out_count_bytes < (in_count_bytes / 2)
     || encoded_bits > in_bits || encoded_bits > out_bits) {
        printf("\tCan't decode!\n");
        exit(1);
    }

    size_t out_tmp = *(size_t *)out;
    size_t dec_bits_tmp = *decoded_bits;
    size_t syndrom_tmp = *syndrom;
    manage_hamming_code_decode(
        in, &out_tmp, in_count_bytes, sizeof(out_tmp),
        encoded_bits, &dec_bits_tmp, &syndrom_tmp, manage_bits);
    if (syndrom_tmp != 0
     && syndrom_tmp <= encoded_bits
     && (syndrom_tmp & (syndrom_tmp - 1)) != 0) {
        size_t k = 0;
        for (k = 0; ((size_t)1 << k) < syndrom_tmp; ++k);
        manage_bits->toggle(&out_tmp, sizeof(out_tmp), syndrom_tmp - k - 1);
    }

    *syndrom = syndrom_tmp;
    *decoded_bits = dec_bits_tmp;
    return;
}

struct I_Hamming_Code {
/*
  0 1 2 3 4
  1 2 4 8 16

  X -> {0,1}

  0 1 2 3 4 5 6 7 8 9  10 11
  X X X X X X X X
  1 2 3 4 5 6 7 8 9 10 11 12
  a b X c X X X d X X  X  X

    1 2 3 4 5 6 7 8 9 10 11 12
	a b X c X X X d X X  X  X
  1 Y   Y   Y   Y   Y    Y
  2   Y Y     Y Y     Y  Y
  4       Y Y Y Y           Y
  8               Y Y Y  Y  Y
*/
    void (*check)();
    void (*encode)(const void * const in,
                   void *out,
                   const size_t in_count_bytes,
                   const size_t out_count_bytes,
                   size_t *encoded_bits,
                   const struct I_Bits_Manager * const manage_bits);
    void (*decode)(const void * const in,
                   void *out,
                   const size_t in_count_bytes,
                   const size_t out_count_bytes,
                   const size_t encoded_bits,
                   size_t *decoded_bits,
                   size_t *syndrom,
                   const struct I_Bits_Manager * const manage_bits);
    void (*fix)(const void * const in,
                void *out,
                const size_t in_count_bytes,
                const size_t out_count_bytes,
                const size_t encoded_bits,
                size_t *decoded_bits,
                size_t *syndrom,
                const struct I_Bits_Manager * const manage_bits);
} static const manage_hamming_code = {
    .check  = &manage_hamming_code_check,
    .encode = &manage_hamming_code_encode,
    .decode = &manage_hamming_code_decode,
    .fix    = &manage_hamming_code_decode_and_fix,
};

int main()
{
    printf("\n----------0----------\n\n");

    uint16_t val = 257;
    printf("Value = %u | 0x%x\n", val, val);
    manage_bits.print(&val, sizeof(val));
    manage_bits.setv(&val, sizeof(val), 9, 1);
    manage_bits.print(&val, sizeof(val));
    unsigned char val_bit = 2;
    int val_bit_position = 9;
    manage_bits.get(&val, sizeof(val), (size_t)val_bit_position, &val_bit);
    printf("Bit value [%d] = %d\n", val_bit_position, (int)val_bit);
    printf("Value = %u | 0x%x\n", val, val);
    manage_bits.toggle(&val, sizeof(val), 9);
    printf("Value = %u | 0x%x\n", val, val);
    manage_bits.set(&val, sizeof(val), 9);
    printf("Value = %u | 0x%x\n", val, val);
    manage_bits.clear(&val, sizeof(val), 9);
    printf("Value = %u | 0x%x\n", val, val);

    printf("\n----------1----------\n\n");
    
    uint8_t A = 2;
    uint16_t B = 0;
    uint16_t C = 0;
    size_t enc_bits = 0;
    printf("A = %u | 0x%x\n", A, A);
    manage_hamming_code.encode(&A, &B, sizeof(A), sizeof(B), &enc_bits, &manage_bits);
    assert(enc_bits == sizeof(A) * CHAR_BIT + 4);
    printf("B = %u | 0x%x\n", B, B);
    printf("Enc_Bits = %zu\n", enc_bits);
    size_t dec_bits = 0;
    size_t syndrom = 0;
    manage_hamming_code.decode(&B, &C, sizeof(B), sizeof(C), enc_bits, &dec_bits, &syndrom, &manage_bits);
    printf("syndrom = %u | 0x%x\n", syndrom, syndrom);
    assert(syndrom == 0);
    assert(dec_bits == sizeof(A) * CHAR_BIT);
    assert((uint16_t)A == C);
    printf("C = %u | 0x%x\n", C, C);
    printf("Dec_Bits = %zu\n", dec_bits);

    printf("\n----------2----------\n\n");

    for (size_t i = 0; i < enc_bits; ++i) {
        uint16_t D = B;
        manage_bits.toggle(&D, sizeof(D), i);
        printf("D = %u | 0x%x\n", D, D);
        manage_hamming_code.fix(&D, &C, sizeof(D), sizeof(C), enc_bits, &dec_bits, &syndrom, &manage_bits);
        printf("syndrom = %u | 0x%x\n", syndrom, syndrom);
        assert(dec_bits == sizeof(A) * CHAR_BIT);
        assert((uint16_t)A == C);
        manage_bits.toggle(&D, sizeof(D), i);
    }

    printf("\n----------3----------\n\n");

    return 0;
}

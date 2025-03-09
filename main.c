#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Mask bit cho các thao tác dịch bit
#define BIT_MASK_32   0x00000001           // Mask 32-bit lấy bit thấp
#define BIT_MASK_64   0x0000000000000001   // Mask 64-bit lấy bit thấp
#define LOWER_32_MASK 0x00000000ffffffff   // Lấy 32 bit thấp từ số 64 bit
#define UPPER_32_MASK 0xffffffff00000000   // Lấy 32 bit cao từ số 64 bit

/* Bảng hoán vị ban đầu (Initial Permutation - IP) */
static char IP_Table[] = {
    58, 50, 42, 34, 26, 18, 10,  2, 
    60, 52, 44, 36, 28, 20, 12,  4, 
    62, 54, 46, 38, 30, 22, 14,  6, 
    64, 56, 48, 40, 32, 24, 16,  8, 
    57, 49, 41, 33, 25, 17,  9,  1, 
    59, 51, 43, 35, 27, 19, 11,  3, 
    61, 53, 45, 37, 29, 21, 13,  5, 
    63, 55, 47, 39, 31, 23, 15,  7
};

/* Bảng hoán vị ngược (Inverse Initial Permutation - IP^-1) */
static char Inverse_IP_Table[] = {
    40,  8, 48, 16, 56, 24, 64, 32, 
    39,  7, 47, 15, 55, 23, 63, 31, 
    38,  6, 46, 14, 54, 22, 62, 30, 
    37,  5, 45, 13, 53, 21, 61, 29, 
    36,  4, 44, 12, 52, 20, 60, 28, 
    35,  3, 43, 11, 51, 19, 59, 27, 
    34,  2, 42, 10, 50, 18, 58, 26, 
    33,  1, 41,  9, 49, 17, 57, 25
};

/* Bảng mở rộng (Expansion Table) mở rộng 32-bit thành 48-bit */
static char Expansion_Table[] = {
    32,  1,  2,  3,  4,  5,  
     4,  5,  6,  7,  8,  9,  
     8,  9, 10, 11, 12, 13, 
    12, 13, 14, 15, 16, 17, 
    16, 17, 18, 19, 20, 21, 
    20, 21, 22, 23, 24, 25, 
    24, 25, 26, 27, 28, 29, 
    28, 29, 30, 31, 32,  1
};

/* Bảng hoán vị sau S-Box (Permutation Function - P) */
static char Permutation_Table[] = {
    16,  7, 20, 21, 
    29, 12, 28, 17, 
     1, 15, 23, 26, 
     5, 18, 31, 10, 
     2,  8, 24, 14, 
    32, 27,  3,  9, 
    19, 13, 30,  6, 
    22, 11,  4, 25
};

/* Các bảng S-Box, mỗi bảng chuyển 6-bit đầu vào thành 4-bit đầu ra */
static char S_Box[8][64] = {{
    /* S1 */
    14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,  
     0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,  
     4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0, 
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
},{
    /* S2 */
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,  
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,  
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15, 
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
},{
    /* S3 */
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,  
    13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,  
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
},{
    /* S4 */
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,  
    13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,  
    10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
},{
    /* S5 */
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9, 
    14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6, 
     4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14, 
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
},{
    /* S6 */
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
},{
    /* S7 */
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
    13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
},{
    /* S8 */
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
     1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
}};

/* Bảng Permuted Choice 1 (PC1) dùng để chuyển khóa 64-bit thành 56-bit */
static char PC1_Table[] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};

/* Bảng Permuted Choice 2 (PC2) dùng để chuyển 56-bit thành 48-bit cho mỗi khóa con */
static char PC2_Table[] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

/* Mảng giá trị dịch vòng (shift schedule) cho 16 vòng */
static char Shift_Schedule[] = {
    1, 1, 2, 2, 2, 2, 2, 2, 
    1, 2, 2, 2, 2, 2, 2, 1
};

/*
 * Hàm thực hiện thuật toán DES.
 * Tham số:
 *   - data: Dữ liệu 64-bit (plaintext hoặc ciphertext)
 *   - key: Khóa 64-bit ban đầu
 *   - mode: 'e' để mã hóa, 'd' để giải mã
 *
 * Trả về:
 *   - Dữ liệu 64-bit sau khi xử lý DES.
 */
uint64_t des_algorithm(uint64_t data, uint64_t key, char mode) {
    int i, j;
    char row, col;  // Chỉ số hàng và cột cho S-Box
    
    // Các biến cho quá trình sinh khóa (key schedule)
    uint32_t keyLeft = 0;    // 28-bit trái của khóa
    uint32_t keyRight = 0;   // 28-bit phải của khóa
    
    // Các biến xử lý dữ liệu (mỗi nửa 32-bit)
    uint32_t L = 0;
    uint32_t R = 0;
    uint32_t sBoxOutput = 0;
    uint32_t fFunctionResult = 0;
    uint32_t tempVal = 0;
    
    // Mảng chứa 16 khóa con (48-bit mỗi khóa)
    uint64_t subKeys[16] = {0};
    // Khối R mở rộng từ 32-bit thành 48-bit
    uint64_t expandedR = 0;
    
    // Khóa sau khi áp dụng Permuted Choice 1 (56-bit)
    uint64_t pc1Key = 0;
    // Khóa kết hợp (C||D) 56-bit, dùng để trích khóa con
    uint64_t combinedKey = 0;
    
    // Các biến cho các bước hoán vị dữ liệu
    uint64_t initialPermResult = 0;
    uint64_t inversePermResult = 0;
    uint64_t preOutput = 0;
    
    /* 1. Initial Permutation: Hoán vị dữ liệu đầu vào theo IP_Table */
    for (i = 0; i < 64; i++) {
        initialPermResult <<= 1;
        initialPermResult |= (data >> (64 - IP_Table[i])) & BIT_MASK_64;
    }
    
    // Tách dữ liệu sau hoán vị thành 2 nửa 32-bit: L (trái) và R (phải)
    L = (uint32_t)(initialPermResult >> 32) & LOWER_32_MASK;
    R = (uint32_t) initialPermResult & LOWER_32_MASK;
    
    /* 2. Sinh khóa: Áp dụng PC1 lên khóa ban đầu để thu được 56-bit */
    for (i = 0; i < 56; i++) {
        pc1Key <<= 1;
        pc1Key |= (key >> (64 - PC1_Table[i])) & BIT_MASK_64;
    }
    
    // Tách khóa 56-bit thành hai nửa 28-bit
    keyLeft = (uint32_t)((pc1Key >> 28) & 0x0fffffff);
    keyRight = (uint32_t)(pc1Key & 0x0fffffff);
    
    /* 3. Sinh 16 khóa con cho 16 vòng DES */
    for (i = 0; i < 16; i++) {
        // Dịch vòng trái các nửa khóa theo Shift_Schedule
        for (j = 0; j < Shift_Schedule[i]; j++) {
            keyLeft = ((keyLeft << 1) & 0x0fffffff) | ((keyLeft >> 27) & 0x1);
            keyRight = ((keyRight << 1) & 0x0fffffff) | ((keyRight >> 27) & 0x1);
        }
        
        // Kết hợp lại 2 nửa thành khóa 56-bit
        combinedKey = (((uint64_t) keyLeft) << 28) | (uint64_t) keyRight;
        
        // Áp dụng Permuted Choice 2 để thu được khóa con 48-bit
        subKeys[i] = 0;
        for (j = 0; j < 48; j++) {
            subKeys[i] <<= 1;
            subKeys[i] |= (combinedKey >> (56 - PC2_Table[j])) & BIT_MASK_64;
        }
    }
    
    /* 4. 16 vòng xử lý DES */
    for (i = 0; i < 16; i++) {
        expandedR = 0;
        // Mở rộng R từ 32-bit thành 48-bit theo bảng Expansion_Table
        for (j = 0; j < 48; j++) {
            expandedR <<= 1;
            expandedR |= (uint64_t)((R >> (32 - Expansion_Table[j])) & BIT_MASK_32);
        }
        
        // XOR giữa R mở rộng và khóa con tương ứng (đảo ngược thứ tự khóa cho giải mã)
        if (mode == 'd')
            expandedR ^= subKeys[15 - i];
        else
            expandedR ^= subKeys[i];
        
        sBoxOutput = 0;
        // Chia 48-bit thành 8 khối 6-bit, xử lý qua từng S-Box
        for (j = 0; j < 8; j++) {
            // Lấy 6-bit cho S-Box thứ j:
            // Bit đầu và cuối tạo thành hàng; 4 bit giữa tạo thành cột
            row = (char)((expandedR & (0x0000840000000000 >> (6 * j))) >> (42 - 6 * j));
            row = (row >> 4) | (row & 0x01);
            col = (char)((expandedR & (0x0000780000000000 >> (6 * j))) >> (43 - 6 * j));
            
            sBoxOutput <<= 4;
            sBoxOutput |= (uint32_t)(S_Box[j][16 * row + col] & 0x0f);
        }
        
        // Áp dụng hoán vị P cho kết quả S-Box
        fFunctionResult = 0;
        for (j = 0; j < 32; j++) {
            fFunctionResult <<= 1;
            fFunctionResult |= (sBoxOutput >> (32 - Permutation_Table[j])) & BIT_MASK_32;
        }
        
        // Cập nhật cho vòng tiếp theo: R mới = L XOR f(R, subkey), sau đó L nhận giá trị R cũ
        tempVal = R;
        R = L ^ fFunctionResult;
        L = tempVal;
    }
    
    /* 5. Kết hợp hai nửa (chú ý hoán đổi L và R) và áp dụng Inverse IP */
    preOutput = (((uint64_t) R) << 32) | (uint64_t) L;
    
    for (i = 0; i < 64; i++) {
        inversePermResult <<= 1;
        inversePermResult |= (preOutput >> (64 - Inverse_IP_Table[i])) & BIT_MASK_64;
    }
    
    return inversePermResult;
}

int main(int argc, const char * argv[]) {
    int i;
    
    // Dữ liệu đầu vào 64-bit (plaintext mẫu)
    uint64_t inputData = 0x0123456789ABCDEF;
    // 3 khóa dùng cho 3DES: key1, key2, key3
    uint64_t keys[3] = {0x0123456789ABCDEF, 0x9474B8E8C73BCA7C, 0x9474B8E8C73BC97D};
    uint64_t result = inputData;
 	uint64_t result_des = 0;
    printf ("\nEncrypting using DES & 3-DES\n------------------------------\n");
    printf ("Input Data: %016llx\n", inputData);
    
    // Mã hóa DES đơn với key1
    result = des_algorithm(inputData, keys[0], 'e');
    result_des = result;
    printf ("DES Encryption: %016llx\n", result_des);
    
    // Mã hóa 3DES: Mã hóa với key1, giải mã với key2, mã hóa với key3
    result = des_algorithm(result, keys[1], 'd');
    result = des_algorithm(result, keys[2], 'e');
    printf ("3DES Encryption: %016llx\n", result);
    
    printf ("\nDecrypting using DES & 3-DES\n------------------------------\n");   
    // Giải mã 3DES: Giải mã với key3, mã hóa với key2, giải mã với key1
    result = des_algorithm(result, keys[2], 'd');
    result_des  = des_algorithm(result_des, keys[0], 'd');
    printf ("DES Decryption: %016llx\n", result_des);
    result = des_algorithm(result, keys[1], 'e');
    result = des_algorithm(result, keys[0], 'd');
    printf ("3DES Decryption: %016llx\n", result);
    
    /*
     * TESTING IMPLEMENTATION OF DES
     * Ronald L. Rivest 
     * X0:  9474B8E8C73BCA7D
     * X16: 1B1A2DDB4C642438
     *
     * OUTPUT (ví dụ):
     * E: 8da744e0c94e5e17
     * D: 0cdb25e3ba3c6d79
     * E: 4784c4ba5006081f
     * D: 1cf1fc126f2ef842
     * E: e4be250042098d13
     * D: 7bfc5dc6adb5797c
     * E: 1ab3b4d82082fb28
     * D: c1576a14de707097
     * E: 739b68cd2e26782a
     * D: 2a59f0c464506edb
     * E: a5c39d4251f0a81e
     * D: 7239ac9a6107ddb1
     * E: 070cac8590241233
     * D: 78f87b6e3dfecf61
     * E: 95ec2578c2c433f0
     * D: 1b1a2ddb4c642438  <-- X16
     */
    
    exit(0);
}

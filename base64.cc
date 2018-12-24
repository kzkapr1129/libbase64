//
//  base64.cc
//  base64
//
//  Created by 中山一輝 on 2018/10/17.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#include "base64.h"
#include <math.h>
#include <string.h>

static const char TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t b64::calcEncodedBufSize(size_t len) {
    // 8bit文字列から変換される6bit文字列の数(1.333...倍)
    size_t char_len = (size_t)ceil(len * 1.33333);
    
    size_t nm_equals = 0;
    size_t restWord = char_len % 4;
    if (restWord > 0) {
        // '='文字列の数
        nm_equals = (4 - restWord);
        
    }
    // 2つの数を合算したが数がbase64化されたときに必要となる文字数
    return char_len + nm_equals;
}

size_t b64::calcDecodedBufSize(const unsigned char* values, size_t len) {
    if (len % 4 != 0 && len < 4) {
        // Invalid base64
        return -1;
    }

    // 文字列の末尾が'='か?
    bool is_last_char_equal = values[len-1] == '=';
    
    // 文字列の末尾-1が'='か?
    bool is_last_m1_char_equal = values[len-2] == '=';

    if (is_last_char_equal && is_last_m1_char_equal) {
        // 文字列の末尾が"=="か?
        return (len - 4) * 0.75 + 1;
    } else if (is_last_char_equal) {
        // 文字列の末尾が"="か?
        return (len - 4) * 0.75 + 2;
    }

    // base64の4文字分がデコード後の3文字分になる(0.75倍)
    return len * 0.75;
}

size_t b64::encode(const unsigned char* values, size_t len, unsigned char* out_buf) {
    unsigned char* p = out_buf;
    for (size_t i = 0; i < len; i += 3) {
        size_t remain = len - i;
        if (remain >= 3) {
            unsigned char c0 = values[i + 0];
            unsigned char c1 = values[i + 1];
            unsigned char c2 = values[i + 2];
            /*
             * ret0 = パターン1 (c0の先頭6bit)
             * ret1 = パターン2 (c0の後方2bit + c1の先頭4bit)
             * ret2 = パターン3 (c1の後方4bit + c2の先頭2bit)
             * ret3 = パターン4 (c2の後方6bit)
             */
            unsigned char ret0 = ((c0 & 0xFC) >> 2);
            unsigned char ret1 = ((c0 & 0x03) << 4) | ((c1 & 0xF0) >> 4);
            unsigned char ret2 = ((c1 & 0x0F) << 2) | ((c2 & 0xC0) >> 6);
            unsigned char ret3 = (c2 & 0x3F);
            *p = TABLE[ret0]; p++;
            *p = TABLE[ret1]; p++;
            *p = TABLE[ret2]; p++;
            *p = TABLE[ret3]; p++;
        } else if (remain >= 2) {
            unsigned char c0 = values[i + 0];
            unsigned char c1 = values[i + 1];
            /*
             * ret0 = パターン1 (c0の先頭6bit)
             * ret1 = パターン2 (c0の後方2bit + c1の先頭4bit)
             * ret2 = パターン3 (c1の後方4bit + 00)
             * ret3 = '='
             */
            unsigned char ret0 = ((c0 & 0xFC) >> 2);
            unsigned char ret1 = ((c0 & 0x03) << 4) | ((c1 & 0xF0) >> 4);
            unsigned char ret2 = ((c1 & 0x0F) << 2);
            unsigned char ret3 = '=';
            *p = TABLE[ret0]; p++;
            *p = TABLE[ret1]; p++;
            *p = TABLE[ret2]; p++;
            *p = ret3; p++;
        } else if (remain >= 1) {
            unsigned char c0 = values[i + 0];
            /*
             * ret0 = パターン1 (c0の先頭6bit)
             * ret1 = パターン2 (c0の後方2bit + 0000)
             * ret2 = '='
             * ret3 = '='
             */
            unsigned char ret0 = ((c0 & 0xFC) >> 2);
            unsigned char ret1 = ((c0 & 0x03) << 4);
            unsigned char ret2 = '=';
            unsigned char ret3 = '=';
            *p = TABLE[ret0]; p++;
            *p = TABLE[ret1]; p++;
            *p = ret2; p++;
            *p = ret3; p++;
        }
    }
    
    return p - out_buf;
}

static unsigned char reverseBase64Char(unsigned char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        unsigned char tmp_index = c - 'a';
        return tmp_index + 26 /*'A' to 'Z'*/;
    } else if (c >= '0' && c <= '9') {
        unsigned char tmp_index = c - '0';
        return tmp_index + 52 /*'A' to 'Z' + 'a' to 'z'*/;
    } else if (c == '+') {
        return 62 /*'A' to 'Z' + 'a' to 'z' + '0' to '9'*/;
    }
    
    return 63; // c must be '/'
}

size_t b64::decode(const unsigned char* values, size_t len, unsigned char* out_buf) {
    int nm_equals = 0;
    size_t i = len - 1;
    while (i >=0 && values[i--] == '=') {
        nm_equals++;
    }

    unsigned char* p = out_buf;
    size_t len_valid_chars = len - nm_equals;
    for (i = 0; i < len_valid_chars; i+=4) {
        size_t remain = len_valid_chars - i;
        if (remain >= 4) {
            unsigned char c0 = values[i + 0];
            unsigned char c1 = values[i + 1];
            unsigned char c2 = values[i + 2];
            unsigned char c3 = values[i + 3];
            unsigned char r0 = reverseBase64Char(c0);
            unsigned char r1 = reverseBase64Char(c1);
            unsigned char r2 = reverseBase64Char(c2);
            unsigned char r3 = reverseBase64Char(c3);
            unsigned char ret0 = ((r0 & 0x3F) << 2) | ((r1 & 0x30) >> 4);
            unsigned char ret1 = ((r1 & 0x0F) << 4) | ((r2 & 0x3C) >> 2);
            unsigned char ret2 = ((r2 & 0x03) << 6) | ((r3 & 0x3F));
            *p = ret0; p++;
            *p = ret1; p++;
            *p = ret2; p++;
        } else if (remain >= 3) {
            unsigned char c0 = values[i + 0];
            unsigned char c1 = values[i + 1];
            unsigned char c2 = values[i + 2];
            unsigned char r0 = reverseBase64Char(c0);
            unsigned char r1 = reverseBase64Char(c1);
            unsigned char r2 = reverseBase64Char(c2);
            unsigned char ret0 = ((r0 & 0x3F) << 2) | ((r1 & 0x30) >> 4);
            unsigned char ret1 = ((r1 & 0x0F) << 4) | ((r2 & 0x3C) >> 2);
            *p = ret0; p++;
            *p = ret1; p++;
        } else if (remain >= 2) {
            unsigned char c0 = values[i + 0];
            unsigned char c1 = values[i + 1];
            unsigned char r0 = reverseBase64Char(c0);
            unsigned char r1 = reverseBase64Char(c1);
            unsigned char ret0 = ((r0 & 0x3F) << 2) | ((r1 & 0x30) >> 4);
            *p = ret0; p++;
        } else if (remain >= 1) {
            // IGNORE
        }
    }
    
    return p - out_buf;
}

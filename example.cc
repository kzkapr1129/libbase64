//
//  main.cpp
//  base64
//
//  Created by 中山一輝 on 2018/10/17.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#include "base64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        printf("Usage: %s string\n", argv[0]);
        return -1;
    }
    
    const unsigned char* str = (const unsigned char*)argv[1];
    const size_t len_str = strlen(argv[1]);
    printf("検査文字列: %s\n", str);
    
    // base64エンコード後の文字列長を取得する
    size_t enc_size = b64::calcEncodedBufSize(len_str);
    
    // base64を格納するためのメモリを確保する
    unsigned char* enc_buf = (unsigned char*)malloc(enc_size + 1);
    
    // base64エンコードを実施する
    size_t ret1 = b64::encode(str, len_str, enc_buf);
    
    // エンコード後の文字列長が正しいかを確認する
    if (ret1 == enc_size) {
        printf("phase1 check: OK\n");
    } else {
        printf("phase1 check: FAIL (%lu, %lu)\n", ret1, enc_size);
    }
    
    // 文字列の末尾にnull文字を追加
    enc_buf[ret1] = '\0';
    
    // base64のディスプレイ出力
    printf("base64: %s\n", enc_buf);
    
    // デコード後の文字列長を取得する
    size_t dec_size = b64::calcDecodedBufSize(enc_buf, enc_size);
    
    // デコード後文字列を格納するためのメモリを確保する
    unsigned char* dec_buf = (unsigned char*)malloc(dec_size + 1);
    
    // デコードの実施
    size_t ret2 = b64::decode(enc_buf, enc_size, dec_buf);
    
    // デコード後の文字列長が正しいかを確認する
    if (ret2 == len_str) {
        printf("phase2 check: OK\n");
    } else {
        printf("phase2 check: FAIL (%lu, %lu)\n", ret2, len_str);
    }
    
    // 文字列の末尾にnull文字を追加
    dec_buf[dec_size] = '\0';
    
    // デコード後文字列のディスプレイ出力
    printf("origin: %s\n", dec_buf);
    
    if (!memcmp(str, dec_buf, dec_size)) {
        printf("phase3 check: OK\n");
    } else {
        printf("phase3 check: FAIL\n");
    }
    
    // メモリの解放
    free(enc_buf);
    free(dec_buf);
    
    return 0;
}

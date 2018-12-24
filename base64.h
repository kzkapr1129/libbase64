//
//  base64.h
//  base64
//
//  Created by 中山一輝 on 2018/10/17.
//  Copyright © 2018年 中山一輝. All rights reserved.
//

#ifndef __BASE64_H__
#define __BASE64_H__

#include <stddef.h>

namespace b64 {
    size_t calcEncodedBufSize(size_t len);
    size_t calcDecodedBufSize(const unsigned char* values, size_t len);
    size_t encode(const unsigned char* values, size_t len, unsigned char* out_buf);
    size_t decode(const unsigned char* values, size_t len, unsigned char* out_buf);
}

#endif // __BASE64_H__

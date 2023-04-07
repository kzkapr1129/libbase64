#include <stdio.h>
#include <string.h>
 
#define ERR_RETURN(v, func) \
    v = func; \
    if (v < 0) { return 0; } \
 
#define APPEND_OR_RETURN(p, tail, v) \
    if (p < tail) {*p = v; p++;} else return 0;
 
#define MAKE_B0(b0, b1) ((b0 & 0x3F) << 2) | ((b1 & 0x30) >> 4)
#define MAKE_B1(b1, b2) ((b1 & 0x0F) << 4) | ((b2 & 0x3C) >> 2)
#define MAKE_B2(b2, b3) ((b2 & 0x03) << 6) | ((b3 & 0x3F))
 
/**
 *  指定文字をテーブル変換した結果を返却する。
 *  テーブルは以下の通り。
 *
 *  Table 1: The Base 64 Alphabet
 *
 *   Value Encoding  Value Encoding  Value Encoding  Value Encoding
 *       0 A            17 R            34 i            51 z
 *       1 B            18 S            35 j            52 0
 *       2 C            19 T            36 k            53 1
 *       3 D            20 U            37 l            54 2
 *       4 E            21 V            38 m            55 3
 *       5 F            22 W            39 n            56 4
 *       6 G            23 X            40 o            57 5
 *       7 H            24 Y            41 p            58 6
 *       8 I            25 Z            42 q            59 7
 *       9 J            26 a            43 r            60 8
 *      10 K            27 b            44 s            61 9
 *      11 L            28 c            45 t            62 +
 *      12 M            29 d            46 u            63 /
 *      13 N            30 e            47 v
 *      14 O            31 f            48 w         (pad) =
 *      15 P            32 g            49 x
 *      16 Q            33 h            50 y
 * 
 *  @return EncodingをValueに変換できた場合はValue、失敗した場合は0未満の値を返却する
*/
int conv(char c) {
    if ('A' <= c && c <= 'Z') {
        return c - 'A';
    } else if ('a' <= c && c <= 'z') {
        return c - 'a' + 26;
    } else if ('0' <= c && c <= '9') {
        return c - '0' + 52;
    } else if (c == '+') {
        return 62;
    } else if (c == '/') {
        return 63;
    }
    return -1;
}
 
unsigned long Base64Decode(const char *base64, unsigned char *bin,  unsigned long max) {
    size_t len;
    size_t i;
    size_t num_equals;
    size_t conv_data_size;
    int c0, c1, c2, c3;
    const char* tail;
    unsigned char r0, r1, r2;
 
    // パラメータチェック
    if (!base64 || !bin || !max || (len = strlen(base64)) <= 0) return 0;
 
    num_equals = 0;
    // 文字列末尾の"="の数をカウント
    for (tail = base64 + len - 1; base64 <= tail && *tail == '='; tail--) {
        num_equals++;
    }
 
    unsigned char *output = bin;
    unsigned char *output_tail = output + max;
    conv_data_size = len - num_equals;
 
    // "="を除外した部分を先頭からデコード
    for (i = 0; i < conv_data_size; i += 4) {
        size_t remain = conv_data_size - i;
        if (4 <= remain) {
            // 走査した残りの文字数が4以上
            ERR_RETURN(c0, conv(base64[i + 0]));
            ERR_RETURN(c1, conv(base64[i + 1]));
            ERR_RETURN(c2, conv(base64[i + 2]));
            ERR_RETURN(c3, conv(base64[i + 3]));
            r0 = MAKE_B0(c0, c1);
            r1 = MAKE_B1(c1, c2);
            r2 = MAKE_B2(c2, c3);
            APPEND_OR_RETURN(output, output_tail, r0);
            APPEND_OR_RETURN(output, output_tail, r1);
            APPEND_OR_RETURN(output, output_tail, r2);
        } else if (3 <= remain) {
            // 走査した残りの文字数が3以上(if文構造により実質残り3)
            ERR_RETURN(c0, conv(base64[i + 0]));
            ERR_RETURN(c1, conv(base64[i + 1]));
            ERR_RETURN(c2, conv(base64[i + 2]));
            r0 = MAKE_B0(c0, c1);
            r1 = MAKE_B1(c1, c2);
            APPEND_OR_RETURN(output, output_tail, r0);
            APPEND_OR_RETURN(output, output_tail, r1);
 
        } else if (2 <= remain) {
            // 走査した残りの文字数が2以上(if文構造により実質残り2)
            ERR_RETURN(c0, conv(base64[i + 0]));
            ERR_RETURN(c1, conv(base64[i + 1]));
            r0 = MAKE_B0(c0, c1);
            APPEND_OR_RETURN(output, output_tail, r0);
        } else if (1 <= remain) {
            // 走査した残りの文字数が1以上(if文構造により実質残り1)
            // このパターンは2ビット足りないので０として扱う。
            ERR_RETURN(c0, conv(base64[i + 0]));
            r0 = ((c0 & 0x3F) << 2) & 0xFC;
            APPEND_OR_RETURN(output, output_tail, r0);
        }
    }
 
    return output - bin;
}
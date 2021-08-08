
#define str_serialization_unit                                      \
    if (*source == slash || *source < hex_ ) {                      \
        if (*source != space && *source != exc_) {                  \
            if (*source == stop) break;                             \
            *out++ = slash;                                         \
            *out++ = char_check(*source++);                         \
            continue;                                               \
        }                                                           \
    }                                                               \
    *out++ = *source++


#define _utf32toutf16(_v)                                           \
    *out++ = slash;                                                 \
    *out++ = 'u';                                                   \
    DEC_TO_HEX(_v, out);                                            \
    out += 4


#define str_serialization_32                                        \
    if (*source < max_2_byte) {                                     \
        str_serialization_unit;                                     \
    } else {                                                        \
        uint32_t h = 0;                                             \
        uint32_t l = 0;                                             \
        convertUTF32ToUTF16(*source++, h, l);                       \
        _utf32toutf16(h);                                           \
        _utf32toutf16(l);                                           \
    }


#define obj_class_checker                                           \
    const char * c = PyUnicode_AsUTF8(key);                         \
    if (PyUnicode_GetLength(key) > 1 && *c == ddf) continue;        \
    if (PyFunction_Check(value) || PyMethod_Check(value)) continue; \
    int error_handler = 0;\
    key = check_field(key, value, js_dataclass_index, error_handler);\
    if (error_handler) return 1;\
    if (!key) continue



#define str_pars_and_check(_n, _kind)                               \
    switch(d[_n]) {                                                 \
        case slash:                                                 \
            return str_escape_parser(d - d_start);                  \
        case quot:                                                  \
            size = (d - d_start) + _n;                              \
            data += size;                                           \
            return set_str((void *)find_str, size, _kind);          \
        default:                                                    \
            if (d[_n] < space) {                                    \
                return set_error("error simbol");                   \
            }                                                       \
    }

#define str_escape_pars_and_check(_n)                               \
    switch(ptr[_n]) {                                               \
        case slash:                                                 \
            ptr += _n + 1;                                          \
            buff[_n] = char_check_2_byte(p);                        \
            buff += _n + 1;                                         \
            continue;                                               \
        case quot:                                                  \
            data += (ptr - source) + _n + n;                        \
            return set_str(buff_start, (buff - buff_start) + _n, 4);\
        default:                                                    \
            if (ptr[_n] < space) {                                  \
                return set_error("error simbol");                   \
            }                                                       \
    }                                                               \
    buff[_n] = ptr[_n]


#define int_pars_and_check                                          \
    if (*data > int_9) {                                            \
        switch (*data) {                                            \
            case close_list:                                        \
            case close_obj:                                         \
                return get_integer(is_float, is_negative, is_exponent);          \
            case 'e':                                               \
            case 'E': \
                 is_exponent = true;                                \
                 data++;                                            \
                 continue;                                          \
            default:                                                \
                 return set_error("error token not integer");       \
        }                                                           \
    } else if (*data < int_0) {                                     \
        switch (*data) {                                            \
            case int_point:                                         \
                is_float = true;                                    \
                data++;                                             \
                continue;                                           \
            case '-':                                               \
            case '+':                                               \
                if (!is_exponent) {                                 \
                    return set_error("error token exponent");       \
                }                                                   \
                break;                                              \
            case comma:                                             \
            case stop:                                              \
            case space:                                             \
                return get_integer(is_float, is_negative, is_exponent);          \
            default:                                                \
                if (*data < space) {                                \
                    return get_integer(is_float, is_negative, is_exponent);      \
                } else {                                            \
                    return set_error("error token not integer");    \
                }                                                   \
            }                                                       \
    }                                                               \
    data++


#define bytes_escape_pars_and_check(_n)                             \
    switch(ptr[_n]) {                                               \
        case slash:                                                 \
            ptr += _n + 1;                                          \
            v = char_check_2_byte(p);                               \
            buff += _n;                                             \
            buff += encode_unicode_character(buff, v);              \
            continue;                                               \
        case quot:                                                  \
            data += (ptr - source + _n + n);                        \
            return set_str(buff_start, (buff - buff_start) + _n, 1);\
        default:                                                    \
            if (ptr[_n] < space) {                                  \
                return set_error("error simbol");                   \
            }                                                       \
    }                                                               \
    buff[_n] = ptr[_n]

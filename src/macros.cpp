
#define str_serialization_unit                                 \
    if (*source <= quot || *source == slash) {                  \
        if (source >= source_end) break; \
        if (*source < space || *source == slash || *source == quot) {\
            out += char_check(*source++, out);                  \
            continue;                                           \
        }                                                       \
    }                                                           \
    *out++ = *source++



#define str_serialization_unit_                                 \
    if (*source <= quot || *source == slash) {                  \
        if (source >= source_end) break; \
        if (*source < space || *source == slash || *source == quot) {\
            out += char_check(*source++, out);                \
            continue;                                           \
        }                                                       \
        *out++ = *source++;\
    } else if (*source >= 192) {\
        if (*source < 224) { \
            *((bytes2 *)out) = *((bytes2 *)source);\
            out += 2; source += 2;\
        } else if (*source < 240) {\
            *((bytes3 *)out) = *((bytes3 *)source);\
            out += 3; source += 3;\
        } else if (*source < 248){\
            *((bytes4 *)out) = *((bytes4 *)source);\
            out += 4; source += 4;\
        }  else if (*source < 252) {\
            *((bytes5 *)out) = *((bytes5 *)source);\
            out += 5; source += 5;\
        } else if (*source < 254) {\
            *((bytes6 *)out) = *((bytes6 *)source);\
            out += 6; source += 6;\
        }  \
    } else {\
        *out++ = *source++; \
    }




#define _utf32toutf16(_v)                                           \
    *out++ = slash;                                                 \
    *out++ = 'u';                                                   \
    DEC_TO_HEX(_v, out);                                            \
    out += 4


#define str_serialization_32                                        \
    if (*source < max_2_byte) {                                     \
        str_serialization_unit;                                     \
    } else {                                                        \
        convertUTF32ToUTF16(*source++, h, l);                       \
        _utf32toutf16(h);                                           \
        _utf32toutf16(l);                                           \
    }


#define obj_class_checker                                           \
    key_name = PyUnicode_AsUTF8(key);                               \
    if (PyUnicode_GetLength(key) > 1 && *key_name == ddf) continue; \
    if (PyFunction_Check(value) || PyMethod_Check(value)) continue; \
    key = check_field(key, value, js_dataclass_index, error_handler);\
    if (error_handler) return;                                      \
    if (!key) continue



#define str_pars_and_check(_n, _kind)                               \
    if (d[_n] == slash || d[_n] <= quot) {                          \
        if (d[_n] <= quot) {                                        \
            if (d[_n] < space) {                                    \
                return set_error("error symbol");                   \
            }                                                       \
            if (d[_n] == quot) {                                    \
                size = (d - d_start) + _n;                          \
                data += size;                                       \
                return set_str(find_str, size, _kind);              \
            }                                                       \
        }                                                           \
        if (d[_n] == slash) {                                       \
            return str_escape_parser(d - d_start);                  \
        }                                                           \
    }


#define str_escape_pars_and_check(_n)                                   \
    if (ptr[_n] == slash || ptr[_n] <= quot) {                          \
        if (ptr[_n] <= quot) {                                            \
            if (ptr[_n] < space) {                                        \
                return set_error("error symbol");                       \
            }                                                           \
            if (ptr[_n] == quot) {                                        \
                data += (ptr - source) + _n + n;                        \
                return set_str(buff_start, (buff - buff_start) + _n, 4);\
            }                                                           \
        }                                                               \
        if (ptr[_n] == slash) {                                           \
            ptr += _n + 1;                                              \
            buff[_n] = char_check_2_byte(p);                            \
            buff += _n + 1;                                             \
            continue;                                                   \
        }                                                               \
    }                                                                   \
    buff[_n] = ptr[_n]



#define int_pars_and_check                                          \
    if (*data > int_9) {                                            \
        switch (*data) {                                            \
            case 'e':                                               \
            case 'E':                                               \
                 is_exponent = true;                                \
                 data++;                                            \
                 continue;                                          \
            case close_list:                                        \
            case close_obj:                                         \
                return get_integer(is_float, is_negative,           \
                 is_exponent, start_zero);                          \
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
                return get_integer(is_float, is_negative,           \
                                    is_exponent, start_zero);       \
            default:                                                \
                if (*data < space) {                                \
                    return get_integer(is_float, is_negative,       \
                                        is_exponent, start_zero);   \
                } else {                                            \
                    return set_error("error token not integer");    \
                }                                                   \
            }                                                       \
    }                                                               \
    data++



#define int_parser_(_n)                                         \
    v = symbols_int[data[_n]];                              \
    if (v != 3) {                                               \
        if (v == 1) {                                           \
            data += _n;                                             \
            return get_integer(is_float, is_negative,           \
                                is_exponent, start_zero);       \
        } else if (v == 2) {  \
            is_float = true;                                        \
        } else if (v >= 4) {                                    \
            if (!is_exponent && v == 5) {                       \
                return set_error("error token exponent");           \
            }                                                      \
            is_exponent = true;                                 \
        } else {                                                \
            return set_error("error token not integer");            \
        }                                                               \
    }



#define bytes_escape_pars_and_check(_n)                                     \
    if (ptr[_n] == slash || ptr[_n] <= quot) {                              \
        if (ptr[_n] <= quot) {                                              \
            if (ptr[_n] < space) {                                          \
                return set_error("error symbol");                           \
            }                                                               \
            if (ptr[_n] == quot) {                                          \
                data += (ptr - source + _n + n);                            \
                return set_str(buff_start, (buff - buff_start) + _n, 1);    \
            }                                                               \
        }                                                                   \
        if (ptr[_n] == slash) {                                             \
            ptr += _n + 1;                                                  \
            v = char_check_2_byte(p);                                       \
            buff += _n;                                                     \
            buff += encode_unicode_character(buff, v);                      \
            continue;                                                       \
        }                                                                   \
    }                                                                      \
    buff[_n] = ptr[_n]





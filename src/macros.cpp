
#define str_serialization_unit                                 \
    if (*source <= '"' || *source == '\\') {                  \
        if (source >= source_end) break; \
        if (*source < ' ' || *source == '\\' || *source == '"') {\
            out += char_check(*source++, out);                  \
            continue;                                           \
        }                                                       \
    }                                                           \
    *out++ = *source++



#define str_serialization_unit_                                 \
    if (*source <= '"' || *source == '\\') {                  \
        if (source >= source_end) break; \
        if (*source < ' ' || *source == '\\' || *source == '"') {\
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



#define str_pars_and_check(_n, _kind)                               \
    if (d[_n] == '\\' || d[_n] <= '"') {                          \
        if (d[_n] <= '"') {                                        \
            if (d[_n] < ' ') {                                    \
                return set_error("error symbol");                   \
            }                                                       \
            if (d[_n] == '"') {                                    \
                size = (d - d_start) + _n;                          \
                data += size;                                       \
                return set_str(find_str, size, _kind);              \
            }                                                       \
        }                                                           \
        if (d[_n] == '\\') {                                       \
            return str_escape_parser(d - d_start);                  \
        }                                                           \
    }


#define str_escape_pars_and_check(_n)                                   \
    if (ptr[_n] == '\\' || ptr[_n] <= '"') {                          \
        if (ptr[_n] <= '"') {                                            \
            if (ptr[_n] < ' ') {                                        \
                return set_error("error symbol");                       \
            }                                                           \
            if (ptr[_n] == '"') {                                        \
                data += (ptr - source) + _n + n;                        \
                return set_str(buff_start, (buff - buff_start) + _n, 4);\
            }                                                           \
        }                                                               \
        if (ptr[_n] == '//') {                                           \
            ptr += _n + 1;                                              \
            buff[_n] = char_check_2_byte(p);                            \
            buff += _n + 1;                                             \
            continue;                                                   \
        }                                                               \
    }                                                                   \
    buff[_n] = ptr[_n]



#define int_pars_and_check                                          \
    if (*data > '9') {                                            \
        switch (*data) {                                            \
            case 'e':                                               \
            case 'E':                                               \
                 is_exponent = true;                                \
                 data++;                                            \
                 continue;                                          \
            case ']':                                               \
            case '}':                                               \
                return get_integer(is_float, is_negative,           \
                 is_exponent, start_zero);                          \
            default:                                                \
                 return set_error("error token not integer");       \
        }                                                           \
    } else if (*data < '0') {                                     \
        switch (*data) {                                            \
            case '.':                                         \
                is_float = true;                                    \
                data++;                                             \
                continue;                                           \
            case '-':                                               \
            case '+':                                               \
                if (!is_exponent) {                                 \
                    return set_error("error token exponent");       \
                }                                                   \
                break;                                              \
            case ',':                                             \
            case '\0':                                              \
            case ' ':                                             \
                return get_integer(is_float, is_negative,           \
                                    is_exponent, start_zero);       \
            default:                                                \
                if (*data < ' ') {                                \
                    return get_integer(is_float, is_negative,       \
                                        is_exponent, start_zero);   \
                } else {                                            \
                    return set_error("error token not integer");    \
                }                                                   \
            }                                                       \
    }                                                               \
    data++


#define bytes_escape_pars_and_check(_n)                                     \
    if (ptr[_n] == '\\' || ptr[_n] <= '"') {                                \
        if (ptr[_n] <= '"') {                                               \
            if (ptr[_n] < ' ') {                                            \
                return set_error("error symbol");                           \
            }                                                               \
            if (ptr[_n] == '"') {                                           \
                data += (ptr - source + _n + n);                            \
                return set_str(buff_start, (buff - buff_start) + _n, 1);    \
            }                                                               \
        }                                                                   \
        if (ptr[_n] == '\\') {                                              \
            ptr += _n + 1;                                                  \
            v = char_check_2_byte(p);                                       \
            buff += _n;                                                     \
            buff += encode_unicode_character(buff, v);                      \
            continue;                                                       \
        }                                                                   \
    }                                                                       \
    buff[_n] = ptr[_n]





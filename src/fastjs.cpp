#include <Python.h>
#include "macros.cpp"
#include <iostream>


using namespace std;


const uint32_t open_obj = '{';
const uint32_t close_obj = '}';
const uint32_t open_list = '[';
const uint32_t close_list = ']';
const uint32_t quot = '"';
const uint32_t slash = '\\';
const uint32_t comma = ',';
const uint32_t colon = ':';
const uint32_t js_true = 't';
const uint32_t js_false = 'f';
const uint32_t js_null = 'n';
const uint32_t int_0 = '0';
const uint32_t int_9 = '9';
const uint32_t df = '-';
const uint32_t ddf = '_';
const uint32_t int_point = '.';
const uint32_t space = ' ';
const uint32_t stop = '\0';
const uint32_t ent = '\n';
const uint32_t tab = '\t';
const uint32_t sr = '\r';
const uint32_t sf = '\f';
const uint32_t sb = '\b';
const uint32_t hex_ = '#';
const uint32_t exc_ = '!';


inline const size_t GetLenLong4(const uint64_t v) {
    if (v < 10000000000) {
        if (v < 10000000) {
            if (v < 10000) {
                return 4;
            } else if (v < 100000) {
                return 5;
            } else if (v < 1000000) {
                return 6;
            }
            return 7;
        } else {
            if (v < 100000000) {
                return 8;
            }
            else if (v < 1000000000) {
                return 9;
            }
        }
        return 10;
    } else {
        if (v < 100000000000000) {

            if (v < 100000000000) {
                return 11;
            }
            else if (v < 1000000000000) {
                return 12;
            }
            else if (v < 10000000000000) {
                return 13;
            }
            return 14;
        } else {
            if (v < 1000000000000000) {
                return 15;
            }
            else if (v < 10000000000000000) {
                return 16;
            }
            else if (v < 100000000000000000) {
                return 17;
            } else if (v < 1000000000000000000) {
                return 18;
            }
            return 19;
        }
    }
}

template<class T>
inline const uint32_t HEX_TO_DEC(const T * str, size_t len) {
    uint8_t k = 0;
    uint32_t s = 0;

    while (len--) {
        if (*str >= int_0 && *str <= int_9) {
            k = *str - '0';

        } else if (*str >= 'a' && *str <= 'f') {
            k = *str - 'a' + 10;

        } else if (*str >= 'A' && *str <= 'F') {
            k = *str - 'A' + 10;

        } else {
            PyErr_SetString(PyExc_RuntimeError, "error unicode simbol \\u0000");
            return 0;
        }
        s = (s << 4) + k;
        str++;
    }
    return s;
}

static const char * const digits_ = "0123456789abcdef";

template <typename T>
inline void DEC_TO_HEX(const uint32_t w, T * buffer) {
    *buffer++ = digits_[(w >> 12) & 0x0f];
    *buffer++ = digits_[(w >> 8) & 0x0f];
    *buffer++ = digits_[(w >> 4) & 0x0f];
    *buffer++ = digits_[w & 0x0f];
}


inline void convertUTF32ToUTF16(const uint32_t cUTF32, uint32_t &h, uint32_t &l) {
    if (cUTF32 < 0x10000) {
        h = 0;
        l = cUTF32;
    }
    uint32_t t = cUTF32 - 0x10000;
    h = (((t << 12) >> 22) + 0xD800);
    l = (((t << 22) >> 22) + 0xDC00);
}


template <class T>
inline const size_t encode_unicode_character(char * buffer, const T ucs_character) {
    if (ucs_character <= 0x7F) {
        buffer[0] = ucs_character;
        return 1;
    }
    else if (ucs_character <= 0x7FF) {
        buffer[0] = 0xC0 | (ucs_character >> 6);
        buffer[1] = 0x80 | ((ucs_character >> 0) & 0x3F);
        return 2;
    }
    else if (ucs_character <= 0xFFFF) {
        buffer[0] = 0xE0 | (ucs_character >> 12);
        buffer[1] = 0x80 | ((ucs_character >> 6) & 0x3F);
        buffer[2] = 0x80 | ((ucs_character >> 0) & 0x3F);
        return 3;
    }
    else if (ucs_character <= 0x1FFFFF) {
        buffer[0] = 0xF0 | (ucs_character >> 18);
        buffer[1] = 0x80 | ((ucs_character >> 12) & 0x3F);
        buffer[2] = 0x80 | ((ucs_character >> 6) & 0x3F);
        buffer[3] = 0x80 | ((ucs_character >> 0) & 0x3F);
        return 4;
    }
    else if (ucs_character <= 0x3FFFFFF) {
        buffer[0] = 0xF8 | (ucs_character >> 24);
        buffer[1] = 0x80 | ((ucs_character >> 18) & 0x3F);
        buffer[2] = 0x80 | ((ucs_character >> 12) & 0x3F);
        buffer[3] = 0x80 | ((ucs_character >> 6) & 0x3F);
        buffer[4] = 0x80 | ((ucs_character >> 0) & 0x3F);
        return 5;
    }
    else if (ucs_character <= 0x7FFFFFFF) {
        buffer[0] = 0xFC | (ucs_character >> 30);
        buffer[1] = 0x80 | ((ucs_character >> 24) & 0x3F);
        buffer[2] = 0x80 | ((ucs_character >> 18) & 0x3F);
        buffer[3] = 0x80 | ((ucs_character >> 12) & 0x3F);
        buffer[4] = 0x80 | ((ucs_character >> 6) & 0x3F);
        buffer[5] = 0x80 | ((ucs_character >> 0) & 0x3F);
        return 6;
    }
    return 1;
}


static const long double pow10_matrix[101] = {
                0,  1.0e+1,  1.0e+2,  1.0e+3,  1.0e+4,  1.0e+5,
                    1.0e+6,  1.0e+7,  1.0e+8,  1.0e+9,  1.0e+10,
                    1.0e+11, 1.0e+12, 1.0e+13, 1.0e+14, 1.0e+15,
                    1.0e+16, 1.0e+17, 1.0e+18, 1.0e+19, 1.0e+20,
                    1.0e+21, 1.0e+22, 1.0e+23, 1.0e+24, 1.0e+25,
                    1.0e+26, 1.0e+27, 1.0e+28, 1.0e+29, 1.0e+30,
                    1.0e+31, 1.0e+32, 1.0e+33, 1.0e+34, 1.0e+35,
                    1.0e+36, 1.0e+37, 1.0e+38, 1.0e+39, 1.0e+40,
                    1.0e+41, 1.0e+42, 1.0e+43, 1.0e+44, 1.0e+45,
                    1.0e+46, 1.0e+47, 1.0e+48, 1.0e+49, 1.0e+50,
                    1.0e+51, 1.0e+52, 1.0e+53, 1.0e+54, 1.0e+55,
                    1.0e+56, 1.0e+57, 1.0e+58, 1.0e+59, 1.0e+60,
                    1.0e+61, 1.0e+62, 1.0e+63, 1.0e+64, 1.0e+65,
                    1.0e+66, 1.0e+67, 1.0e+68, 1.0e+69, 1.0e+70,
                    1.0e+71, 1.0e+72, 1.0e+73, 1.0e+74, 1.0e+75,
                    1.0e+76, 1.0e+77, 1.0e+78, 1.0e+79, 1.0e+80,
                    1.0e+81, 1.0e+82, 1.0e+83, 1.0e+84, 1.0e+85,
                    1.0e+86, 1.0e+87, 1.0e+88, 1.0e+89, 1.0e+90,
                    1.0e+91, 1.0e+92, 1.0e+93, 1.0e+94, 1.0e+95,
                    1.0e+96, 1.0e+97, 1.0e+98, 1.0e+99, 1.0e+100
};


struct char1 {
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
};

static const char1 * const matrix = (char1 *)"000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232233234235236237238239240241242243244245246247248249250251252253254255256257258259260261262263264265266267268269270271272273274275276277278279280281282283284285286287288289290291292293294295296297298299300301302303304305306307308309310311312313314315316317318319320321322323324325326327328329330331332333334335336337338339340341342343344345346347348349350351352353354355356357358359360361362363364365366367368369370371372373374375376377378379380381382383384385386387388389390391392393394395396397398399400401402403404405406407408409410411412413414415416417418419420421422423424425426427428429430431432433434435436437438439440441442443444445446447448449450451452453454455456457458459460461462463464465466467468469470471472473474475476477478479480481482483484485486487488489490491492493494495496497498499500501502503504505506507508509510511512513514515516517518519520521522523524525526527528529530531532533534535536537538539540541542543544545546547548549550551552553554555556557558559560561562563564565566567568569570571572573574575576577578579580581582583584585586587588589590591592593594595596597598599600601602603604605606607608609610611612613614615616617618619620621622623624625626627628629630631632633634635636637638639640641642643644645646647648649650651652653654655656657658659660661662663664665666667668669670671672673674675676677678679680681682683684685686687688689690691692693694695696697698699700701702703704705706707708709710711712713714715716717718719720721722723724725726727728729730731732733734735736737738739740741742743744745746747748749750751752753754755756757758759760761762763764765766767768769770771772773774775776777778779780781782783784785786787788789790791792793794795796797798799800801802803804805806807808809810811812813814815816817818819820821822823824825826827828829830831832833834835836837838839840841842843844845846847848849850851852853854855856857858859860861862863864865866867868869870871872873874875876877878879880881882883884885886887888889890891892893894895896897898899900901902903904905906907908909910911912913914915916917918919920921922923924925926927928929930931932933934935936937938939940941942943944945946947948949950951952953954955956957958959960961962963964965966967968969970971972973974975976977978979980981982983984985986987988989990991992993994995996997998999";


static PyObject * __dict__ = PyUnicode_FromString("__dict__");

static PyObject * __indent = PyUnicode_FromString("indent");
static const Py_hash_t __indent_hash = PyObject_Hash(__indent);

static PyObject * __non_string_key = PyUnicode_FromString("non_string_key");
static const Py_hash_t __non_string_key_hash = PyObject_Hash(__non_string_key);

static PyObject * __as_string = PyUnicode_FromString("as_string");
static const Py_hash_t __as_string_hash = PyObject_Hash(__as_string);

template<typename Type>
struct Spaces {
    Type a[4] {' ', ' ', ' ', ' '};
};

template<typename Type>
struct False {
    Type a[5] = {'f', 'a', 'l', 's', 'e'};
};

template<typename Type>
struct True {
    Type a[4] = {'t', 'r', 'u', 'e'};
};

template<typename Type>
struct Null {
    Type a[4] = {'n', 'u', 'l', 'l'};
};



static void * mem_char_ = nullptr;
static size_t mem_size_ = 1024 * 16;


static const size_t SIZE_CACHE = 1024;
static const size_t MAX_SIZE_CACHE_STR = 64;


struct Cache {
    char ptr[MAX_SIZE_CACHE_STR + 2];
    Py_hash_t hash = 0;

    inline void set_size(const char v) {
        ptr[MAX_SIZE_CACHE_STR] = v;
    }

    inline void set_kind(const char v) {
        ptr[MAX_SIZE_CACHE_STR + 1] = v;
    }


    inline const size_t size() const {
        return ptr[MAX_SIZE_CACHE_STR];
    }

    inline const size_t kind() const {
        return ptr[MAX_SIZE_CACHE_STR + 1];
    }
};


Cache * mem_cache_string_ = nullptr;


int d2s_buffered_n(double f, char* result);
int check_js_dataclass(PyObject * v);
PyObject * check_field(PyObject * key, PyObject * value, int i, int & error_handler);
PyObject * check_object(PyObject * obj);


static const char simbols[93] = {
          0,   0,   0,   0,   0,   0,   0,   0,  98, 116,
        110,   0, 102, 114,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,  34,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,  92,
};


// error 0, stop 1, float 2, int 3, exp 4, exp sign 5

static const char simbols_int[256] = {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, // 3
        0, 0, 0, 5, 1, 5, 2, 0, 3, 3, // 4
        3, 3, 3, 3, 3, 3, 3, 3, 0, 0, // 5
        0, 0, 0, 0, 0, 0, 0, 0, 0, 4, // 6
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, // 9
        0, 4, 0, 0, 0, 0, 0, 0, 0, 0, // 10
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 11
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, // 12
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 13
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 14
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 15
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 17
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 18
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 19
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 21
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 22
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 23
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 24
        0, 0, 0, 0, 0, 0,             // 25

};




template<typename Type>
class BaseDump {
protected:
    Type * str = nullptr;
    Type * start_str = nullptr;
    Type * end_str = nullptr;
    size_t buffer_size = 0;

    size_t indent_size = 0;
    size_t indent_depth = 0;

    size_t recursion_depth = 0;
    size_t max_recursion_depth = 1024;

    bool non_string_key = false;
    bool using_cache_string = false;

    bool has_error = false;


    virtual inline void indent() {
        *str++ = ent;

        Spaces<Type> _spaces = Spaces<Type>();
        Spaces<Type> * ptr = (Spaces<Type> *)str;

        size_t x = 0;
        for (; x < indent_depth; x += 4) {
            *ptr++ = _spaces;
        }

        str += indent_depth;
    }

    inline void inc_indent() {
        indent_depth += indent_size;
    }

    inline void dec_indent(){
        indent_depth -= indent_size;
    }

    void add_list_indent(PyObject * s) {
        const size_t size = ((PyListObject *)s)->ob_base.ob_size;

        *str++ = open_list;
        if (size == 0) {
            *str++ = close_list;
            return;
        }

        check((size + indent_depth + 4) * 12);

        inc_indent();
        indent();

        size_t i = 0;
        while (i < size) {
            if (i > 0) {
                *str++ = comma;
                indent();
            }
            main_dump(PyList_GetItem(s, i++));
            if (has_error) return;
        }

        dec_indent();
        indent();
        *str++ = close_list;
    }

    void add_tuple_indent(PyObject * s) {
        const size_t size = ((PyTupleObject *)s)->ob_base.ob_size;

        *str++ = open_list;
        if (size == 0) {
            *str++ = close_list;
            return;
        }

        check((size + indent_depth + 4) * 12);

        inc_indent();
        indent();

        size_t i = 0;
        while (i < size) {
            if (i > 0) {
                *str++ = comma;
                indent();
            }
            main_dump(PyTuple_GetItem(s, i++));
            if (has_error) return;
        }

        dec_indent();
        indent();
        *str++ = close_list;
    }

    void add_dict_indent(PyObject * s) {
        PyObject *key, *value;
        const size_t size = PyDict_Size(s);
        check((size + indent_depth + 4) * 16);

        *str++ = open_obj;
        if (size == 0) {
            *str++ = close_obj;
            return;
        }

        inc_indent();
        indent();

        Py_ssize_t i = 0;
        while (PyDict_Next(s, &i, &key, &value)) {
            if (i > 1) {
                *str++ = comma;
                indent();
            }
            add_obj_key(key);

            *str++ = colon;
            *str++ = space;

            main_dump(value);
            if (has_error) return;
        }

        dec_indent();
        indent();
        *str++ = close_obj;
    }

    void add_class_indent(PyObject * s, const int js_dataclass_index = -1) {
        PyObject *key, *value;
        const size_t size = PyDict_Size(s);
        check((size + indent_depth + 4) * 16);

        *str++ = open_obj;
        if (size == 0) {
            *str++ = close_obj;
            return;
        }

        inc_indent();
        indent();

        const char * key_name = nullptr;
        int error_handler = 0;

        Py_ssize_t i = 0;
        while (PyDict_Next(s, &i, &key, &value)) {
            obj_class_checker;
            if (i > 1) {
                *str++ = comma;
                indent();
            }
            add_obj_key(key);
            *str++ = colon;
            *str++ = space;
            main_dump(value);
            if (has_error) return;
        }

        dec_indent();
        indent();
        *str++ = close_obj;
    }


    void set_error(const char * msg) {
        PyErr_Format(PyExc_ValueError, "%s", msg);
        has_error = true;
    }

    inline void realloc_mem(const size_t n) {
        size_t old_pos = str - start_str;
        buffer_size = (buffer_size + n) * 2;
        buffer_size = (buffer_size / 1024 + 1) * 1024;

        mem_char_ = PyMem_RawRealloc(mem_char_, buffer_size);

        mem_size_ = buffer_size;

        start_str = (Type *)mem_char_;
        str = start_str + old_pos;
        end_str = start_str + (buffer_size / sizeof(Type));
    }

    inline void init_cache_mem() {
        if (using_cache_string && mem_cache_string_ == nullptr) {
            mem_cache_string_ = (Cache *)PyMem_RawMalloc(sizeof(Cache) * SIZE_CACHE);
            for (int i = 0; i < SIZE_CACHE; i++) {
                *(mem_cache_string_ + i) = Cache();
            }
        }
    }

    inline void init_mem() {
        if (mem_char_ == nullptr) {
            mem_char_ = PyMem_RawMalloc(mem_size_);
        }
        start_str = (Type *)mem_char_;
        buffer_size = mem_size_;

        str = start_str;
        end_str = start_str + (buffer_size / sizeof(Type));
    }

    inline void check(const size_t n) {
        if ((str + n) > end_str) {
            realloc_mem(n);
        }
    }

    inline void add_charn(const uint32_t c, const size_t n) {
        *(str + n) = c;
    }

    inline const size_t char_check(const uint32_t c, Type * out) const {
        *out++ = slash;

        const uint32_t v = *(simbols + c);
        if (v) {
            *out = v;
            return 2;
        } else {
            *out++ = 'u';
            DEC_TO_HEX<Type>(c, out);
            return 6;
        }
    }

    int add_int(PyObject * s) {
        int size = ((PyLongObject *)s)->ob_base.ob_size;
        digit * d = ((PyLongObject *)s)->ob_digit;
        uint64_t val = 0;

        if (size < 0) {
            size = -size;
            *str++ = df;

        } else if (size == 0) {
            *str++ = int_0;
            return 0;
        }

        for (size_t i = 0; i < size; ++i) {
            val += (uint64_t)d[i] << (PyLong_SHIFT * i);
        }

        check(30);

        return write_int(val);
    };

    virtual inline const size_t write_int(uint64_t val) {
        const char1 * c;

        if (val < 10) {
            *str++ = val + '0';
            return ((val != 0) ? 1 : 0);

        } else if (val < 100) {
            c = matrix + (val % 100);
            *str++ = c->c2;
            *str++ = c->c3;
            return 2;

        } else if (val < 1000) {
            c = matrix + (val % 1000);
            *str++ = c->c1;
            *str++ = c->c2;
            *str++ = c->c3;
            return 3;
        }

        const size_t len = GetLenLong4(val);

        int x = len - 1;
        while (x > 1) {
            c = matrix + (val % 1000);
            add_charn(c->c3, x--);
            add_charn(c->c2, x--);
            add_charn(c->c1, x--);
            val /= 1000;
        }

        switch (x) {
            case 0:
                add_charn((matrix + (val % 1000))->c3, x);
                break;
            case 1:
                c = matrix + (val % 1000);
                add_charn(c->c3, x--);
                add_charn(c->c2, x);
        }

        str += len;
        return len;
    };

    virtual inline void add_float(PyObject * s) {
        double v = PyFloat_AsDouble(s);

        check(30);

        if (isinf(v)) {
            if (v > 0) {
                const char * I = "Infinity";
                while(*I) *str++ = *I++;

            } else {
                const char * I = "-Infinity";
                while(*I) *str++ = *I++;
            }
            return;

        } else if (isnan(v)) {
            const char * I = "NaN";
            while(*I) *str++ = *I++;
            return;

        } else if (v < 0) {
            *str++ = df;
            v = -v;

        } else if (!v) {
            *str++ = int_0;
            return;
        }

        if (v >= 1.0e14 || v <= 1.0e-5) {
            char buff[30];
            const size_t size = d2s_buffered_n(v, buff);
            for (int i = 0; i < size; i++) {
                *str++ = buff[i];
            }
            return;
        }

        const size_t r = v;

        const long long len = 18 - write_int(v);
        *str++ = int_point;

        const char1 * c;



        for (int i = 3; i < len; i += 3) {
            size_t d = v * pow10_matrix[i];
            c = matrix + (d % 1000);
            *str++ = c->c1;
            *str++ = c->c2;
            *str++ = c->c3;
        }

        if (*(str - 2) == int_0 && *(str - 3) == int_0) {
            str -= 2;
            while (*str == int_0 && *(str - 1) != int_point) {
                str--;
            }
            str++;
        }

        if (*(str - 1) == int_0 && *(str - 2) != int_point) {
            str--;
        }
    }

    void add_list(PyObject * s) {
        const size_t size = ((PyListObject *)s)->ob_base.ob_size;

        *str++ = open_list;
        if (size == 0) {
            *str++ = close_list;
           return;
        }

        check(size * 8);

        PyObject * item = nullptr;

        size_t i = 0;
        while (i < size) {
            main_dump(PyList_GetItem(s, i++));
            if (has_error) return;
            *str++ = comma;
        }

        *(str - 1) = close_list;
    }

    void add_tuple(PyObject * s) {
        const size_t size = ((PyTupleObject *)s)->ob_base.ob_size;

        *str++ = open_list;
        if (size == 0) {
            *str++ = close_list;
            return;
        }
        check(size * 8 + 2);

        size_t i = 0;
        while (i < size) {
            main_dump(PyTuple_GetItem(s, i++));
            if (has_error) return;
            *str++ = comma;
        }

        *(str - 1) = close_list;
    }

    void add_dict(PyObject * s) {
        PyObject *key, *value;
        const size_t size = PyDict_Size(s);

        check(size * 14 + 2);
        *str++ = open_obj;

        if (size == 0) {
            *str++ = close_obj;
            return;
        }

        Py_ssize_t i = 0;
        while (PyDict_Next(s, &i, &key, &value)) {
            add_obj_key(key);
            *str++ = colon;
            main_dump(value);
            if (has_error) return;
            *str++ = comma;
        }

        *(str - 1) = close_obj;
    }

    void add_class(PyObject * s, const int js_dataclass_index = -1) {
        PyObject *key, *value;
        const size_t size = PyDict_Size(s);
        check(size * 14 + 2);
        *str++ = open_obj;

        if (size == 0) {
            *str++ = close_obj;
            return;
        }

        const char * key_name = nullptr;
        int error_handler = 0;

        Py_ssize_t i = 0;
        while (PyDict_Next(s, &i, &key, &value)) {
            obj_class_checker;

            add_obj_key(key);
            *str++ = colon;
            main_dump(value);
            if (has_error) return;
            *str++ = comma;
        }

        *(str - 1) = close_obj;
    }

    template <class T>
    inline const size_t string_serialization8_16(
            Type * out,
            const T * source,
            const size_t size
            ) const {

        const Type * const out_start = out;
        const T * const source_end = source + size;

        *out++ = quot;
        while (true) {
            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;

            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;

            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;

            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;
            str_serialization_unit;
        }
        *out++ = quot;

        return out - out_start;
    }

    template <class T>
    inline const size_t string_serialization32(
            Type * out,
            const T * source,
            const size_t size
            ) const {

        const Type * const out_start = out;
        const T * const source_end = source + size;

        const uint32_t max_2_byte = 0xffff;

        uint32_t h = 0;
        uint32_t l = 0;

        *out++ = quot;
        while (true) {
            str_serialization_32
            str_serialization_32
            str_serialization_32
            str_serialization_32

            str_serialization_32
            str_serialization_32
            str_serialization_32
            str_serialization_32
        }
        *out++ = quot;

        return out - out_start;
    }

    inline void add_string_with_cache(PyObject * s) {
        Py_hash_t hash = PyObject_Hash(s);

        if (hash < 0) {
            hash = -hash;
        }

        const size_t cache_index = hash % SIZE_CACHE;
        Cache * const cache = mem_cache_string_ + cache_index;

        if (cache->hash == hash && cache->kind() == sizeof(Type)) {
            check(cache->size());
            memcpy(str, cache->ptr, cache->size() * sizeof(Type));
            str += cache->size();
            return;
        }

        const Type * const start = str;

        size_t size = 0;
        add_string(s, size);

        if (size * sizeof(Type) < MAX_SIZE_CACHE_STR) {
            memcpy(cache->ptr, start, size * sizeof(Type));
            cache->set_size(size);
            cache->set_kind(sizeof(Type));
            cache->hash = hash;
        }
    }

    virtual inline void add_string(PyObject * s, size_t & len) {
        const size_t size = PyUnicode_GetLength(s);
        if (!size) {
            *str++ = quot;
            *str++ = quot;
            return;
        }

        const size_t kind = PyUnicode_KIND(s);
        check(size * kind * 2);

        if (kind == 1) {
            const uint8_t * source = PyUnicode_1BYTE_DATA(s);
            if (!source) {
                return set_error("not valid utf-8");
            }
            len = string_serialization8_16<uint8_t>(str, source, size);

        } else if (kind == 2) {
            const uint16_t * source = PyUnicode_2BYTE_DATA(s);
            if (!source) {
                return set_error("not valid utf-16");
            }
            len = string_serialization8_16<uint16_t>(str, source, size);

        } else if (kind == 4) {
            const uint32_t * source = PyUnicode_4BYTE_DATA(s);
            if (!source) {
                return set_error("not valid utf-32");
            }
            if (sizeof(Type) == 4) {
                len = string_serialization8_16<uint32_t>(str, source, size);
            } else {
                len = string_serialization32<uint32_t>(str, source, size);
            }
        }
        str += len;
    }

    inline void add_obj_key(PyObject * s) {
        if (PyUnicode_Check(s)) {
            if (!using_cache_string) {
                size_t i = 0;
                return add_string(s, i);

            } else {
                return add_string_with_cache(s);

            }
        } else if (non_string_key) {
            *str++ = quot;

            if (PyBool_Check(s)) {
                add_bool(s);
            }
            else if (PyLong_Check(s)) {
                add_int(s);
            }
            else if (PyFloat_Check(s)) {
                add_float(s);
            }
            else if (s == Py_None) {
                add_null();

            } else {
                return set_error("non serealisebl type");
            }

            *str++ = quot;

        } else {
            return set_error("key need be str");
        }
    }

    inline void add_null() {
        check(6);
        *((Null<Type> *)str) = Null<Type>();
        str += 4;
    }

    inline void add_bool(PyObject * s) {
        check(7);
        if (s == Py_True) {
            *((True<Type> *)str) = True<Type>();
            str += 4;

        } else {
            *((False<Type> *)str) = False<Type>();
            str += 5;
        }
    }

    virtual void main_dump(PyObject * o) {
        if (recursion_depth > max_recursion_depth) {
            has_error = true;
            return set_error("Maximum recursion depth");
        }

        recursion_depth++;

        if (PyUnicode_Check(o)) {
            if (!using_cache_string) {
                size_t i = 0;
                add_string(o, i);

            } else {
                add_string_with_cache(o);

            }
        }

        else if (PyBool_Check(o)) {
            add_bool(o);
        }

        else if (PyLong_Check(o)) {
            add_int(o);
        }

        else if (PyFloat_Check(o)) {
            add_float(o);
        }

        else if (PyList_Check(o)) {
            add_list(o);
        }

        else if (PyDict_Check(o)) {
            add_dict(o);
        }

        else if (o == Py_None) {
            add_null();
        }

        else if (PyObject_HasAttr(o, __dict__) && !(PyFunction_Check(o) || PyMethod_Check(o))) {
            PyObject * d = PyObject_GenericGetDict(o, nullptr);
            add_class(d, check_js_dataclass(o));
        }

        else if (PyTuple_Check(o)) {
            add_tuple(o);
        }

        else {
            set_error("not dumped type");
        }

        recursion_depth--;
    }

public:
    virtual inline PyObject * dump(PyObject * o) {
        main_dump(o);
        if (has_error) return nullptr;
        return PyUnicode_FromKindAndData(sizeof(Type), start_str, str - start_str);
    }

    BaseDump(){
        init_mem();
        init_cache_mem();
    }

    ~BaseDump() {
        if (mem_size_ > 1024 * 256) {
            PyMem_RawFree(mem_char_);
            mem_char_ = nullptr;
        }
        if (mem_size_ / (str - start_str) >= 4) {
            mem_size_ = mem_size_ / 2;
        }
    }
};


class dumper : public BaseDump<uint16_t> {

public:
    dumper(bool _non_string_key) {
        non_string_key = _non_string_key;
    }

    dumper(){}
};


class dumper__ : public BaseDump<uint32_t> {

public:
    dumper__(bool _non_string_key) {
        non_string_key = _non_string_key;
    }

    dumper__(){}
};


class dumper_indent : public dumper__ {
private:
    void main_dump(PyObject * o) override {
        if (recursion_depth > max_recursion_depth) {
            has_error = true;
            return set_error("Maximum recursion depth");
        }

        recursion_depth++;

        if (PyUnicode_Check(o)) {
            if (!using_cache_string) {
                size_t i = 0;
                add_string(o, i);

            } else {
                add_string_with_cache(o);

            }
        }
        else if (PyBool_Check(o)) {
            add_bool(o);
        }
        else if (PyLong_Check(o)) {
            add_int(o);
        }
        else if (PyFloat_Check(o)) {
            add_float(o);
        }
        else if (PyList_Check(o)) {
            add_list_indent(o);
        }
        else if (PyDict_Check(o)) {
            add_dict_indent(o);
        }
        else if (o == Py_None) {
            add_null();
        }
        else if (PyObject_HasAttr(o, __dict__) && !(PyFunction_Check(o) || PyMethod_Check(o))) {
            PyObject * d = PyObject_GenericGetDict(o, nullptr);
            add_class_indent(d, check_js_dataclass(o));
        }
        else if (PyTuple_Check(o)) {
            add_tuple_indent(o);
        }
        else {
            set_error("not dumped type");
        }

        recursion_depth--;
    }

public:
    dumper_indent(bool _non_string_key, const size_t _indent_size) {
        non_string_key = _non_string_key;
        indent_size = _indent_size;
    }
};


class dumper_bytes : public BaseDump<uint8_t> {
protected:
    inline void add_string(PyObject * s, size_t & len)
    override
    {
        const uint8_t * source = (const uint8_t *)PyUnicode_AsUTF8(s);
        if (!source) {
            return set_error("not valid utf-8");
        }
        size_t size = 0;
        const size_t kind = PyUnicode_KIND(s);

        if (kind != 1) {
            size = ((PyCompactUnicodeObject *)s)->utf8_length;
        } else {
            size = PyUnicode_GetLength(s);
        }

        if (!size) {
            *str++ = quot;
            *str++ = quot;
            return;
        }

        check(size * kind * 2);

        len = string_serialization8_16<uint8_t>(str, source, size);
        str += len;
    }

    inline const size_t write_int(uint64_t val)
    override {
        const char1 * c;

        if (val < 10) {
            *str++ = (uint8_t)(val + '0');
            return ((val) ? 1 : 0);

        } else if (val < 100) {
            c = matrix + (val % 100);
            *str++ = c->c2;
            *str++ = c->c3;
            return 2;

        } else if (val < 1000) {
            c = matrix + (val % 1000);
            *((char1 *)str) = *c;
            str += 3;
            return 3;
        }

        const size_t len = GetLenLong4(val);

        long long x = len - 3;
        while (x > -1) {
            *((char1 *)(str + x)) = *(matrix + (val % 1000));
            //memcpy(str + x, matrix + (val % 1000), 3);
            val /= 1000;
            x -= 3;
        }

        switch (len % 3) {
            case 1:
                *str = (matrix + (val % 1000))->c3;
                break;
            case 2:
                c = matrix + (val % 1000);
                *(str + 1) = c->c3;
                *str = c->c2;
        }

        str += len;
        return len;
    };

    inline void add_float(PyObject * s)
    override
    {
        double v = PyFloat_AsDouble(s);
        check(30);

        if (isinf(v)) {
            if (v > 0) {
                const char * I = "Infinity";
                while(*I) *str++ = *I++;

            } else {
                const char * I = "-Infinity";
                while(*I) *str++ = *I++;

            }
            return;

        } else if (isnan(v)) {
            const char * I = "NaN";
            while(*I) *str++ = *I++;
            return;

        } else if (v < 0) {
            *str++ = df;
            v = -v;

        } else if (!v) {
            *str++ = int_0;
            return;
        }

        if (v >= 1.0e14 || v <= 1.0e-5) {
            str += d2s_buffered_n(v, (char *)str);
            return;
        }

        const size_t r = (size_t)v;

        const long long len = 17 - write_int(r);
        *str++ = int_point;

        const char1 * c = nullptr;

        size_t d = 0;
        long long i = 3;
        const long long imax = len - len % 3 + 3;

        while (i < imax) {
            d = (size_t)(v * pow10_matrix[i]) % 1000;
            c = matrix + d;
            *((char1 *)str) = *c;
            str += 3;
            i += 3;
        }

        d = (size_t)(v * pow10_matrix[i]) % 1000;
        c = matrix + d;

        switch (len % 3) {
            case 0:
                break;
            case 1:
                *str++ = c->c1;
                break;
            case 2:
                *str++ = c->c1;
                *str++ = c->c2;
        }

        if (*(str - 2) == int_0 && *(str - 3) == int_0) {
            str -= 2;
            while (*str == int_0 && *(str - 1) != int_point) {
                str--;
            }
            str++;
        }

        if (*(str - 1) == int_0 && *(str - 2) != int_point) {
            str--;
        }
    }

    inline void add_float_(PyObject * s)
    //override
    {
        long double v = PyFloat_AsDouble(s);
        check(30);

    }




public:
    inline PyObject * dump(PyObject * o) override {
        main_dump(o);
        if (has_error) return nullptr;
        return PyBytes_FromStringAndSize((const char *)start_str, str - start_str);
    }

    dumper_bytes(bool _non_string_key) {
        non_string_key = _non_string_key;
    }

    dumper_bytes() {}
};


class dumper_indent_bytes : public dumper_bytes {
private:
    void main_dump(PyObject * o) override {
        if (recursion_depth > max_recursion_depth) {
            has_error = true;
            return set_error("Maximum recursion depth");
        }

        recursion_depth++;

        if (PyUnicode_Check(o)) {
            if (!using_cache_string) {
                size_t i = 0;
                add_string(o, i);

            } else {
                add_string_with_cache(o);

            }
        }
        else if (PyBool_Check(o)) {
            add_bool(o);
        }
        else if (PyLong_Check(o)) {
            add_int(o);
        }
        else if (PyFloat_Check(o)) {
            add_float(o);
        }
        else if (PyList_Check(o)) {
            add_list_indent(o);
        }
        else if (PyDict_Check(o)) {
            add_dict_indent(o);
        }
        else if (o == Py_None) {
            add_null();
        }
        else if (PyObject_HasAttr(o, __dict__) && !(PyFunction_Check(o) || PyMethod_Check(o))) {
            PyObject * d = PyObject_GenericGetDict(o, nullptr);
            add_class_indent(d, check_js_dataclass(o));
        }
        else if (PyTuple_Check(o)) {
            add_tuple_indent(o);
        }
        else {
            set_error("not dumped type");
        }

        recursion_depth--;
    }

public:
    dumper_indent_bytes(bool _non_string_key, const size_t _indent_size)  {
        non_string_key = _non_string_key;
        indent_size = _indent_size;
    }

};



template<class Type>
class BaseParser {
protected:
    const Type * data_start = nullptr;
    const Type * data = nullptr;
    const Type * data_end = nullptr;
    const Type * find_str = nullptr;

    size_t size_source_string = 0;

    bool has_error = false;

    PyObject * set_error(const char * msg) {
        if (has_error) {
            return nullptr;
        } else {
            has_error = true;
        }
        PyErr_Format(PyExc_ValueError, "%s, char pos = %d", msg, data - data_start);
        return nullptr;
    }

    inline void next_char_not_space() {
        data++;
        while (*data <= space && *data) {
            data++;
        }
    }

    inline const double fast_atod(const Type * str, const size_t len) const {
        double r = 0.0;
        double f = 0.0;
        const Type * const end = str + len;
        int n = 0;

        while (*str >= int_0) {
            r = (r * 10.0) + (*str++ - int_0);
        }

        ++str;
        while (str < end) {
            f = (f * 10.0) + (*str++ - int_0);
            ++n;
        }

        r += f / pow10_matrix[n];

        return r;
    }

    inline const double add_exponent(double r, int e, bool sig) const {
        if (!e) { return r; }

        double exp = 0.0;
        if (e <= 100) {
            exp = pow10_matrix[e];
        } else {
            exp = pow(10.0, e);
        }

        if (sig) {
            return r * exp;
        } else {
            return r / exp;
        }
    }

    inline const double fast_atod_and_exponent(const Type * str, const size_t len, bool is_float) const {
        double r = 0;
        double f = 0.0;
        const Type * const end = str + len;
        int n = 0;

        if (is_float) {
            while (*str != '.') {
                r = (r * 10.0) + (*str++ - int_0);
            }

            ++str;
            while (*str <= '9') {
                f = (f * 10.0) + (*str++ - int_0);
                ++n;
            }
            r += f / pow10_matrix[n];

        } else {
            while (*str <= '9') {
                r = (r * 10.0) + (*str++ - int_0);
                n++;
            }
        }

        ++str;
        uint64_t e = 0;
        if (*str == '-') {
            while (++str < end) {
                e = e * 10 + (*str - int_0);
            }
            return add_exponent(r, e, false);

        } else if (*str == '+') {
            while (++str < end) {
                e = e * 10 + (*str - int_0);
            }
            return add_exponent(r, e, true);

        } else {
            while (str < end) {
                e = e * 10 + (*str++ - int_0);
            }
            return add_exponent(r, e, true);
        }
    }

    inline const uint64_t fast_atoll(const Type * str, const size_t len) const {
        uint64_t val = 0;
        const Type * const end = str + len;

        while (str < end) {
            val = val * 10 + (*str++ - int_0);
        }
        return val;
    }

    inline const uint32_t fast_atoi(const Type * str, const size_t len) const {
        uint32_t val = 0;
        const Type * const end = str + len;

        while (str < end) {
            val = val * 10 + (*str++ - int_0);
        }
        return val;
    }

    PyObject *obj_parser() {
        PyObject * object = PyDict_New(), * key, * value;
        bool first = true;

        while (*data) {
            next_char_not_space();
            if (*data != close_obj) {
                if (!first) {
                    if (*data == comma) {
                        next_char_not_space();

                    } else {
                        return set_error("error obj token ','");
                    }

                } else {
                    first = false;
                }

            } else {
                return check_object(object);
            }

            if (*data++ != quot) { return set_error("error start key obj token '\"'"); }

            key = str_parser();
            if (key == nullptr) { return key; }

            next_char_not_space();
            if (*data != colon) { return set_error("error obj token ':'"); }

            next_char_not_space();
            if (*data == comma) { return set_error("error obj token ','"); }

            value = main_pars();
            if (value != nullptr) {
                PyDict_SetItem(object, key, value);
                Py_DECREF(value);
                Py_DECREF(key);

            } else {
                Py_DECREF(key);
                return value;
            }
        }
        return set_error("obj pars error");
    }

    PyObject *list_parser() {
        PyObject *object = PyList_New(0), * value;
        bool first = true;

        while (*data) {
            next_char_not_space();
            if (*data != close_list) {
                if (!first) {
                    if (*data == comma) {
                        next_char_not_space();
                        if (*data == close_list) { return set_error("error list token ','"); }
                    } else { return set_error("error list token ','"); }
                } else { first = false; }
            } else { return object; }

            value = main_pars();
            if (value != nullptr) {
                PyList_Append(object, value);
                Py_DECREF(value);

            } else { return value; }
        }
        return set_error("error list token ']'");
    }

    virtual inline PyObject * set_str(const void * buff, const size_t size, const int kind_) const {
        return PyUnicode_FromKindAndData(kind_, buff, size);
    }

    inline const uint32_t char_check(const Type ** source) {
        uint32_t v = 0;
        const Type * const p = *source;

        if (*p >= 'b') {
            switch (*p) {
                case 'n':
                    return ent;
                case 't':
                    return tab;
                case 'u':
                    v = HEX_TO_DEC<Type>(p + 1, 4);
                    (*source) += 4;
                    return v;
                case 'r':
                    return sr;
                case 'f':
                    return sf;
                case 'b':
                    return sb;
            }

        } else {
            switch (*p) {
                case slash:
                case quot:
                case '/':
                    return *p;
            }
        }

        set_error("Unrecognized escape sequence when decoding 'string'");
        return 0;


    }

    inline const uint32_t char_check_2_byte(const Type ** p) {
        uint32_t v = char_check(p);

        const int c = (v << 10) - 0x35fdc00;
        if (c > 0 && c <= 138240) {
            (*p) += 2;
            v = c + char_check(p);
        }
        (*p) ++;
        return v;
    }

    template<class T>
    inline PyObject * string_writer(
            const T * const buff_start,
            T * buff,
            const Type * source,
            const size_t n
            ) {

        const Type * ptr = source;
        const Type ** const p = &ptr;

        while (true) {
            str_escape_pars_and_check(0);
            str_escape_pars_and_check(1);
            str_escape_pars_and_check(2);
            str_escape_pars_and_check(3);

            str_escape_pars_and_check(4);
            str_escape_pars_and_check(5);
            str_escape_pars_and_check(6);
            str_escape_pars_and_check(7);

            str_escape_pars_and_check(8);
            str_escape_pars_and_check(9);
            str_escape_pars_and_check(10);
            str_escape_pars_and_check(11);

            str_escape_pars_and_check(12);
            str_escape_pars_and_check(13);
            str_escape_pars_and_check(14);
            str_escape_pars_and_check(15);

            buff += 16;
            ptr += 16;
        }
    }


    virtual PyObject * str_escape_parser(const size_t size) {
        if (mem_char_ == nullptr ) {
            mem_char_ = PyMem_RawMalloc(mem_size_);
        }
        if (mem_size_ < size_source_string * 4) {
            mem_size_ = size_source_string * 5;
            mem_char_ = PyMem_Realloc(mem_char_, mem_size_);
        }

        uint32_t * buff = (uint32_t *)mem_char_;
        const uint32_t * const buff_start = buff;
        const Type * target_str = find_str;

        for (size_t x = 0; x < size; x++) {
            *buff++ = *target_str++;
        }
        return string_writer<uint32_t>(buff_start, buff, target_str, size);
    }

    virtual PyObject * str_parser() {
        find_str = data;

        const Type * d = data;
        const Type * d_start = d;

        size_t size = 0;

        while (true) {
            str_pars_and_check(0, sizeof(Type))
            str_pars_and_check(1, sizeof(Type))
            str_pars_and_check(2, sizeof(Type))
            str_pars_and_check(3, sizeof(Type))

            str_pars_and_check(4, sizeof(Type))
            str_pars_and_check(5, sizeof(Type))
            str_pars_and_check(6, sizeof(Type))
            str_pars_and_check(7, sizeof(Type))

            str_pars_and_check(8, sizeof(Type))
            str_pars_and_check(9, sizeof(Type))
            str_pars_and_check(10, sizeof(Type))
            str_pars_and_check(11, sizeof(Type))

            str_pars_and_check(12, sizeof(Type))
            str_pars_and_check(13, sizeof(Type))
            str_pars_and_check(14, sizeof(Type))
            str_pars_and_check(15, sizeof(Type))

            d += 16;
        }
    }

    inline PyObject * get_big_int(const Type * str, size_t size) {
        char * buff = (char *)PyMem_Malloc(size + 1);

        for (int i = 0; i < size; i++) {
            buff[i] = *str++;
        }
        buff[size] = 0;

        PyObject * v = PyLong_FromString(buff, nullptr, 10);
        PyMem_Free(buff);

        return v;
    }

    inline PyObject * get_integer(
            const bool is_float,
            const bool is_negative,
            const bool is_exponent,
            const bool start_zero
            ) {
        size_t size = data - find_str;
        data--;

        if (start_zero && !is_float && size > 1) {
            return set_error("The number starts from zero");
        }

        if (!is_float && !is_exponent) {
            PyObject * v = nullptr;

            if (size < 10) {
                long val = fast_atoi(find_str, size);
                return PyLong_FromLong(((!is_negative) ? val : -val));

            } else if (size <= 18) {
                long long val = fast_atoll(find_str, size);
                return PyLong_FromLongLong(((!is_negative) ? val : -val));

            } else {
                return get_big_int(find_str, size);
            }

        } else {
            if (!is_exponent) {
                double val = fast_atod(find_str, size);
                return PyFloat_FromDouble(((!is_negative) ? val : -val));

            } else {
                if (size > 13 && !is_float) {
                    PyObject * f = PyUnicode_FromStringAndSize((char *)find_str, size);
                    Py_DECREF(f);
                    return PyFloat_FromString(f);
                }

                double val = fast_atod_and_exponent(find_str, size, is_float);
                return PyFloat_FromDouble(((!is_negative) ? val : -val));
            }
        }
    }

    PyObject * int_parser(const bool is_negative) {
        bool is_float = false;
        bool is_exponent = false;
        bool start_zero = false;

        if (*data == int_0) {
            start_zero = true;
        }

        find_str = data++;

        while (true) {
            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;

            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;

            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;

            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;
            int_pars_and_check;
        }
    }

    PyObject * int_parser__(const bool is_negative) {
        bool is_float = false;
        bool is_exponent = false;
        bool start_zero = false;

        if (*data == int_0) {
            start_zero = true;
        }

        find_str = data;

        char v = 0;

        while (true) {
            int_parser_(0)
            int_parser_(1)
            int_parser_(2)
            int_parser_(3)

            int_parser_(4)
            int_parser_(5)
            int_parser_(6)
            int_parser_(7)

            int_parser_(8)
            int_parser_(9)
            int_parser_(10)
            int_parser_(11)

            int_parser_(12)
            int_parser_(13)
            int_parser_(14)
            int_parser_(15)

            data += 16;
        }
    }

    PyObject * main_pars() {
        while (*data) {
            if (*data <= int_9) {
                if (*data >= int_0) {
                    return int_parser(false);
                }
                switch (*data) {
                    case quot:
                        data++;
                        return str_parser();
                    case df:
                        data++;
                        return int_parser(true);
                    case comma:
                        return set_error("error token ,");
                }

            } else {
                switch (*data) {
                    case open_obj:
                        return obj_parser();
                    case open_list:
                        return list_parser();
                    case js_true:
                        data += 3;
                        Py_RETURN_TRUE;
                    case js_false:
                        data += 4;
                        Py_RETURN_FALSE;
                    case js_null:
                        data += 3;
                        Py_RETURN_NONE;
                    case close_obj:
                    case close_list:
                    case colon:
                        return set_error("error token }]:");
                }
            }

            if (*data != space && *data != ent) {
                return set_error("pars error");
            }

            next_char_not_space();
        }
        return set_error("pars error");
    }


public:
    inline PyObject * pars() {
        PyObject * result = main_pars();

        if (result == nullptr || has_error) return nullptr;

        next_char_not_space();

        if (data < data_end) { return set_error("sintax error"); }

        return result;
    }

    BaseParser(const Type * _data, const size_t _size_source_string) {
        data = _data;
        data_start = data;
        size_source_string = _size_source_string;
        data_end = data + size_source_string;
    }

    BaseParser() {}

    ~BaseParser() {
        if (mem_size_ > 1024 * 256) {
            PyMem_RawFree(mem_char_);
            mem_char_ = nullptr;
        }
    }
};



class parser_string {
public:
    inline PyObject *pars(PyObject * val) {
        const size_t kind = PyUnicode_KIND(val);
        const size_t size_source_string = PyUnicode_GetLength(val);

        if (kind == 1) {
            auto p = BaseParser<uint8_t>((const uint8_t *)PyUnicode_1BYTE_DATA(val),
                                         size_source_string);
            return p.pars();
        } else if (kind == 2) {
            auto p = BaseParser<uint16_t>((const uint16_t *)PyUnicode_2BYTE_DATA(val),
                                         size_source_string);
            return p.pars();
        } else if (kind == 4) {
            auto p = BaseParser<uint32_t>((const uint32_t *)PyUnicode_4BYTE_DATA(val),
                                         size_source_string);
            return p.pars();
        }
        return nullptr;
    }
};



class parser_bytes : public BaseParser<char> {
    inline PyObject * set_str(const void * buff, const size_t size, const int kind_) const
    override
    {
        return PyUnicode_FromStringAndSize((const char *)buff, size);
    }

    inline PyObject * string_writer(
            const char * buff_start,
            char * buff,
            const char * source,
            const size_t n) {
        const char * ptr = source;
        const char ** p = &ptr;

        uint32_t v = 0;

        while (true) {
            bytes_escape_pars_and_check(0);
            bytes_escape_pars_and_check(1);
            bytes_escape_pars_and_check(2);
            bytes_escape_pars_and_check(3);

            bytes_escape_pars_and_check(4);
            bytes_escape_pars_and_check(5);
            bytes_escape_pars_and_check(6);
            bytes_escape_pars_and_check(7);

            bytes_escape_pars_and_check(8);
            bytes_escape_pars_and_check(9);
            bytes_escape_pars_and_check(10);
            bytes_escape_pars_and_check(11);

            bytes_escape_pars_and_check(12);
            bytes_escape_pars_and_check(13);
            bytes_escape_pars_and_check(14);
            bytes_escape_pars_and_check(15);

            buff += 16;
            ptr += 16;
        }
    }

    PyObject * str_escape_parser(const size_t size)
    override
    {
        if (mem_char_ == nullptr ) {
            mem_char_ = PyMem_RawMalloc(mem_size_);
        }
        if (mem_size_ < size_source_string * 2) {
            mem_size_ = size_source_string * 2;
            mem_char_ = PyMem_Realloc(mem_char_, mem_size_);
        }

        char * buff = (char *)mem_char_;
        const char * buff_start = buff;
        char * target_str = (char *)find_str;

        memcpy(buff, target_str, size);
        buff += size;
        target_str += size;

        return string_writer(buff_start, buff, target_str, size);
    }

public:
    parser_bytes(PyObject * val) {
        size_source_string = PyBytes_Size(val);
        data = PyBytes_AsString(val);
        data_end = data + size_source_string;
    }
};




static PyObject * clear(PyObject *self, PyObject *args) {
    if (mem_char_ != nullptr) {
        PyMem_RawFree(mem_char_);
        mem_char_ = nullptr;
        mem_size_ = 1024 * 8;
    }

    if (mem_cache_string_ != nullptr) {
        PyMem_RawFree(mem_cache_string_);
        mem_cache_string_ = nullptr;
    }

    Py_RETURN_NONE;
}


static PyObject * test(PyObject *self, PyObject *args) {
    PyObject * val = PyTuple_GetItem(args, 0);

    long double v = PyFloat_AsDouble(val);
    const size_t r = (size_t)v;

    const size_t _size = GetLenLong4(r);

    char buff[30];

    int size = d2s_buffered_n(v, buff);

    cout << _size << " " << size << endl;
    cout << buff << endl;

    return PyUnicode_FromStringAndSize(buff + _size + 1, size - _size - 3);
}


inline PyObject * loads_(PyObject * val) {
    if (PyUnicode_Check(val)) {
        parser_string p;
        return p.pars(val);

    } else if (PyBytes_Check(val)) {
        parser_bytes p = val;
        return p.pars();

    } else {
        PyErr_SetString(PyExc_ValueError, "error type, only 'str' or 'bytes'");
        return nullptr;
    }
}


static PyObject * loads(PyObject *self, PyObject *args) {
    PyObject * val = PyTuple_GetItem(args, 0);
    if (!val) { return nullptr; }
    return loads_(val);
}


static PyObject * load(PyObject *self, PyObject *args) {
    PyObject * val = PyTuple_GetItem(args, 0);
    if (!val) { return nullptr; }

    if (!PyObject_HasAttrString (val, "read")) {
        PyErr_Format (PyExc_TypeError, "error file open");
        return nullptr;
    }

    PyObject * read_func = PyObject_GetAttrString(val, "read");

    if (!PyCallable_Check(read_func)) {
        Py_XDECREF(read_func);
        PyErr_Format (PyExc_TypeError, "error file open");
        return nullptr;
    }

    PyObject * file_data = PyObject_CallObject(read_func, nullptr);
    Py_XDECREF(read_func);

    if (!file_data) {
        return nullptr;
    }

    return loads_(file_data);
}


static PyObject * dumps(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * val = PyTuple_GetItem(args, 0);
    if (!val) { return nullptr; }

    bool _non_string_key = false;
    bool as_string = false;
    size_t indent = 0;

    if (kwargs) {
        PyObject *key, *value;
        Py_ssize_t size = PyDict_Size(kwargs);
        Py_ssize_t i = 0;
        Py_hash_t hash = 0;

        while (i < size) {
            PyDict_Next(kwargs, &i, &key, &value);

            hash = PyObject_Hash(key);

            if (hash == __indent_hash && PyLong_Check(value)) {
                indent = ((PyLongObject *)value)->ob_digit[0];

            } else if (hash == __as_string_hash && PyLong_Check(value)) {
                as_string = ((PyLongObject *)value)->ob_digit[0];

            } else if (hash == __non_string_key_hash && PyLong_Check(value)) {
                _non_string_key = ((PyLongObject *)value)->ob_digit[0];
            }
        }
    }

    if (!as_string) {
        if (!indent) {
            dumper_bytes d(_non_string_key);
            return d.dump(val);

        } else {
            dumper_indent_bytes d(_non_string_key, indent);
            return d.dump(val);
        }

    } else {
        if (!indent) {
            //dumper d(_non_string_key);
            dumper__ d(_non_string_key);
            return d.dump(val);

        } else {
            dumper_indent d(_non_string_key, indent);
            return d.dump(val);
        }
    }
}


static PyObject * dumps_(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * val = PyTuple_GetItem(args, 0);

    dumper__ d;
    return d.dump(val);
}



PyObject * validate(PyObject *self, PyObject *args, PyObject * kwargs);
//PyObject * print(PyObject *self, PyObject *args);
PyObject * free(PyObject *self, PyObject *args);



static PyMethodDef methods[] = {
        { "test", (PyCFunction)test, METH_VARARGS,  "test" },
        { "free", (PyCFunction)free, METH_VARARGS,  "free" },
        //{ "print", (PyCFunction)print, METH_VARARGS,  "print" },
        { "clear", (PyCFunction)clear, METH_VARARGS,  "clear" },
        { "validate", (PyCFunction)validate, METH_VARARGS | METH_KEYWORDS,  "validate" },
        { "dumps", (PyCFunction)dumps, METH_VARARGS | METH_KEYWORDS,  "dumps" },
        { "dumps_", (PyCFunction)dumps_, METH_VARARGS | METH_KEYWORDS,  "dumps_" },
        { "loads", (PyCFunction)loads, METH_VARARGS,  "loads" },
        { "load", (PyCFunction)load, METH_VARARGS,  "load" },
        // Terminate the array with an object containing nulls.
        { nullptr, nullptr, 0, nullptr }
};


static struct PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        "fastjs",
        nullptr,
        -1,
        methods,
        nullptr,
};



PyMODINIT_FUNC PyInit_fastjs() {
    return PyModule_Create(&module_def);
}



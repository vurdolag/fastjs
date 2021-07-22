#include <Python.h>
#include "ryu/d2s.c"
#include "macros.cpp"


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
const uint32_t int_1 = '1';
const uint32_t int_2 = '2';
const uint32_t int_3 = '3';
const uint32_t int_4 = '4';
const uint32_t int_5 = '5';
const uint32_t int_6 = '6';
const uint32_t int_7 = '7';
const uint32_t int_8 = '8';
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



static size_t GetLenLong4(uint64_t v) {
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
            }
            return 18;
        }
    }
}


template<class T>
static inline uint32_t HEX_TO_DEC(const T * str, size_t len) {
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

const char * digits_ = "0123456789abcdef";

template <typename T>
inline void DEC_TO_HEX(uint32_t w, T * buffer) {
    *buffer++ = digits_[(w >> 12) & 0x0f];
    *buffer++ = digits_[(w >> 8) & 0x0f];
    *buffer++ = digits_[(w >> 4) & 0x0f];
    *buffer++ = digits_[w & 0x0f];
}


void convertUTF32ToUTF16(uint32_t cUTF32, uint32_t &h, uint32_t &l) {
    if (cUTF32 < 0x10000) {
        h = 0;
        l = cUTF32;
    }
    uint32_t t = cUTF32 - 0x10000;
    h = (((t << 12) >> 22) + 0xD800);
    l = (((t << 22) >> 22) + 0xDC00);
}


template <class T>
inline size_t encode_unicode_character(char * buffer, const T ucs_character) {
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


static long double pow10_matrix[100] = {
        0,
        1.0e+1,
        1.0e+2,
        1.0e+3,
        1.0e+4,
        1.0e+5,
        1.0e+6,
        1.0e+7,
        1.0e+8,
        1.0e+9,
        1.0e+10,
        1.0e+11,
        1.0e+12,
        1.0e+13,
        1.0e+14,
        1.0e+15,
        1.0e+16,
        1.0e+17,
        1.0e+18,
        1.0e+19,
        1.0e+20,
};


struct char1 {
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
};

static const char1 * matrix = (char1 *)"000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232233234235236237238239240241242243244245246247248249250251252253254255256257258259260261262263264265266267268269270271272273274275276277278279280281282283284285286287288289290291292293294295296297298299300301302303304305306307308309310311312313314315316317318319320321322323324325326327328329330331332333334335336337338339340341342343344345346347348349350351352353354355356357358359360361362363364365366367368369370371372373374375376377378379380381382383384385386387388389390391392393394395396397398399400401402403404405406407408409410411412413414415416417418419420421422423424425426427428429430431432433434435436437438439440441442443444445446447448449450451452453454455456457458459460461462463464465466467468469470471472473474475476477478479480481482483484485486487488489490491492493494495496497498499500501502503504505506507508509510511512513514515516517518519520521522523524525526527528529530531532533534535536537538539540541542543544545546547548549550551552553554555556557558559560561562563564565566567568569570571572573574575576577578579580581582583584585586587588589590591592593594595596597598599600601602603604605606607608609610611612613614615616617618619620621622623624625626627628629630631632633634635636637638639640641642643644645646647648649650651652653654655656657658659660661662663664665666667668669670671672673674675676677678679680681682683684685686687688689690691692693694695696697698699700701702703704705706707708709710711712713714715716717718719720721722723724725726727728729730731732733734735736737738739740741742743744745746747748749750751752753754755756757758759760761762763764765766767768769770771772773774775776777778779780781782783784785786787788789790791792793794795796797798799800801802803804805806807808809810811812813814815816817818819820821822823824825826827828829830831832833834835836837838839840841842843844845846847848849850851852853854855856857858859860861862863864865866867868869870871872873874875876877878879880881882883884885886887888889890891892893894895896897898899900901902903904905906907908909910911912913914915916917918919920921922923924925926927928929930931932933934935936937938939940941942943944945946947948949950951952953954955956957958959960961962963964965966967968969970971972973974975976977978979980981982983984985986987988989990991992993994995996997998999";


static PyObject * __dict__ = PyUnicode_FromString("__dict__");

static PyObject * __indent = PyUnicode_FromString("indent");
Py_hash_t __indent_hash = PyObject_Hash(__indent);

static PyObject * __non_string_key = PyUnicode_FromString("non_string_key");
Py_hash_t __non_string_key_hash = PyObject_Hash(__non_string_key);

static PyObject * __as_string = PyUnicode_FromString("as_string");
Py_hash_t __as_string_hash = PyObject_Hash(__as_string);



struct Spaces4 {
    uint8_t a[8] = " \0 \0 \0 ";
};

struct Spaces4_ {
    uint8_t a[5] = "    ";
};

struct False5 {
    uint8_t a[10] = "f\0a\0l\0s\0e";
};

struct False5_ {
    uint8_t a[6] = "false";
};

struct True4 {
    uint8_t a[8] = "t\0r\0u\0e";
};

struct True4_ {
    uint8_t a[5] = "true";
};

struct Null4 {
    uint8_t a[8] = "n\0u\0l\0l";
};

struct Null4_ {
    uint8_t a[5] = "null";
};



static void * mem_char_ = nullptr;
static size_t mem_size_ = 1024 * 8;


const size_t SIZE_CACHE = 1024;
const size_t MAX_SIZE_CACHE_STR = 64;


struct Cache {
    char ptr[MAX_SIZE_CACHE_STR + 2];
    Py_hash_t hash = 0;

    inline void set_size(char v) {
        ptr[MAX_SIZE_CACHE_STR] = v;
    }

    inline void set_kind(char v) {
        ptr[MAX_SIZE_CACHE_STR + 1] = v;
    }


    inline size_t size() {
        return ptr[MAX_SIZE_CACHE_STR];
    }

    inline size_t kind() {
        return ptr[MAX_SIZE_CACHE_STR + 1];
    }
};


static Cache * mem_cache_string_ = nullptr;



int check_js_dataclass(PyObject * v);
PyObject * check_field(PyObject * key, PyObject * value, int  i);
PyObject * check_object(PyObject * obj);


template<class Type>
class BaseDump {
protected:
    Type * str = nullptr;
    Type * start_str = nullptr;
    Type * end_str = nullptr;
    size_t buffer_size = 0;

    bool non_string_key = false;
    bool using_cache_string = false;


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

    inline void check(size_t n) {
        if ((str + n) > end_str) {
            realloc_mem(n);
        }
    }

    inline void add_char(const uint32_t c) {
        *str++ = c;
    }

    inline void add_charn(const uint32_t c, size_t n) {
        *(str + n) = c;
    }

    inline Type char_check(const Type c) {
        switch (c) {
            case ent:
                return 'n';
            case tab:
                return 't';
            case sr:
                return 'r';
            case sf:
                return 'f';
            case sb:
                return 'b';
            default:
                return c;
        }
    }

    int add_int(PyObject * s) {
        int size = ((PyLongObject *)s)->ob_base.ob_size;
        digit * d = ((PyLongObject *)s)->ob_digit;
        uint64_t val = 0;

        if (size < 0) {
            size = -size;
            add_char(df);
        } else if (size == 0) {
            add_char(int_0);
            return 0;
        }

        for (size_t i = 0; i < size; ++i) {
            val += (uint64_t)d[i] << (PyLong_SHIFT * i);
        }

        check(30);

        return write_int(val);
    };

    virtual inline size_t write_int(uint64_t val) {
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

        size_t len = GetLenLong4(val);

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
                while(*I) add_char(*I++);
            } else {
                const char * I = "-Infinity";
                while(*I) add_char(*I++);
            }
            return;
        } else if (isnan(v)) {
            const char * I = "NaN";
            while(*I) add_char(*I++);
            return;
        } else if (v < 0) {
            add_char(df);
            v = -v;
        } else if (!v) {
            add_char('0');
            return;
        }

        if (v >= 1.0e16 || v <= 1.0e-15) {
            char buff[30];
            size_t size = d2s_buffered_n(v, buff);
            for (int i = 0; i < size; i++) {
                *str++ = buff[i];
            }
            return;
        }

        size_t r = v;

        const long long len = 18 - write_int(v);
        add_char('.');

        const char1 * c;
        for (int i = 3; i < len; i += 3) {
            size_t d = v * pow10_matrix[i];
            c = matrix + (d % 1000);
            add_char(c->c1);
            add_char(c->c2);
            add_char(c->c3);
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

    virtual void add_list(PyObject * s) {
        Py_ssize_t len, i = 0;

        len = PyList_Size(s);
        check(len * 8);

        add_char(open_list);
        while (i < len) {
            main_dump(PyList_GetItem(s, i));
            add_char(comma);
            ++i;
        }
        *(str - 1) = close_list;
    }

    virtual void add_tuple(PyObject * s) {
        Py_ssize_t len, i = 0;

        len = PyTuple_Size(s);
        check(len * 8);

        add_char(open_list);
        while (i < len) {
            main_dump(PyTuple_GetItem(s, i));
            add_char(comma);
            ++i;
        }
        *(str - 1) = close_list;
    }

    virtual void add_dict(PyObject * s) {
        PyObject *key, *value;
        Py_ssize_t i = 0, len_s = 0;

        check(PyDict_Size(s) * 14);

        add_char(open_obj);
        while (PyDict_Next(s, &i, &key, &value)) {
            add_obj_key(key);
            add_char(colon);
            main_dump(value);
            add_char(comma);
        }
        *(str - 1) = close_obj;
    }

    virtual void add_class(PyObject * s, int js_dataclass_index = -1) {
        PyObject *key, *value;
        Py_ssize_t i = 0;

        check(PyDict_Size(s) * 14);

        add_char(open_obj);
        while (PyDict_Next(s, &i, &key, &value)) {
            obj_class_checker;

            add_obj_key(key);
            add_char(colon);
            main_dump(value);
            add_char(comma);
        }
        *(str - 1) = close_obj;
    }

    template <class T>
    inline size_t string_serialization8_16(Type * out, const T * source) {
        Type * s = out;

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

        return out - s;
    }

    template <class T>
    inline size_t string_serialization32(T * out, const uint32_t * source) {
        T * s = out;

        uint32_t max_2_byte = 0xffff;

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

        return out - s;
    }

    virtual inline void add_string_with_cache(PyObject * s) {
        size_t cache_index = 0, size = 0;
        Cache * cache = nullptr;

        Py_hash_t hash = PyObject_Hash(s);
        cache_index = hash % SIZE_CACHE;

        if (cache_index < 0) {
            cache_index = -cache_index;
        }

        cache = mem_cache_string_ + cache_index;

        if (cache->hash == hash && cache->kind() == sizeof(Type)) {
            check(cache->size());
            memcpy(str, cache->ptr, cache->size() * sizeof(Type));
            str += cache->size();
            return;
        }

        Type * start = str;
        size = add_string(s);

        if (size * sizeof(Type) < MAX_SIZE_CACHE_STR) {
            memcpy(cache->ptr, start, size * sizeof(Type));
            cache->set_size(size);
            cache->set_kind(sizeof(Type));
            cache->hash = hash;
        }
    }

    virtual inline size_t add_string(PyObject * s) {
        size_t kind = PyUnicode_KIND(s);
        size_t size = PyUnicode_GetLength(s);
        check(size * kind * 2);

        size_t len = 0;
        if (kind == 1) {
            const uint8_t * source = PyUnicode_1BYTE_DATA(s);
            if (source == nullptr) {
                PyErr_SetString(PyExc_RuntimeError, "not valid utf-8");
                return 0;
            }
            len = string_serialization8_16<uint8_t>(str, source);

        } else if (kind == 2) {
            const uint16_t * source = PyUnicode_2BYTE_DATA(s);
            if (source == nullptr) {
                PyErr_SetString(PyExc_RuntimeError, "not valid utf-16");
                return 0;
            }
            len = string_serialization8_16<uint16_t>(str, source);

        } else if (kind == 4) {
            const uint32_t * source = PyUnicode_4BYTE_DATA(s);
            if (source == nullptr) {
                PyErr_SetString(PyExc_RuntimeError, "not valid utf-32");
                return 0;
            }
            if (sizeof(Type) == 4) {
                len = string_serialization8_16<uint32_t>(str, source);
            } else {
                len = string_serialization32<uint16_t>((uint16_t *)str, source);
            }
        }

        str += len;

        return len;
    }

    inline void add_obj_key(PyObject * s) {
        if (PyUnicode_Check(s)) {
            if (using_cache_string) {
                add_string_with_cache(s);
            } else {
                add_string(s);
            }
        } else if (non_string_key) {
            add_char(quot);
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
                PyErr_SetString(PyExc_RuntimeError, "non serealisebl type");
            }
            add_char(quot);
        } else {
            PyErr_SetString(PyExc_RuntimeError, "key need be str");
        }
    }

    virtual inline void add_null() {
        check(10);

        *str++ = 'n';
        *str++ = 'u';
        *str++ = 'l';
        *str++ = 'l';
    }

    virtual inline void add_bool(PyObject * s) {
        check(10);
        if (s == Py_True) {
            *str++ = 't';
            *str++ = 'r';
            *str++ = 'u';
            *str++ = 'e';
        } else {
            *str++ = 'f';
            *str++ = 'a';
            *str++ = 'l';
            *str++ = 's';
            *str++ = 'e';
        }
    }

    void main_dump(PyObject * o) {
        if (PyUnicode_Check(o)) {
            if (using_cache_string) {
                add_string_with_cache(o);
            } else {
                add_string(o);
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
            PyErr_SetString(PyExc_RuntimeError, "not dumped type");
        }
    }

public:
    virtual inline PyObject * dump(PyObject * o) {
        main_dump(o);
        return PyUnicode_FromKindAndData(sizeof(Type), start_str, str - start_str);
    }

    BaseDump(){
        init_mem();
        init_cache_mem();
    }

    ~BaseDump() {
        //if (mem_size_ > 1024 * 512) {
        //    PyMem_RawFree(mem_char_);
        //    mem_char_ = nullptr;
        //}
        //if (mem_size_ / (str - start_str) >= 4) {
        //    mem_size_ = mem_size_ / 2;
       // }
    }

};


class dumper : public BaseDump<uint16_t> {
    inline void add_null() override  {
        check(14);
        *(Null4 *)str = Null4();
        str += 4;
    }

    inline void add_bool (PyObject * s) override {
        check(10);
        if (s == Py_True) {
            *(True4 *)str = True4();
            str += 4;
        } else {
            *(False5 *)str = False5();
            str += 5;
        }
    }

public:
    dumper(bool _non_string_key) {
        non_string_key = _non_string_key;
    }

    dumper(){}
};


class dumper_indent : public BaseDump<uint16_t> {
protected:
    const size_t indent_size = 0;
    size_t indent_depth = 0;

    inline void indent() {
        add_char(ent);

        size_t i = indent_depth % 4;
        size_t i4 = indent_depth / 4;

        while (i4-- > 0) {
            *(Spaces4 *)str = Spaces4();
            str += 4;
        }

        while (i-- > 0) {
            add_char(space);
        }

    }

    inline void inc_indent() {
        indent_depth += indent_size;
    }

    inline void dec_indent(){
        indent_depth -= indent_size;
    }

    void add_list(PyObject * s) override {
        Py_ssize_t len, i = 0;
        bool first = true;

        len = PyList_Size(s);
        check((len + indent_depth + 4) * 12);

        add_char(open_list);
        inc_indent();
        indent();
        while (i < len) {
            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }
            main_dump(PyList_GetItem(s, i++));
        }
        dec_indent();
        indent();
        add_char(close_list);
    }

    void add_tuple(PyObject * s) override {
        Py_ssize_t len, i = 0;
        bool first = true;

        len = PyTuple_Size(s);
        check((len + indent_depth + 4) * 12);

        add_char(open_list);
        inc_indent();
        indent();
        while (i < len) {
            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }
            main_dump(PyTuple_GetItem(s, i++));
        }
        dec_indent();
        indent();
        add_char(close_list);
    }

    void add_dict(PyObject * s) override {
        PyObject *key, *value;
        Py_ssize_t i = 0, len_s = 0;
        bool first = true;

        check((PyDict_Size(s) + indent_depth + 4) * 16);

        add_char(open_obj);
        inc_indent();
        indent();
        while (PyDict_Next(s, &i, &key, &value)) {
            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }
            add_obj_key(key);
            add_char(colon);
            add_char(space);
            main_dump(value);
        }
        dec_indent();
        indent();
        add_char(close_obj);
    }

    void add_class(PyObject * s, int js_dataclass_index = -1) override {
        PyObject *key, *value;
        Py_ssize_t i = 0;
        bool first = true;

        check((PyDict_Size(s) + indent_depth + 4) * 14);

        add_char(open_obj);
        inc_indent();
        indent();
        while (PyDict_Next(s, &i, &key, &value)) {
            obj_class_checker;

            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }

            add_obj_key(key);
            add_char(colon);
            add_char(space);
            main_dump(value);
        }
        dec_indent();
        indent();
        add_char(close_obj);
    }

public:
    dumper_indent(bool _non_string_key, const size_t _indent_size) : indent_size(_indent_size) {
        non_string_key = _non_string_key;
    }

};


class dumper_bytes : public BaseDump<uint8_t> {
protected:
    inline size_t add_string(PyObject * s)
    override
    {
        size_t size = PyUnicode_GetLength(s);
        size_t kind = PyUnicode_KIND(s);

        check(size * kind * 2);

        const uint8_t * source = (const uint8_t *)PyUnicode_AsUTF8(s);
        if (source == nullptr) {
            PyErr_SetString(PyExc_RuntimeError, "not valid utf-8");
            return -1;
        }

        size_t len = string_serialization8_16<uint8_t>(str, source);

        str += len;
        return len;
    }

    inline void add_null() override {
        check(6);
        *((Null4_ *)str) = Null4_();
        str += 4;
    }

    inline void add_bool(PyObject * s)
    override
    {
        check(10);
        if (s == Py_True) {
            *((True4_ *)str) = True4_();
            str += 4;
        } else {
            *((False5_ *)str) = False5_();
            str += 5;
        }
    }

    inline size_t write_int(uint64_t val) override {
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

        size_t len = GetLenLong4(val);

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
                while(*I) add_char(*I++);
            } else {
                const char * I = "-Infinity";
                while(*I) add_char(*I++);
            }
            return;
        } else if (isnan(v)) {
            const char * I = "NaN";
            while(*I) add_char(*I++);
            return;
        } else if (v < 0) {
            *str++ = df;
            v = -v;
        } else if (!v) {
            *str++ = int_0;
            return;
        }

        if (v >= 1.0e16 || v <= 1.0e-15) {
            str += d2s_buffered_n(v, (char *)str);
            return;
        }

        size_t r = (size_t)v;

        const long long len = 17 - write_int(r);
        *str++ = int_point;

        const char1 * c = nullptr;

        size_t d = 0;
        long long i = 3;
        long long imax = len - len % 3 + 3;

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
                add_char(c->c1);
                break;
            case 2:
                add_char(c->c1);
                add_char(c->c2);
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


public:
    inline PyObject * dump(PyObject * o) override {
        main_dump(o);
        return PyBytes_FromStringAndSize((const char *)start_str, str - start_str);
    }

    dumper_bytes(bool _non_string_key) {
        non_string_key = _non_string_key;
    }

    dumper_bytes() {}
};


class dumper_indent_bytes : public dumper_bytes {
    size_t indent_size = 0;
    size_t indent_depth = 0;

    inline void indent() {
        add_char(ent);

        size_t i = indent_depth % 4;
        size_t i4 = indent_depth / 4;

        while (i4-- > 0) {
            *(Spaces4_ *)str = Spaces4_();
            str += 4;
        }

        while (i-- > 0) {
            add_char(space);
        }

    }

    inline void inc_indent() {
        indent_depth += indent_size;
    }

    inline void dec_indent(){
        indent_depth -= indent_size;
    }

    void add_list(PyObject * s) override {
        Py_ssize_t len, i = 0;
        bool first = true;

        len = PyList_Size(s);
        check((len + indent_depth + 4) * 12);

        add_char(open_list);
        inc_indent();
        indent();
        while (i < len) {
            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }
            main_dump(PyList_GetItem(s, i++));
        }
        dec_indent();
        indent();
        add_char(close_list);
    }

    void add_tuple(PyObject * s) override {
        Py_ssize_t len, i = 0;
        bool first = true;

        len = PyTuple_Size(s);
        check((len + indent_depth + 4) * 12);

        add_char(open_list);
        inc_indent();
        indent();
        while (i < len) {
            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }
            main_dump(PyTuple_GetItem(s, i++));
        }
        dec_indent();
        indent();
        add_char(close_list);
    }

    void add_dict(PyObject * s) override {
        PyObject *key, *value;
        Py_ssize_t i = 0, len_s = 0;
        bool first = true;

        check((PyDict_Size(s) + indent_depth + 4) * 14);

        add_char(open_obj);
        inc_indent();
        indent();
        while (PyDict_Next(s, &i, &key, &value)) {
            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }
            add_obj_key(key);
            add_char(colon);
            add_char(space);
            main_dump(value);
        }
        dec_indent();
        indent();
        add_char(close_obj);
    }

    void add_class(PyObject * s, int js_dataclass_index = -1) override {
        PyObject *key, *value;
        Py_ssize_t i = 0;
        bool first = true;

        check((PyDict_Size(s) + indent_depth + 4) * 14);

        add_char(open_obj);
        inc_indent();
        indent();
        while (PyDict_Next(s, &i, &key, &value)) {
            obj_class_checker;

            if (!first) {
                add_char(comma);
                indent();
            } else { first = false; }

            add_obj_key(key);
            add_char(colon);
            add_char(space);
            main_dump(value);
        }
        dec_indent();
        indent();
        add_char(close_obj);
    }


public:
    dumper_indent_bytes(bool _non_string_key, const size_t _indent_size) {
        indent_size = _indent_size;
        non_string_key = _non_string_key;
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

    PyObject * set_error(const char * msg) {
        PyErr_Format(PyExc_ValueError, "%s, char pos = %d", msg, data - data_start);
        return nullptr;
    }

    inline void next_char_not_space() {
        data++;
        while (*data <= space && *data) {
            data++;
        }
    }

    inline double fast_atod(const Type * str, const size_t len) {
        double r = 0.0;
        double f = 0.0;
        const Type * end = str + len;
        int n = 0;

        while (*str != int_point) {
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

    inline double fast_atod_and_exponent(const Type * str, const size_t len) {
        double r = 0.0;
        double f = 0.0;
        const Type * end = str + len;
        int n = 0;

        while (*str != int_point) {
            r = (r * 10.0) + (*str++ - int_0);
        }

        ++str;
        while (*str != 'e' && *str != 'E') {
            f = (f * 10.0) + (*str++ - int_0);
            ++n;
        }
        r += f / pow10_matrix[n];

        ++str;
        int e = 0;
        if (*str == df) {
            while (++str < end) {
                e = e * 10 + (*str - int_0);
            }
            r = r / pow10_matrix[e];

        } else if (*str == '+') {
            while (++str < end) {
                e = e * 10 + (*str - int_0);
            }
            r = r * pow10_matrix[e];

        } else {
            while (str < end) {
                e = e * 10 + (*str - int_0);
                str++;
            }
            r = r * pow10_matrix[e];
        }

        return r;
    }

    inline uint64_t fast_atoll(const Type * str, const size_t len) {
        uint64_t val = 0;
        const Type * end = str + len;

        while (str < end) {
            val = val * 10 + (*str++ - int_0);
        }
        return val;
    }

    inline uint32_t fast_atoi(const Type * str, const size_t len) {
        uint32_t val = 0;
        const Type * end = str + len;

        while (str < end) {
            val = val * 10 + (*str++ - int_0);
        }
        return val;
    }

    PyObject *obj_parser() {
        PyObject *object = PyDict_New(), * key, * value;
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
            if (*data != quot) { return set_error("error start key obj token '\"'"); }

            data++;

            key = str_parser();
            if (key == nullptr) { return key; }

            next_char_not_space();

            if (*data != colon || *data == comma) { return set_error("error obj token ':'"); }

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
            if (*data != close_list) {
                if (!first) {
                    if (*data == comma) {
                        next_char_not_space();
                    } else {
                        return set_error("error list token ','");
                    }
                } else { first = false; }
            } else {
                return object;
            }

            value = main_pars();

            if (value != nullptr) {
                PyList_Append(object, value);
                Py_DECREF(value);
            } else { return value; }

            next_char_not_space();
        }
        return set_error("error list token ']'");
    }

    virtual inline PyObject * set_str(const void * buff, const size_t size, const int kind_) {
        return PyUnicode_FromKindAndData(kind_, buff, size);
    }

    inline uint32_t char_check(const Type ** source) {
        uint32_t v = 0;
        const Type * p = *source;
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
            case slash:
            case quot:
            case '/':
                return *p;

            default:
                set_error("Unrecognized escape sequence when decoding 'string'");
                return 0;
        }

    }

    inline uint32_t char_check_2_byte(const Type ** p) {
        uint32_t v = char_check(p);

        int c = (v << 10) - 0x35fdc00;
        if (c > 0 && c <= 138240) {
            (*p) += 2;
            v = c + char_check(p);
        }
        (*p) ++;
        return v;
    }

    inline PyObject * string_writer(
            const uint32_t * buff_start,
            uint32_t * buff,
            const Type * source,
            const size_t n
            ) {

        const Type * ptr = source;
        const Type ** p = &ptr;

        while (true) {
            str_escape_pars_and_check(0);
            str_escape_pars_and_check(1);
            str_escape_pars_and_check(2);
            str_escape_pars_and_check(3);

            str_escape_pars_and_check(4);
            str_escape_pars_and_check(5);
            str_escape_pars_and_check(6);
            str_escape_pars_and_check(7);

            buff += 8;
            ptr += 8;
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
        const uint32_t * buff_start = buff;
        const Type * target_str = find_str;

        for (size_t x = 0; x < size; ++x) {
            *buff++ = *target_str++;
        }
        return string_writer(buff_start, buff, target_str, size);
    }

    virtual PyObject *str_parser() {
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

    inline PyObject *get_integer(
            const bool is_float,
            const bool is_negative,
            const bool is_exponent) {
        size_t len = data - find_str;
        data--;

        PyObject * obj = nullptr;

        if (!is_float) {
            if (len < 10) {
                long val = fast_atoi(find_str, len);
                val = (!is_negative) ? val : -val;
                obj = PyLong_FromLong(val);

            } else {
                long long val = fast_atoll(find_str, len);
                val = (!is_negative) ? val : -val;
                obj = PyLong_FromLongLong(val);
            }
        } else {
            if (!is_exponent) {
                double val = fast_atod(find_str, len);
                val = (!is_negative) ? val : -val;
                obj = PyFloat_FromDouble(val);
            } else {
                double val = fast_atod_and_exponent(find_str, len);
                val = (!is_negative) ? val : -val;
                obj = PyFloat_FromDouble(val);
            }
        }
        return obj;
    }

    PyObject *int_parser(const bool is_negative) {
        bool is_float = false;
        bool is_exponent = false;
        find_str = data;

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

    PyObject *main_pars() {
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
                        data++;
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
            next_char_not_space();
        }
        return set_error("pars error");
    }


public:
    inline PyObject *pars() {
        PyObject * result = main_pars();

        if (result == nullptr) return result;

        next_char_not_space();

        if (*data) { return set_error("sintax error"); }

        return result;
    }

    BaseParser(const Type * _data, size_t _size_source_string) {
        data = _data;
        data_start = data;
        size_source_string = _size_source_string;
        data_end = data + size_source_string;
    }

    BaseParser() {}

};



class parser_string {
public:
    inline PyObject *pars(PyObject * val) {
        size_t kind = PyUnicode_KIND(val);
        size_t size_source_string = PyUnicode_GetLength(val);

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
    inline PyObject * set_str(const void * buff, const size_t size, const int kind_) override {
        return PyUnicode_FromStringAndSize((const char *)buff, size);
    }

    inline PyObject * string_writer(char * buff_start, char * buff,
            const char * source, const size_t n) {
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

    PyObject * str_escape_parser(const size_t size) override {
        if (mem_char_ == nullptr ) {
            mem_char_ = PyMem_RawMalloc(mem_size_);
        }
        if (mem_size_ < size_source_string * 2) {
            mem_size_ = size_source_string * 2;
            mem_char_ = PyMem_Realloc(mem_char_, mem_size_);
        }

        char * buff = (char *)mem_char_;
        char * buff_start = buff;
        char * target_str = (char *)find_str;

        for (size_t x = 0; x < size; ++x) {
            *buff++ = *target_str++;
        }

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


static PyObject * loads(PyObject *self, PyObject *args) {
    PyObject * val = PyTuple_GetItem(args, 0);

    if (PyUnicode_Check(val)) {
        parser_string p;
        return p.pars(val);
    } else if (PyBytes_Check(val)) {
        parser_bytes p = val;
        return p.pars();
    } else {
        PyErr_SetString(PyExc_RuntimeError, "error type, only 'str' or 'bytes'");
        return nullptr;
    }
}


static PyObject * dumps(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * val = PyTuple_GetItem(args, 0);

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
            dumper d(_non_string_key);
            return d.dump(val);
        } else {
            dumper_indent d(_non_string_key, indent);
            return d.dump(val);
        }
    }
}



PyObject * validate(PyObject *self, PyObject *args, PyObject * kwargs);
//PyObject * print(PyObject *self, PyObject *args);
PyObject * free(PyObject *self, PyObject *args);



static PyMethodDef methods[] = {
        { "free", (PyCFunction)free, METH_VARARGS,  "free" },
        //{ "print", (PyCFunction)print, METH_VARARGS,  "print" },
        { "clear", (PyCFunction)clear, METH_VARARGS,  "clear" },
        { "validate", (PyCFunction)validate, METH_VARARGS | METH_KEYWORDS,  "validate" },
        { "dumps", (PyCFunction)dumps, METH_VARARGS | METH_KEYWORDS,  "dumps" },
        { "loads", (PyCFunction)loads, METH_VARARGS,  "loads" },
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



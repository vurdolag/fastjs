#include <Python.h>
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


double pow10(int n) {
    double p = 1;
    for (int i = 0; i < n; i++) {
        p *= 10.0;
    }
    return p;
}


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
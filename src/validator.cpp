#include <Python.h>
#include <unordered_map>


using namespace std;



PyObject * __origin__ = PyUnicode_FromString("__origin__");
extern PyObject * __dict__;
PyObject * __args__ = PyUnicode_FromString("__args__");
PyObject * __name__ = PyUnicode_FromString("__name__");
PyObject * __dataclass_fields__ = PyUnicode_FromString("__dataclass_fields__");
PyObject * __skip = PyUnicode_FromString("skip");
PyObject * __skip_if_none = PyUnicode_FromString("skip_if_none");
PyObject * __const_len = PyUnicode_FromString("const_len");
PyObject * __js_name = PyUnicode_FromString("name");


PyObject * get_dict(PyObject * val) {
    PyObject * class_dict = PyObject_GenericGetDict(val, nullptr);
    if (!class_dict) {
        PyErr_SetString(PyExc_AttributeError, "class doesn't have '__dict__'");
        return nullptr;
    }
    return class_dict;
}

PyObject * get_origin_type(PyObject * val) {
    PyObject * origin_type = PyDict_GetItemWithError(val, __origin__);
    if (!origin_type) {
        PyErr_SetString(PyExc_AttributeError, "class doesn't have '__origin__'");
        return nullptr;
    }
    return origin_type;
}

PyObject * get_tuple_subtypes(PyObject * val) {
    PyObject * tuple_subtypes = PyDict_GetItemWithError(val, __args__);
    if (!tuple_subtypes) {
        PyErr_SetString(PyExc_AttributeError, "class doesn't have '__args__'");
        return nullptr;
    }
    return tuple_subtypes;
}

PyObject * get_dataclass_fields(PyObject * val) {
    PyObject * class_dict = PyObject_GenericGetDict(val, nullptr);
    if (!class_dict) {
        PyErr_SetString(PyExc_AttributeError, "class doesn't have '__dict__'");
        return nullptr;
    }
    PyObject * dataclass_fields = PyDict_GetItemWithError(class_dict, __dataclass_fields__);
    if (!dataclass_fields) {
        PyErr_SetString(PyExc_AttributeError, "class doesn't have '__dataclass_fields__'");
        return nullptr;
    }
    return dataclass_fields;
}




PyObject * _validate(PyObject * val, PyObject * kwargs);
bool is_already_add_PyType(PyObject * val);


static PyObject * module_typing = nullptr;
static PyObject * module_dataclasses = nullptr;



enum Python_types: size_t {
    ANY_, OPTIONAL_, UNION_, NONE_, STR_, BYTES_, INT_, FLOAT_, BOOL_,
    LIST_, DICT_, TUPLE_, SET_, USERTYPE_
};



inline size_t check_type_str(PyObject * val) {
    if (!val) return ANY_;

    const char * c = PyUnicode_AsUTF8(val);

    if (!strcmp(c, "list")) {
        return LIST_;

    } else if (!strcmp(c, "dict")) {
        return DICT_;

    } else if (!strcmp(c, "tuple")) {
        return TUPLE_;

    } else if (!strcmp(c, "set")) {
        return SET_;

    } else {
        return ANY_;
    }
}

inline size_t check_type(PyObject * val, bool is_str=false) {
    if (is_str) {
        return check_type_str(val);
    }

    if (!val) {
        return USERTYPE_;
    }

    if (val == PyObject_GetAttrString(module_typing, "Union")) {
        return UNION_;
    }

    unsigned long f = ((PyTypeObject *)val)->tp_flags;

    if (f == PyLong_Type.tp_flags) {
        return INT_;
    } else if (f == PyFloat_Type.tp_flags) {
        return FLOAT_;
    } else if (f == PyBool_Type.tp_flags) {
        return BOOL_;
    } else if (f == PyUnicode_Type.tp_flags) {
        return STR_;
    } else if (f == PyBytes_Type.tp_flags) {
        return BYTES_;
    } else if (f == PyList_Type.tp_flags) {
        return LIST_;
    } else if (f == PyTuple_Type.tp_flags) {
        return TUPLE_;
    } else if (f == PySet_Type.tp_flags) {
        return SET_;
    } else if (f == PyDict_Type.tp_flags) {
        return DICT_;
    } else if (f == Py_None->ob_type->tp_flags) {
        return NONE_;
    }

    return USERTYPE_;
}



struct Base_type {
    Base_type * types = nullptr;
    size_t size = 0;
    size_t type = 0;

    void free() {
        if (types) {
            for (int i = 0; i < size; i++) {
                types[i].free();
            }
        }
        PyMem_Free(types);
    }
};


struct Metadata {
    PyObject * js_name = nullptr;
    Py_hash_t hash = 0;
    long const_len = -1;
    bool skip_if_none = false;
    bool skip = false;
};


struct TypeField {
    char * name = nullptr;
    PyObject * name_ = nullptr;
    Base_type * subtypes = nullptr;
    Metadata * metadata = nullptr;
    size_t type = ANY_;
    Py_hash_t hash_name = 0;

    void set_name(PyObject * val) {
        name_ = val;
        auto str_name = PyUnicode_AsUTF8(val);
        auto size = strlen(str_name);
        name = (char *)PyMem_Malloc(size + 1);
        memcpy(name, str_name, size);
        *(name + size) = '\0';
        hash_name = PyObject_Hash(val);
    }

    void set_type(PyObject * val) {
        size_t t = check_type(val);

        switch (t) {
            case LIST_:
            case DICT_:
            case TUPLE_:
                break;
            case USERTYPE_:
                if (is_already_add_PyType(val)) {
                    type = (size_t)val;
                } else {
                    _validate(val, nullptr);
                    type = (size_t)val;
                }

                type = (size_t)val;
                return;
        }
        type = t;
    }

    void write_sub_type(Base_type * base_type, PyObject * tuple, size_t n, size_t t) {
        auto base_types = (Base_type *)PyMem_Malloc(sizeof(Base_type) * n);
        for (int i = 0; i < n; i++) {
            base_types[i] = Base_type();
        }
        *base_type = {base_types, n, t};

        for (size_t i = 0; i < n; i++) {
            PyObject * sub_obj = PyTuple_GetItem(tuple, i);

            if (sub_obj && ((PyTypeObject*)sub_obj)->tp_name[0] == 1) {
                PyObject * tuple_subtypes = nullptr;
                t = get_type_(sub_obj, &tuple_subtypes);
                size_t size_tuple = PyTuple_Size(tuple_subtypes);
                write_sub_type(base_type->types + i, tuple_subtypes, size_tuple, t);
                continue;
            }

            size_t sub_type = check_type(sub_obj);

            if (sub_obj && sub_type == USERTYPE_) {
                if (!is_already_add_PyType(sub_obj)) {
                    _validate(sub_obj, nullptr);
                }
                sub_type = (size_t)sub_obj;
            }
            base_type->types[i] = {nullptr, 0, sub_type};
        }
    }

    void alloc_metadata() {
        if (!metadata) {
            metadata = (Metadata *) PyMem_Malloc(sizeof(Metadata));
            *metadata = Metadata();
        }
    }

    void set_metadata(PyObject * val) {
        PyObject * field_metadata = PyObject_GetAttrString(val, "metadata");
        if (!field_metadata) return;

        PyObject * is_skip = PyObject_GetItem(field_metadata, __skip);
        if (is_skip) {
            alloc_metadata();
            metadata->skip = PyObject_IsTrue(is_skip);
        }

        PyObject * is_skip_if_none = PyObject_GetItem(field_metadata, __skip_if_none);
        if (is_skip_if_none) {
            alloc_metadata();
            metadata->skip_if_none = PyObject_IsTrue(is_skip_if_none);
        }

        PyObject * _name = PyObject_GetItem(field_metadata, __js_name);
        if (_name) {
            alloc_metadata();
            metadata->js_name = _name;
            metadata->hash = PyObject_Hash(_name);
        }

        PyObject * const_len = PyObject_GetItem(field_metadata, __const_len);
        if (const_len && PyLong_Check(const_len)) {
            alloc_metadata();
            metadata->const_len = PyLong_AsLong(const_len);
        }
    }

    size_t get_type_(PyObject * field_type, PyObject ** tuple) {
        PyObject * origin_type,  * class_dict;

        if (!strcmp("types.GenericAlias", field_type->ob_type->tp_name)) {
            origin_type = PyObject_GetAttr(field_type, __name__);
            if (!origin_type) {
                PyErr_SetString(PyExc_RuntimeError, "class doesn't have '__name__'");
                return 0;
            }

            *tuple = PyObject_GetAttr(field_type, __args__);
            if (!tuple) {
                PyErr_SetString(PyExc_RuntimeError, "class doesn't have '__args__'");
                return 0;
            }

            return check_type(origin_type, true);

        } else {
            class_dict = get_dict(field_type);
            if (!class_dict) return 0;
            origin_type = get_origin_type(class_dict);
            if (!origin_type) return 0;
            *tuple = get_tuple_subtypes(class_dict);
            if (!tuple) return 0;
            return check_type(origin_type);
        }
    }

    void set(PyObject * key_obj, PyObject * value) {
        PyObject * field_type = PyObject_GetAttrString(value, "type");
        if (!field_type) return;

        set_name(key_obj);
        set_metadata(value);

        if (field_type == PyObject_GetAttrString(module_typing, "Any")) {
            type = ANY_;
            return;
        }

        if (((PyTypeObject*)(field_type))->tp_name[0] == 1) {
            PyObject * tuple = nullptr;

            type = get_type_(field_type, &tuple);

            size_t tuple_size = PyTuple_Size(tuple);

            subtypes = (Base_type *)PyMem_Malloc(sizeof(Base_type));
            write_sub_type(subtypes, tuple, tuple_size, type);
            return;
        }

        set_type(field_type);
    }

    void free() {
        PyMem_Free(name);
        if (metadata) {
            PyMem_Free(metadata);
        }
        if (subtypes) {
            subtypes->free();
            PyMem_Free(subtypes);
        }

    }

};


using maps = unordered_map<Py_hash_t, TypeField*>;


struct PyType {
    char * name = nullptr;
    TypeField * field_list = nullptr;
    maps * fields_map;
    PyObject * type = nullptr;
    size_t size = 0;
    size_t index = 0;
    bool fields_has_metadata = false;

    inline uint64_t get_flags() const {
        return ((PyTypeObject *)type)->tp_flags;
    }

    void alloc_fields(size_t n) {
        size = n;
        field_list = (TypeField *)PyMem_Malloc(sizeof(TypeField) * n);
        for (int i = 0; i < n; i++) {
            *(field_list + i) = TypeField();
        }
    }

    void set_name(PyObject * val) {
        auto name_ = PyObject_GetAttr(val, __name__);
        auto str_name = PyUnicode_AsUTF8(name_);
        auto size_ = strlen(str_name);
        name = (char *)PyMem_Malloc(size_ + 1);
        memcpy(name, str_name, size_);
        *(name + size_) = 0;
        type = val;
    }

    inline TypeField * next_field() {
        return field_list + (index++ % size);
    }

    void check_metadata() {
        for (int i = 0; i < size; i++) {
            if ((field_list + i)->metadata) {
                fields_has_metadata = true;
                break;
            }
        }
    }

    void free()  {
        for (int i = 0; i < size; i++) {
            field_list[i].free();
        }
        delete fields_map;
        PyMem_Free(field_list);
        PyMem_Free(name);
    }

    PyType() {
        fields_map = new maps;
        name = nullptr;
        field_list = nullptr;
        type = nullptr;
        size = 0;
        index = 0;
        fields_has_metadata = false;
    }
 };




static PyType * PyTypeList = nullptr;
static size_t PyTypeList_size = 16;
static size_t PyTypeList_index = 0;




inline PyType * get_type(size_t i) {
    return PyTypeList + i;
}

inline PyType * new_type(PyObject * val, size_t size) {
    if (PyTypeList == nullptr) {
        PyTypeList = (PyType *)PyMem_Malloc(sizeof(PyType) * PyTypeList_size);
        for (int i = 0; i < PyTypeList_size; i++) {
            PyTypeList[i] = PyType();
        }
    }

    if (PyTypeList_index >= PyTypeList_size) {
        int i = PyTypeList_size;
        PyTypeList_size *= 2;
        PyTypeList = (PyType *)PyMem_Realloc(PyTypeList, PyTypeList_size);
        for (; i < PyTypeList_size; i++) {
            PyTypeList[i] = PyType();
        }
    }

    PyType * type = PyTypeList + PyTypeList_index++;

    type->set_name(val);
    type->alloc_fields(size);

    return type;
}

inline void add_type(PyObject * obj, PyObject * origin_obj) {
    PyObject * dataclass_fields = get_dataclass_fields(obj);
    if (!dataclass_fields) return;

    size_t size_field = PyDict_Size(dataclass_fields);

    auto type = new_type(origin_obj, size_field);

    PyObject *key, *value;
    Py_ssize_t i = 0;
    while (PyDict_Next(dataclass_fields, &i, &key, &value)) {
        TypeField * field = type->next_field();
        field->set(key, value);
        (*type->fields_map)[field->hash_name] = field;
        if (field->metadata && field->metadata->hash) {
            (*type->fields_map)[field->metadata->hash] = field;
        }
    }
    type->check_metadata();
}



/*

void print_type(size_t t) {
    switch (t) {
        case ANY_:
            cout << "ANY" << endl;
            break;
        case OPTIONAL_:
            cout << "OPTIONAL" << endl;
            break;
        case NONE_:
            cout << "NONE" << endl;
            break;
        case STR_:
            cout << "STR" << endl;
            break;
        case INT_:
            cout << "INT" << endl;
            break;
        case FLOAT_:
            cout << "FLOAT" << endl;
            break;
        case BOOL_:
            cout << "BOOL" << endl;
            break;
        case LIST_:
            cout << "LIST" << endl;
            break;
        case DICT_:
            cout << "DICT" << endl;
            break;
        case TUPLE_:
            cout << "TUPLE" << endl;
            break;
        case SET_:
            cout << "SET" << endl;
            break;
        case UNION_:
            cout << "UNION" << endl;
            break;
        case BYTES_:
            cout << "BYTES" << endl;
            break;
        case USERTYPE_:
            cout << "USERTYPE" << endl;
            break;
    }

    if (t > USERTYPE_) {
        for (int i = 0; i < PyTypeList_index; i++) {
            PyType * type = PyTypeList + i;
            if (type && (size_t)type->type == t) {
                cout << type->name << endl;
                return;
            }
        }
        cout << "USERTYPE " << t << endl;
    }
}

*/


inline bool is_already_add_PyType(PyObject * val) {
    for (int i = 0; i < PyTypeList_index; i++) {
        PyType * type = (PyType *)PyTypeList + i;
        if (type && type->type == val) {
            return true;
        }
    }
    return false;
}


int check_js_dataclass(PyObject * v) {
    if (!PyTypeList) {
        return -1;
    }

    for (int i = 0; i < PyTypeList_index; i++) {
        PyType * type = get_type(i);
        if (type && type->get_flags() == v->ob_type->tp_flags) {
            return i;
        }
    }
    return -1;
}


PyObject * check_field(PyObject * key, PyObject * value, int index, int & error_handler) {
    if (index < 0) return key;


    PyType * type = get_type(index);

    if (!type->fields_has_metadata) return key;

    TypeField * field = (*type->fields_map)[PyObject_Hash(key)];

    if (field->metadata) {
        if (field->metadata->skip) { return nullptr; }

        if (field->metadata->skip_if_none && value == Py_None) { return nullptr; }

        if (field->metadata->js_name) { return field->metadata->js_name; }

        if (field->metadata->const_len != -1) {
            if (PyObject_Length(value) != field->metadata->const_len) {
                error_handler = 1;
                PyErr_Format(PyExc_TypeError,
                        "field len %d != %d const len",
                        PyObject_Length(value),
                        field->metadata->const_len);
                return nullptr;
            }

        }
    }

    return key;
}


PyObject * __tuple = PyTuple_New(0);


inline PyObject * init_object(PyObject * obj, PyType * type) {

    PyObject * v = PyObject_Call(type->type, __tuple, obj);

    //Py_DECREF(tuple);
    Py_DECREF(obj);

    return v;
}


inline size_t check_object_type(PyObject * o) {
    if (PyUnicode_Check(o)) {
        return STR_;
    }
    else if (PyBool_Check(o)) {
        return BOOL_;

    }
    else if (PyLong_Check(o)) {
        return INT_;
    }
    else if (PyFloat_Check(o)) {
        return FLOAT_;
    }
    else if (PyList_Check(o)) {
        return LIST_;


    }
    else if (PyDict_Check(o)) {
        return DICT_;

    }
    else if (o == Py_None) {
        return NONE_;

    }
    return USERTYPE_;
}


inline bool check(size_t type_value, Base_type * sub, PyObject * val) {
    PyObject * key = nullptr, * value = nullptr, * object = nullptr;
    Py_ssize_t len = 0, i = 0;


    if (sub->type == ANY_) {
        return true;

    } else if (sub->type == UNION_ || sub->type == OPTIONAL_) {
        for (; i < sub->size; i++) {
            if (check(type_value, sub->types + i, val)) {
                return true;
            }
        }


    } else if (type_value == LIST_) {
        if (sub->type == LIST_ || sub->type == SET_) {
            len = PyList_Size(val);
            while (i < len) {
                object = PyList_GetItem(val, i);
                if (!check(check_object_type(object), sub->types, object)) {
                    return false;
                }
                ++i;
            }
            return true;

        } else if (sub->type == TUPLE_) {
            len = PyList_Size(val);
            if (len != sub->size) {
                return false;
            }

            while (i < len) {
                object = PyList_GetItem(val, i);
                if (!check(check_object_type(object), sub + i, object)) {
                    return false;
                }
                ++i;
            }
            return true;
        }

    } else if (type_value == DICT_ && sub->type == DICT_) {
        if (sub->size != 2) return false;

        Base_type *sub_key = sub->types;
        Base_type *sub_value = sub->types + 1;

        while (PyDict_Next(val, &i, &key, &value)) {
            if (!check(check_object_type(key), sub_key, key)) {
                return false;
            }

            if (!check(check_object_type(value), sub_value, value)) {
                return false;
            }
        }
        return true;
    } else if (type_value == sub->type) {
        return true;
    }

    return false;
}





inline bool check_field_obj(PyObject * key, PyObject * value, PyType * type, PyObject * obj) {
    Py_hash_t hash_key = PyObject_Hash(key);

    TypeField * field = (*type->fields_map)[hash_key];

    if (field) {
        size_t t = check_object_type(value);

        if (t == USERTYPE_) {
            for (int x = 0; x < PyTypeList_index; x++) {
                PyType *_type = get_type(x);
                if (_type && (size_t) _type->type == (size_t) value) {
                    if (field->type != (size_t) _type->type) {
                        return false;
                    }
                }
            }
            if (field->hash_name != hash_key) {
                PyDict_SetItem(obj, field->name_, value);
                PyDict_DelItem(obj, key);
            }
            if (field->metadata) {
                if (field->metadata->const_len != PyObject_Length(value)) {
                    return false;
                }
            }
            return true;

        } else if (field->subtypes) {
            if (check(t, field->subtypes, value)) {
                if (field->hash_name != hash_key) {
                    PyDict_SetItem(obj, field->name_, value);
                    PyDict_DelItem(obj, key);
                }
                if (field->metadata) {
                    if (field->metadata->const_len != PyObject_Length(value)) {
                        return false;
                    }
                }
                return true;
            }

        } else if (field->type == ANY_ || field->type == t) {
            if (field->hash_name != hash_key) {
                PyDict_SetItem(obj, field->name_, value);
                PyDict_DelItem(obj, key);
            }
            if (field->metadata) {
                if (field->metadata->const_len != PyObject_Length(value)) {
                    return false;
                }
            }
            return true;
        }
    }

    return false;

    //ret_true:
        //if (field->hash_name != hash_key) {
        //    PyDict_SetItem(obj, field->name_, value);
        //    PyDict_DelItem(obj, key);
        //}
        //return true;
}



PyObject * check_object(PyObject * obj) {
    if (!PyTypeList) return obj;

    for (int i = 0; i < PyTypeList_index; i++) {
        PyType * type = get_type(i);

        PyObject *key, *value;
        Py_ssize_t j = 0;

        bool init = true;

        while (PyDict_Next(obj, &j, &key, &value)) {
            if (!check_field_obj(key, value, type, obj)) {
                init = false;
                break;
            }
        }

        if (init) {
            return init_object(obj, type);
        }
    }

    return obj;
}



PyObject * _validate(PyObject * val, PyObject * kwargs) {
    PyObject * dataclass_val = nullptr;

    if (PyFunction_Check(val) || PyMethod_Check(val)) {
        PyErr_SetString(PyExc_TypeError, "'function' and 'method' not allowed");
        return nullptr;
    }

    if (PyType_CheckExact(val)) {
        if (module_typing == nullptr) {
            module_typing = PyImport_ImportModule("typing");
        }

        if (module_dataclasses == nullptr) {
            module_dataclasses = PyImport_ImportModule("dataclasses");
        }

        if (!module_dataclasses || !module_typing) {
            PyErr_SetString(PyExc_ImportError, "error import module 'dataclasses' or 'typing'");
            return nullptr;
        }

        PyObject * dataclass = PyObject_GetAttrString(module_dataclasses, "dataclass");

        if (!dataclass) {
            PyErr_SetString(PyExc_AttributeError, "'dataclasses' not attr 'dataclass'");
            return nullptr;
        }

        if (kwargs) {
            dataclass_val = PyObject_Call(dataclass, val, kwargs);
        } else {
            dataclass_val = PyObject_CallFunctionObjArgs(dataclass, val);
        }

        if (dataclass_val && !is_already_add_PyType(val)) {
            add_type(dataclass_val, val);
        }
    }

    return dataclass_val;
}


/*

void print_sub(Base_type * b) {
    print_type(b->type);
    cout << b->types << endl;
    cout << b->size << endl;
    cout << endl;


    if (b->types) {
        for (size_t j = 0; j < b->size; j++) {
            print_sub(b->types + j);
        }
    }
}


PyObject * print(PyObject *self, PyObject *args) {
    for (size_t i = 0; i < PyTypeList_index; i++) {
        PyType * t = (PyType *)PyTypeList + i;
        cout << "== type == " << t->name << " " << t->size << endl;
        auto e = t->field_list;

        for (size_t x = 0; x < t->size; x++) {
            cout << "-- field -- " << e[x].name << " " << e[x].hash_name << endl;
            if (e[x].metadata) {
                cout << "skip=" << e[x].metadata->skip << " skip_if_none=" << e[x].metadata->skip_if_none << endl;

            }
            print_type(e[x].type);

            auto s = e[x].subtypes;
            if (!s) { continue;}
            cout << "== subtypes == " << s->size << endl;
            print_sub(s);
            cout << "--------" << endl;
        }
        cout << "========\n\n" << endl;

    }

    Py_RETURN_NONE;
}


*/


PyObject * free(PyObject *self, PyObject *args) {
    for (int i = 0; i < PyTypeList_index; i++) {
        ((PyType *)PyTypeList + i)->free();
    }
    PyTypeList_index = 0;
    Py_RETURN_NONE;
}


PyObject * validate(PyObject *self, PyObject *args, PyObject * kwargs) {
    PyObject * val = PyTuple_GetItem(args, 0);
    return _validate(val, kwargs);
}


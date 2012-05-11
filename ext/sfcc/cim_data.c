#include <string.h>
#include <stdint.h>
#include "cim_string.h"
#include "cim_instance.h"
#include "cim_object_path.h"
#include "cim_data.h"

VALUE cSfccCimData;
static VALUE types_module;
static VALUE valstates_module;
static VALUE cim_module;
static VALUE s_types_i2s;
static VALUE s_valstates_i2s;

typedef struct {
    char const * name;
    CIMCType value;
}type_mapping_t;

static type_mapping_t const type_mapping[] = {
    {"CIMC_null", CIMC_null},
    {"CIMC_SIMPLE", CIMC_SIMPLE},
    {"CIMC_boolean", CIMC_boolean},
    {"CIMC_char16", CIMC_char16},

    {"CIMC_REAL", CIMC_REAL},
    {"CIMC_real32", CIMC_real32},
    {"CIMC_real64", CIMC_real64},

    {"CIMC_UINT", CIMC_UINT},
    {"CIMC_uint8", CIMC_uint8},
    {"CIMC_uint16", CIMC_uint16},
    {"CIMC_uint32", CIMC_uint32},
    {"CIMC_uint64", CIMC_uint64},
    {"CIMC_SINT", CIMC_SINT},
    {"CIMC_sint8", CIMC_sint8},
    {"CIMC_sint16", CIMC_sint16},
    {"CIMC_sint32", CIMC_sint32},
    {"CIMC_sint64", CIMC_sint64},
    {"CIMC_INTEGER", CIMC_INTEGER},

    {"CIMC_ENC", CIMC_ENC},
    {"CIMC_instance", CIMC_instance},
    {"CIMC_ref", CIMC_ref},
    {"CIMC_args", CIMC_args},
    {"CIMC_class", CIMC_class},
    {"CIMC_filter", CIMC_filter},
    {"CIMC_enumeration", CIMC_enumeration},
    {"CIMC_string", CIMC_string},
    {"CIMC_chars", CIMC_chars},
    {"CIMC_dateTime", CIMC_dateTime},
    {"CIMC_ptr", CIMC_ptr},
    {"CIMC_charsptr", CIMC_charsptr},

    {"CIMC_ARRAY", CIMC_ARRAY},
    {"CIMC_SIMPLEA", CIMC_SIMPLEA},
    {"CIMC_booleanA", CIMC_booleanA},
    {"CIMC_char16A", CIMC_char16A},

    {"CIMC_REALA", CIMC_REALA},
    {"CIMC_real32A", CIMC_real32A},
    {"CIMC_real64A", CIMC_real64A},

    {"CIMC_UINTA", CIMC_UINTA},
    {"CIMC_uint8A", CIMC_uint8A},
    {"CIMC_uint16A", CIMC_uint16A},
    {"CIMC_uint32A", CIMC_uint32A},
    {"CIMC_uint64A", CIMC_uint64A},
    {"CIMC_SINTA", CIMC_SINTA},
    {"CIMC_sint8A", CIMC_sint8A},
    {"CIMC_sint16A", CIMC_sint16A},
    {"CIMC_sint32A", CIMC_sint32A},
    {"CIMC_sint64A", CIMC_sint64A},
    {"CIMC_INTEGERA", CIMC_INTEGERA},

    {"CIMC_ENCA", CIMC_ENCA},
    {"CIMC_stringA", CIMC_stringA},
    {"CIMC_charsA", CIMC_charsA},
    {"CIMC_dateTimeA", CIMC_dateTimeA},
    {"CIMC_instanceA", CIMC_instanceA},
    {"CIMC_refA", CIMC_refA},
    {"CIMC_ptrA", CIMC_ptrA},
    {"CIMC_charsptrA", CIMC_charsptrA},

    // the following are CIMCObjectPath key-types synonyms
    // and are valid only when CIMC_keyValue of CIMCValueState is set

    {"CIMC_keyInteger", CIMC_keyInteger},
    {"CIMC_keyString", CIMC_keyString},
    {"CIMC_keyBoolean", CIMC_keyBoolean},
    {"CIMC_keyRef", CIMC_keyRef},

    // the following are predicate types only

    {"CIMC_charString", CIMC_charString},
    {"CIMC_integerString", CIMC_integerString},
    {"CIMC_realString", CIMC_realString},
    {"CIMC_numericString", CIMC_numericString},
    {"CIMC_booleanString", CIMC_booleanString},
    {"CIMC_dateTimeString", CIMC_dateTimeString},
    {"CIMC_classNameString", CIMC_classNameString},
    {"CIMC_nameString", CIMC_nameString},
    {NULL, 0}
    
};

typedef struct {
    char const *name;
    CIMCValueState value;
}valstate_mapping_t;

static valstate_mapping_t const valstate_mapping[] = {
    {"CIMC_goodValue", CIMC_goodValue},
    {"CIMC_nullValue", CIMC_nullValue},
    {"CIMC_keyValue", CIMC_keyValue},
    {"CIMC_notFound", CIMC_notFound},
    {"CIMC_badValue", CIMC_badValue},
    {NULL, 0},
};

#define CIMCRelease(o) ((o)->ft->release((o)))

static void
dealloc(CIMCData *d)
{
    if (  ((d)->type < CIMC_instance  || (d)->type > CIMC_dateTime)
       && ((d)->type < CIMC_instanceA || (d)->type > CIMC_dateTimeA)
       && d->type != CIMC_chars && d->type != CIMC_charsptr) return;

    if (d->state != CIMC_goodValue && d->state != CIMC_keyValue) return;

    if (d->type & CMPI_ARRAY) {
        CIMCRelease(d->value.array);
    }else {
        switch (d->type) {
            case CIMC_instance: CIMCRelease(d->value.inst); break;
            case CIMC_ref: CIMCRelease(d->value.ref); break;
            case CIMC_args: d->value.args->ft->release(*d->value.args); break;
            case CIMC_class: CIMCRelease(d->value.cls); break;
            //case CIMC_filter: CIMCRelease(d->value.filter); break;
            case CIMC_enumeration: CIMCRelease(d->value.Enum); break;
            case CIMC_string: CIMCRelease(d->value.string); break;
            case CIMC_dateTime: CIMCRelease(d->value.dateTime); break;
            case CIMC_chars: free(d->value.chars); break;
            case CIMC_charsptr:
                 free(d->value.dataPtr.ptr);
                 d->value.dataPtr.length = 0;
                 break;
            default: break;
        }
    }
    d->state = CMPI_nullValue;
    memset(&d->value, 0, sizeof(CIMCValue));
}

static VALUE itype2str(CIMCType t)
{
    switch (t) {
        case CIMC_string: return rb_str_new2("CIMC_string");
        case CIMC_chars:  return rb_str_new2("CIMC_chars");
        default: return rb_hash_lookup(s_types_i2s, INT2FIX(t));
    }
}

static VALUE numeric2str(VALUE v) {
    VALUE tmp = rb_str_new("", 0);
    char buf[50];
    switch (TYPE(v)) {
        case T_FIXNUM: tmp = rb_fix2str(v, 10); break;
        case T_BIGNUM: tmp = rb_big2str(v, 10); break;
        case T_FLOAT:
            snprintf(buf, 50, "%f", NUM2DBL(v));
            tmp = rb_str_new2(buf);
            break;
    }
    return tmp;
}

static void do_set_type(CIMCData *data, VALUE type)
{
    switch (TYPE(type)) {
        case T_STRING:
            if (strncmp("CIMC_", STR2CSTR(type), 5)) {
                type = rb_str_concat(rb_str_new2("CIMC_"), type);
            }
            type = rb_intern(STR2CSTR(type));
        case T_SYMBOL:
            type = rb_const_get(types_module, type);
            break;
        case T_FIXNUM:
            if (  !RHASH(s_types_i2s)->tbl
               || !st_lookup(RHASH(s_types_i2s)->tbl, type, NULL)) {
                data->state = CIMC_badValue;
                rb_raise(rb_eTypeError, "unsupported value of cimc type: 0x%x",
                        FIX2UINT(type));
            }
            break;
        default:
            data->state = CIMC_badValue;
            rb_raise(rb_eTypeError,
                    "expected one of CIMC_* symbols or Fixnum, not: %s",
                    STR2CSTR(rb_any_to_s(rb_obj_class(type))));
            break;
    }
    if (data->type != (CIMCType) FIX2UINT(type)) {
        dealloc(data);
        data->type = (CIMCType) FIX2UINT(type);
        data->state = CIMC_nullValue;
    }
}

static void do_set_value(CIMCData *data, VALUE value)
{
    dealloc(data);
    data->state = CIMC_goodValue;

    switch (TYPE(value)) {
        case T_NIL:
            /*
            if (data->type != CIMC_nullValue) {
                data->state = CIMC_badValue;
                rb_raise(rb_eTypeError,
                        "null value can be set only for CIMC_nullValse type");
            }else {
            */
                data->state = CIMC_nullValue;
            //}
            break;

        case T_SYMBOL:
            value = rb_any_to_s(value);
        case T_STRING:
            if ((data->type & (CIMC_UINT | CIMC_char16))) {
                uint64_t tmp = NUM2ULL(rb_str2inum(value, 10));
                switch (data->type) {
                    case CIMC_uint32: data->value.uint32 = tmp; break;
                    case CIMC_uint16: data->value.uint16 = tmp; break;
                    case CIMC_uint8:  data->value.uint8 = tmp; break;
                    default: data->value.uint64 = tmp; break;
                }
            }else if (data->type & CIMC_SINT) {
                int64_t tmp = NUM2LL(rb_str2inum(value, 10));
                switch (data->type) {
                    case CIMC_sint32: data->value.sint32 = tmp; break;
                    case CIMC_sint16: data->value.sint16 = tmp; break;
                    case CIMC_sint8: data->value.sint8 = tmp; break;
                    default: data->value.sint64 = tmp; break;
                }
            }else if (data->type & CIMC_REAL) {
                double tmp = NUM2LL(rb_Float(value));
                switch (data->type) {
                    case CIMC_real32: data->value.real32 = tmp; break;
                    default: data->value.real64 = tmp; break;
                }
            }else if (data->type == CIMC_string) {
                data->value.string = cimcEnv->ft->newString(
                        cimcEnv, STR2CSTR(value), NULL);
            }else if (data->type == CIMC_chars) {
                data->value.chars = strdup(STR2CSTR(value));
            }else if (data->type == CIMC_charsptr) {
                data->value.dataPtr.ptr = strdup(STR2CSTR(value));
                data->value.dataPtr.length = rb_funcall(
                        value, rb_intern("length"), 0);
            }else {
                data->state = CIMC_badValue;
                rb_raise(rb_eTypeError, "unsupported data type(%s) for value"
                        ", when data.type set to %s",
                        STR2CSTR(rb_any_to_s(CLASS_OF(value))),
                        STR2CSTR(rb_hash_lookup(s_valstates_i2s, data->type)));
            }
            break;

        case T_TRUE:
        case T_FALSE:
            if (data->type != CIMC_boolean) {
                data->state = CIMC_badValue;
                rb_raise(rb_eTypeError, "boolean values are supported only"
                       " for CIMC_boolean type");
                data->value.boolean = TYPE(value) == T_TRUE;
            }
            break;

        case T_FIXNUM:
        case T_BIGNUM:
        case T_FLOAT:
            if ((data->type & (CIMC_UINT | CIMC_char16))) {
                uint64_t tmp = NUM2ULL(value);
                switch (data->type) {
                    case CIMC_uint32: data->value.uint32 = tmp; break;
                    case CIMC_uint16: data->value.uint16 = tmp; break;
                    case CIMC_uint8:  data->value.uint8 = tmp; break;
                    default: data->value.uint64 = tmp; break;
                }
            }else if (data->type & CIMC_SINT) {
                int64_t tmp = NUM2LL(value);
                switch (data->type) {
                    case CIMC_sint32: data->value.sint32 = tmp; break;
                    case CIMC_sint16: data->value.sint16 = tmp; break;
                    case CIMC_sint8: data->value.sint8 = tmp; break;
                    default: data->value.sint64 = tmp; break;
                }
            }else if (data->type & CIMC_REAL) {
                double tmp = NUM2DBL(value);
                switch (data->type) {
                    case CIMC_real32: data->value.real32 = tmp; break;
                    default: data->value.real64 = tmp; break;
                }
            }else if (data->type == CIMC_string) {
                data->value.string = cimcEnv->ft->newString(
                        cimcEnv, STR2CSTR(numeric2str(value)), NULL);
            }else if (data->type == CIMC_chars) {
                data->value.chars = strdup(STR2CSTR(numeric2str(value)));
            }else if (data->type == CIMC_charsptr) {
                VALUE tmp = numeric2str(value);
                data->value.dataPtr.ptr = strdup(STR2CSTR(tmp));
                data->value.dataPtr.length = rb_funcall(
                        tmp, rb_intern("length"), 0);
            }else {
                data->state = CIMC_badValue;
                rb_raise(rb_eTypeError, "unsupported data type(%s) for value"
                        ", when data.type set to %s",
                        STR2CSTR(rb_any_to_s(CLASS_OF(value))),
                        STR2CSTR(itype2str(data->type)));
            }
            break;

        /* not yet supported
        case T_ARRAY:
        break;
        case T_HASH:
        break;
        */
        case T_DATA:
        default:
            if (CLASS_OF(value) == cSfccCimString) {
                if (data->type & CIMC_string) {
                    Data_Get_Struct(value, CIMCString, data->value.string);
                }else {
                    data->state = CIMC_badValue;
                    rb_raise(rb_eTypeError, "string value can be set only for"
                           " CIMC_string type");
                }
            }else if (CLASS_OF(value) == cSfccCimInstance) {
                if (data->type & CIMC_instance) {
                    CIMCInstance *tmp;
                    CIMCStatus rc;
                    Data_Get_Struct(value, CIMCInstance, tmp);
                    data->value.inst = tmp->ft->clone(tmp, &rc);
                    if (!data->value.inst || rc.rc != CIMC_RC_OK) {
                        data->state = CIMC_badValue;
                        rb_raise(rb_eNoMemError, "failed to clone instance");
                    }
                }else {
                    data->state = CIMC_badValue;
                    rb_raise(rb_eTypeError, "instance can only be set for"
                            " CIMC_instance type");
                }
            }else if (CLASS_OF(value) == cSfccCimObjectPath) {
                if (data->type & CIMC_ref) {
                    CIMCObjectPath *tmp;
                    CIMCStatus rc;
                    Data_Get_Struct(value, CIMCObjectPath, tmp);
                    data->value.ref = tmp->ft->clone(tmp, &rc);
                    if (!data->value.ref || rc.rc != CIMC_RC_OK) {
                        data->state = CIMC_badValue;
                        rb_raise(rb_eNoMemError, "failed to clone object path");
                    }
                }else {
                    data->state = CIMC_badValue;
                    rb_raise(rb_eTypeError, "object path can only be set for"
                            " CIMC_ref type");
                }
            }else {
                VALUE cname;
                const char *class_name;
                data->state = CIMC_badValue;
                cname = rb_funcall(rb_funcall(value, rb_intern("class"), 0),
                        rb_intern("to_s"), 0);
                class_name = to_charptr(cname);
                rb_raise(rb_eTypeError, "unsupported data type: %s",
                        class_name);
            }
    }
}

static VALUE new(VALUE klass, VALUE type, VALUE value)
{
    VALUE res;
    CIMCData *data;
    res = Data_Make_Struct(klass, CIMCData, NULL, dealloc, data);
    do_set_type(data, type);
    data->state = CMPI_nullValue;
    do_set_value(data, value);
    rb_obj_call_init(res, 0, NULL);
    return res;
}

static VALUE from_value(VALUE klass, VALUE value)
{
    VALUE res;
    CIMCData *data;
    res = Data_Make_Struct(klass, CIMCData, NULL, dealloc, data);
    *data = sfcc_value_to_cimdata(value);
    return res;
}

static VALUE type(VALUE self)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    return INT2FIX(data->type);
}

static VALUE type_s(VALUE self)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    return itype2str(data->type);
}

static VALUE set_type(VALUE self, VALUE type)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    do_set_type(data, type);
    return type;
}

static VALUE state(VALUE self)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    return INT2FIX(data->state);
}

static VALUE state_s(VALUE self)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    return rb_hash_lookup(s_valstates_i2s, INT2FIX(data->state));
}

/*
static VALUE set_state(VALUE self, VALUE state)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    switch (TYPE(state)) {
        case T_STRING:
            state = rb_intern(STR2CSTR(state));
        case T_SYMBOL:
            state = rb_const_get(valstates_module, state);
            break;
        case T_FIXNUM:
            if (Qtrue != rb_hash_has_value(s_valstates_i2s, state)) {
                rb_raise(rb_const_get(cim_module, rb_intern("ErrorInvalidValueState")),
                        "unsupported value of cimc valuestate: 0x%x", FIX2UINT(state));
                return Qnil;
            }
            break;
        default:
            rb_raise(rb_const_get(cim_module, rb_intern("ErrorInvalidValueState")),
                    "expected one of CIMC_* symbols or Fixnum, not: %s",
                    STR2CSTR(rb_any_to_s(rb_obj_class(value))));
            return Qnil;
    }
    data->state (CIMCValueState) FIX2UINT(state);
    return state;
}
*/

static VALUE value(VALUE self)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    return sfcc_cimdata_to_value(*data);
}

static VALUE set_value(VALUE self, VALUE value)
{
    CIMCData *data;
    Data_Get_Struct(self, CIMCData, data);
    do_set_value(data, value);
    return value;
}

void init_cim_data()
{
    VALUE sfcc = rb_define_module("Sfcc");
    types_module = rb_define_module_under(sfcc, "Types");
    valstates_module = rb_define_module_under(sfcc, "ValueStates");
    cim_module = rb_define_module_under(sfcc, "Cim");

    VALUE klass = rb_define_class_under(cim_module, "Data", rb_cObject);

    cSfccCimData = klass;
    rb_define_singleton_method(klass, "new", new, 2);
    rb_define_singleton_method(klass, "from_value", from_value, 1);
    rb_define_method(klass, "type", type, 0);
    rb_define_method(klass, "type_s", type_s, 0);
    rb_define_method(klass, "type=", set_type, 1);
    rb_define_method(klass, "state", state, 0);
    rb_define_method(klass, "state_s", state_s, 0);
    rb_define_method(klass, "value", value, 0);
    rb_define_method(klass, "value=", set_value, 1);

    s_types_i2s = rb_hash_new();
    type_mapping_t const *tpptr = type_mapping;
    for (; tpptr->name; ++tpptr) {
        rb_define_const(types_module, tpptr->name, INT2FIX(tpptr->value));
        rb_hash_aset(s_types_i2s, INT2FIX(tpptr->value),
                rb_str_new2(tpptr->name));
    }
    s_valstates_i2s = rb_hash_new();
    valstate_mapping_t const *vsptr = valstate_mapping;
    for (; vsptr->name; ++vsptr) {
        rb_define_const(valstates_module, vsptr->name, INT2FIX(vsptr->value));
        rb_hash_aset(s_valstates_i2s, INT2FIX(vsptr->value),
                rb_str_new2(vsptr->name));
    }

}

VALUE
Sfcc_wrap_cim_data(CIMCData *data)
{
    return Data_Wrap_Struct(cSfccCimData, NULL, dealloc, data);
}

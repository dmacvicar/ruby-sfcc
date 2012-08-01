#include <string.h>
#include <stdint.h>
#include "cim_string.h"
#include "cim_instance.h"
#include "cim_object_path.h"
#include "cim_type.h"
#include "cim_enumeration.h"
#include "cim_class.h"
#include "sfcc.h"
#include "cim_data.h"

VALUE cSfccCimData;
static VALUE cim_module;

typedef struct {
    CIMCData data;
    /* Says, whether data contains value, that is allocated by other
     * module. In that case, value is not freed upon garbage collection
     * of data object
     */
    bool reference;
}rb_sfcc_data;

static void dealloc_cimdata(CIMCData *d) {
    if (  ((d)->type < CIMC_instance  || (d)->type > CIMC_dateTime)
       && ((d)->type < CIMC_instanceA || (d)->type > CIMC_dateTimeA)
       && d->type != CIMC_chars && d->type != CIMC_charsptr
       && !(d->type & CIMC_ARRAY)) return;

    if (d->state != CIMC_goodValue && d->state != CIMC_keyValue) return;

    if (d->type & CIMC_ARRAY) {
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
    d->state = CIMC_nullValue;
    memset(&d->value, 0, sizeof(CIMCValue));
}

static void
dealloc(rb_sfcc_data *rd)
{
    if (!rd->reference) {
        dealloc_cimdata(&rd->data);
    }
    free(rd);
}

static char const * state2cstr(unsigned int state)
{
    char const * s = NULL;
    switch (state) {
        case CIMC_goodValue: s = "Good"; break;
        case CIMC_nullValue: s = "Null"; break;
        case CIMC_keyValue : s = "Key"; break;
        case CIMC_notFound : s = "NotFound"; break;
        case CIMC_badValue : s = "Bad"; break;
        default:
            rb_raise(rb_eRuntimeError, "invalid value of state: 0x%x",
                    state);
            break;
    }
    return s;
}

static char * data2cstr(CIMCData * data)
{
    int ret;
    int const buf_size = 80;
    char buf[buf_size];
    char * result;
    ret = snprintf(buf, buf_size - 1, "Data(state=%s, type=%s",
            state2cstr(data->state),
            Sfcc_cim_type_to_s(data->type));
    if (data->state == CIMC_goodValue) {
        char const * valcstr = "null";
        if (data->type != CIMC_null) {
            VALUE val = sfcc_cimdata_to_value(data, Qnil, true);
            if (val != Qnil) {
                valcstr = to_charptr(val);
            }
        }
        ret += snprintf(buf + ret, buf_size - 1 - ret, ", value=%s", valcstr);
        if (ret >= buf_size - 3) {
            buf[buf_size - 4] = '.';
            buf[buf_size - 3] = '.';
            buf[buf_size - 2] = '.';
            ret = buf_size - 2;
        }
    }
    buf[ret] = ')';
    buf[ret + 1] = '\0';
    result = strndup(buf, ret + 1);
    return result;
}

static VALUE numeric2str(VALUE v)
{
    VALUE tmp = rb_str_new("", 0);
    char buf[100];
    switch (TYPE(v)) {
        case T_FIXNUM: tmp = rb_fix2str(v, 10); break;
        case T_BIGNUM: tmp = rb_big2str(v, 10); break;
        case T_FLOAT:
            snprintf(buf, 100, "%f", NUM2DBL(v));
            tmp = rb_str_new2(buf);
            break;
    }
    return tmp;
}

static VALUE itype2str(CIMCType t)
{
    char const * s = Sfcc_cim_type_to_s(t);
    if (s) {
        return rb_str_new2(s);
    }
    return Qnil;
}


#define CLONEDATAVAL(val_attr) \
    dst->value.val_attr = \
        src->value.val_attr->ft->clone(src->value.val_attr, &rc); \
    if (rc.rc || (!dst->value.val_attr && src->value.val_attr)) return 1;
/**
 * @return 0 on success, 1 on memory error
 */
static int clone_cimdata(CIMCData *dst, CIMCData *src)
{
    CIMCCount i;
    dst->state = src->state;
    dst->type = src->type;
    memset(&dst->value, 0, sizeof(CIMCValue));
    if (src->state == CIMC_nullValue || src->type == CIMC_null) return 0;
    CIMCStatus rc;
    if (dst->type & CIMC_ARRAY) {
        CLONEDATAVAL(array);
        for ( i=0; i < src->value.array->ft->getSize(src->value.array, &rc)
            ; ++i)
        {
            CIMCData tmpd, tmps;
            tmps = src->value.array->ft->getElementAt(
                       src->value.array, i, NULL);
            if (clone_cimdata(&tmpd, &tmps)) {
                CIMCRelease(dst->value.array);
                return 1;
            }
            dst->value.array->ft->setElementAt(dst->value.array, i,
                    &tmpd.value, tmpd.type);
        }
    }else {
        switch(dst->type) {
            case CIMC_chars:
                dst->value.chars = src->value.chars ? strdup(src->value.chars):NULL;
                if (!dst->value.chars && src->value.chars) return 1;
                break;
            case CIMC_charsptr:
                dst->value.dataPtr.ptr = malloc(src->value.dataPtr.length);
                if (!dst->value.dataPtr.ptr) return 1;
                strncpy(dst->value.dataPtr.ptr, src->value.dataPtr.ptr,
                        src->value.dataPtr.length);
                dst->value.dataPtr.length = src->value.dataPtr.length;
                break;
            case CIMC_class      : CLONEDATAVAL(cls); break;
            case CIMC_instance   : CLONEDATAVAL(inst); break;
            case CIMC_ref        : CLONEDATAVAL(ref); break;
            case CIMC_args       : CLONEDATAVAL(args); break;
            case CIMC_enumeration: CLONEDATAVAL(Enum); break;
            case CIMC_string     : CLONEDATAVAL(string); break;
            case CIMC_dateTime   : CLONEDATAVAL(dateTime); break;
            default:
                dst->value = src->value;
                break;
        }
    }
    return 0;
}

static void do_set_type(rb_sfcc_data *data, VALUE type)
{
    char buf[100];
    snprintf(buf, 100, "expected Type instance, Symbol,"
            " String or Fixnum, not: %s",
            to_charptr(rb_obj_class(type))); 
    VALUE type_except = rb_exc_new2(rb_eTypeError, buf);

    switch (TYPE(type)) {
        case T_STRING:
            type = rb_intern(STR2CSTR(type));
        case T_SYMBOL:
            type = rb_const_get(cSfccCimType, type);
            break;
        case T_DATA:
            if (CLASS_OF(type) != cSfccCimType) {
                rb_exc_raise(type_except);
                return;
            }
            type = INT2FIX(Sfcc_rb_type_to_i(type));
        case T_FIXNUM:
            if (!Sfcc_cim_type_to_s(FIX2UINT(type))) return;
            break;
        default:
            data->data.state = CIMC_badValue;
            rb_exc_raise(type_except);
            return;
    }
    if (data->data.type != (CIMCType) FIX2UINT(type)) {
        if (!data->reference) dealloc_cimdata(&data->data);
        data->reference = false;
        data->data.type = (CIMCType) FIX2UINT(type);
        data->data.state = CIMC_nullValue;
    }
}

#define STORE_DATA_VAL(data_type_suf, struct_suf, struct_attr, value_attr, cimc_suf) \
    if (CLASS_OF(value) == cSfccCim ## data_type_suf) { \
        if (d->type == CIMC_ ## cimc_suf) { \
            rb_sfcc_ ## struct_suf * tmp; \
            Data_Get_Struct(value, rb_sfcc_ ## struct_suf, tmp); \
            if (deep_copy) { \
                d->value.value_attr = CIMCClone(tmp->struct_attr, &rc); \
                if (!d->value.value_attr || rc.rc != CIMC_RC_OK) { \
                    d->state = CIMC_badValue; \
                    rb_raise(rb_eNoMemError, "failed to clone " #data_type_suf); \
                } \
            }else { \
                d->value.value_attr = tmp->struct_attr; \
                data->reference = false; \
            } \
        }else { \
            d->state = CIMC_badValue; \
            rb_raise(rb_eTypeError, #data_type_suf \
                    " object can only be set for " #cimc_suf " type"); \
        } \
    } \

/**
 * @param deep_copy | true -> clone underlying value object
 */
static void do_set_value(rb_sfcc_data *data, VALUE value, bool deep_copy)
{
    if (!data->reference) dealloc_cimdata(&data->data);
    CIMCStatus rc;
    CIMCData *d = &data->data;
    data->reference = false;
    d->state = CIMC_goodValue;

    switch (TYPE(value)) {
        case T_NIL:
            /*
            if (data->type != CIMC_nullValue) {
                data->state = CIMC_badValue;
                rb_raise(rb_eTypeError,
                        "null value can be set only for CIMC_nullValse type");
            }else {
            */
                d->state = CIMC_nullValue;
            //}
            break;

        case T_SYMBOL:
            value = rb_any_to_s(value);
        case T_STRING:
            if ((d->type & (CIMC_UINT | CIMC_char16))) {
                uint64_t tmp = NUM2ULL(rb_str2inum(value, 10));
                switch (d->type) {
                    case CIMC_uint32: d->value.uint32 = tmp; break;
                    case CIMC_uint16: d->value.uint16 = tmp; break;
                    case CIMC_uint8:  d->value.uint8 = tmp; break;
                    default: d->value.uint64 = tmp; break;
                }
            }else if (d->type & CIMC_SINT) {
                int64_t tmp = NUM2LL(rb_str2inum(value, 10));
                switch (d->type) {
                    case CIMC_sint32: d->value.sint32 = tmp; break;
                    case CIMC_sint16: d->value.sint16 = tmp; break;
                    case CIMC_sint8: d->value.sint8 = tmp; break;
                    default: d->value.sint64 = tmp; break;
                }
            }else if (d->type & CIMC_REAL) {
                double tmp = NUM2LL(rb_Float(value));
                switch (d->type) {
                    case CIMC_real32: d->value.real32 = tmp; break;
                    default: d->value.real64 = tmp; break;
                }
            }else if (d->type == CIMC_string) {
                d->value.string = cimcEnv->ft->newString(
                        cimcEnv, STR2CSTR(value), NULL);
            }else if (d->type == CIMC_chars) {
                d->value.chars = strdup(STR2CSTR(value));
            }else if (d->type == CIMC_charsptr) {
                d->value.dataPtr.ptr = strdup(STR2CSTR(value));
                d->value.dataPtr.length = rb_funcall(
                        value, rb_intern("length"), 0);
            }else {
                d->state = CIMC_badValue;
                rb_raise(rb_eTypeError, "unsupported data type(%s) for value"
                        ", when data.type set to \"%s\"",
                        to_charptr(rb_obj_class(value)),
                        Sfcc_cim_type_to_s(d->type));
            }
            break;

        case T_TRUE:
        case T_FALSE:
            if (d->type != CIMC_boolean) {
                d->state = CIMC_badValue;
                rb_raise(rb_eTypeError, "boolean values are supported only"
                       " for CIMC_boolean type");
                d->value.boolean = TYPE(value) == T_TRUE;
            }
            break;

        case T_FIXNUM:
        case T_BIGNUM:
        case T_FLOAT:
            if ((d->type & (CIMC_UINT | CIMC_char16))) {
                uint64_t tmp = NUM2ULL(value);
                switch (d->type) {
                    case CIMC_uint32: d->value.uint32 = tmp; break;
                    case CIMC_uint16: d->value.uint16 = tmp; break;
                    case CIMC_uint8:  d->value.uint8 = tmp; break;
                    default: d->value.uint64 = tmp; break;
                }
            }else if (d->type & CIMC_SINT) {
                int64_t tmp = NUM2LL(value);
                switch (d->type) {
                    case CIMC_sint32: d->value.sint32 = tmp; break;
                    case CIMC_sint16: d->value.sint16 = tmp; break;
                    case CIMC_sint8: d->value.sint8 = tmp; break;
                    default: d->value.sint64 = tmp; break;
                }
            }else if (d->type & CIMC_REAL) {
                double tmp = NUM2DBL(value);
                switch (d->type) {
                    case CIMC_real32: d->value.real32 = tmp; break;
                    default: d->value.real64 = tmp; break;
                }
            }else if (d->type == CIMC_string) {
                d->value.string = cimcEnv->ft->newString(
                        cimcEnv, STR2CSTR(numeric2str(value)), NULL);
            }else if (d->type == CIMC_chars) {
                d->value.chars = strdup(STR2CSTR(numeric2str(value)));
            }else if (d->type == CIMC_charsptr) {
                VALUE tmp = numeric2str(value);
                d->value.dataPtr.ptr = strdup(STR2CSTR(tmp));
                d->value.dataPtr.length = rb_funcall(
                        tmp, rb_intern("length"), 0);
            }else {
                d->state = CIMC_badValue;
                rb_raise(rb_eTypeError, "unsupported data type(%s) for value"
                        ", when data.type set to \"%s\"",
                        to_charptr(rb_obj_class(value)),
                        STR2CSTR(itype2str(d->type)));
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
                if (d->type == CIMC_string) {
                    Data_Get_Struct(value, CIMCString, d->value.string);
                    if (deep_copy) {
                        d->value.string = CIMCClone(d->value.string, &rc);
                        if (!d->value.string || rc.rc != CIMC_RC_OK) {
                            d->state = CIMC_badValue;
                            rb_raise(rb_eNoMemError, "failed to clone string");
                        }
                    }else {
                        data->reference = true;
                    }
                }else {
                    d->state = CIMC_badValue;
                    rb_raise(rb_eTypeError, "string value can be set only for"
                           " String type");
                }
            }else STORE_DATA_VAL(Instance, instance, inst, inst, instance)
            else  STORE_DATA_VAL(ObjectPath, object_path, op, ref, ref)
            else  STORE_DATA_VAL(Enumeration, enumeration, enm, Enum, enumeration)
            else if (CLASS_OF(value) == cSfccCimClass) {
                if (d->type == CIMC_class) {
                    Data_Get_Struct(value, CIMCClass, d->value.cls);
                    if (deep_copy) {
                        d->value.cls = CIMCClone(d->value.cls, &rc);
                        if (!d->value.cls || rc.rc != CIMC_RC_OK) {
                            d->state = CIMC_badValue;
                            rb_raise(rb_eNoMemError, "failed to clone class");
                        }
                    }else {
                        data->reference = true;
                    }
                }else {
                    d->state = CIMC_badValue;
                    rb_raise(rb_eTypeError,
                            "class can only be set for Class type");
                }
            }else {
                VALUE cname;
                const char *class_name;
                d->state = CIMC_badValue;
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
    VALUE res = Qnil;
    CIMCData tmp;
    tmp.state = CIMC_nullValue;
    tmp.type = CIMC_null;
    if ((res = Sfcc_wrap_cim_data(&tmp)) != Qnil) {
        rb_sfcc_data *data;
        Data_Get_Struct(res, rb_sfcc_data, data);
        do_set_type(data, type);
        data->reference = false;
        data->data.state = CIMC_nullValue;
        do_set_value(data, value, true); //make a deep copy of value
        rb_obj_call_init(res, 0, NULL);
    }
    return res;
}

static VALUE from_value(VALUE klass, VALUE value)
{
    VALUE res = Qnil;
    CIMCData tmp;
    tmp.state = CIMC_nullValue;
    tmp.type = CIMC_null;
    if ((res = Sfcc_wrap_cim_data(&tmp)) != Qnil) {
        rb_sfcc_data *data;
        Data_Get_Struct(res, rb_sfcc_data, data);
        data->data = sfcc_value_to_cimdata(value);
        data->reference = TYPE(value) == T_DATA ? true:false;
    }
    return res;
}

static VALUE type(VALUE self)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    return Sfcc_wrap_cim_type(data->data.type);
}

static VALUE set_type(VALUE self, VALUE type)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    do_set_type(data, type);
    return type;
}

static VALUE state(VALUE self)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    return INT2FIX(data->data.state);
}

static VALUE state_s(VALUE self)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    char const * s = state2cstr(data->data.state);
    if (s) {
        return rb_str_new2(s);
    }
    return Qnil;
}

/**
 * call-seq:
 *   state_is(state) -> Boolean
 *
 * Check the value state
 */
static VALUE state_is(VALUE self, VALUE state)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    switch (TYPE(state)) {
        case T_STRING:
            state = rb_intern(STR2CSTR(state));
        case T_SYMBOL:
            state = rb_const_get(cSfccCimData, state);
            break;
        case T_FIXNUM:
            if (!state2cstr(FIX2UINT(state))) {
                return Qfalse;
            }
            break;
        default:
            rb_raise(rb_eTypeError,
                    "unsupported state type(%s),"
                    " supported are Fixnum, Symbol or String",
                    STR2CSTR(rb_any_to_s(rb_obj_class(state))));
            return Qfalse;
    }
    if (data->data.state == FIX2INT(state)) return Qtrue;
    return Qfalse;
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

/**
 * call-seq:
 *   value()
 *
 * Get the value of the data
 */
static VALUE value(VALUE self)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    return sfcc_cimdata_to_value(&data->data, Qnil, true);
}

static VALUE set_value(VALUE self, VALUE value)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    do_set_value(data, value, true);
    return value;
}

static VALUE to_s(VALUE self)
{
    rb_sfcc_data *data;
    Data_Get_Struct(self, rb_sfcc_data, data);
    char * cstr = data2cstr(&data->data);
    VALUE ret;
    if (cstr) {
        ret = rb_str_new2(cstr);
        free(cstr);
    }else {
        rb_raise(rb_eNoMemError, "failed to allocate string for data");
        return Qnil;
    }
    return ret;
}

void init_cim_data()
{
    VALUE sfcc = rb_define_module("Sfcc");
    cim_module = rb_define_module_under(sfcc, "Cim");

    VALUE klass = rb_define_class_under(cim_module, "Data", rb_cObject);

    cSfccCimData = klass;
    rb_define_singleton_method(klass, "new", new, 2);
    rb_define_singleton_method(klass, "from_value", from_value, 1);
    rb_define_method(klass, "type", type, 0);
    rb_define_method(klass, "type=", set_type, 1);
    rb_define_method(klass, "state", state, 0);
    rb_define_method(klass, "state_s", state_s, 0);
    rb_define_method(klass, "value", value, 0);
    rb_define_method(klass, "value=", set_value, 1);
    rb_define_method(klass, "to_s", to_s, 0);

    rb_define_method(klass, "state_is", state_is, 1);

    rb_define_const(klass, "Good", INT2FIX(CIMC_goodValue)); /* (0) */
    rb_define_const(klass, "Null", INT2FIX(CIMC_nullValue)); /* (1<<8) */
    rb_define_const(klass, "Key", INT2FIX(CIMC_keyValue)); /*  (2<<8) */
    rb_define_const(klass, "NotFound", INT2FIX(CIMC_notFound)); /*  (4<<8) */
    rb_define_const(klass, "Bad", INT2FIX(CIMC_badValue)); /*  (0x80<<8) */

}

VALUE
Sfcc_wrap_cim_data(CIMCData *data)
{
    rb_sfcc_data *d = malloc(sizeof(rb_sfcc_data));
    if (!d) {
        rb_raise(rb_eNoMemError, "failed to allocate data");
        return Qnil;
    }
    d->data = *data;
    d->reference = true;
    return Data_Wrap_Struct(cSfccCimData, NULL, dealloc, d);
}

VALUE
Sfcc_make_rb_cim_data(CIMCData *cimdata) {
    CIMCData d;
    if (clone_cimdata(&d, cimdata)) {
        rb_raise(rb_eNoMemError, "failed to clone CIMCData");
        return Qnil;
    }
    return Sfcc_wrap_cim_data(&d);
}

void Sfcc_free_cim_data(CIMCData *data)
{
    dealloc_cimdata(data);
}

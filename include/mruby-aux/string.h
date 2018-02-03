#ifndef MRUBY_AUX_STRING_H__
#define MRUBY_AUX_STRING_H__ 1

#include "compat/string.h"

#define MRBX_STR_MAX    (MRB_INT_MAX - 1)

static inline struct RString *
_aux_mrb_str_ptr(mrb_state *mrb, mrb_value v)
{
    if (mrb_nil_p(v)) {
        return (struct RString *)NULL;
    } else {
        mrb_check_type(mrb, v, MRB_TT_STRING);
        return mrb_str_ptr(v);
    }
}

static inline struct RString *
_aux_to_str_ptr(mrb_state *mrb, struct RString *p)
{
    return p;
}

static inline struct RString *
_aux_mrb_str_new_cstr(mrb_state *mrb, const char *str)
{
    return mrb_str_ptr(mrb_str_new_cstr(mrb, str));
}

#define RString(V)                                      \
    _Generic((V),                                       \
             mrb_value:             _aux_mrb_str_ptr,   \
             struct RString *:      _aux_to_str_ptr,    \
             char *:                _aux_mrb_str_new_cstr, \
             const char *:          _aux_mrb_str_new_cstr) \
        (mrb, (V))                                      \

static inline struct RString *
mrbx_str_set_len(mrb_state *mrb, struct RString *dest, mrb_int len)
{
    RSTR_SET_LEN(dest, len);
    return dest;
}

static inline struct RString *
mrbx_str_set_len_value(mrb_state *mrb, mrb_value dest, mrb_int len)
{
    mrb_check_type(mrb, dest, MRB_TT_STRING);
    RSTR_SET_LEN(RSTRING(dest), len);
    return RSTRING(dest);
}

#define mrbx_str_set_len(MRB, DEST, LEN)                \
    _Generic((DEST),                                    \
             mrb_value:         mrbx_str_set_len_value, \
             struct RString *:  mrbx_str_set_len)       \
        ((MRB), (DEST), (LEN))                          \

static inline struct RString *
mrbx_str_reserve(mrb_state *mrb, struct RString *str, mrb_int len)
{
    if (RSTR_CAPA(str) < len) {
        mrb_int l = RSTR_LEN(str);
        mrb_str_resize(mrb, VALUE(str), len);
        RSTR_SET_LEN(str, l);
    } else {
        mrb_str_modify(mrb, str);
    }

    return str;
}

static inline struct RString *
mrbx_str_recycle(mrb_state *mrb, struct RString *str, mrb_int len)
{
    if (str && !MRB_FROZEN_P(str)) {
        return mrbx_str_reserve(mrb, str, len);
    }

    return RSTRING(mrb_str_buf_new(mrb, len));
}

static inline struct RString *
mrbx_str_force_recycle(mrb_state *mrb, struct RString *str, mrb_int len)
{
    if (!str) {
        return RSTRING(mrb_str_buf_new(mrb, len));
    } else {
        return mrbx_str_reserve(mrb, str, len);
    }
}

static inline struct RString *
mrbx_str_force_recycle_with_check(mrb_state *mrb, VALUE str, mrb_int len)
{
    if (mrb_nil_p(str)) {
        return RSTRING(mrb_str_buf_new(mrb, len));
    } else {
        mrb_check_type(mrb, str, MRB_TT_STRING);
        return mrbx_str_reserve(mrb, RString(str), len);
    }
}

#define mrbx_str_force_recycle(MRB, STR, LEN)                       \
    _Generic((STR),                                                 \
            struct RString *:   mrbx_str_force_recycle,             \
            mrb_value:          mrbx_str_force_recycle_with_check)  \
        ((MRB), (STR), (LEN))                                       \

#endif /* MRUBY_AUX_STRING_H__ */

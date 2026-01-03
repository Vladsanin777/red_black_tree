#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define CREATE_PRINT_ARRAY(type, syffix) \
    void print_ ## type ## _array(const type * \
            const array, const size_t size) { \
        const type * const end = array + size; \
        for (const type * ptr = array; ptr < end; ptr++) \
            printf("%" # syffix " ", *ptr); \
        putchar('\n'); \
    }

#define CREATE_INPUT_ARRAY(type, syffix) \
    void input_ ## type ## _array(type * const \
            array, const size_t size) { \
        const type * const end = array + size; \
        for (type * ptr = array; ptr < end; ptr++) \
            scanf("%" # syffix, ptr); \
    }

#define CREATE_PREFFIX_SUM_ARRAY(type) \
    type preffix_sum_ ## type ## _array( \
            type * const preffix, const type * \
            const array, size_t size) { \
        const type * const array_end = array + size; \
        *preffix = *array; \
        type * preffix_ptr = preffix + 1; \
        const type * array_ptr = array + 1; \
        for (; array_ptr < array_end; preffix_ptr++, array_ptr++) \
            *preffix_ptr = *array_ptr + *(preffix_ptr - 1); \
        return *(preffix_ptr - 1); \
    }

/* ================= HELPERS ================= */                                 \
#define RB(idx) (array->m_begin[(idx)])                                          \

#define CREATE_BINARY_ARRAY(type, default_value, index_type)                     \
                                                                                 \
struct type##_##index_type##_red_black {                                          \
    type m_value;                                                                 \
    index_type m_left;                                                           \
    index_type m_right;                                                          \
    index_type m_parent;                                                         \
    bool m_is_node : 1;                                                          \
    bool m_is_right : 1;                                                         \
    bool m_is_black : 1;                                                         \
    bool m_is_owner : 1;                                                         \
};                                                                               \
                                                                                 \
struct type##_##index_type##_binary_array {                                       \
    struct type##_##index_type##_red_black * const m_begin;                      \
    index_type m_current_size;                                                   \
    const index_type m_size;                                                     \
    type m_past_element;                                                         \
};                                                                               \
                                                                                 \
/* ================= CONSTRUCTOR ================= */                             \
struct type##_##index_type##_binary_array *                                      \
constructor_##type##_##index_type##_binary_array(size_t size) {                 \
    struct type##_##index_type##_binary_array *array =                           \
        malloc(sizeof(*array));                                                  \
    if (!array) return NULL;                                                     \
                                                                                 \
    *(struct type##_##index_type##_red_black**)&array->m_begin =                 \
        calloc(size, sizeof(*array->m_begin));                                   \
    if (!array->m_begin) { free(array); return NULL; }                           \
                                                                                 \
    *(index_type*)&array->m_size = size;                                         \
    array->m_current_size = 1;                                                   \
                                                                                 \
    struct type##_##index_type##_red_black *root = array->m_begin;               \
    root->m_value = default_value;                                               \
    root->m_is_black = true;                                                     \
    root->m_is_owner = true;                                                     \
    root->m_is_node = true;                                                      \
                                                                                 \
    array->m_past_element = default_value;                                       \
    return array;                                                                \
}                                                                                \
                                                                                 \
static inline index_type new_node_##type##_##index_type##_binary_array(          \
    struct type##_##index_type##_binary_array *array) {                          \
    if (array->m_current_size >= array->m_size) return 0;                       \
    index_type idx = array->m_current_size;                                      \
    array->m_current_size++;                                                    \
    return idx;                                                                 \
}                                                                                \
                                                                                 \
/* ================= FIND / INSERT POSITION ================= */                  \
struct type##_##index_type##_red_black *                                         \
strict_find_##type##_##index_type##_binary_array(                                \
    struct type##_##index_type##_binary_array *array, type value) {              \
                                                                                 \
    index_type cur = 0;                                                          \
                                                                                 \
    for (;;) {                                                                   \
        struct type##_##index_type##_red_black *n = &RB(cur);                    \
                                                                                 \
        if (value == n->m_value) return NULL;                                    \
                                                                                 \
        bool go_right = value > n->m_value;                                      \
        index_type *next = go_right ? &n->m_right : &n->m_left;                 \
                                                                                 \
        if (*next == 0) {                                                        \
            index_type idx = new_node_##type##_##index_type##_binary_array(array); \
            if (!idx) return NULL;                                               \
                                                                                 \
            *next = idx;                                                         \
            struct type##_##index_type##_red_black *c = &RB(idx);                \
            c->m_parent = cur;                                                   \
            c->m_is_right = go_right;                                            \
            return c;                                                            \
        }                                                                        \
        cur = *next;                                                             \
    }                                                                            \
    return NULL;                                                                 \
}                                                                                \
                                                                                 \
/* ================= ROTATIONS ================= */                               \
static void rotate_left_##type##_##index_type##_binary_array(                   \
    struct type##_##index_type##_binary_array *array, index_type x) {            \
    index_type y = RB(x).m_right;                                                \
    if (!y) return; /* Невозможно выполнить левое вращение */                    \
                                                                                 \
    /* 1. Перемещаем левое поддерево y в правое поддерево x */                  \
    RB(x).m_right = RB(y).m_left;                                                \
    if (RB(y).m_left) {                                                          \
        RB(RB(y).m_left).m_parent = x;                                           \
        RB(RB(y).m_left).m_is_right = false;                                     \
    }                                                                            \
                                                                                 \
    /* 2. Устанавливаем родителя для y */                                       \
    RB(y).m_parent = RB(x).m_parent;                                             \
                                                                                 \
    /* 3. Если x был корнем, то y становится корнем */                          \
    if (RB(x).m_is_owner) {                                                      \
        RB(x).m_is_owner = false;                                                \
        RB(y).m_is_owner = true;                                                 \
        /* y теперь корень, поэтому у него нет направления от родителя */       \
        RB(y).m_is_right = false;                                                \
    } else {                                                                     \
        /* Иначе обновляем указатель родителя x на y */                         \
        if (RB(x).m_is_right) {                                                  \
            RB(RB(x).m_parent).m_right = y;                                      \
        } else {                                                                 \
            RB(RB(x).m_parent).m_left = y;                                       \
        }                                                                        \
        RB(y).m_is_right = RB(x).m_is_right;                                     \
    }                                                                            \
                                                                                 \
    /* 4. Перемещаем x в левое поддерево y */                                    \
    RB(y).m_left = x;                                                            \
    RB(x).m_parent = y;                                                          \
    RB(x).m_is_right = false;                                                    \
}                                                                                \
                                                                                 \
static void rotate_right_##type##_##index_type##_binary_array(                  \
    struct type##_##index_type##_binary_array *array, index_type x) {            \
    index_type y = RB(x).m_left;                                                 \
    if (!y) return; /* Невозможно выполнить правое вращение */                   \
                                                                                 \
    /* 1. Перемещаем правое поддерево y в левое поддерево x */                  \
    RB(x).m_left = RB(y).m_right;                                                \
    if (RB(y).m_right) {                                                         \
        RB(RB(y).m_right).m_parent = x;                                          \
        RB(RB(y).m_right).m_is_right = true;                                     \
    }                                                                            \
                                                                                 \
    /* 2. Устанавливаем родителя для y */                                       \
    RB(y).m_parent = RB(x).m_parent;                                             \
                                                                                 \
    /* 3. Если x был корнем, то y становится корнем */                          \
    if (RB(x).m_is_owner) {                                                      \
        RB(x).m_is_owner = false;                                                \
        RB(y).m_is_owner = true;                                                 \
        /* y теперь корень, поэтому у него нет направления от родителя */       \
        RB(y).m_is_right = false;                                                \
    } else {                                                                     \
        /* Иначе обновляем указатель родителя x на y */                         \
        if (RB(x).m_is_right) {                                                  \
            RB(RB(x).m_parent).m_right = y;                                      \
        } else {                                                                 \
            RB(RB(x).m_parent).m_left = y;                                       \
        }                                                                        \
        RB(y).m_is_right = RB(x).m_is_right;                                     \
    }                                                                            \
                                                                                 \
    /* 4. Перемещаем x в правое поддерево y */                                   \
    RB(y).m_right = x;                                                           \
    RB(x).m_parent = y;                                                          \
    RB(x).m_is_right = true;                                                     \
}                                                                                \
                                                                                 \
/* ================= FIXUP ================= */                                   \
static void normalize_##type##_##index_type##_binary_array(                     \
    struct type##_##index_type##_binary_array *array, index_type z) {            \
    /* z - индекс вставленного узла (красного) */                               \
    while (z && !RB(RB(z).m_parent).m_is_black) {                                \
        index_type p = RB(z).m_parent;                                           \
        index_type g = RB(p).m_parent;                                           \
                                                                                 \
        /* Определяем направление от родителя */                                \
        bool p_right = RB(p).m_is_right;                                         \
        index_type u = p_right ? RB(g).m_left : RB(g).m_right;                  \
                                                                                 \
        /* Случай 1: дядя красный */                                            \
        if (u && !RB(u).m_is_black) {                                            \
            RB(p).m_is_black = true;                                             \
            RB(u).m_is_black = true;                                             \
            RB(g).m_is_black = false;                                            \
            z = g;                                                               \
        } else {                                                                 \
            /* Случай 2: z и родитель в разных направлениях */                  \
            if (RB(z).m_is_right != p_right) {                                   \
                z = p;                                                           \
                if (p_right) {                                                   \
                    rotate_left_##type##_##index_type##_binary_array(array, z);  \
                } else {                                                         \
                    rotate_right_##type##_##index_type##_binary_array(array, z); \
                }                                                                \
                /* После вращения обновляем p и g */                             \
                p = RB(z).m_parent;                                              \
                g = RB(p).m_parent;                                              \
                p_right = RB(p).m_is_right;                                      \
            }                                                                    \
                                                                                 \
            /* Случай 3: z и родитель в одном направлении */                    \
            RB(p).m_is_black = true;                                             \
            RB(g).m_is_black = false;                                            \
            if (p_right) {                                                       \
                rotate_right_##type##_##index_type##_binary_array(array, g);     \
            } else {                                                             \
                rotate_left_##type##_##index_type##_binary_array(array, g);      \
            }                                                                    \
        }                                                                        \
    }                                                                            \
    /* Корень всегда черный */                                                   \
    /* Находим корень (узел с m_is_owner = true) */                             \
    for (index_type i = 0; i < array->m_current_size; ++i) {                     \
        if (RB(i).m_is_owner) {                                                  \
            RB(i).m_is_black = true;                                             \
            break;                                                               \
        }                                                                        \
    }                                                                            \
}                                                                                \
                                                                                 \
void reset_##type##_##index_type##_binary_array(                                  \
    struct type##_##index_type##_binary_array * const array);                     \
                                                                                 \
/* ================= INSERT ================= */                                   \
char insert_##type##_##index_type##_binary_array(                                \
    struct type##_##index_type##_binary_array *array, type value) {              \
                                                                                 \
    struct type##_##index_type##_red_black *n =                                  \
        strict_find_##type##_##index_type##_binary_array(array, value);          \
    if (!n) {                                                                    \
        return 1; /* элемент уже есть, просто возвращаем 1 */                   \
    }                                                                            \
                                                                                 \
    n->m_value = value;                                                          \
    n->m_is_node = true;                                                         \
    n->m_is_black = false;                                                       \
    array->m_past_element = value;                                               \
                                                                                 \
    normalize_##type##_##index_type##_binary_array(                              \
        array, (index_type)(n - array->m_begin));                                \
    return 0;                                                                    \
}                                                                                \
                                                                                 \
/* =================== RESET =================== */                              \
void reset_##type##_##index_type##_binary_array(                                  \
    struct type##_##index_type##_binary_array * const array)                     \
{                                                                               \
    memset((void *)array->m_begin, 0,                                           \
            array->m_current_size * sizeof(*array->m_begin));                   \
                                                                                 \
    array->m_current_size = 1;                                                    \
                                                                                 \
    struct type##_##index_type##_red_black *root = array->m_begin;               \
                                                                                 \
    root->m_value    = array->m_past_element;                                    \
    root->m_is_node  = true;                                                     \
    root->m_is_black = true;                                                     \
    root->m_is_owner = true;                                                     \
                                                                                 \
    root->m_left   = 0;                                                          \
    root->m_right  = 0;                                                          \
    root->m_parent = 0;                                                          \
    root->m_is_right = 0;                                                        \
}                                                                                \
                                                                                 \
/* ================= DESTRUCTOR ================= */                               \
void destructor_##type##_##index_type##_binary_array(                            \
    struct type##_##index_type##_binary_array *array) {                          \
    free((void*)array->m_begin);                                                 \
    free(array);                                                                \
}

typedef int64_t s64;
typedef uint32_t u32;

CREATE_INPUT_ARRAY(s64, lld)
CREATE_PRINT_ARRAY(s64, lld)
CREATE_PREFFIX_SUM_ARRAY(s64)
CREATE_BINARY_ARRAY(s64, 0, u32)

int main(void) {
    size_t size_arr1;
    scanf("%zu", &size_arr1);
    {
        s64 *arr1 = malloc(size_arr1 * sizeof(s64));
        struct s64_u32_binary_array *cash1 =
            constructor_s64_u32_binary_array(size_arr1 + 5);
        s64 answer = 0;

        input_s64_array(arr1, size_arr1);
        
        // Вставляем начальный ноль (префиксная сумма пустого отрезка)
        insert_s64_u32_binary_array(cash1, 0);
        
        s64 curr = 0;
        for (size_t i = 0; i < size_arr1; ++i) {
            curr += arr1[i];
            // Если текущая префиксная сумма уже встречалась, нужна вставка
            if (insert_s64_u32_binary_array(cash1, curr) == 1) {
                answer++;
                reset_s64_u32_binary_array(cash1);
                // Начинаем новый отрезок с текущего элемента
                insert_s64_u32_binary_array(cash1, 0);
                curr = arr1[i];
                insert_s64_u32_binary_array(cash1, curr);
            }
        }

        printf("%lld\n", answer);

        destructor_s64_u32_binary_array(cash1);
        free(arr1);
    }
    return 0;
}

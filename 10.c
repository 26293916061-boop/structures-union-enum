#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 6
#define OK 0
#define BAD_ARG 1
#define WRONG_TYPE 2
#define NO_MEMORY 3

typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

typedef struct {
    DataType type;
    Data data;
} Container;

int fail_alloc = 0;

char *make_copy(const char *text) {
    if (fail_alloc == 1) {
        fail_alloc = 0;
        return NULL;
    }

    char *copy = malloc(strlen(text) + 1);

    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, text, strlen(text) + 1);

    return copy;
}

int container_init(Container *c) {
    if (c == NULL) {
        return BAD_ARG;
    }
    c->type = TYPE_EMPTY;
    c->data.string_value = NULL;

    return OK;
}

int container_clear(Container *c) {
    if (c == NULL) {
        return BAD_ARG;
    }
    if (c->type == TYPE_STRING) {
        free(c->data.string_value);
    }
    c->type = TYPE_EMPTY;
    c->data.string_value = NULL;

    return OK;
}

int container_set_int(Container *c, int value) {
    if (container_clear(c) != OK) {
        return BAD_ARG;
    }
    c->data.int_value = value;
    c->type = TYPE_INT;

    return OK;
}

int container_set_float(Container *c, float value) {
    if (container_clear(c) != OK) {
        return BAD_ARG;
    }
    c->data.float_value = value;
    c->type = TYPE_FLOAT;

    return OK;
}

int container_set_char(Container *c, char value) {
    if (container_clear(c) != OK) {
        return BAD_ARG;
    }
    c->data.char_value = value;
    c->type = TYPE_CHAR;

    return OK;
}

int container_set_string(Container *c, const char *value) {
    if (c == NULL || value == NULL) {
        return BAD_ARG;
    }

    char *copy = make_copy(value);

    if (copy == NULL) {
        return NO_MEMORY;
    }
    container_clear(c);
    c->data.string_value = copy;
    c->type = TYPE_STRING;

    return OK;
}

int container_get_int(const Container *c, int *result) {
    if (c == NULL || result == NULL) {
        return BAD_ARG;
    }
    if (c->type != TYPE_INT) {
        return WRONG_TYPE;
    }
    *result = c->data.int_value;

    return OK;
}

int container_get_float(const Container *c, float *result) {
    if (c == NULL || result == NULL) {
        return BAD_ARG;
    }
    if (c->type != TYPE_FLOAT) {
        return WRONG_TYPE;
    }
    *result = c->data.float_value;

    return OK;
}

int container_get_char(const Container *c, char *result) {
    if (c == NULL || result == NULL) {
        return BAD_ARG;
    }
    if (c->type != TYPE_CHAR) {
        return WRONG_TYPE;
    }
    *result = c->data.char_value;

    return OK;
}

int container_get_string(const Container *c, const char **result) {
    if (c == NULL || result == NULL) {
        return BAD_ARG;
    }
    if (c->type != TYPE_STRING) {
        return WRONG_TYPE;
    }
    *result = c->data.string_value;

    return OK;
}

int container_copy(Container *destination, const Container *source) {
    if (destination == NULL || source == NULL) {
        return BAD_ARG;
    }
    if (destination == source) {
        return OK;
    }
    if (source->type == TYPE_STRING) {
        return container_set_string(destination, source->data.string_value);
    }
    container_clear(destination);
    destination->data = source->data;
    destination->type = source->type;

    return OK;
}

const char *container_type_name(const Container *c) {
    if (c == NULL) {
        return "invalid";
    }
    if (c->type == TYPE_EMPTY) {
        return "empty";
    }
    if (c->type == TYPE_INT) {
        return "int";
    }
    if (c->type == TYPE_FLOAT) {
        return "float";
    }
    if (c->type == TYPE_CHAR) {
        return "char";
    }
    if (c->type == TYPE_STRING) {
        return "string";
    }

    return "invalid";
}

void container_print(const Container *c) {
    if (c == NULL) {
        printf("no container\n");
        return;
    }
    if (c->type == TYPE_EMPTY) {
        printf("empty\n");
    } else if (c->type == TYPE_INT) {
        printf("%d\n", c->data.int_value);
    } else if (c->type == TYPE_FLOAT) {
        printf("%.2f\n", c->data.float_value);
    } else if (c->type == TYPE_CHAR) {
        printf("%c\n", c->data.char_value);
    } else if (c->type == TYPE_STRING) {
        printf("\"%s\"\n", c->data.string_value);
    } else {
        printf("invalid type\n");
    }
}

void show(const char *label, const Container *c) {
    printf("%s [%s] ", label, container_type_name(c));
    container_print(c);
}

void show_status(const char *label, int status) {
    printf("%s: ", label);
    if (status == OK) {
        printf("ok\n");
    } else if (status == WRONG_TYPE) {
        printf("wrong type\n");
    } else if (status == NO_MEMORY) {
        printf("no memory\n");
    } else {
        printf("bad argument\n");
    }
}

void demo_array() {
    Container items[SIZE];

    for (int i = 0; i < SIZE; i++) {
        container_init(&items[i]);
    }

    container_set_int(&items[0], 10);
    container_set_float(&items[1], 3.14f);
    container_set_char(&items[2], 'A');
    container_set_string(&items[3], "Hello");
    container_set_int(&items[4], 0);
    container_set_string(&items[5], "Data");

    printf("--- Array ---\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d: ", i);
        show("", &items[i]);
    }

    printf("\n--- Deep copy ---\n");

    Container copy;

    container_init(&copy);
    container_copy(&copy, &items[5]);
    container_set_string(&items[5], "Changed");
    show("original", &items[5]);
    show("copy    ", &copy);

    for (int i = 0; i < SIZE; i++) {
        container_clear(&items[i]);
    }
    container_clear(&copy);
}

void demo_edge_cases() {
    Container c;
    Container other;
    int number = -1;
    const char *text = NULL;
    int status;

    container_init(&c);
    container_init(&other);

    printf("\n--- Edge cases ---\n");

    status = container_get_int(&c, &number);
    show_status("get int from empty", status);
    show("empty", &c);

    container_set_int(&c, 0);
    status = container_get_int(&c, &number);
    printf("zero: %d\n", number);
    show_status("get int zero", status);

    status = container_get_string(&c, &text);
    show_status("get string from int", status);

    container_set_string(&c, "");
    show("empty string", &c);

    printf("string -> int -> string -> float\n");
    container_set_string(&c, "first");
    show("  ", &c);
    container_set_int(&c, 5);
    show("  ", &c);
    container_set_string(&c, "second");
    show("  ", &c);
    container_set_float(&c, 2.5f);
    show("  ", &c);

    container_set_string(&c, "keep me");
    fail_alloc = 1;
    status = container_set_string(&c, "new text");
    show_status("set string with failed malloc", status);
    show("  still", &c);

    container_get_string(&c, &text);
    status = container_set_string(&c, text);
    show_status("set string with its own pointer", status);
    show("  ", &c);

    status = container_copy(&c, &c);
    show_status("copy onto itself", status);
    show("  ", &c);

    container_set_string(&other, "other");
    fail_alloc = 1;
    status = container_copy(&other, &c);
    show_status("copy with failed malloc", status);
    show("  still", &other);

    container_clear(&c);
    container_clear(&c);
    show("cleared twice", &c);
    container_clear(&other);
}

int main() {
    demo_array();
    demo_edge_cases();

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stddef.h>

typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

enum ValueType { VALUE_INT, VALUE_FLOAT, VALUE_TEXT };

typedef struct {
    enum ValueType type;
    UnionData value;
} TaggedValue;

int set_int(TaggedValue *r, int n)
{
    r->value.i = n;
    r->type = VALUE_INT;
    return 1;
}

int set_float(TaggedValue *r, float x)
{
    r->value.f = x;
    r->type = VALUE_FLOAT;
    return 1;
}

int set_text(TaggedValue *r, const char *s)
{
    if (strlen(s) > 19) {
        printf("Error: text is too long (maximum 19 characters)\n");
        return 0;
    }
    strcpy(r->value.str, s);
    r->type = VALUE_TEXT;
    return 1;
}

void show_record(TaggedValue r)
{
    if (r.type == VALUE_INT)
        printf("int = %d\n", r.value.i);
    else if (r.type == VALUE_FLOAT)
        printf("float = %.2f\n", r.value.f);
    else if (r.type == VALUE_TEXT)
        printf("text = %s\n", r.value.str);
    else
        printf("invalid tag, nothing to show\n");
}

int main()
{
    UnionData u;
    StructData s;
    TaggedValue records[5];
    TaggedValue t;
    int i;

    printf("--- Size and alignment ---\n");
    printf("UnionData   : size %lu, alignment %lu\n",
           (unsigned long)sizeof(UnionData), (unsigned long)_Alignof(UnionData));
    printf("StructData  : size %lu, alignment %lu\n",
           (unsigned long)sizeof(StructData), (unsigned long)_Alignof(StructData));
    printf("TaggedValue : size %lu, alignment %lu\n",
           (unsigned long)sizeof(TaggedValue), (unsigned long)_Alignof(TaggedValue));

    printf("\n--- Union addresses (all the same) ---\n");
    printf("u.i   : %p\n", (void *)&u.i);
    printf("u.f   : %p\n", (void *)&u.f);
    printf("u.str : %p\n", (void *)u.str);

    printf("\n--- Structure addresses and offsets ---\n");
    printf("s.i   : %p  offset %lu\n", (void *)&s.i, (unsigned long)offsetof(StructData, i));
    printf("s.f   : %p  offset %lu\n", (void *)&s.f, (unsigned long)offsetof(StructData, f));
    printf("s.str : %p  offset %lu\n", (void *)s.str, (unsigned long)offsetof(StructData, str));

    printf("\n--- Padding ---\n");
    printf("Members add up to %lu bytes, sizeof(StructData) is %lu bytes\n",
           (unsigned long)(sizeof(int) + sizeof(float) + 20),
           (unsigned long)sizeof(StructData));
    printf("Padding added by the compiler: %lu bytes\n",
           (unsigned long)(sizeof(StructData) - (sizeof(int) + sizeof(float) + 20)));

    printf("\n--- Memory for 100 records ---\n");
    printf("100 StructData  : %lu bytes\n", (unsigned long)(100 * sizeof(StructData)));
    printf("100 UnionData   : %lu bytes\n", (unsigned long)(100 * sizeof(UnionData)));
    printf("100 TaggedValue : %lu bytes\n", (unsigned long)(100 * sizeof(TaggedValue)));

    printf("\n--- Five tagged records ---\n");
    set_int(&records[0], 10);
    set_float(&records[1], 220.5);
    set_text(&records[2], "Union Programming");
    set_int(&records[3], 42);
    set_text(&records[4], "Hello");
    for (i = 0; i < 5; i++) {
        printf("Record %d: ", i + 1);
        show_record(records[i]);
    }

    printf("\n--- Edge cases ---\n");
    if (set_text(&t, "ABCDEFGHIJKLMNOPQRS"))
        printf("19 characters accepted: ");
    show_record(t);
    if (!set_text(&t, "ABCDEFGHIJKLMNOPQRST"))
        printf("20 characters rejected, record is unchanged: ");
    show_record(t);

    set_int(&t, 7);
    printf("After set_int  : ");
    show_record(t);
    set_text(&t, "seven");
    printf("After set_text : ");
    show_record(t);
    set_float(&t, 7.5);
    printf("After set_float: ");
    show_record(t);

    t.type = 99;
    printf("Invalid tag    : ");
    show_record(t);

    printf("\n--- Observations ---\n");
    printf("A union stores all its members at the same address, so its size is\n");
    printf("the size of its largest member. A structure keeps every member in\n");
    printf("separate memory, so it is bigger.\n");
    printf("A union saves memory when a record holds only one type at a time,\n");
    printf("but the values cannot be stored together because writing one\n");
    printf("member overwrites the others.\n");
    printf("The enum tag tells us which member is valid, so we only read that one.\n");

    return 0;
}

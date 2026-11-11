
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NAME_LEN      50         
#define LINE_LEN      256
#define MAX_CAPACITY  1000000UL  

typedef struct {
    int   roll;
    char  name[NAME_LEN];
    int   age;
    float gpa;
} Student;

typedef struct {
    Student *data;
    size_t   count;
    size_t   capacity;
} StudentDB;

static int read_line(char *buf, size_t size)
{
    if (!fgets(buf, (int)size, stdin))
        return 0;
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        return 1;
    }
    int c;                               
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    return 2;
}

static int read_int(const char *prompt, int *out)
{
    char buf[LINE_LEN], *end;
    for (;;) {
        printf("%s", prompt);
        int r = read_line(buf, sizeof buf);
        if (r == 0) return 0;
        if (r == 2) { printf("Error: input too long.\n"); continue; }
        errno = 0;
        long v = strtol(buf, &end, 10);
        if (end == buf || errno == ERANGE || v > INT_MAX || v < INT_MIN) {
            printf("Error: please enter a valid whole number.\n");
            continue;
        }
        while (isspace((unsigned char)*end)) end++;
        if (*end != '\0') {
            printf("Error: please enter a valid whole number.\n");
            continue;
        }
        *out = (int)v;
        return 1;
    }
}

static int read_float(const char *prompt, float *out)
{
    char buf[LINE_LEN], *end;
    for (;;) {
        printf("%s", prompt);
        int r = read_line(buf, sizeof buf);
        if (r == 0) return 0;
        if (r == 2) { printf("Error: input too long.\n"); continue; }
        errno = 0;
        double v = strtod(buf, &end);
        if (end == buf || errno == ERANGE) {
            printf("Error: please enter a valid number.\n");
            continue;
        }
        while (isspace((unsigned char)*end)) end++;
        if (*end != '\0') {
            printf("Error: please enter a valid number.\n");
            continue;
        }
        *out = (float)v;
        return 1;
    }
}


static int valid_age(int age)    { return age >= 16 && age <= 100; }
static int valid_gpa(float gpa)  { return gpa >= 0.0f && gpa <= 4.0f; }

static int read_name(char *dest)
{
    char buf[LINE_LEN];
    for (;;) {
        printf("Enter name (max 49 characters): ");
        int r = read_line(buf, sizeof buf);
        if (r == 0) return 0;
        if (r == 2 || strlen(buf) >= NAME_LEN) {
            printf("Error: name too long (maximum 49 characters).\n");
            continue;
        }
        if (buf[0] == '\0') {
            printf("Error: name cannot be empty.\n");
            continue;
        }
        strcpy(dest, buf);
        return 1;
    }
}

static int read_valid_age(int *age)
{
    for (;;) {
        if (!read_int("Enter age (16-100): ", age)) return 0;
        if (valid_age(*age)) return 1;
        printf("Error: age must be between 16 and 100.\n");
    }
}

static int read_valid_gpa(float *gpa)
{
    for (;;) {
        if (!read_float("Enter GPA (0.00-4.00): ", gpa)) return 0;
        if (valid_gpa(*gpa)) return 1;
        printf("Error: GPA must be between 0.00 and 4.00.\n");
    }
}


static int init_db(StudentDB *db, size_t cap)
{
    if (cap == 0 || cap > MAX_CAPACITY || cap > SIZE_MAX / sizeof(Student)) {
        printf("Error: capacity must be between 1 and %lu.\n", MAX_CAPACITY);
        return 0;
    }
    db->data = malloc(cap * sizeof(Student));
    if (db->data == NULL) {
        printf("Error: not enough memory for %zu records.\n", cap);
        return 0;
    }
    db->count = 0;
    db->capacity = cap;
    return 1;
}

static int grow_db(StudentDB *db)
{
    if (db->capacity > SIZE_MAX / 2) {
        printf("Error: capacity overflow.\n");
        return 0;
    }
    size_t new_cap = db->capacity * 2;
    if (new_cap > SIZE_MAX / sizeof(Student)) {
        printf("Error: allocation size overflow.\n");
        return 0;
    }
    Student *tmp = realloc(db->data, new_cap * sizeof(Student));
    if (tmp == NULL) {
        printf("Error: could not expand storage; existing records are safe.\n");
        return 0;
    }
    db->data = tmp;
    db->capacity = new_cap;
    printf("(Storage expanded to %zu records.)\n", new_cap);
    return 1;
}

static void free_db(StudentDB *db)
{
    free(db->data);
    db->data = NULL;
    db->count = db->capacity = 0;
}

static long find_index(const StudentDB *db, int roll)
{
    for (size_t i = 0; i < db->count; i++)
        if (db->data[i].roll == roll)
            return (long)i;
    return -1;
}

static void print_header(void)
{
    printf("%-8s %-49s %-5s %-5s\n", "Roll", "Name", "Age", "GPA");
}

static void print_student(const Student *s)
{
    printf("%-8d %-49s %-5d %.2f\n", s->roll, s->name, s->age, s->gpa);
}

static void add_student(StudentDB *db)
{
    Student s;
    if (!read_int("Enter roll number (positive): ", &s.roll)) return;
    if (s.roll <= 0) {
        printf("Error: roll number must be positive.\n");
        return;
    }
    if (find_index(db, s.roll) != -1) {
        printf("Error: roll number %d already exists.\n", s.roll);
        return;
    }
    if (!read_name(s.name)) return;
    if (!read_valid_age(&s.age)) return;
    if (!read_valid_gpa(&s.gpa)) return;

    if (db->count == db->capacity && !grow_db(db)) {
        printf("Error: record not added.\n");
        return;
    }
    db->data[db->count++] = s;
    printf("Success: student %d added.\n", s.roll);
}

static void display_all(const StudentDB *db)
{
    if (db->count == 0) {
        printf("No records to display.\n");
        return;
    }
    print_header();
    for (size_t i = 0; i < db->count; i++)
        print_student(&db->data[i]);
}

static void search_student(const StudentDB *db)
{
    int roll;
    if (!read_int("Enter roll number to search: ", &roll)) return;
    if (db->count == 0) {
        printf("The collection is empty.\n");
        return;
    }
    long idx = find_index(db, roll);
    if (idx < 0) {
        printf("Error: roll number %d not found.\n", roll);
        return;
    }
    print_header();
    print_student(&db->data[idx]);
}

static void update_student(StudentDB *db)
{
    int roll;
    if (!read_int("Enter roll number to update: ", &roll)) return;
    long idx = find_index(db, roll);
    if (idx < 0) {
        printf("Error: roll number %d not found.\n", roll);
        return;
    }
    Student s = db->data[idx];             
    printf("Enter the new details (roll number stays %d).\n", roll);
    if (!read_name(s.name)) return;
    if (!read_valid_age(&s.age)) return;
    if (!read_valid_gpa(&s.gpa)) return;
    db->data[idx] = s;
    printf("Success: student %d updated.\n", roll);
}

static void delete_student(StudentDB *db)
{
    int roll;
    if (!read_int("Enter roll number to delete: ", &roll)) return;
    long idx = find_index(db, roll);
    if (idx < 0) {
        printf("Error: roll number %d not found.\n", roll);
        return;
    }
    memmove(&db->data[idx], &db->data[idx + 1],
            (db->count - (size_t)idx - 1) * sizeof(Student));
    db->count--;
    printf("Success: student %d deleted.\n", roll);
}

static void show_summary(const StudentDB *db)
{
    if (db->count == 0) {
        printf("No records: GPA summary is not available.\n");
        return;
    }
    const Student *hi = &db->data[0], *lo = &db->data[0];
    double sum = 0.0;
    for (size_t i = 0; i < db->count; i++) {
        const Student *s = &db->data[i];
        if (s->gpa > hi->gpa) hi = s;
        if (s->gpa < lo->gpa) lo = s;
        sum += s->gpa;
    }
    printf("Highest GPA: %s (roll %d) - %.2f\n", hi->name, hi->roll, hi->gpa);
    printf("Lowest GPA : %s (roll %d) - %.2f\n", lo->name, lo->roll, lo->gpa);
    printf("Average GPA: %.2f\n", sum / (double)db->count);
}

static int compare_students(const void *a, const void *b)
{
    const Student *x = a, *y = b;
    if (x->gpa < y->gpa) return 1;
    if (x->gpa > y->gpa) return -1;
    return (x->roll > y->roll) - (x->roll < y->roll);
}

static void display_sorted(const StudentDB *db)
{
    if (db->count == 0) {
        printf("No records to display.\n");
        return;
    }

    Student *copy = malloc(db->count * sizeof(Student));
    if (copy == NULL) {
        printf("Error: not enough memory to sort.\n");
        return;
    }
    memcpy(copy, db->data, db->count * sizeof(Student));
    qsort(copy, db->count, sizeof(Student), compare_students);
    print_header();
    for (size_t i = 0; i < db->count; i++)
        print_student(&copy[i]);
    free(copy);
}

static void print_menu(void)
{
    printf("\n===== Student Records System =====\n"
           "1. Add student\n"
           "2. Display all students\n"
           "3. Search by roll number\n"
           "4. Update student\n"
           "5. Delete student\n"
           "6. GPA summary (highest, lowest, average)\n"
           "7. Display sorted by GPA (descending)\n"
           "0. Exit\n");
}

int main(void)
{
    StudentDB db = { NULL, 0, 0 };
    int cap;

    for (;;) {                                 
        if (!read_int("Enter initial capacity: ", &cap)) return 0;
        if (cap <= 0) {
            printf("Error: capacity must be a positive number.\n");
            continue;
        }
        if (init_db(&db, (size_t)cap)) break;
    }

    int choice;
    for (;;) {
        print_menu();
        if (!read_int("Enter your choice: ", &choice)) break;  
        if (choice == 0) break;
        switch (choice) {
            case 1: add_student(&db);     break;
            case 2: display_all(&db);     break;
            case 3: search_student(&db);  break;
            case 4: update_student(&db);  break;
            case 5: delete_student(&db);  break;
            case 6: show_summary(&db);    break;
            case 7: display_sorted(&db);  break;
            default: printf("Error: invalid choice, try again.\n");
        }
    }

    free_db(&db);
    printf("Memory released. Goodbye!\n");
    return 0;
}

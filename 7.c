#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DESC_LEN 100
#define CMD_LEN 20

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef enum {
    PENDING,
    DONE
} Status;

typedef struct {
    int id;
    char description[DESC_LEN];
    Status status;
} Task;

Task *tasks = NULL;
int count = 0;
int capacity = 0;
int end_of_input = 0;

int read_line(char *buf, int size)
{
    int len, c;

    if (fgets(buf, size, stdin) == NULL) {
        end_of_input = 1;
        return 0;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        return 1;
    }
    c = getchar();
    if (c == EOF || c == '\n')
        return 1;
    while (c != '\n' && c != EOF)
        c = getchar();
    return 2;
}

void make_upper(char *s)
{
    int i;

    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = s[i] - 'a' + 'A';
    }
}

CommandType get_command_type(const char *commandStr)
{
    if (strcmp(commandStr, "ADD") == 0) return CMD_ADD;
    if (strcmp(commandStr, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(commandStr, "VIEW") == 0) return CMD_VIEW;
    if (strcmp(commandStr, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(commandStr, "COMPLETE") == 0) return CMD_COMPLETE;
    if (strcmp(commandStr, "FIND") == 0) return CMD_FIND;
    if (strcmp(commandStr, "HELP") == 0) return CMD_HELP;
    if (strcmp(commandStr, "EXIT") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

int is_blank(const char *s)
{
    int i;

    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] != ' ' && s[i] != '\t')
            return 0;
    }
    return 1;
}

int parse_id(const char *s)
{
    int i, value = 0;

    if (s[0] == '\0') {
        printf("Error: ID is missing.\n");
        return 0;
    }
    if (strlen(s) > 9) {
        printf("Error: ID is too long.\n");
        return 0;
    }
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') {
            printf("Error: ID must be a positive whole number.\n");
            return 0;
        }
        value = value * 10 + (s[i] - '0');
    }
    if (value == 0) {
        printf("Error: ID must be greater than zero.\n");
        return 0;
    }
    return value;
}

int ask_id(const char *prompt)
{
    char buf[20];
    int r;

    printf("%s", prompt);
    r = read_line(buf, sizeof(buf));
    if (r == 0)
        return 0;
    if (r == 2) {
        printf("Error: ID is too long.\n");
        return 0;
    }
    return parse_id(buf);
}

int ask_description(char *dest)
{
    char buf[DESC_LEN];
    int r;

    printf("Enter description: ");
    r = read_line(buf, sizeof(buf));
    if (r == 0)
        return 0;
    if (r == 2) {
        printf("Error: description is too long (maximum %d characters).\n", DESC_LEN - 1);
        return 0;
    }
    if (is_blank(buf)) {
        printf("Error: description cannot be empty.\n");
        return 0;
    }
    strcpy(dest, buf);
    return 1;
}

int find_index(int id)
{
    int i;

    for (i = 0; i < count; i++) {
        if (tasks[i].id == id)
            return i;
    }
    return -1;
}

void add_task()
{
    int id;
    char desc[DESC_LEN];
    Task *bigger;

    id = ask_id("Enter task ID: ");
    if (id == 0)
        return;
    if (find_index(id) != -1) {
        printf("Error: task %d already exists.\n", id);
        return;
    }
    if (!ask_description(desc))
        return;

    if (count == capacity) {
        bigger = realloc(tasks, capacity * 2 * sizeof(Task));
        if (bigger == NULL) {
            printf("Error: could not expand the task list. Task %d not added.\n", id);
            return;
        }
        tasks = bigger;
        capacity = capacity * 2;
        printf("(Task list expanded to %d tasks.)\n", capacity);
    }

    tasks[count].id = id;
    strcpy(tasks[count].description, desc);
    tasks[count].status = PENDING;
    count++;
    printf("Task %d added.\n", id);
}

void delete_task()
{
    int id, index, i;

    if (count == 0) {
        printf("The task list is empty.\n");
        return;
    }
    id = ask_id("Enter task ID to delete: ");
    if (id == 0)
        return;
    index = find_index(id);
    if (index == -1) {
        printf("Error: task %d not found.\n", id);
        return;
    }
    for (i = index; i < count - 1; i++)
        tasks[i] = tasks[i + 1];
    count--;
    printf("Task %d deleted.\n", id);
}

void update_task()
{
    int id, index;
    char desc[DESC_LEN];

    if (count == 0) {
        printf("The task list is empty.\n");
        return;
    }
    id = ask_id("Enter task ID to update: ");
    if (id == 0)
        return;
    index = find_index(id);
    if (index == -1) {
        printf("Error: task %d not found.\n", id);
        return;
    }
    if (!ask_description(desc))
        return;
    strcpy(tasks[index].description, desc);
    printf("Task %d updated.\n", id);
}

void complete_task()
{
    int id, index;

    if (count == 0) {
        printf("The task list is empty.\n");
        return;
    }
    id = ask_id("Enter task ID to complete: ");
    if (id == 0)
        return;
    index = find_index(id);
    if (index == -1) {
        printf("Error: task %d not found.\n", id);
        return;
    }
    if (tasks[index].status == DONE) {
        printf("Task %d is already DONE.\n", id);
        return;
    }
    tasks[index].status = DONE;
    printf("Task %d marked DONE.\n", id);
}

void print_task(Task t)
{
    printf("ID: %d | %s | %s\n", t.id, t.description,
           t.status == DONE ? "DONE" : "PENDING");
}

void find_task()
{
    int id, index;

    if (count == 0) {
        printf("The task list is empty.\n");
        return;
    }
    id = ask_id("Enter task ID to find: ");
    if (id == 0)
        return;
    index = find_index(id);
    if (index == -1) {
        printf("Error: task %d not found.\n", id);
        return;
    }
    print_task(tasks[index]);
}

void view_tasks()
{
    int i;

    if (count == 0) {
        printf("The task list is empty.\n");
        return;
    }
    for (i = 0; i < count; i++)
        print_task(tasks[i]);
}

void show_help()
{
    printf("Commands (not case sensitive):\n");
    printf("  ADD      - add a new task\n");
    printf("  DELETE   - delete a task by ID\n");
    printf("  VIEW     - show all tasks\n");
    printf("  UPDATE   - change the description of a task\n");
    printf("  COMPLETE - mark a task as DONE\n");
    printf("  FIND     - show one task by ID\n");
    printf("  HELP     - show this list\n");
    printf("  EXIT     - quit the program\n");
}

int main()
{
    char line[CMD_LEN];
    int r, done = 0;
    CommandType cmd;

    capacity = 2;
    tasks = malloc(capacity * sizeof(Task));
    if (tasks == NULL) {
        printf("Error: not enough memory.\n");
        return 1;
    }

    printf("Task Manager (type HELP to see the commands)\n");

    while (!done) {
        printf("\nEnter command: ");
        r = read_line(line, sizeof(line));
        if (r == 0) {
            printf("\nEnd of input.\n");
            break;
        }
        if (r == 2) {
            printf("Error: command is too long.\n");
            continue;
        }
        if (is_blank(line))
            continue;

        make_upper(line);
        cmd = get_command_type(line);

        switch (cmd) {
            case CMD_ADD:      add_task();      break;
            case CMD_DELETE:   delete_task();   break;
            case CMD_VIEW:     view_tasks();    break;
            case CMD_UPDATE:   update_task();   break;
            case CMD_COMPLETE: complete_task(); break;
            case CMD_FIND:     find_task();     break;
            case CMD_HELP:     show_help();     break;
            case CMD_EXIT:
                printf("Exiting program.\n");
                done = 1;
                break;
            default:
                printf("Unknown command. Type HELP to see the commands.\n");
        }

        if (end_of_input) {
            printf("\nEnd of input.\n");
            break;
        }
    }

    free(tasks);
    return 0;
}

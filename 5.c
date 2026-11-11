#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 1000

typedef struct {
    int id;
    double width;
    double height;
    double area;
    double perimeter;
} Rectangle;

Rectangle *rects;   
int count = 0;     
int capacity = 0;  

void clear_line() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int read_int(char *text, int min, int max) {
    int x;
    while (1) {
        printf("%s", text);
        int r = scanf("%d", &x);
        if (r == EOF) exit(0);
        clear_line();
        if (r != 1) printf("Enter a whole number.\n");
        else if (x < min || x > max) printf("Must be between %d and %d.\n", min, max);
        else return x;
    }
}

double read_size(char *text) {
    double x;
    while (1) {
        printf("%s", text);
        int r = scanf("%lf", &x);
        if (r == EOF) exit(0);
        clear_line();
        if (r != 1) printf("Enter a number.\n");
        else if (!isfinite(x)) printf("nan and infinity are not allowed.\n");
        else if (x <= 0) printf("Must be greater than 0.\n");
        else if (x > 1000000) printf("Too big (max 1000000).\n");
        else return x;
    }
}

void calculate(int i) {
    rects[i].area = rects[i].width * rects[i].height;
    rects[i].perimeter = 2 * (rects[i].width + rects[i].height);
}

int is_square(int i) {
    return fabs(rects[i].width - rects[i].height) < 0.000001;
}

int find(int id) {
    for (int i = 0; i < count; i++)
        if (rects[i].id == id) return i;
    return -1;
}

void print_one(int i) {
    if (i < 0 || i >= count) {
        printf("Invalid index.\n");
        return;
    }
    printf("ID %d: width %.2f, height %.2f, area %.2f, perimeter %.2f, %s\n",
           rects[i].id, rects[i].width, rects[i].height, rects[i].area,
           rects[i].perimeter, is_square(i) ? "square" : "not a square");
}

void display_all() {
    if (count == 0) {
        printf("No rectangles.\n");
        return;
    }
    for (int i = 0; i < count; i++) print_one(i);
}

void add_rectangle() {
    if (count == MAX) {
        printf("Full, cannot add more than %d.\n", MAX);
        return;
    }
    int id = read_int("ID: ", 1, 2147483647);
    if (find(id) != -1) {
        printf("ID %d already exists.\n", id);
        return;
    }
    double w = read_size("Width: ");
    double h = read_size("Height: ");

    if (count == capacity) {         
        int newcap = capacity * 2;
        if (newcap > MAX) newcap = MAX;
#ifdef FORCE_FAIL
        Rectangle *temp = NULL;      
#else
        Rectangle *temp = realloc(rects, newcap * sizeof(Rectangle));
#endif
        if (temp == NULL) {
            printf("Could not resize the array. Not added.\n");
            return;
        }
        rects = temp;
        capacity = newcap;
    }

    rects[count].id = id;
    rects[count].width = w;
    rects[count].height = h;
    calculate(count);
    count++;
    printf("Added.\n");
}

void search_rectangle() {
    int i = find(read_int("ID to search: ", 1, 2147483647));
    if (i == -1) printf("Not found.\n");
    else print_one(i);
}

void update_rectangle() {
    int i = find(read_int("ID to update: ", 1, 2147483647));
    if (i == -1) {
        printf("Not found.\n");
        return;
    }
    rects[i].width = read_size("New width: ");
    rects[i].height = read_size("New height: ");
    calculate(i);                 
    printf("Updated.\n");
}

void delete_rectangle() {
    int i = find(read_int("ID to delete: ", 1, 2147483647));
    if (i == -1) {
        printf("Not found.\n");
        return;
    }
    for (int j = i; j < count - 1; j++)  
        rects[j] = rects[j + 1];
    count--;
    printf("Deleted.\n");

    if (capacity > 4 && count <= capacity / 4) {   
        Rectangle *temp = realloc(rects, (capacity / 2) * sizeof(Rectangle));
        if (temp != NULL) {
            rects = temp;
            capacity = capacity / 2;
        }
    }
}

void largest_smallest() {
    if (count == 0) {
        printf("No rectangles.\n");
        return;
    }
    int big = 0, small = 0;
    for (int i = 1; i < count; i++) {
        if (rects[i].area > rects[big].area ||
            (rects[i].area == rects[big].area && rects[i].id < rects[big].id))
            big = i;
        if (rects[i].area < rects[small].area ||
            (rects[i].area == rects[small].area && rects[i].id < rects[small].id))
            small = i;
    }
    printf("Largest:  ");
    print_one(big);
    printf("Smallest: ");
    print_one(small);
}

void total_average() {
    if (count == 0) {
        printf("No rectangles, so no total or average.\n");
        return;
    }
    double total = 0;
    for (int i = 0; i < count; i++) total += rects[i].area;
    printf("Total area: %.2f\n", total);
    printf("Average area: %.2f\n", total / count);
}

void show_sorted() {
    if (count == 0) {
        printf("No rectangles.\n");
        return;
    }
    int *order = malloc(count * sizeof(int));
    if (order == NULL) {
        printf("Not enough memory.\n");
        return;
    }
    for (int i = 0; i < count; i++) order[i] = i;

    for (int i = 0; i < count - 1; i++) {          
        for (int j = 0; j < count - 1 - i; j++) {
            Rectangle a = rects[order[j]];
            Rectangle b = rects[order[j + 1]];
            if (a.area > b.area || (a.area == b.area && a.id > b.id)) {
                int t = order[j];
                order[j] = order[j + 1];
                order[j + 1] = t;
            }
        }
    }
    for (int i = 0; i < count; i++) print_one(order[i]);
    free(order);
}

void filter_area() {
    double limit = read_size("Show rectangles with area greater than: ");
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (rects[i].area > limit) {
            print_one(i);
            found++;
        }
    }
    if (found == 0) printf("None found.\n");
}

int main() {
    capacity = read_int("How many rectangles (1-1000)? ", 1, MAX);
    rects = malloc(capacity * sizeof(Rectangle));
    if (rects == NULL) {
        printf("Not enough memory.\n");
        return 1;
    }

    while (count < capacity) {
        printf("Rectangle %d of %d\n", count + 1, capacity);
        add_rectangle();
    }

    while (1) {
        printf("\n1 Add  2 Display  3 Search  4 Update  5 Delete\n");
        printf("6 Largest/Smallest  7 Total/Average  8 Sorted  9 Filter  0 Exit\n");
        int choice = read_int("Choice: ", 0, 9);

        if (choice == 0) break;
        if (choice == 1) add_rectangle();
        if (choice == 2) display_all();
        if (choice == 3) search_rectangle();
        if (choice == 4) update_rectangle();
        if (choice == 5) delete_rectangle();
        if (choice == 6) largest_smallest();
        if (choice == 7) total_average();
        if (choice == 8) show_sorted();
        if (choice == 9) filter_area();
    }

    free(rects);
    printf("Bye\n");
    return 0;
}

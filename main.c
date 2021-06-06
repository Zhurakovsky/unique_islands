#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EARTH '@'
#define WATER '.'

typedef struct {
    size_t width;
    size_t height;
    char *data;
} Map;




void display_map(const Map *map);
void process_map(const Map *src_map, Map *dst_map);

void check_map(const Map *map) {
    size_t size = map->data ? strlen(map->data) : 0;
    if (size != map->width * map->height) {
        fprintf(stderr, "Invalid size: %lu != %lux%lu\n", size, map->width, map->height);
        exit(1);
    }
}

int main(void) {

    // Declare the map
    Map map = { 20, 10, (char *)
        "...@@@@@@.@..@..@@.."
        "..@@@........@.....@"
        "@@@.@@...@..@......."
        ".@..@@@.@.@@@....@@@"
        "..@@...@@@@.@..@.@@."
        "...@@@.@@....@@@..@@"
        "@@@.@@..@.@@@...@@.@"
        "@@@.@.@@@.@.@.@..@.."
        "@.@@..@@@..@..@..@.."
        "@@@@@.....@....@.@@@"
    };

    // Map map = { 20, 10, (char *)
    //    "...@@@@@@.@..@..@@.."
    //     "..@@@@..@..@@......."
    //     ".@@.....@..@.@@@...."
    //     ".@@.@.@...@@.@..@@@."
    //     ".@@.....@@...@@...@@"
    //     ".@..@..@...@@@.@..@@"
    //     ".@....@@..@@..@@.@.."
    //     ".@@@.@..@@.@.@.@@@@@"
    //     ".....@..@.@.@...@.@@"
    //     ".@..@..@.@..@@@@@.@@"
    // };

    // Map map = {4, 4, "@@.."
    //                  "..@@"
    //                  "..@@"
    //                  ".@@@"};
 
    Map new_map = { 0, 0, NULL };

    check_map(&map);
    process_map(&map, &new_map);
    check_map(&new_map);

    // Display the maps
    printf("Raw map:\n");
    display_map(&map);
    printf("\nProcessed map:\n");
    display_map(&new_map);

    // Free what should have been allocated
    if (new_map.data) {
        free(new_map.data);
    }

    return 0;
}

/* Write your own code below */

#define VISITED     1
#define NOT_VISITED 0

typedef struct {
    char label;
    int visited;
    int col;
    int row;
} Cell;

typedef struct {
    Cell cell;
    struct List *next;
} List;

typedef struct {
    List* head;
    List* tail;
} Queue;

Queue* createQueue() {
    Queue* queue;
    queue = (Queue*)malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

int queueIsEmpty(Queue* queue) {
    return ((queue->head == NULL) ? 1 : 0);
}

void insertCell(Queue* queue, Cell cell) {
    List* list;
    list = (List*) malloc(sizeof(List));
    list->next = NULL;
    list->cell = cell;
    if (queue->tail != NULL) {
        queue->tail->next = (struct List *)list;
    } else {
        queue->head = list;
    }
    queue->tail = list;
}

int popCell(Queue* queue, Cell* cell)
{
    if (queueIsEmpty(queue)) {
        return 0;
    }

    List* temp;
    temp = queue->head;

    cell->label = temp->cell.label;
    cell->visited = temp->cell.visited;
    cell->row = temp->cell.row;
    cell->col = temp->cell.col;

    queue->head = (List *)temp->next;
    free(temp);
    if (queue->head ==  NULL) {
        queue->tail = NULL;
    }
    return 1;
}

void display_map(const Map *map) {
    
    size_t total_cnt = 0;
    size_t cols = map->width;
    size_t rows = map->height;

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            printf("%c", map->data[i * cols + j]);
            total_cnt++;
        }
        printf("\n");
    }
}

void process_map(const Map *src, Map *dst) {

    size_t cols = src->width;
    size_t rows = src->height;

    Cell** cell_data = NULL;
    cell_data = (Cell**) malloc(rows * sizeof(Cell*));
    for (size_t i = 0; i < rows; i++) {
        cell_data[i] = (Cell*)malloc(cols * sizeof(Cell));
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            
            Cell* new_cell;
            new_cell = (Cell*)malloc(sizeof(Cell));
            new_cell->label = src->data[i * cols + j];
            new_cell->visited = 0;
            new_cell->col = j;
            new_cell->row = i;

            cell_data[i][j] = *new_cell;

            free(new_cell);
        }
    }

    char curr_label = 'A';
    
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            Cell *new_cell;
            new_cell = &cell_data[i][j];
            char lbl = new_cell->label;
            int vstd = new_cell->visited;
            if ((lbl != WATER) && (vstd == NOT_VISITED)) {
                new_cell->label = curr_label++;
                Queue* queue = createQueue();
                insertCell(queue, *new_cell);
                while (!queueIsEmpty(queue)) {
                    Cell popped_cell;
                    
                    if (popCell(queue, &popped_cell)) {
                        int current_cell_row = popped_cell.row;
                        int current_cell_col = popped_cell.col;
                        popped_cell.visited = VISITED;
                        cell_data[current_cell_row][current_cell_col].visited = popped_cell.visited;
                        cell_data[current_cell_row][current_cell_col].label = popped_cell.label;

                        // Get bottom cell if possible
                        if (current_cell_row < (rows - 1)) {
                            Cell bottom_cell;
                            bottom_cell = cell_data[current_cell_row + 1][current_cell_col];
                            if ((bottom_cell.label != WATER) && (bottom_cell.visited == NOT_VISITED)) {
                                bottom_cell.label = popped_cell.label;
                                insertCell(queue, bottom_cell);
                            }
                        }

                        // Get right cell if possible
                        if (current_cell_col < (cols - 1)) {
                            Cell right_cell;
                            right_cell = cell_data[current_cell_row][current_cell_col + 1];
                            if ((right_cell.label != WATER) && (right_cell.visited == NOT_VISITED)) {
                                right_cell.label = popped_cell.label;
                                insertCell(queue, right_cell);
                            }
                            
                        }

                        // Get left cell if possible
                        if (current_cell_col > 0) {
                            Cell left_cell;
                            left_cell = cell_data[current_cell_row][current_cell_col - 1];
                            if ((left_cell.label != WATER) && (left_cell.visited == NOT_VISITED)) {
                                
                                left_cell.label = popped_cell.label;
                                insertCell(queue, left_cell);
                            }
                        }

                        // Get upper cell if possible
                        if (current_cell_row > 0) {
                            Cell upper_cell;
                            upper_cell = cell_data[current_cell_row-1][current_cell_col];
                            if ((upper_cell.label != WATER) && (upper_cell.visited == NOT_VISITED)) {
                                upper_cell.label = popped_cell.label;
                                insertCell(queue, upper_cell);
                            }
                        }
                    }
                } 
            }
        }
    }
    
    dst->width = cols;
    dst->height = rows;

    char* dst_data;

    dst_data = (char*)calloc(cols*rows+1, sizeof(char));
    dst->data = (char*)calloc(cols*rows+1, sizeof(char));

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            Cell *new_cell;
            new_cell = &cell_data[i][j];
            memcpy(&dst_data[i * cols + j], &new_cell->label, 1);
        }
    }

    memcpy(dst->data, dst_data, strlen(dst_data));

    for (size_t i = 0; i < rows; i++) {
        free(cell_data[i]);
    }
    free(cell_data);
    free(dst_data);
}

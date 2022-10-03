#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

enum
{
    ARGS_QUANT = 1,
    OFFSET_LINES_ = 3,
    OFFSET_COLS_ = 3,
    H_BORDERS = 2,
    W_BORDERS = 2,
    ROWNUMS_OFFSET = 3,
    QUIT_KEY_ = 27,
};

int
digits_quant(size_t n)
{
    int len = 0;
    if (n < 0){
        return -1;
    } else if (n == 0){
        return 1;
    }
    while (n > 0){
        len += 1;
        n /= 10;
    }
    return len;
}

typedef struct File_data
{
    size_t file_size;
    char *buf;
    size_t rows_quant;
    char **rows;
    size_t *rows_len;
    size_t max_row_len;
} File_data;

typedef struct Window_settings
{
    int H;
    int W;
    int n;
    int m;
    WINDOW *win;
} Window_settings;

size_t
get_file_size(FILE *file)
{
    struct stat file_stat;
    fstat(fileno(file), &file_stat);
    return file_stat.st_size;
}

File_data *
get_file_data(FILE *file)
{
    File_data *res = calloc(1, sizeof(*res));

    size_t file_size = get_file_size(file);

    // reading file to the buffer
    char *buf = calloc(file_size + 1, sizeof(*buf));
    size_t breaks_quant = 0;
    char c = 0;
    for (size_t i = 0; (c = fgetc(file)) != EOF; ++i) {
            buf[i] = c;
            if (c == '\n') {
                breaks_quant++;
            }
    }
    buf[file_size] = 0;

    // rows quantity
    size_t rows_quant = file_size > 0 ? breaks_quant + 1 : 0;
    char **rows = calloc(rows_quant, sizeof(*rows));
    size_t rows_index = 0;
    rows[rows_index++] = buf;
    for (size_t i = 0; i < file_size; ++i){
        if (buf[i] == '\n') {
            buf[i] = 0;
            rows[rows_index++] = buf + sizeof(*buf) * (i + 1);
        }
    }

    // rows length
    size_t max_row_len = 0;
    size_t *rows_len = calloc(rows_quant, sizeof(*rows_len));
    for (size_t i = 0; i < rows_quant; ++i){
        size_t len = strlen(rows[i]);
        if (len > max_row_len) {
            max_row_len = len;
        }
        rows_len[i] = len;
    }

    // data to return
    res->file_size = file_size;
    res->buf = buf;
    res->rows_quant = rows_quant;
    res->rows = rows;
    res->rows_len = rows_len;
    res->max_row_len = max_row_len;
    return res;
}

void
free_file_data(File_data *file_data)
{
    if (file_data != NULL){
        free(file_data->buf);
        free(file_data->rows);
        free(file_data->rows_len);
    }
    free(file_data);
}

int
min(int a, int b)
{
    if (a > b){
        return b;
    }
    return a;
}

int
max(int a, int b)
{
    if (a > b){
        return a;
    }
    return b;
}

int
abs(int a)
{
    if (a > 0){
        return a;
    }
    return -a;
}

void
update_win(Window_settings *win_set, char c, File_data *file_data)
{
    WINDOW *win = win_set->win;
    werase(win);

    // calculating rows position based on commands
    size_t rows_quant = file_data->rows_quant;
    int num_width = digits_quant(rows_quant - 1);
    size_t max_row_len = file_data->max_row_len;
    int H = win_set->H;
    int W = win_set->W;
    int n = win_set->n;
    int m = win_set->m;

    // for calculating
    int work_H = H - H_BORDERS;
    int work_W = W - W_BORDERS - (ROWNUMS_OFFSET + num_width);
    int limit_H = rows_quant <= work_H ? 0 : rows_quant - work_H;
    int limit_W = max_row_len <= work_W ? 0 : max_row_len - work_W;
    int delta_1 = 1;
    int delta_2 = work_H;
    switch (c) {
        // delta_1
        case ' ':
        case '\n':
        case (char) KEY_DOWN: n = min(n + delta_1, limit_H); break;
        case (char) KEY_RIGHT: m = min(m + delta_1, limit_W); break;
        case (char) KEY_UP: n = max(n - delta_1, 0); break;
        case (char) KEY_LEFT: m = max(m - delta_1, 0); break;
        // delta_2
        case (char) KEY_NPAGE: n = min(n + delta_2, limit_H); break;
        case (char) KEY_PPAGE: n = max(n - delta_2, 0); break;
        default: break;
    }
    win_set->n = n;
    win_set->m = m;

    // drawing rows
    size_t *rows_len = file_data->rows_len;
    char **rows = file_data->rows;
    for (int i = 0; i < work_H && i + n < rows_quant; ++i){
        int row_num = i + n + 1;
        if (m > rows_len[i + n]){
            mvwprintw(win, i + 1, 1, " %*d| %s\n", num_width, row_num, "");
        } else {
            mvwprintw(win, i + 1, 1, " %*d| %s\n", num_width, row_num, rows[i + n] + sizeof(**rows) * m);
        }
    }

    box(win, 0, 0);
    wrefresh(win);
}

int
main(int argc, char *argv[]) 
{
    if (argc != ARGS_QUANT + 1) {
        fprintf(stderr, "Error! It should be (only) %d command line argument - filename!\n", ARGS_QUANT);
        return -1;
    }

    // open, dump and close the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error! Cannot open file");
        return -1;
    }
    File_data *file_data = get_file_data(file);
    fclose(file);
    
    // setting interface
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    set_escdelay(0);
    // curs_set(0);

    // drawing around window
    printw("%s", argv[1]);
    refresh();

    // setting window
    int H = LINES - 2 * OFFSET_LINES_;
    int W = COLS - 2 * OFFSET_COLS_;
    WINDOW *win = newwin(H, W, OFFSET_LINES_, OFFSET_COLS_);
    keypad(win, TRUE);
    scrollok (win, TRUE);

    // cycle of work
    Window_settings *win_set = calloc(1, sizeof(*win_set));
    win_set->win = win;
    win_set->H = H;
    win_set->W = W;

    char c = 0;
    // initial "update" == initial drawing in window
    update_win(win_set, c, file_data);
    while ((c = wgetch(win)) != QUIT_KEY_) {
        update_win(win_set, c, file_data);
    }

    endwin();

    free_file_data(file_data);
    free(win_set);

    return 0;
}

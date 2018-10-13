
#include <stdio.h>
#include <string.h>
#define MAXSIZE 10000
#define DOUBLESIZE 20000
#define LINE 1
#define COLUMN 2

typedef struct {
    long int line;
    long int column;
    double value;
    int count_line;
} Position;

typedef struct {
    long int min_line;
    long int min_column;
    long int max_line;
    long int max_column;
    long int size;
    double dens;
    int index_vector;
} Info;



typedef Position Item;
#define key(A) (A)
#define less(A, B) (key(A) < key(B))
#define equal(A, B) (key(A) == key(B))
#define more(A, B) (key(A) > key(B))


long int load_from_file(int argc, char *argv[], Position matrix[MAXSIZE], long int count, double zero);
void clean_line();
void clean_line_file(FILE *fp);
long int create_new_position(Position matrix[MAXSIZE], long int count, double zero, int inputs, Position new);
int isnot_zero(double value, double zero);
void print_position(Position matrix[MAXSIZE], long int index);
void print_all_positions(Position matrix[MAXSIZE], long int count);
void print_matrix_info(Position matrix[MAXSIZE], long int count);
long int change_zero(Position matrix[MAXSIZE], long int count, double zero);
unsigned int line_has_no_elements(long int ref, Info matrix_info);
unsigned int column_has_no_elements(long int ref, Info matrix_info);
void line_or_column(Position matrix[MAXSIZE], long int count, double zero, unsigned int flag, char empty_expression[15], unsigned int (*func_test)(long int, Info), int (*func_sort)(Position, Position));
void print_line(Position matrix[MAXSIZE], long int count, long int min, long int max, double zero);
void print_column(Position matrix[MAXSIZE], long int count, long int min, long int max, double zero, long int column);
void save(Position matrix[MAXSIZE], long int count, char* file_name);
int sort_the_line(Position a, Position b);
int sort_the_column(Position a, Position b);
int sort_by_line_column(Position a, Position b);
int sort_by_column_line(Position a, Position b);
int sort_by_lineDensity_line_column(Position a, Position b);
void insertion_sort(Position matrix[MAXSIZE], long int l, long int r, int (*func)(Position, Position));
void count_line_elements(Position matrix[MAXSIZE], int count);
void compress(Position matrix[MAXSIZE], long int count, double zero);
long int compress_line(double vector_value[DOUBLESIZE], long int vector_index[DOUBLESIZE], long int vector_offset[MAXSIZE], Position matrix[MAXSIZE], unsigned int value_count, long int line, unsigned int start, double zero, long int min_column, long int max_column, long int min_line, long int max_line);
void print_compression(double v_value[DOUBLESIZE], long int v_index[DOUBLESIZE], long int v_offset[MAXSIZE], unsigned int value_count, long int min_line, long int max_line);



int main(int argc, char *argv[])
{
    Position matrix[MAXSIZE];
    Position new;
    int inputs;
    long int count;
    int input_action;
    count = 0;
    double zero = 0;
    char write_file[80];
    write_file[0] = '\0';
    int c;
    char order[7];

    count = load_from_file(argc, argv, matrix, count, zero);

    while ((input_action = getchar()) != 'q' && input_action != -1) {
        switch (input_action) {
            case 'a':
                inputs = scanf("%ld %ld %lf", &new.line, &new.column, &new.value);
                clean_line();
                count = create_new_position(matrix, count, zero, inputs, new);
                break;
            case 'p':
                print_all_positions(matrix, count);
                break;
            case 'i':
                print_matrix_info(matrix, count);
                break;
            case 'l':
                line_or_column(matrix, count, zero, LINE, "empty line", line_has_no_elements, sort_the_line);
                break;
            case 'c':
                line_or_column(matrix, count, zero, COLUMN, "empty column", column_has_no_elements, sort_the_column);
                break;
            case 'o':
                if ((c = getchar()) == '\n') insertion_sort(matrix, 0, count-1, sort_by_line_column);
                else {
                    scanf("%6s", order);
                    if (!strcmp(order, "column"))
                        insertion_sort(matrix, 0, count-1, sort_by_column_line);
                    clean_line();
                }
                break;
            case 'z':
                scanf("%lf", &zero);
                clean_line();
                count = change_zero(matrix, count, zero);
                break;
            case 's':
                compress(matrix, count, zero);
                break;
            case 'w':
                if ((c = getchar()) == '\n' && write_file[0] !='\0') save(matrix, count, write_file);
                else { 
                    if (c != '\n') {
                        scanf("%79s", write_file);
                        save(matrix, count, write_file);
                        clean_line();
                    }
                }
                break;
            case '\n':
                break;
            default:
                clean_line();
                break;
        }
    }
    return 0;
}


long int load_from_file(int argc, char *argv[], Position matrix[MAXSIZE], long int count, double zero) {
    /* loads matrix positions from file */

    FILE *fp;
    int inputs, i;
    Position new;

    if (argc > 1) {
        for (i = 1; i < argc; ++i) {
            if ((fp = fopen(argv[i], "r"))) {
                while ((inputs = fscanf(fp, "%ld %ld %lf", &new.line, &new.column, &new.value)) > EOF) {
                    clean_line_file(fp);
                    count = create_new_position(matrix, count, zero, inputs, new);
                }
                fclose(fp);
            }
            else
                perror(argv[i]);
        }
    }
    return count;
}


void clean_line_file(FILE *fp) {
    /* ignores the rest of the line in the file after scanning what we want*/

    int c;

    while ((c = fgetc(fp)) != '\n')
            ;
}


void clean_line() {
    /* ignores the rest of the line in the terminal after scanning what we want */

    int c;

    while ((c = getchar()) != '\n')
            ;
}


long int create_new_position(Position matrix[MAXSIZE], long int count, double zero, int inputs, Position new) {
    /* creates or replaces a position, testing if the value given is zero and reacting accordingly by not creating it or removing the existing position if it was replaced by the zero */

    unsigned int index;
    
    if (inputs < 3 || new.line < 0 || new.column < 0 ) return count;

    index = 0;
    while (index < count && (matrix[index].line != new.line || matrix[index].column != new.column)) ++index;

    if (isnot_zero(new.value, zero)) {
        new.count_line = 0;
        matrix[index] = new;
        if (index > count - 1)
            return ++count;
    }
    else {
        if (index < count) {
            while (index < count - 1) {
            matrix[index] = matrix[index + 1];
            ++index;
        }
        --count;
        }
    }
    return count;   
}


int isnot_zero(double value, double zero) {
    /* checks if a value is equal to the zero defined or not */

    return value != zero;
}


void print_position(Position matrix[MAXSIZE], long int index) { 
    /* prints a position of the matrix */

    printf("[%ld;%ld]=%.3lf\n", matrix[index].line, matrix[index].column, matrix[index].value);
}


void print_all_positions(Position matrix[MAXSIZE], long int count) {
    /* prints all positions of the matrix */

    unsigned int index;

    if (count < 1)
        printf("empty matrix\n");
    else {
        for (index = 0; index < count; ++index)
            print_position(matrix, index);
    }
}


Info calculate_matrix_info(Position matrix[MAXSIZE], Position matrix_vector[MAXSIZE], int flag, long int ref, int count) {
    /* calculates matrix info and if requested adds the elements of a certain line or column to a vector*/

    Info matrix_info;
    int index;

    matrix_info.min_line = matrix_info.max_line = matrix[0].line;
    matrix_info.min_column = matrix_info.max_column = matrix[0].column;
    if (flag) {
        matrix_info.index_vector = 0;
        if ( (flag == LINE && matrix[0].line == ref) || (flag == COLUMN && matrix[0].column == ref) ) {
            matrix_vector[matrix_info.index_vector] = matrix[0];
            ++matrix_info.index_vector;
        }
    }
    
    for (index = 1; index < count; ++index) {
        if (matrix[index].line < matrix_info.min_line)
            matrix_info.min_line = matrix[index].line;
        else if (matrix[index].line > matrix_info.max_line)
            matrix_info.max_line = matrix[index].line;
        if (matrix[index].column < matrix_info.min_column)
            matrix_info.min_column = matrix[index].column;
        else if (matrix[index].column > matrix_info.max_column)
            matrix_info.max_column = matrix[index].column;

        if (flag) {
            if ( (flag == LINE && matrix[index].line == ref) || (flag == COLUMN && matrix[index].column == ref) ) {
                matrix_vector[matrix_info.index_vector] = matrix[index];
                ++matrix_info.index_vector;
            }
        }
    }
    matrix_info.size = (matrix_info.max_line - matrix_info.min_line + 1) * (matrix_info.max_column - matrix_info.min_column + 1);
    matrix_info.dens = 100 * (double)count / matrix_info.size;

    return matrix_info;
}


void print_matrix_info(Position matrix[MAXSIZE], long int count) {
    /* calculates the matrix info and prints it */

    Info matrix_info;
    
    if (count < 1)
        printf("empty matrix\n");
    else {
        matrix_info = calculate_matrix_info(matrix, NULL, 0, 0, count);
        printf("[%ld %ld] [%ld %ld] %ld / %ld = %.3lf%%\n", matrix_info.min_line, matrix_info.min_column, matrix_info.max_line, matrix_info.max_column, count, matrix_info.size, matrix_info.dens);
    }
}


long int change_zero(Position matrix[MAXSIZE], long int count, double zero) {
    /* changes the matrix zero and removes the positions where its value is the matrix zero */

    unsigned int index, index2;   
    long int zero_count;

    index = index2 = 0;
    zero_count = 0;
    for (index = 0; index < count; ++index) {
        if ( isnot_zero(matrix[index].value, zero) ) {
            matrix[index2] = matrix[index];
            ++index2;               
        }
        else
            ++zero_count; 
    }

    return count - zero_count;
}


unsigned int line_has_no_elements(long int ref, Info matrix_info) {
    /* checks if the line in analysis is not between the min_line and max_line or if the line has no elements */

    return ref < matrix_info.min_line || ref > matrix_info.max_line || matrix_info.index_vector == 0;
}


unsigned int column_has_no_elements(long int ref, Info matrix_info) {
    /* checks if the column in analysis is not between the min_column and max_column or if the column has no elements */

    return ref < matrix_info.min_column || ref > matrix_info.max_column || matrix_info.index_vector == 0;
}


void line_or_column(Position matrix[MAXSIZE], long int count, double zero, unsigned int flag, char empty_expression[15], unsigned int (*func_test)(long int, Info), int (*func_sort)(Position, Position)) {
    /* gathers the elements of a matrix line or column and prints it */

    Position matrix_vector[MAXSIZE];
    int inputs;
    long int ref;
    Info matrix_info;

    inputs = scanf("%ld", &ref);
    clean_line();
    if (inputs < 1 || count < 1)
        printf("%s\n", empty_expression);
    else {
        matrix_info = calculate_matrix_info(matrix, matrix_vector, flag, ref, count);
        if (func_test(ref, matrix_info)) 
            printf("%s\n", empty_expression);
        else {
            insertion_sort(matrix_vector, 0, matrix_info.index_vector-1, func_sort);
            (flag == LINE) ? print_line(matrix_vector, matrix_info.index_vector, matrix_info.min_column, matrix_info.max_column, zero) : print_column(matrix_vector, matrix_info.index_vector, matrix_info.min_line, matrix_info.max_line, zero, ref);
        }
    } 
}


void print_line(Position matrix[MAXSIZE], long int count, long int min, long int max, double zero) {
    /* prints a matrix line */

    long int column, index_matrix;

    index_matrix = 0;
    for (column = min; column < max + 1; ++column) {
        if (index_matrix < count && column == matrix[index_matrix].column) {
            printf(" %.3lf",  matrix[index_matrix].value);
            ++index_matrix;
        }
        else
            printf(" %.3lf",  zero);
    }
    printf("\n");
} 


void print_column(Position matrix[MAXSIZE], long int count, long int min, long int max, double zero, long int column) {
    /* prints a matrix column */

    long int line, index_matrix;

    index_matrix = 0;
    for (line = min; line < max + 1; ++line) {
        if (index_matrix < count && line == matrix[index_matrix].line) {
            printf("[%ld;%ld]=%.3lf\n", matrix[index_matrix].line, matrix[index_matrix].column, matrix[index_matrix].value);
            ++index_matrix;
        }
        else
            printf("[%ld;%ld]=%.3lf\n", line, column, zero);
    }
}


void save(Position matrix[MAXSIZE], long int count, char* file_name) {
    /* saves the matrix elements in a file */

    unsigned int index;
    FILE *fp;

    if ((fp = fopen(file_name, "w"))) {
		for (index = 0; index < count; ++index) {
			fprintf(fp, "%ld %ld %.3lf\n", matrix[index].line, matrix[index].column, matrix[index].value);
        }        
		fclose(fp);
	} else perror(file_name);
}


int sort_the_line(Position a, Position b) {
    return less(a.column, b.column);
}


int sort_the_column(Position a, Position b) {
    return less(a.line, b.line);
}


int sort_by_line_column(Position a, Position b) {
    return less(a.line, b.line) || (equal(a.line, b.line) && (less(a.column, b.column)));
}


int sort_by_column_line(Position a, Position b) {
    return less(a.column, b.column) || (equal(a.column, b.column) && (less(a.line, b.line)));
}


int sort_by_lineDensity_line_column(Position a, Position b) {
    return more(a.count_line, b.count_line) || (equal(a.count_line, b.count_line) && less(a.line, b.line)) || (equal(a.count_line, b.count_line) && equal(a.line, b.line) && less(a.column, b.column));
}


void insertion_sort(Position matrix[MAXSIZE], long int l, long int r, int (*func)(Position, Position)) {
    long int i,j;
    Position aux;

    for (i = l+1; i <= r; i++) {
        aux = matrix[i];
        j = i-1;
        while ( j>=l && func(aux, matrix[j]) ) {
            matrix[j+1] = matrix[j];
            j--;
        }
    matrix[j+1] = aux;
    }
}


void count_line_elements(Position matrix[MAXSIZE], int count) {
    /* counts the number of elements of each line and puts it in each matrix  position to use it for sorting purposes in "compress" function */

    int i, index;
    long int line;

    for (index = 0; index < count; ++index) {
        line = matrix[index].line;
        for (i = 0; i < count; ++i) {
            if (matrix[i].line == line) ++matrix[i].count_line;
        }
    }
}


void compress(Position matrix[MAXSIZE], long int count, double zero) {
    /* compresses a matrix */
    
    long int line;
    unsigned int i;
    Position dup_matrix[MAXSIZE]; /* copy of the matrix to be ordered without changing the original matrix */
    long int vector_index[DOUBLESIZE] = {0}; 
    double vector_value[DOUBLESIZE] = {zero};
    long int vector_offset[MAXSIZE] = {0};
    unsigned int value_count = 0; /* counts the vector_value elements */
    unsigned int start; /* stores the matrix position of the first element of the line that is being addressed */
    Info matrix_info;

    if (count < 1)
        printf("empty matrix\n");
    else {
        /* calculates the matrix info and the number of elements of each line */
        matrix_info = calculate_matrix_info(matrix, NULL, 0, 0, count);

        /* with the info we check the density, if it is less or equal then 0.5 we compress it */
        if (matrix_info.dens > 50) {
            printf("dense matrix\n");
        }
        else {      
            for (i = 0; i < count; ++i) dup_matrix[i] = matrix[i]; /* duplicating the original matrix to be ordered */
            count_line_elements(dup_matrix, count);
            insertion_sort(dup_matrix, 0, count-1, sort_by_lineDensity_line_column);
            start = 0;
            /* cycle that takes care of the dup_matrix compression */
            while (start < count) {
                line = dup_matrix[start].line;
                value_count = compress_line(vector_value, vector_index, vector_offset, dup_matrix, value_count, line, start, zero, matrix_info.min_column, matrix_info.max_column, matrix_info.min_line, matrix_info.max_line);
                start += dup_matrix[start].count_line;
            }

            /* printing compression vectors */
            print_compression(vector_value, vector_index, vector_offset, value_count, matrix_info.min_line, matrix_info.max_line);
        }
    } 
}


long int compress_line(double vector_value[DOUBLESIZE], long int vector_index[DOUBLESIZE], long int vector_offset[MAXSIZE], Position matrix[MAXSIZE], unsigned int value_count, long int line, unsigned int start, double zero, long int min_column, long int max_column, long int min_line, long int max_line) {
    /* compresses a line */

    double value_positions[MAXSIZE]; /* stores the values to put in the vector_value */
    long int index_positions[MAXSIZE]; /* stores the positions where to put the values in vector_value and the line in the index_value */
    unsigned int index; /* index to iterate through the matrix number of columns */
    long int vector_value_index; /* index to iterate between the positions of vector_value  */
    long int value_positions_index; /* index to iterate between the positions of value_positions and index_positions */
    long int vector_value_index_start; /* index to iterate and store the position of vector_value where we are starting to compress the line */
    long int column; /* stores the column number */
    long int count_line; /* combines with variable start to iterate between the elementes of matrix */
    int compressed_line = 0; /* flag that stores if is possible to compress the line to exit the line compression cycle */
    int flag = 1; /* flag to alert in case we are trying to put a value in a vector_value position that is already occupied to abort the cycle and reiniciate the compression starting in vector_value_index_start + 1 */
    long int i; 
    
    vector_value_index_start = 0;
    while (!compressed_line) {
        value_positions_index = 0;
        index = 0;
        column = min_column;   
        vector_value_index = vector_value_index_start;
        count_line = 0; 

        /* cycle to test if it is possible to compress the line starting in vector_value_index_start */
        while (flag == 1 && index < max_column - min_column + 1) {
            if (line == matrix[start + count_line].line && column == matrix[start + count_line].column) {
                /* if the vector_value position in analysis (vector_value_index) is occupied by a non-zero value then compressing the line by starting on the vector_value_index_start set will not successed so we can stop the cycle (flag = 0) and try again starting one position to the right*/
                if (!isnot_zero(vector_value[vector_value_index], zero)) {
                    value_positions[value_positions_index] = matrix[start + count_line].value;
                    index_positions[value_positions_index] = vector_value_index;
                    ++value_positions_index;
                    ++vector_value_index;
                    ++index;
                    ++column;
                    ++count_line;
                }
                else flag = 0;
            }  
            else {
                ++column;
                ++vector_value_index;
                ++index;  
            }
        }

        /* checks if it was able to compress the line, if not we need to try again starting one position to the right*/
        if (flag == 1) compressed_line = 1;
        else {
            flag = 1;
            ++vector_value_index_start;
        }
    }

    /* when the compressin is successful for the full line it puts the compression info about the line in analysis in the respective vectors */
    for (i = 0; i < value_positions_index; ++i) {
        vector_index[index_positions[i]] = line;
        vector_value[index_positions[i]] = value_positions[i];
        vector_offset[line - min_line] = vector_value_index_start;
    }

    /* returns the count of the vector value so that when printing vector_value and vector_index we know when to stop */
    if (value_count < vector_value_index)
        return vector_value_index;
    else
        return value_count;    
}


void print_compression(double v_value[DOUBLESIZE], long int v_index[DOUBLESIZE], long int v_offset[MAXSIZE], unsigned int value_count, long int min_line, long int max_line) {
    unsigned int index;
    /* prints the compression vectors */

    printf("value =");
    for (index = 0; index < value_count; ++index) printf(" %.3lf", v_value[index]);
    printf("\n");

    printf("index =");
    for (index = 0; index < value_count; ++index) printf(" %ld", v_index[index]);
    printf("\n");

    printf("offset =");
    for (index = 0; index < max_line - min_line + 1; ++index) printf(" %ld", v_offset[index]);
    printf("\n");
}
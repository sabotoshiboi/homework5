/*
hole = free memeory spalce
block = occupied memeory space
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

const int N = 100;
const double avg_size = 4.0;
const double stddev = 1.0;
int X = 5;
int search_count = 0;

double rand_gen();
double normalRandom();
void reset_memory(int memory[]);
void init_memory(int memory[]);
int generate_random_request();
double memory_utilization(int memory[]);
void release_occupied_block(int memory[]);
void update_size(int memory[]);

int first_fit(int memory[], int s);
int next_fit(int memory[], int s);
int best_fit(int memory[], int s);
int worst_fit(int memory[], int s);

int main()
{
    int memory[N];
    double memory_utilizations[5];
    reset_memory(memory);
    init_memory(memory);

    int total_search = 0;
    int requests = 0;
    for (size_t i = 0; i < X; i++)
    {
        int fail = 0;  // -1 = fail, 0 = succeed
        while (fail > -1)
        {
            int s = generate_random_request();
            // call your memory allocation method below:
            // fail = first_fit(memory, s);
            // fail = next_fit(memory, s);
            //  fail = best_fit(memory, s);
             fail = worst_fit(memory, s);
            requests++;
            total_search += search_count;
            search_count = 0;
        }

        memory_utilizations[i] = memory_utilization(memory);
        release_occupied_block(memory);

    }

    // display the results
    double ratio = (double)total_search / (double)requests;
    printf("ratio = %f\n", ratio);
    double total = 0.0;
    for (size_t i = 0; i < X; i++)
    {
        total += memory_utilizations[i];
    }
    printf("Average memory utilizatioin = %f\n", total / X);

    system("pause");

    return 0;
}

// 0: succeed; -1: failed to allocate
int first_fit(int memory[], int s)
{
    for (int j = 0; j < N - s; j++) {
        if ((memory[j] < 0) && s <= (memory[j]*-1)) {
            for (int k = 0; k < s; k++) {
                memory[j + k] = s;
            }
            update_size(memory);

            return 0;
        }
        search_count++;
    }

    return -1;
}

// return the index of the hole being allocated, or -1 if failed
int next_fit(int memory[], int s)
{
    for (int j = 0; j < N - s; j++) {
        if ((memory[j] < 0) && s <= (memory[j] * -1)) {
            for (int k = 0; k < s; k++) {
                memory[j + k] = s;
            }
            update_size(memory);
            return j;
        }
        search_count++;
    }

    return -1;

}

int best_fit(int memory[], int s)
{
    for (int j = 0; j < N - s; j++) {
        for (int r = 0; r < avg_size; r++) {
            if ((memory[j] < 0) && s == (memory[j] * -1) + r) {
                for (int k = 0; k < s; k++) {
                    memory[j + k] = s;
                }
                update_size(memory);
                return 0;
            }
        }
        search_count++;
    }
    return -1;


}

int worst_fit(int memory[], int s)
{
    int max_index = 0, max_number = 0;
    for (int i = 0; i < N - s; i++) {
        if (memory[i] > max_number) {
            max_number = memory[i];
            max_index = i;
        }
    }

    for (int j = max_index; j < N - s; j++) {
        if ((memory[j] < 0) && s <= (memory[j]*-1)) {
            for (int k = 0; k < s; k++) {
                memory[j + k] = s;
            }
            update_size(memory);
            return 0;
        }
        search_count++;
    }

    return -1;


}


void reset_memory(int memory[])
{
    for (size_t i = 0; i < N; i++)
    {
        memory[i] = 0;
    }

}

int generate_random_request()
{
    srand(time(NULL));
    int s = N;
    while (s < 1 || s > N - 1)
    {
        s = (int)(normalRandom() * stddev + avg_size);
    }
    return s;
}

void init_memory(int memory[])
{
    srand(time(NULL));
    int hole_or_block = rand() % 2;

    int i = 0;
    while (i < N)
    {
        int size = N;
        while (size < 0 || size > N - 1)
        {
            size = (int)(normalRandom() * stddev + avg_size);
        }

        if (i + size >= N)
        {
            size = N - i;
        }

        int value = size;
        if (hole_or_block == 0)
        {
            value = -value;
            hole_or_block = 1;
        }
        else
        {
            hole_or_block = 0;
        }

        for (size_t j = 0; j < size; j++)
        {
            memory[i] = value;
            i++;
        }


    }

}

double rand_gen() {
    return ((double)(rand()) + 1.) / ((double)(RAND_MAX)+1.);
}
double normalRandom() {
    double v1 = rand_gen();
    double v2 = rand_gen();
    return cos(2 * 3.14 * v2) * sqrt(-2. * log(v1));
}

double memory_utilization(int memory[])
{
    int occupied = 0;
    for (size_t i = 0; i < N; i++)
    {
        if (memory[i] > 0)
        {
            occupied++;
        }
    }
    return (double)occupied / (double)N;

}

void release_occupied_block(int memory[])
{
    int occupied_indexes[N];
    int number_occupied_blocks = 0;

    int i = 0;
    while (i < N)
    {
        if (memory[i] > 0)
        {
            occupied_indexes[number_occupied_blocks] = i;
            number_occupied_blocks++;
        }
        i += abs(memory[i]);
    }

    srand(time(NULL));
    int lower = 0;
    int upper = number_occupied_blocks - 1;
    int random_index = rand() % (upper - lower + 1) + lower;
    int block_index = occupied_indexes[random_index];
    int block_size = memory[block_index];
    int new_hole_size = block_size;
    int new_index = block_index;
    if (block_index > 0)
    {
        int before_value = memory[block_index - 1];
        if (before_value < 0)
        {
            new_hole_size += abs(before_value);
            new_index -= abs(before_value);
        }
    }
    if (block_index + block_size < N)
    {
        int after_value = memory[block_index + block_size];
        if (after_value < 0)
        {
            new_hole_size += abs(after_value);
        }
    }

    for (size_t i = 0; i < new_hole_size; i++)
    {
        memory[new_index + i] = -new_hole_size;
    }


}

void update_size(int memory[]) {
    int temp = 0, size, i = 0;

    while (i < N) {
        size = 0;
        if (memory[i] < 0) {
            temp = i;
            for (int j = 0; j < N; j++) {
                if (memory[i + j] < 0)
                    size++;
                else
                    break;
            }
            if (size != -memory[i]) {
                for (int k = 0; k < size; k++) {
                    if ((temp+k) < N-1) {
                        memory[temp + k] = -size;
                    }
                }
            }
            i += size;
        }
        else {
            i++;
        }
    }
}
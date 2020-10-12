#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_RESULT_STRING_LENGTH 32

#define array_length(x) (sizeof(x) / sizeof(x[0]))
#define error(...) (fprintf(stderr, __VA_ARGS__))


struct interval_t {
    double left_border;
    double right_border;
};

double read_interval(struct interval_t *interval) {
    if (printf("Enter interval's left border: ") < 0) {
		error("Cannot write to stdout");
		return -1;
	}
	if (scanf("%lf", &(interval->left_border)) != 1) {
		error("Cannot read interval's left border");
		return -1;
	}
	if (interval->left_border < 0) {
		error("Left border of the interval must be >= 0");
		return -1;
	}
	if (printf("Enter interval's right border: ") < 0) {
		error("Cannot write to stdout");
		return -1;
	}
	if (scanf("%lf", &(interval->right_border)) != 1) {
		error("Cannot read interval's right border");
		return -1;
	}
	if (interval->right_border > M_PI) {
		error("Right border of the interval must be <= pi");
		return -1;
	}
	if (interval->left_border > interval->right_border) {
		error("Right border of the interval must > left border");
		return -1;
	}
	return 0;
}

double use_simpson_rule(struct interval_t interval) {
    return (interval.right_border - interval.left_border) / 6 * (sin(interval.left_border) +
		4 * sin((interval.left_border + interval.right_border) / 2) + sin(interval.right_border));
}

double use_rectangle_method(struct interval_t interval) {
    return (interval.right_border - interval.left_border) * sin((interval.left_border + interval.right_border) / 2);
}

double integrate(struct interval_t interval, unsigned int number_of_rectangles, double (*integrate_function)(struct interval_t)) {
    double area = 0.0;
    double step = (interval.right_border - interval.left_border) / (double)number_of_rectangles;
    for (unsigned int i = 0; i < number_of_rectangles; i++) {
        interval.right_border = interval.left_border + step;
        area += integrate_function (interval);
        interval.left_border = interval.right_border;
    }
    return area;
}

void free_string_array(char** array, unsigned int array_length) {
	for (unsigned int i = 0; i < array_length; i++) {
		free(array[i]);
	}
	free(array);
}

char **calculate_integrals(struct interval_t interval, unsigned int experiments_count, const int *number_of_rectangles) {
    char **results = (char **)malloc(sizeof(char *) * experiments_count);
	if (!results) {
        error("Cannot allocate memory for results\n");
		return NULL;
	}

	for (unsigned int i = 0; i < experiments_count; i++) {
		double integral_rectangle = integrate(interval, number_of_rectangles[i], use_rectangle_method);
		double integral_simpson = integrate(interval, number_of_rectangles[i], use_simpson_rule);

		results[i] = (char *)malloc(sizeof(char) * MAX_RESULT_STRING_LENGTH);
		if (!results[i]) {
			free_string_array(results, i);
			error("Cannot allocate memory for result string in %d experiment\n", i);
			return NULL;
		}

		if (!sprintf(results[i], "%d %.5f %.5f", number_of_rectangles[i], integral_rectangle, integral_simpson)) {
			free_string_array(results, i + 1);
			error("Cannot write results to string in %d experiment\n", i);
			return NULL;
		}
	}

	return results;
}

int main(){
    struct interval_t interval;
    if (read_interval (&interval))
        return 1;

    unsigned int number_of_rectangles[] = {6, 10, 20, 100, 500, 1000};
    size_t experiments_count = array_length(number_of_rectangles);
    char **results = calculate_integrals(interval, experiments_count, number_of_rectangles);
    if (!results)
        return 1;
    for (unsigned int i = 0; i < experiments_count; i++) {
		if (printf("%s\n", results[i]) < 0) {
			error("Cannot write %d result to stdout", i);
			break;
		}
	}
	free_string_array(results, experiments_count);
    return 0;
}

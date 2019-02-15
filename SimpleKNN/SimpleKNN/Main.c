#define _CRT_SECURE_NO_WARNINGS

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

#define DATATYPE double

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

struct sample
{
	DATATYPE * dim;
	uint32_t group;
	DATATYPE tmp_distance;
};

struct knn_data
{
	uint32_t k;
	struct sample ** best_voters;
	struct sample * samples[2];
	uint32_t samples_count[2];
	uint32_t samples_dimensions[2];
};

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

void parse_string_to_sample(struct sample *, char *, uint32_t, uint8_t);
void parse_file_to_samples(struct knn_data *, char *);
void parse_samples_to_file(struct knn_data *, char *);
void knn_algorithm(struct knn_data *);

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

int main()
{
	char tmp_str[256];

	struct knn_data knn;

	printf("Set number of voters (k=):");
	scanf("%d", &knn.k);

	knn.best_voters = (struct samples **) malloc(knn.k * sizeof(struct samples *));

	printf("Provide categorized samples file (train data):");
	scanf("%s", tmp_str);

	parse_file_to_samples(&knn, tmp_str);

	printf("Provide uncategorized samples file (new data):");
	scanf("%s", tmp_str);

	parse_file_to_samples(&knn, tmp_str);

	printf("Perform k-nn algorithm\n");
	knn_algorithm(&knn);

	printf("Do you want to save the output (yes/no)?");
	scanf("%s", tmp_str);

	if (strcmp(tmp_str, "yes") == 0 || strcmp(tmp_str, "y") == 0)
	{
		printf("Where do you want to save the newly categorized data (filepath)?");
		scanf("%s", tmp_str);

		parse_samples_to_file(&knn, tmp_str);
		printf("Completed.\n");
	}

	printf("Training Samples:\n");

	for (int i = 0; i < knn.samples_count[0]; i++)
	{
		printf("Sample %d -", i);

		for (int j = 0; j < knn.samples_dimensions[0]; j++)
			printf(" %f", (knn.samples[0] + i)->dim[j]);

		printf(" | %d\n", (knn.samples[0] + i)->group);
	}

	printf("New Categorized Samples:\n");

	for (int i = 0; i < knn.samples_count[1]; i++)
	{
		printf("Sample %d -", i);

		for (int j = 0; j < knn.samples_dimensions[1]; j++)
			printf(" %f", (knn.samples[1] + i)->dim[j]);

		printf(" | %d\n", (knn.samples[1] + i)->group);
	}

	return 0;
}

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

void parse_string_to_sample(struct sample * sample, char * string, uint32_t max_dimensions,
	uint8_t has_group)
{
	int tmp_cnt = has_group == 0 ? 0 : 1;

	char * tmp_ptr = strtok(string, ",");

	if (has_group == 0)
		sample->group = atoi(tmp_ptr);

	else
		sample->dim[0] = atof(tmp_ptr);

	while ((tmp_ptr = strtok(NULL, ",")) != NULL)
		sample->dim[tmp_cnt++] = atof(tmp_ptr);
}

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

void parse_file_to_samples(struct knn_data * knn, char * filepath)
{
	int tmp_cnt;

	char line[256];

	FILE *file_pointer;

	file_pointer = fopen(filepath, "r");

	fgets(line, 128, file_pointer);

	tmp_cnt = strstr(line, "uncategorized") == NULL ? 0 : 1;

	fgets(line, 128, file_pointer);

	knn->samples_count[tmp_cnt] = atoi(line);

	knn->samples[tmp_cnt] =
		(struct sample *) malloc(knn->samples_count[tmp_cnt] * sizeof(struct sample));

	fgets(line, 128, file_pointer);

	knn->samples_dimensions[tmp_cnt] = atoi(line);

	for (uint32_t i = 0; i < knn->samples_count[tmp_cnt]; i++)
	{
		(knn->samples[tmp_cnt] + i)->dim =
			(DATATYPE *)malloc(knn->samples_dimensions[tmp_cnt] * sizeof(DATATYPE));

		fgets(line, 128, file_pointer);

		parse_string_to_sample(knn->samples[tmp_cnt] + i, line,
			knn->samples_dimensions[tmp_cnt], tmp_cnt);
	}
}

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */

void parse_samples_to_file(struct knn_data * knn, char * filepath)
{
	printf(":) not working yet...");
}

/*   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---    */


void knn_algorithms_sort_asc_voters(struct knn_data * knn)
{
	struct sample * tmp_smpl = NULL;

	for (int i = 0; i < knn->k; i++)
		for (int j = 0; j < knn->k - 1; j++)
			if (knn->best_voters[j]->tmp_distance > knn->best_voters[j + 1]->tmp_distance)
			{
				tmp_smpl = knn->best_voters[j];
				knn->best_voters[j] = knn->best_voters[j + 1];
				knn->best_voters[j + 1] = tmp_smpl;
			}
}

void knn_algorithm(struct knn_data * knn)
{
	double euclidean_distance;

	uint32_t * most_common[2], selected_group_pos;

	most_common[0] = (uint32_t *)malloc(knn->k * sizeof(uint32_t));
	most_common[1] = (uint32_t *)malloc(knn->k * sizeof(uint32_t));

	for (int i = 0; i < knn->samples_count[1]; i++)
	{
		for (int q = 0; q < knn->k; q++)
			knn->best_voters[q] = NULL;

		for (int j = 0; j < knn->samples_count[0]; j++)
		{
			euclidean_distance = 0;

			for (int q = 0; q < knn->samples_dimensions[0]; q++)
				euclidean_distance += pow(
				(knn->samples[0] + j)->dim[q] - (knn->samples[1] + i)->dim[q]
					, 2);

			(knn->samples[0] + j)->tmp_distance = sqrt(euclidean_distance);

			if (j < knn->k)
			{
				knn->best_voters[j] = (knn->samples[0] + j);
			}
			else
			{
				if (j == knn->k)
					knn_algorithms_sort_asc_voters(knn);

				for (int q = 0; q < knn->k; q++)
					if (knn->best_voters[q]->tmp_distance > (knn->samples[0] + j)->tmp_distance)
					{
						for (int z = knn->k - 1; z >= q + 1; z--)
							knn->best_voters[z] = knn->best_voters[z - 1];

						knn->best_voters[q] = (knn->samples[0] + j);

						break;
					}
			}
		}

		memset(most_common[0], 0, knn->k * sizeof(uint32_t));
		memset(most_common[1], 0, knn->k * sizeof(uint32_t));

		for (int j = 0; j < knn->k; j++)
		{
			for (int q = 0; q < knn->k; q++)

				if (*(most_common[0] + q) == knn->best_voters[j]->group)
				{
					*(most_common[1] + q) += 1;
					break;
				}
				else if (*(most_common[0] + q) == 0)
				{
					*(most_common[0] + q) = knn->best_voters[j]->group;
					*(most_common[1] + q) += 1;
					break;
				}
		}

		selected_group_pos = 0;

		for (int j = 1; j < knn->k; j++)
		{
			if (*(most_common[1] + j) == 0)
				break;

			if (*(most_common[0] + j) > *(most_common[0] + selected_group_pos))
				selected_group_pos = j;
		}

		(knn->samples[1] + i)->group = *(most_common[0] + selected_group_pos);
	}
}

/*   ---   ---   ---   ---   ---   ---   ---     ---   ---   ---    */

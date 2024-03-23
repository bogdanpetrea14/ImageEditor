#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//elibereaza matricea care retine pixelii
void free_mat(int *dim, int **image) {
	for (int i = 0; i < dim[0]; i++)
		free(image[i]);
	free(image);
}

void free_mat_effect(int *dim, double **image) {
	for (int i = 0; i < dim[0]; i++)
		free(image[i]);
	free(image);
}
//alocare de memorie pentru vectorul de frecventa
int* alloc_freq(int *dim) {
	int *freq = (int *)calloc(dim[3] + 1, sizeof(int));
	if (!freq) {
		fprintf(stderr, "Error: cannot alloc memory-11-");
		return NULL;
	}
	return freq;
}

int** copy_mat(int **img2, int **img1, int *dim) {
	if (dim[2] == 6 || dim[2] == 3) {
		for (int i = 0; i < dim[0]; i++)
			for (int j = 0; j < dim[1] * 3; j++)
				img2[i][j] = img1[i][j];
		return img2;
	}
	for (int i = 0; i < dim[0]; i++)
		for (int j = 0; j < dim[1]; j++)
			img2[i][j] = img1[i][j];
	return img2;
}
//alocare de memorie pentru a salva imaginile alb-negru
int** alloc_mat_grey(int *dim, int **image) {
	image = (int **)calloc(dim[0], sizeof(int *));
	if (!image) {
		fprintf(stderr, "Cannot alloc memory -1-\n");
		free(image);
		return NULL;
	}
	for (int i = 0; i < dim[0]; i++) {
		image[i] = (int *)calloc(dim[1], sizeof(int));
		if (!image[i]) {
			fprintf(stderr, "Cannot alloc memory -2-\n");
			for (int j = i; j >=0; j--)
				free(image[i]);
			free(image);
		}
	}
	return image;
}
//aloca memorie pentru a salva imaginile color
int** alloc_mat_col(int *dim, int **image) {
	image = (int **)calloc(dim[0], sizeof(int *));
	if (!image) {
		fprintf(stderr, "Cannot alloc memory -1-\n");
		free(image);
		return NULL;
	}
	for (int i = 0; i < dim[0]; i++) {
		image[i] = (int *)calloc(dim[1] * 3, sizeof(int));
		if (!image[i]) {
			fprintf(stderr, "Cannot alloc memory -2-\n");
			for (int j = i; j >=0; j--)
				free(image[i]);
			free(image);
		}
	}
	return image;
}

double** alloc_mat_effect(int *dim, double **effect) {
	effect = (double **)calloc(dim[0], sizeof(double *));
	if (!effect) {
		fprintf(stderr, "Cannot alloc memory -1-\n");
		free(effect);
		return NULL;
	}
	for (int i = 0; i < dim[0]; i++) {
		effect[i] = (double *)calloc(dim[1] * 3, sizeof(double));
		if (!effect[i]) {
			fprintf(stderr, "Cannot alloc memory -2-\n");
			for (int j = i; j >=0; j--)
				free(effect[i]);
			free(effect);
		}
	}
	return effect;
}
//determina magic number-ul pozei (P6, P5, P3, P2);
int which_type(char *name) {
	char t, trash[256], type[3];
	FILE *file = fopen(name, "rb");
	if (file == NULL)
		return -1;
	while (fscanf(file, "%c", &t) == '#')
		fgets(trash, sizeof(trash), file);
	fseek(file, -sizeof(char), 1);
	fscanf(file, "%s", type);
	fclose(file);
	if (type[0] != 'P')
		return -1;
	if (type[1] == '2' || type[1] == '3')
		return 1; //for text files
	else return 2; //for binary files
}
//extrage din fisier o valoare(char) si o converteste la int(pentru
//heights si width si maxsize)
int value(FILE *file) {
	char aux[15], t;
	int i = 0, size;
	do {
		fread(&t, sizeof(char), 1, file);
		aux[i] = t;
		i++;
	} while (t != ' ' && t != '\n');
	aux[i] = '\0';
	size = atoi(aux);
	return size;
}
// incarcarea in memorie a imaginii, plus extragerea numelui fisierului
//se se citeste bit cu bit din fisier, iar liniile care au # pe prima
//linie sunt ignorate, ele fiind comentarii
//se determina tipul imaginii, inaltimea, lungimea si valoarea maxima a
//pixelilor
//se aloca memorie pentru matricea respectiva si se salveaza valorile
//in ea
int** load_image(char *name, int **image, int *dim, char *command) {
	int j = 0, k = strlen(command);
	for (int i = 5; i < k; i++) {
		name[j] = command[i];
		j++;
 	}
 	j = strlen(command) - 5;
 	name[j - 1] = '\0';
 	int which, i = 0;
 	which = which_type(name);
	if (which < 0) {
 		printf("Failed to load %s\n", name);
 		return NULL;
 	}
 	char type[3], trash[256], t;
 	FILE *file;
 	if (which == 2) {
 		file = fopen(name, "rb");
 		if (file == NULL) {
 			fprintf(stderr, "Failed to load\n");
 			return NULL;
 		}
		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -sizeof(char), 1);
 		do {
 			fread(&t, 1, 1, file);
 			type[i] = t;
 			i++;
 		} while(t != ' ' && t != '\n');
 		type[i - 1] = '\0';
 		if (type[0] != 'P')
 			return NULL;
 		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -sizeof(char), 1);
 		dim[1] = value(file);
 		dim[0] = value(file);
 		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -sizeof(char), 1);
 		dim[3] = value(file);
 		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -sizeof(char), 1);
 		if (type[1] == '6') {
 			dim[2] = 6;
 			image = alloc_mat_col(dim, image);
 			if (image == NULL) {
 				fprintf(stderr, "Error: cannot alloc memory -4-\n");
 				return NULL;
 			}
 			for (int i = 0; i < dim[0]; i++)
 				for (int j = 0; j < dim[1] * 3; j++)
 					fread(&image[i][j], 1, 1, file);
 			fclose(file);
 		}
 		if (type[1] == '5') {
 			dim[2] = 5;
 			image = alloc_mat_grey(dim, image);
 			if (image == NULL) {
 				fprintf(stderr, "Error: cannot alloc memory -5-\n");
 				return NULL;
 			}
 			for (int i = 0; i < dim[0]; i++)
 				for (int j = 0; j < dim[1]; j++)
 					fread(&image[i][j], 1, 1, file);
 			fclose(file);
 		}
 	} else {
 		file = fopen(name, "r");
 		if (file == NULL) {
 			fprintf(stderr, "Failed to load\n");
 		return NULL;
 		}
 		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -1, 1);
 		fscanf(file, "%s", type);
 		if (type[0] != 'P')
 			return NULL;
 		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -1, 1);
 		fscanf(file, "%d %d", &dim[1], &dim[0]);
		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -1, 1);
 		fscanf(file, "%d", &dim[3]);
 		while (fscanf(file, "%c", &t) == '#')
 			fgets(trash, sizeof(trash), file);
 		fseek(file, -1, 1);
 		if (type[1] == '3') {
 			dim[2] = 3;
 			image = alloc_mat_col(dim, image);
 			if (image == NULL) {
 				fprintf(stderr, "Error: cannot alloc memory -7-\n");
 				return NULL;
 			}
 			for (int i = 0; i < dim[0]; i++)
 				for (int j = 0; j < dim[1] * 3; j++)
 					fscanf(file, "%d", &image[i][j]);
 			fclose(file);
 		}
 		if (type[1] == '2') {
 			dim[2] = 2;
 			image = alloc_mat_grey(dim, image);
 			if (image == NULL) {
 				fprintf(stderr, "Error: cannot alloc memory -8-\n");
 				return NULL;
 			}
 			for (int i = 0; i < dim[0]; i++)
 				for (int j = 0; j < dim[1]; j++)
 					fscanf(file, "%d", &image[i][j]);
 			fclose(file);
 		}
 	}
 	if (image == NULL) {
 		fprintf(stderr, "Failed to load\n");
 		return NULL;
 	}
 	printf("Loaded %s\n", name);
 	return image;
}
//salveaza imaginea in format P2 sau P3
void save_asc(int **image, int *dim, char *save_name) {
 	FILE *fil = fopen(save_name, "w");
 	if (fil == NULL) {
 		perror("fopen");
 		exit -1;
 	}
 	if (dim[2] == 3 || dim[2] == 6) {
 		fprintf(fil, "P3\n%d %d\n%d\n", dim[1], dim[0], dim[3]);
 		for (int i = 0; i < dim[0]; i++) {
 			for (int j = 0; j < dim[1] * 3; j++)
 				fprintf(fil, "%d ", image[i][j]);
 			fprintf(fil,"\n");
 		}
 	} else {
 		fprintf(fil, "P2\n%d %d\n%d\n", dim[1], dim[0], dim[3]);
 		for (int i = 0; i < dim[0]; i++) {
 			for (int j = 0; j < dim[1]; j++)
 				fprintf(fil, "%d ", image[i][j]);
 			fprintf(fil,"\n");
 		}
 	}
 	fclose(fil);
}
//Salveaza imaginea in format P5 sau P6
void save_bin(int **image, int *dim, char *save_name) {
 	FILE *fil = fopen(save_name, "wb");
 	if (fil == NULL) {
 		perror("fopen");
 		exit -1;
 	}
 	if (dim[2] == 6 || dim[2] == 3) {
 		fprintf(fil, "P6\n%d %d\n%d\n", dim[1], dim[0], dim[3]);
 		for (int i = 0; i < dim[0]; i++) {
 			for (int j = 0; j < dim[1] * 3; j++) {
 				fwrite(&image[i][j], 1, 1, fil);
 			} 
 		}
 	} else {
 		fprintf(fil, "P5\n%d %d\n%d\n", dim[1], dim[0], dim[3]);
 		for (int i = 0; i < dim[0]; i++) {
 			for (int j = 0; j < dim[1]; j++) {
 				fwrite(&image[i][j], 1, 1, fil);
 			}
 		}
 	}
 	fclose(fil);
}
//verifica coordonatele sa fie valide, adica sa nu depaseasca marginile pozei
int verify_coords(int *val, int *dim) {
	if (val[0] == val[2] || val[1] == val[3]) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
 	if (val[0] > dim[1] || val[0] < 0) {
 		printf("Invalid set of coordinates\n");
 		return 0;
 	}
 	if (val[2] > dim[1] || val[2] < 0) {
 		printf("Invalid set of coordinates\n");
 		return 0;
 	}
 	if (val[1] > dim[0] || val[1] < 0) {
 		printf("Invalid set of coordinates\n");
 		return 0;
 	}
 	if (val[3] > dim[0] || val[3] < 0) {
 		printf("Invalid set of coordinates\n");
 		return 0;
 	}
	return 1;
}
//sunt extrase valorile selectului, cele de inceput si de oprire
// si sunt verificate sa fie valide, ulterior
//daca acestea sunt corecte, sunt salvate in vectorul val, iar
//ok1 devine 1, pentru a retine faptul ca exista o selectie valida
int slct(char *command, int *dim, int *dim2, int *val, int *ok1) {
 	char *token;
 	int j, coord, copy_val[4];
	if (ok1[0] == 1) {
		copy_val[0] = val[0];
		copy_val[1] = val[1];
		copy_val[2] = val[2];
		copy_val[3] = val[3];
	}
 	token = strtok(command, " ");
 	token = strtok(NULL, " ");
 	j = 0;
 	while (token != NULL) {
 		val[j] = atoi(token);
		if (val[j] == 0) {
			long tr = strlen(token);
			for (int i = 0; i < tr; i++) {
				if (token[i] > '9') {
					printf("Invalid command\n");
					if (ok1[0] == 1) {
						val[0] = copy_val[0];
						val[1] = copy_val[1];
						val[2] = copy_val[2];
						val[3] = copy_val[3];
						return 1;
					}
					return 0;
				}
			}
		}
 		token = strtok(NULL, " ");
 		j++;
 	}
	coord = verify_coords(val, dim);
 	if (coord == 0) {
		if (ok1[0] == 1) {
			val[0] = copy_val[0];
			val[1] = copy_val[1];
			val[2] = copy_val[2];
			val[3] = copy_val[3];
			return 1;
		}
 		return 0;
	} else {
		ok1[0] = 1;
	}
 	if (val[0] > val[2]) {
 		int aux = val[0];
 		val[0] = val[2];
 		val[2] = aux;
 	}
 	if (val[1] > val[3]) {
 		int aux = val[1];
 		val[1] = val[3];
 		val[3] = aux;
 	}
	printf("Selected %d %d %d %d\n", val[0], val[1], val[2], val[3]);
 	dim2[0] = val[3] - val[1];
 	dim2[1] = val[2] - val[0];
 	ok1[0] = 1;
 	return ok1[0];
}
//selecteaza zona de lucru la intreaga imagine,
//ok1 devine 0 caci e ca si cum niciun select nu a fost facut inaite
int slct_all(int*dim, int *dim2, int *ok1, int *val) {
 	val[0] = 0;
 	val[1] = 0;
 	val[2] = 0;
 	val[3] = 0;
 	dim2[0] = dim[0];
 	dim2[1] = dim[1];
 	printf("Selected ALL\n");
 	ok1[0] = 0;
 	return ok1[0];
}
//reduce imaginea la zona selectata anterior
int** crop(int **image, int *dim, int *dim2, int *val, int *ok1) {
 	int **work_image = NULL, k = 0, l, trash[2];
 	trash[0] = dim[0];
 	trash[1] = dim[1];
 	if (ok1[0] == 0)
 		return image;
 	if (dim[2] == 3 || dim[2] == 6) {
 		work_image = alloc_mat_col(dim, work_image);
		if (!work_image) {
			fprintf(stderr, "ERROR!");
			return NULL;
		}
 		for (int i = 0; i < dim[0]; i++)
 			for (int j = 0; j < dim[1] * 3; j++) 
 				work_image[i][j] = image[i][j];
 		for (int i = 0; i < dim[0]; i++)
 			free(image[i]);
 		dim[0] = dim2[0];
 		dim[1] = dim2[1];
 		for (int i = 0; i < dim[0]; i++)
 			image[i] = (int *)calloc(dim[1] * 3, sizeof(int));
 		k = 0;
 		for (int i = val[1]; i < val[3]; i++) {
 			l = 0;
 			for (int j = val[0] * 3; j < val[2] * 3; j++) {
 				image[k][l] = work_image[i][j];
 				l++;
 			}
 			k++;
 		}
 		free_mat(trash, work_image);
 	} else {
 		work_image = alloc_mat_grey(dim, work_image);
 		for (int i = 0; i < dim[0]; i++) {
 			for (int j = 0; j < dim[1]; j++) 
 				work_image[i][j] = image[i][j];
 		}
 		for (int i = 0; i < dim[0]; i++)
 			free(image[i]);
 		dim[0] = dim2[0];
 		dim[1] = dim2[1];
 		for (int i = 0; i < dim[0]; i++)
 			image[i] = (int *)calloc(dim[1], sizeof(int));
 		k = 0;
 		for (int i = val[1]; i < val[3]; i++) {
 			l = 0;
 			for (int j = val[0]; j < val[2]; j++) {
 				image[k][l] = work_image[i][j];
 				l++;
 			}
 			k++;
 		}
 		free_mat(trash, work_image);
 	}
 	ok1[0] = 0;
 	return image;
}
//afiseaza diagrama care corespunde frecventei pixelilor in imaginea alb-negru
void histogram(int **image, int *dim, int *val, char *command, int *ok1) {
 	char *token;
 	int x, y;
	char *string[50];
 	token = strtok(command, " \n");
 	token = strtok(NULL, " \n");
	if (token == NULL) {
		printf("Invalid command\n");
		exit -1;
	}
 	x = atoi(token);
 	token = strtok(NULL, " \n");
	if (token == NULL) {
		printf("Invalid command\n");
		exit -1;
	}
 	y = atoi(token);
	token = strtok(NULL, " \n");
	if (token != NULL) {
		printf("Invalid command\n");
		exit -1;
	}
	int *freq = alloc_freq(dim), how = 256 / y;
	if (freq == NULL) {
		fprintf(stderr, "Cannot alloc memory!-13-");
		exit -1;
	}
	int *val_freq = (int*)calloc(y, sizeof(int));
	if (val_freq == NULL) {
		fprintf(stderr, "Cannot alloc memory -12-");
		exit -1;
	}
	for (int i = 0; i < dim[0]; i++)
		for (int j = 0; j < dim[1]; j++) {
			freq[image[i][j]]++;
		}
	float when = 0, sum = 0;
	int l = 0;
	long max = 0;
	for (int i = 0; i < dim[3]; i++) {
		sum += freq[i];
		when++;
		if (when == how) {
			when = 0;
			if (max < sum) {
				max = sum;
			}
			sum = 0;
		}
	}
	when = 0;
	sum = 0;
	for (int i = 0; i < dim[3]; i++) {
		sum += freq[i];
		when++;
		if (when == how) {
			when = 0;
			printf("\n%f * %d / %ld", sum, x, max);
			val_freq[l] = (float)(sum * x) / (max);
			l++;
			sum = 0;
		}
	}
	for (int i = 0; i < y; i++) {
		printf("%d\t|\t", val_freq[i]);
		for (int j = 0; j < val_freq[i]; j++) {
			printf("*");
		}
		printf("\n");
	}
	free(freq);
	free(val_freq);
}
//egalizeaza imaginea
int** equalize(int **image, int *dim, int *ok1, int *val) {
	int *freq = alloc_freq(dim);
	double newpix, sum;
	if (freq == NULL) {
		fprintf(stderr, "Cannot alloc memory!-13-");
		return NULL;
	}
	for (int i = 0; i < dim[0]; i++)
		for (int j = 0; j < dim[1]; j++)
			freq[image[i][j]]++;
	for (int i = 0; i < dim[0]; i++) {
		for (int j = 0; j < dim[1]; j++) {
			sum = 0;
			for (int k = 0; k <= image[i][j]; k++)
				sum += freq[k];
			newpix = (255 * sum) / (dim[0] * dim[1]);
			newpix = round(newpix);
			image[i][j] = newpix;
		}
	}
	free(freq);
	return image;
}

int** edge(int **image, int *dim, int *val, int *ok1) {
	double **effect = NULL;
	effect = alloc_mat_effect(dim, effect);
	for (int i = 0; i < dim[0]; i++)
		for (int j = 0; j < dim[1]; j++)
			effect[i][j] = 0;
	double sumg, sumr, sumb;
	int shar[3][3] = { {-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
	for (int i = 1; i < dim[0] - 1; i++) {
		for (int j = 3; j < dim[1] * 3 - 3; j++) {
			sumg = 0;
			sumb = 0;
			sumr = 0;
			int m = 0;
			int n = 0;
			if (j % 3 == 0) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0 ;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumr += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				if (sumr < 0)
					sumr = 0;
				if (sumr > 255)
					sumr = 255;
				effect[i][j] = sumr;
			}
			if (j % 3 == 1) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumg += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				if (sumg < 0)
					sumg = 0;
				if (sumg > 255)
					sumg = 255;
				effect[i][j] = sumg;
			}
			if (j % 3 == 2) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumb += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				if (sumb < 0)
					sumb = 0;
				if (sumb > 255)
					sumb = 255;
				effect[i][j] = sumb;
			}
		}
	}
	int x1 = val[0], y1 = val[1], x2 = val[2], y2 = val[3];
	if (ok1[0] == 1) {
		if (x1 == 0)
			x1 += 1;
		if (y1 == 0)
			y1 += 1;
		if (x2 == dim[1])
			x2 -= 1;
		if (y2 == dim[0])
			y2 -= 1;
		for (int i = y1; i < y2; i++)
			for (int j = x1 * 3; j < x2 * 3; j++)
				image[i][j] = effect[i][j];
		free_mat_effect(dim, effect);
		return image;
	}
	for (int i = 1; i < dim[0] - 1; i++)
		for (int j = 3; j < dim[1] * 3 - 3; j++)
			image[i][j] = effect[i][j];
	free_mat_effect(dim, effect);
	return image;
}

int** sharpen(int **image, int *dim, int *val, int *ok1) {
	double **effect = NULL;
	effect = alloc_mat_effect(dim, effect);
	double sumg, sumr, sumb;
	int shar[3][3] = { {0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
	for (int i = 1; i < dim[0] - 1; i++) {
		for (int j = 3; j < dim[1] * 3 - 3; j++) {
			sumg = 0;
			sumb = 0;
			sumr = 0;
			int m = 0;
			int n = 0;
			if (j % 3 == 0) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0 ;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumr += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				if (sumr < 0)
					sumr = 0;
				if (sumr > 255)
					sumr = 255;
				effect[i][j] = sumr;
			}
			if (j % 3 == 1) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumg += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				if (sumg < 0)
					sumg = 0;
				if (sumg > 255)
					sumg = 255;
				effect[i][j] = sumg;
			}
			if (j % 3 == 2) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumb += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				if (sumb < 0)
					sumb = 0;
				if (sumb > 255)
					sumb = 255;
				effect[i][j] = sumb;
			}
		}
	}
	int x1 = val[0], y1 = val[1], x2 = val[2], y2 = val[3];
	if (ok1[0] == 1) {
		if (x1 == 0)
			x1 += 1;
		if (y1 == 0)
			y1 += 1;
		if (x2 == dim[1])
			x2 -= 1;
		if (y2 == dim[0])
			y2 -= 1;
		for (int i = y1; i < y2; i++)
			for (int j = x1 * 3; j < x2 * 3; j++)
				image[i][j] = effect[i][j];
		free_mat_effect(dim, effect);
		return image;
	}
	for (int i = 1; i < dim[0] - 1; i++)
		for (int j = 3; j < dim[1] * 3 - 3; j++)
			image[i][j] = effect[i][j];
	free_mat_effect(dim, effect);
	return image;
}

int** blur(int **image, int *dim, int *val, int *ok1) {
	double **effect = NULL;
	effect = alloc_mat_effect(dim, effect);
	double sumg, sumr, sumb;
	int shar[3][3] = { {1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
	for (int i = 1; i < dim[0] - 1; i++) {
		for (int j = 3; j < dim[1] * 3 - 3; j++) {
			sumg = 0;
			sumb = 0;
			sumr = 0;
			int m = 0;
			int n = 0;
			if (j % 3 == 0) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0 ;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumr += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				sumr = sumr / 9;
				sumr = round(sumr);
				effect[i][j] = sumr;
			}
			if (j % 3 == 1) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumg += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				sumg /= 9;
				sumg = round(sumg);
				effect[i][j] = sumg;
			}
			if (j % 3 == 2) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumb += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				sumb /= 9;
				sumb = round(sumb);
				effect[i][j] = sumb;
			}
		}
	}
	int x1 = val[0], y1 = val[1], x2 = val[2], y2 = val[3];
	if (ok1[0] == 1) {
		if (x1 == 0)
			x1 += 1;
		if (y1 == 0)
			y1 += 1;
		if (x2 == dim[1])
			x2 -= 1;
		if (y2 == dim[0])
			y2 -= 1;
		for (int i = y1; i < y2; i++)
			for (int j = x1 * 3; j < x2 * 3; j++)
				image[i][j] = effect[i][j];
		free_mat_effect(dim, effect);
		return image;
	}
	for (int i = 1; i < dim[0] - 1; i++)
		for (int j = 3; j < dim[1] * 3 - 3; j++)
			image[i][j] = effect[i][j];
	free_mat_effect(dim, effect);
	return image;
}

int** gaussian(int **image, int *dim, int *val, int *ok1) {
	double **effect = NULL;
	effect = alloc_mat_effect(dim, effect);
	double sumg, sumr, sumb;
	int shar[3][3] = { {1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
	for (int i = 1; i < dim[0] - 1; i++) {
		for (int j = 3; j < dim[1] * 3 - 3; j++) {
			sumg = 0;
			sumb = 0;
			sumr = 0;
			int m = 0;
			int n = 0;
			if (j % 3 == 0) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0 ;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumr += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				sumr /= 16;
				sumr = round(sumr);
				effect[i][j] = sumr;
			}
			if (j % 3 == 1) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumg += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				sumg /= 16;
				sumg = round(sumg);
				effect[i][j] = sumg;
			}
			if (j % 3 == 2) {
				for (int k = i - 1; k <= i + 1; k++) {
					n = 0;
					for (int l = j - 3; l <= j + 3; l += 3) {
						sumb += shar[m][n] * (double)image[k][l];
						n++;
					}
					m++;
				}
				sumb /= 16;
				sumb = round(sumb);
				effect[i][j] = sumb;
			}
		}
	}
	int x1 = val[0], y1 = val[1], x2 = val[2], y2 = val[3];
	if (ok1[0] == 1) {
		if (x1 == 0)
			x1 += 1;
		if (y1 == 0)
			y1 += 1;
		if (x2 == dim[1])
			x2 -= 1;
		if (y2 == dim[0])
			y2 -= 1;
		for (int i = y1; i < y2; i++)
			for (int j = x1 * 3; j < x2 * 3; j++)
				image[i][j] = effect[i][j];
		free_mat_effect(dim, effect);
		return image;
	}
	for (int i = 1; i < dim[0] - 1; i++)
		for (int j = 3; j < dim[1] * 3 - 3; j++)
			image[i][j] = effect[i][j];
	free_mat_effect(dim, effect);
	return image;
}

void save_img(int **image, int *dim, char *command) {
	char save_name[50], *token;
	token = strtok(command, " ");
	token = strtok(NULL, " \n");
	if (token == NULL) {
		printf("Invalid command\n");
		exit -1;
	}
	strcpy(save_name, token);
	token = strtok(NULL, " ");
	if (token != NULL) {
		save_asc(image, dim, save_name);
		printf("Saved %s\n", save_name);
	} else {
		save_bin(image, dim, save_name);
		printf("Saved %s\n", save_name);
	}
}

int verify_token(char *token) {
	char com[14] = "GAUSSIAN_BLUR\0", com1[5] = "EDGE\0";
	char com2[8] = "SHARPEN\0", com3[5] = "BLUR\0";
	char trash[15];
	int oko = 1;
	for (int i = 0; i < 4; i++)
		if (token[i] != com1[i])
			oko = 0;
	if (oko == 1)
		return 1;
	oko = 1;
	for (int i = 0; i < 4;i ++)
		if (token[i] != com3[i])
			oko = 0;
	if (oko == 1)
		return 1;
	oko = 1;
	for (int i = 0; i < 7; i++)
		if (token[i] != com2[i])
			oko = 0;
	if (oko = 1)
		return 1;
	for (int i = 0; i < 13; i++)
		if (token[i] != com[i])
			oko = 0;
	if (oko == 1)
		return 1;
	return 0;
}

int** rotate(int **image, int *dim, int *ok1, int *val) {
	if (ok1[0] == 0) {
		if (dim[2] == 3 || dim[2] == 6) {
			int **work_image = (int **)calloc(dim[1], sizeof(int *));
			if (!work_image) {
				fprintf(stderr, "Error: cannot alloc memory!-20-");
				return NULL;
			}
			for (int i = 0; i < dim[1]; i++) {
				work_image[i] = (int *)calloc(3 * dim[0], sizeof(int));
				if (!work_image[i]) {
					fprintf(stderr, "Error: cannot alloc memory!-21-");
					for (int j = 1; j >= 0;j --)
						free(work_image[j]);
					free(work_image);
					return NULL;
				}
			}
			for (int i = 0; i < dim[0]; i++)
				for (int j = 0; j < dim[1] * 3; j += 3) {
					work_image[j / 3][i] = image[i][j];
					work_image[j / 3][i + 1] = image[i][j + 1];
					work_image[j / 3][i + 2] = image[i][j + 2];
				}
			for (int i = 0; i < dim[0]; i++)
				free(image[i]);
			image = (int **)realloc(image, dim[1] * sizeof(int *));
			for (int i = 0; i < dim[1]; i++) {
				image[i] = (int *)calloc(dim[0] * 3, sizeof(int));
				if (!image[i]) {
					fprintf(stderr, "Error: cannot alloc memory!-22-");
					for (int j = i; j >= 0; j--)
						free(image[i]);
				}
			}
			int l = 0;
			for (int i = dim[1] - 1; i >= 0; i--) {
				for  (int j = 0; j < dim[0] * 3; j++)
					image[l][j] = work_image[i][j];
				l++;
			}
			for (int i = 0; i < dim[1]; i++)
				free(work_image[i]);
			free(work_image);
			int aux = dim[0];
			dim[0] = dim[1];
			dim[1] = aux;
		} else {
			int **work_image = (int **)calloc(dim[1], sizeof(int *));
			if (!work_image) {
				fprintf(stderr, "Error: cannot alloc memory!-20-");
				return NULL;
			}
			for (int i = 0; i < dim[1]; i++) {
				work_image[i] = (int *)calloc(dim[0], sizeof(int));
				if (!work_image[i]) {
					fprintf(stderr, "Error: cannot alloc memory!-21-");
					for (int j = 1; j >= 0;j --)
						free(work_image[j]);
					free(work_image);
					return NULL;
				}
			}
			for (int i = 0; i < dim[0]; i++)
				for (int j = 0; j < dim[1]; j++)
					work_image[j][i] = image[i][j];
			for (int i = 0; i < dim[0]; i++)
				free(image[i]);
			image = (int **)realloc(image, dim[1] * sizeof(int *));
			for (int i = 0; i < dim[1]; i++) {
				image[i] = (int *)calloc(dim[0], sizeof(int));
				if (!image[i]) {
					fprintf(stderr, "Error: cannot alloc memory!-22-");
					for (int j = i; j >= 0; j--)
						free(image[i]);
				}
			}
			int l = 0;
			for (int i = dim[1] - 1; i >= 0; i--) {
				for  (int j = 0; j < dim[0]; j++)
					image[l][j] = work_image[i][j];
				l++;
			}
			for (int i = 0; i < dim[1]; i++)
				free(work_image[i]);
			free(work_image);
			int aux = dim[0];
			dim[0] = dim[1];
			dim[1] = aux;
		}
	} else {
		if (val[2] - val[0] != val[3] - val[1]) {
			printf("The selection must be square\n");
			return 0;
		}
			if (dim[2] == 3 || dim[2] == 6) {
				int **work_image = (int **)calloc(dim[1], sizeof(int *));
				if (!work_image) {
					fprintf(stderr, "Error: cannot alloc memory!-20-");
					return NULL;
				}
				for (int i = 0; i < dim[1]; i++) {
					work_image[i] = (int *)calloc(3 * (val[3] - val[1] + 1), sizeof(int));
					if (!work_image[i]) {
						fprintf(stderr, "Error: cannot alloc memory!-21-");
						for (int j = 1; j >= 0;j --)
							free(work_image[j]);
						free(work_image);
						return NULL;
					}
				}
				for (int i = val[1]; i < val[3]; i++)
					for (int j = 3 * val[0]; j < val[2] * 3; j += 3) {
						work_image[j / 3][i] = image[i][j];
						work_image[j / 3][i + 1] = image[i][j + 1];
						work_image[j / 3][i + 2] = image[i][j + 2];
					}
				int l = val[1];
				for (int i = val[3] - 1; i >= val[1]; i--) {
					for  (int j = val[0] * 3; j < val[2] * 3; j++)
						image[l][j] = work_image[i][j];
					l++;
				}
				for (int i = 0; i < dim[1]; i++)
					free(work_image[i]);
				free(work_image);
		} else {
				int **work_image = (int **)calloc(dim[1], sizeof(int *));
				if (!work_image) {
					fprintf(stderr, "Error: cannot alloc memory!-20-");
					return NULL;
				}
				for (int i = 0; i < dim[1]; i++) {
					work_image[i] = (int *)calloc(val[3] - val[1] + 1, sizeof(int));
					if (!work_image[i]) {
						fprintf(stderr, "Error: cannot alloc memory!-21-");
						for (int j = 1; j >= 0;j --)
							free(work_image[j]);
						free(work_image);
						return NULL;
					}
				}
				for (int i = val[1]; i < val[3]; i++)
					for (int j = val[0]; j < val[2]; j++)
						work_image[j][i] = image[i][j];
				int l = val[1];
				for (int i = val[3] - 1; i >= val[1]; i--) {
					for  (int j = val[0]; j < val[2]; j++)
						image[l][j] = work_image[i][j];
					l++;
				}
				for (int i = 0; i < val[2] - val[0]; i++)
					free(work_image[i]);
				free(work_image);
		}
	}
	return image;
}

int** Rotate(char *command, int *ok1, int *val, int *dim, int **image) {
	char *token = strtok(command, " ");
	int rot;
	token = strtok(NULL, " \n");
	if (!token) {
		printf("Invalid command\n");
		return image;
	}
	rot = atoi(token);
	if (rot % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return image;
	}
	if (rot == 0) {
		printf("Rotated 0\n");
		return image;
	}
	int coprot = rot;
	if (rot > 0) {
		if (rot > 360)
			rot = rot % 90;
		if (rot == 90)
			for (int i = 0; i < 3; i++)
				image = rotate(image, dim, ok1, val);
		if (rot == 180)
			for (int i = 0; i < 2; i++)
				image = rotate(image, dim, ok1, val);
		if (rot == 270)
			image = rotate(image, dim, ok1, val);
	}
	if (rot < 0) {
		if (rot < -360)
			rot = rot % 90;
		if (rot == -90)
			image = rotate(image, dim, ok1, val);
		if (rot == -180)
			for (int i = 0; i < 2; i++)
				image = rotate(image, dim, ok1, val);
		if (rot == -270)
			for (int i = 0; i < 3; i++)
				image = rotate(image, dim, ok1, val);
	}
	printf("Rotated %d\n", coprot);
	return image;
}

int main() {
	//dim[0] == rows
	//dim[1] == colls
	//dim[2] == magic numbwe
	//dim[3] == maxvalue
	char command[50], name[50], first_letter, *token, *tok;
	int **image, dim[4], **work_image, dim2[2], j = 0, ok = 0, ok1[1], val[4];
	int coord, x, y;
	ok1[0] = 0;
	do {
		fgets(command, sizeof(command), stdin);
		first_letter = command[0];
		switch (first_letter) {
			case 'L':
				if (ok == 0) {
					image = load_image(name, image, dim, command);
					if (!image) {
						if (ok == 1) {
							ok = 0;
							break;
						}
						break;
					}
					ok = 1;
					ok1[0] = 0;
				} else {
					free_mat(dim, image);
					image = load_image(name, image, dim, command);
					if (!image) {
						ok = 0;
						break;
					}
				}
				if (!image)
					ok = 0;
			break;
			case 'S':
				if (ok == 0) {
					printf("No image loaded\n");
					break;
				}
				if (command[1] == 'A') {
					if (command[2] != 'V') {
						printf("Invalid command\n");
						break;
					}
					save_img(image, dim, command);
					break;
				}
				if (strlen(command) > 12) {
					if (command[2] != 'L') {
						printf("Invalid command\n");
						break;
					}
					ok1[0] = slct(command, dim, dim2, val, ok1);
				} else {
					if (command[2] != 'L') {
						printf("Invalid command\n");
						break;
					}
					token = strtok(command, " \n");
					int contor = 0;
					while (token) {
						contor++;
						token = strtok(NULL, " \n");
					}
					if (contor > 2) {
						printf("Invalid command\n");
						break;
					}
					ok1[0] = slct_all(dim, dim2, ok1, val);
				}
			break;
			case 'C':
				if (ok == 0) {
					printf("No image loaded\n");
					break;
				}
				image = crop(image, dim, dim2, val, ok1);
				if (image != NULL)
					printf("Image cropped\n");
			break;
			case 'H':
				if (ok == 0) {
					printf("No image loaded\n");
					break;
				}
				if (dim[2] == 3 || dim[2] == 6) {
					printf("Black and white image needed\n");
					break;
				}
				histogram(image, dim, val, command, ok1);
			break;
			case 'A':
				if (ok == 0) {
					printf("No image loaded\n");
					break;
				}
				token = strtok(command, " ");
				if (strcmp(token, "APPLY") != 0) {
					printf("Invalid command\n");
					break;
				}
				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Invalid command\n");
					break;
				}
				int oko = verify_token(token);
				if (oko == 0) {
					printf("APPLY parameter invalid\n");
					break;
				}
				if (dim[2] == 2 || dim[2] == 5) {
					printf("Easy, Charlie Chaplin\n");
					break;
				}
				if (token[0] == 'E') {
					image = edge(image, dim, val, ok1);
					printf("APPLY EDGE done\n");
				break;
				}
				if (token[0] == 'S') {
					image = sharpen(image, dim, val, ok1);
					printf("APPLY SHARPEN done\n");
				break;
				}
				if (token[0] == 'B') {
					image = blur(image, dim, val, ok1);
					printf("APPLY BLUR done\n");
				break;
				}
				if (token[0] == 'G') {
					image = gaussian(image, dim, val, ok1);
					printf("APPLY GAUSSIAN_BLUR done\n");
				break;
				}
				printf("APPLY parameter invalid\n");
			break;
			case 'R':
				if (ok == 0) {
					printf("No image loaded\n");
					break;
				}
				image = Rotate(command, ok1, val, dim, image);
			break;
			case 'E':
				if (command[1] == 'Q') {
					if (ok == 0) {
						printf("No image loaded\n");
						break;
					}
					if (dim[2] == 6 || dim[2] == 3) {
						printf("Black and white image needed\n");
						break;
					}
					image = equalize(image, dim, ok1, val);
					if (image != NULL)
						printf("Equalize done\n");
					break;
				}
				if (command[1] == 'X') {
					if (ok == 0)
						printf("No image loaded\n");
					else
						free_mat(dim, image);
				}
				break;
			default:
				printf("Invalid command\n");
			break;
		}
	} while (command[1] != 'X');
}
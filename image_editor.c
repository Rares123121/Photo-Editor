/// Copyright Gheorghe Rares-Mihail 311CA 2023-2024
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	int rosu, albastru, verde;
} img_color;

typedef struct {
	int **matrice;
	int n, m;
	char cuv[4];
	int incarcat;
	int maxim;
	img_color **mat_col;
	int tip_color;
	int x1, x2, y1, y2;
} img;

void comentarii(FILE *in)
{
	char linie[256], c;
	fscanf(in, "%c", &c);
	while (c == '#' || c == ' ' || c == '\n' || c == '\t') {
		if (c == '#') {
			fgets(linie, 256, in);
		} else {
			if (c == EOF)
				return;
		}
		fscanf(in, "%c", &c);
	}
	fseek(in, -1, SEEK_CUR);
}

int **alocare(int n, int m)
{
	int **maux;
	maux = (int **)malloc(n * sizeof(int *));
	for (int i = 0; i < n; i++)
		maux[i] = (int *)malloc(m * sizeof(int));

	return maux;
}

img_color **alocare2(int n, int m)
{
	img_color **mat;
	mat = (img_color **)malloc(n * sizeof(img_color *));
	for (int i = 0; i < n; i++)
		mat[i] = (img_color *)malloc(m * sizeof(img_color));

	return mat;
}

void eliberare(int n, int **matrice)
{
	for (int i = 0; i < n; i++)
		free(matrice[i]);
	free(matrice);
}

void eliberare2(int n, img_color **mat)
{
	for (int i = 0; i < n; i++)
		free(mat[i]);
	free(mat);
}

void reatribuire(img *mat)
{
	if (mat->incarcat == 1) {
		if (mat->tip_color == 0)
			eliberare(mat->n, mat->matrice);
		else if (mat->tip_color == 1)
			eliberare2(mat->n, mat->mat_col);

		mat->n = 0;
		mat->m = 0;
		mat->incarcat = 0;
		mat->maxim = 0;
		mat->x1 = 0;
		mat->x2 = 0;
		mat->y2 = 0;
		mat->y1 = 0;
	}
}

void gri(img *mat, int m1, int n1, int max, FILE *in)
{
	mat->x1 = 0;
	mat->x2 = m1;
	mat->y1 = 0;
	mat->y2 = n1;
	mat->n = n1;
	mat->m = m1;
	mat->maxim = max;
	mat->incarcat = 1;
	mat->tip_color = 0;
	mat->matrice = alocare(mat->n, mat->m);
	if (mat->cuv[1] == '2') {
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++)
				fscanf(in, "%d", &mat->matrice[i][j]);
	} else if (mat->cuv[1] == '5') {
		unsigned char nr;
		for (int i = 0; i < n1; i++)
			for (int j = 0; j < m1; j++) {
				fread(&nr, sizeof(unsigned char), 1, in);
				mat->matrice[i][j] = (int)nr;
			}
	}
}

void color(img *mat, int m1, int n1, int max, FILE *in)
{
	int c1, c2, c3;
	mat->x1 = 0;
	mat->x2 = m1;
	mat->y1 = 0;
	mat->y2 = n1;
	mat->n = n1;
	mat->m = m1;
	mat->maxim = max;
	mat->incarcat = 1;
	mat->tip_color = 1;
	mat->mat_col = alocare2(mat->n, mat->m);
	if (mat->cuv[1] == '3') {
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++) {
				fscanf(in, "%d", &c1);
				fscanf(in, "%d", &c2);
				fscanf(in, "%d", &c3);
				mat->mat_col[i][j].rosu = c1;
				mat->mat_col[i][j].albastru = c3;
				mat->mat_col[i][j].verde = c2;
			}
	} else if (mat->cuv[1] == '6') {
		unsigned char rosu, verde, albastru;
		for (int i = 0; i < n1; i++)
			for (int j = 0; j < m1; j++) {
				fread(&rosu, sizeof(unsigned char), 1, in);
				fread(&verde, sizeof(unsigned char), 1, in);
				fread(&albastru, sizeof(unsigned char), 1, in);
				mat->mat_col[i][j].rosu = (int)rosu;
				mat->mat_col[i][j].albastru = (int)albastru;
				mat->mat_col[i][j].verde = (int)verde;
			}
	}
}

void load(img *mat, char sir[])
{
	reatribuire(mat);

	FILE *in = fopen(sir, "rt");
	if (!in) {
		printf("Failed to load %s\n", sir);
		reatribuire(mat);
		return;
	}

	int n1, m1, maxim1;
	char cuv2[4];
	fscanf(in, "%s", cuv2);
	comentarii(in);
	fscanf(in, "%d", &m1);
	fscanf(in, "%d", &n1);
	comentarii(in);
	fscanf(in, "%d", &maxim1);
	comentarii(in);
	if (cuv2[1] == '2') {
		strcpy(mat->cuv, cuv2);
		gri(mat, m1, n1, maxim1, in);
	} else if (cuv2[1] == '3') {
		strcpy(mat->cuv, cuv2);
		color(mat, m1, n1, maxim1, in);
	} else {
		int poz = ftell(in);
		fclose(in);
		in = fopen(sir, "rb");
		fseek(in, poz, SEEK_SET);
		if (cuv2[1] == '5') {
			strcpy(mat->cuv, cuv2);
			gri(mat, m1, n1, maxim1, in);
		} else if (cuv2[1] == '6') {
			strcpy(mat->cuv, cuv2);
			color(mat, m1, n1, maxim1, in);
		}
	}

	printf("Loaded %s\n", sir);
	fclose(in);
}

int nr_cuv(char sir[])
{
	char *p;
	char c[256];
	strcpy(c, sir);
	int nr = 0;
	p = strtok(c, "\n ");
	while (p) {
		nr++;
		p = strtok(NULL, "\n ");
	}
	return nr;
}

void select_simplu(img *mat, int x3, int x4, int y3, int y4)
{
	if (y3 >= 0 && y3 <= mat->n && y4 >= 0 && y4 <= mat->n &&
		x3 >= 0 && x3 <= mat->m && x4 >= 0 && x4 <= mat->m) {
		if (y3 != y4) {
			mat->y1 = y3;
			mat->y2 = y4;
		} else if (y3 == y4) {
			printf("Invalid set of coordinates\n");
			return;
		}

		if (x3 != x4) {
			mat->x1 = x3;
			mat->x2 = x4;
		} else if (x3 == x4) {
			printf("Invalid set of coordinates\n");
			return;
		}
	} else {
		printf("Invalid set of coordinates\n");
		return;
	}

	printf("Selected %d %d %d %d\n", x3, y3, x4, y4);
}

void select_tot(img *mat)
{
	mat->x1 = 0;
	mat->y1 = 0;
	mat->x2 = mat->m;
	mat->y2 = mat->n;
	printf("Selected ALL\n");
}

void selectare(img *mat, char sir[])
{
	if (mat->incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	int x3, x4, y3, y4;
	char *p, c[256], s[256];
	strcpy(s, sir);
	int nr = nr_cuv(s);
	p = strtok(sir, "\n ");
	p = strtok(NULL, "\n ");
	strcpy(c, p);
	int tip;
	if (strcmp(c, "ALL") == 0) {
		select_tot(mat);
		return;
	}
	if (nr == 5) {
		if ((c[0] >= '0' && c[0] <= '9') || (atoi(c) < 0)) {
			x3 = atoi(c);
		} else {
			printf("Invalid command\n");
			return;
		}
		p = strtok(NULL, "\n ");
		if ((p[0] >= '0' && p[0] <= '9') || (atoi(p) < 0)) {
			y3 = atoi(p);
		} else {
			printf("Invalid command\n");
			return;
		}
		p = strtok(NULL, "\n ");
		if ((p[0] >= '0' && p[0] <= '9') || (atoi(p) < 0)) {
			x4 = atoi(p);
		} else {
			printf("Invalid command\n");
			return;
		}
		p = strtok(NULL, "\n ");
		if ((p[0] >= '0' && p[0] <= '9') || (atoi(p) < 0)) {
			y4 = atoi(p);
		} else {
			printf("Invalid command\n");
			return;
		}
		tip = 0;
	} else {
		printf("Invalid command\n");
		return;
	}

	if (x3 > x4) {
		int aux = x3;
		x3 = x4;
		x4 = aux;
	}

	if (y3 > y4) {
		int aux = y3;
		y3 = y4;
		y4 = aux;
	}

	if (tip == 0) {//selectare simpla
		select_simplu(mat, x3, x4, y3, y4);
		return;
	}
}

int clamp(double x, int min, int max)
{
	if ((int)x < min)
		return min;
	if ((int)x > max)
		return max;
	return (int)x;
}

int putere(int nr)
{
	int c = nr;
	while (c % 2 == 0)
		c /= 2;
	return c;
}

void histo(img *mat, char sir[])
{
	if (mat->incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	int x, y;
	char *p, c[256];
	strcpy(c, sir);
	int nr = nr_cuv(c);
	if (nr == 1) {
		printf("Invalid command\n");
		return;
	}

	if (nr != 3) {
		printf("Invalid command\n");
		return;
	}

	p = strtok(sir, "\n ");
	p = strtok(NULL, "\n ");
	x = atoi(p);
	p = strtok(NULL, "\n ");
	y = atoi(p);

	if (mat->tip_color == 1) {
		printf("Black and white image needed\n");
		return;
	}

	if (y < 2 || y > 256 || putere(y) != 1) {
		printf("Invalid set of parameters\n");
		return;
	}

	int poz = 256 / y;
	int *f, maxim = -1, *fr;
	f = (int *)malloc(256 * sizeof(int));
	for (int i = 0; i < 256; i++)
		f[i] = 0;

	fr = (int *)malloc(256 * sizeof(int));
	for (int i = 0; i < 256; i++)
		fr[i] = 0;

	for (int i = 0; i < mat->n; i++)
		for (int j = 0; j < mat->m; j++)
			f[mat->matrice[i][j]]++;

	int k = 0;
	for (int i = 0; i < 256; i = i + poz) {
		for (int j = i; j < i + poz; j++)
			fr[k] += f[j];
		k++;
	}

	for (int i = 0; i < y; i++)
		f[i] = fr[i];

	for (int i = 0; i < y; i++)
		if (f[i] > maxim)
			maxim = f[i];

	for (int i = 0; i < y; i++, printf("\n")) {
		int stea;
		double stea2 = 1.0 * f[i] / maxim * x;
		stea = (int)stea2;
		printf("%d\t|\t", stea);
		for (int j = 0; j < stea; j++)
			printf("*");
	}
	free(f);
	free(fr);
}

void equalize(img *mat)
{
	if (mat->incarcat == 0) {
		printf("No image loaded\n");
		return;
	}
	if (mat->tip_color == 1) {
		printf("Black and white image needed\n");
		return;
	}

	int *vf;
	vf = (int *)malloc((mat->maxim + 1) * sizeof(int));
	for (int i = 0; i <= mat->maxim; i++)
		vf[i] = 0;

	for (int i = 0; i < mat->n; i++)
		for (int j = 0; j < mat->m; j++)
			vf[mat->matrice[i][j]]++;

	int suprafata = mat->n * mat->m;
	int s;
	double f_a;

	for (int i = 0; i < mat->n; i++)
		for (int j = 0; j < mat->m; j++) {
			s = 0;
			for (int k = 0; k <= mat->matrice[i][j]; k++)
				s += vf[k];
			f_a = 255.0 * 1 / suprafata * s;
			mat->matrice[i][j] = clamp(round(f_a), 0, 255);
		}

	free(vf);
	printf("Equalize done\n");
}

void crop_gri(img *mat)
{
	int cn = mat->n;
	int **copie, *vector, k = 0;
	long long val = (mat->y2 - mat->y1) * (mat->x2 - mat->x1);

	copie = (int **)malloc((mat->y2 - mat->y1) * sizeof(int *));
	for (int i = 0; i < mat->y2 - mat->y1; i++)
		copie[i] = (int *)malloc((mat->x2 - mat->x1) * sizeof(int));

	vector = (int *)malloc(val * sizeof(int));

	for (int i = mat->y1; i < mat->y2; i++)
		for (int j = mat->x1; j < mat->x2; j++)
			vector[k++] = mat->matrice[i][j];

	k = 0;
	for (int i = 0; i < mat->y2 - mat->y1; i++)
		for (int j = 0; j < mat->x2 - mat->x1; j++)
			copie[i][j] = vector[k++];

	eliberare(cn, mat->matrice);

	mat->matrice = alocare(mat->y2 - mat->y1, mat->x2 - mat->x1);

	for (int i = 0; i < mat->y2 - mat->y1; i++)
		for (int j = 0; j < mat->x2 - mat->x1; j++)
			mat->matrice[i][j] = copie[i][j];

	int xaux, yaux;
	xaux = mat->x2 - mat->x1;
	yaux = mat->y2 - mat->y1;
	mat->x1 = 0;
	mat->y1 = 0;
	mat->x2 = xaux;
	mat->y2 = yaux;
	mat->n = yaux;
	mat->m = xaux;

	eliberare(mat->y2 - mat->y1, copie);
	free(vector);
	printf("Image cropped\n");
}

void crop_color(img *mat)
{
	int cn = mat->n;
	long long val = (mat->y2 - mat->y1) * (mat->x2 - mat->x1);
	img_color **copie, *vector;
	copie = alocare2(mat->y2 - mat->y1, mat->x2 - mat->x1);
	vector = (img_color *)malloc(val * sizeof(img_color));
	int k = 0;
	for (int i = mat->y1; i < mat->y2; i++)
		for (int j = mat->x1; j < mat->x2; j++) {
			vector[k].rosu = mat->mat_col[i][j].rosu;
			vector[k].albastru = mat->mat_col[i][j].albastru;
			vector[k].verde = mat->mat_col[i][j].verde;
			k++;
		}
	k = 0;
	for (int i = 0; i < mat->y2 - mat->y1; i++)
		for (int j = 0; j < mat->x2 - mat->x1; j++) {
			copie[i][j].rosu = vector[k].rosu;
			copie[i][j].verde = vector[k].verde;
			copie[i][j].albastru = vector[k].albastru;
			k++;
		}

	eliberare2(cn, mat->mat_col);

	mat->mat_col = alocare2(mat->y2 - mat->y1,  mat->x2 - mat->x1);

	for (int i = 0; i < mat->y2 - mat->y1; i++)
		for (int j = 0; j < mat->x2 - mat->x1; j++) {
			mat->mat_col[i][j].rosu = copie[i][j].rosu;
			mat->mat_col[i][j].albastru = copie[i][j].albastru;
			mat->mat_col[i][j].verde = copie[i][j].verde;
		}

	int xaux, yaux;
	xaux = mat->x2 - mat->x1;
	yaux = mat->y2 - mat->y1;
	mat->x1 = 0;
	mat->y1 = 0;
	mat->x2 = xaux;
	mat->y2 = yaux;
	mat->n = yaux;
	mat->m = xaux;
	free(vector);
	eliberare2(mat->y2 - mat->y1, copie);
	printf("Image cropped\n");
}

void fcrop(img *mat)
{
	if (mat->incarcat == 0) {
		printf("No image loaded\n");
		return;
	}
	if (mat->tip_color == 0)
		crop_gri(mat);
	else
		crop_color(mat);
}

void rot_90(img *mat)
{
	if (mat->x1 == 0 && mat->y1 == 0 &&
		mat->x2 == mat->m && mat->y2 == mat->n) {
		int **copie;
		copie = alocare(mat->m, mat->n);
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++)
				copie[j][i] = mat->matrice[mat->n - i - 1][j];

		eliberare(mat->n, mat->matrice);

		int aux;
		aux = mat->x2;
		mat->x2 = mat->y2;
		mat->y2 = aux;
		aux = mat->n;
		mat->n = mat->m;
		mat->m = aux;

		mat->matrice = alocare(mat->n, mat->m);
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++)
				mat->matrice[i][j] = copie[i][j];

		eliberare(mat->n, copie);
	} else {
		int **copie, *v;
		copie = alocare(mat->y2 - mat->y1, mat->y2 - mat->y1);
		int val = (mat->y2 - mat->y1) * (mat->y2 - mat->y1);
		v = (int *)malloc(val * sizeof(int));
		int k = 0;
		for (int i = mat->y1; i < mat->y2; i++)
			for (int j = mat->x1; j < mat->x2; j++)
				v[k++] = mat->matrice[i][j];

		k = 0;
		for (int i = 0; i < mat->y2 - mat->y1; i++)
			for (int j = 0; j < mat->y2 - mat->y1; j++)
				copie[i][j] = v[k++];

		k = 0;
		for (int i = 0; i < mat->y2 - mat->y1; i++)
			for (int j = 0; j < mat->y2 - mat->y1; j++)
				v[k++] = copie[mat->y2 - mat->y1 - 1 - j][i];

		k = 0;
		for (int i = mat->y1; i < mat->y2; i++)
			for (int j = mat->x1; j < mat->x2; j++)
				mat->matrice[i][j] = v[k++];

		eliberare(mat->y2 - mat->y1, copie);
		free(v);
	}
}

void rot_90_color(img *mat)
{
	if (mat->x1 == 0 && mat->y1 == 0 &&
		mat->x2 == mat->m && mat->y2 == mat->n) {
		img_color **v;

		v = alocare2(mat->m, mat->n);
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++) {
				v[j][i].rosu = mat->mat_col[mat->n - i - 1][j].rosu;
				v[j][i].albastru = mat->mat_col[mat->n - i - 1][j].albastru;
				v[j][i].verde = mat->mat_col[mat->n - i - 1][j].verde;
			}

		eliberare2(mat->n, mat->mat_col);

		int aux;
		aux = mat->x2;
		mat->x2 = mat->y2;
		mat->y2 = aux;
		aux = mat->n;
		mat->n = mat->m;
		mat->m = aux;

		mat->mat_col = alocare2(mat->n, mat->m);
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++) {
				mat->mat_col[i][j].rosu = v[i][j].rosu;
				mat->mat_col[i][j].albastru = v[i][j].albastru;
				mat->mat_col[i][j].verde = v[i][j].verde;
			}

		eliberare2(mat->n, v);
	} else {
		img_color **copie, *v;
		copie = alocare2(mat->y2 - mat->y1, mat->y2 - mat->y1);
		int val = (mat->y2 - mat->y1) * (mat->y2 - mat->y1);
		v = (img_color *)malloc(val * sizeof(img_color));
		int k = 0;
		for (int i = mat->y1; i < mat->y2; i++)
			for (int j = mat->x1; j < mat->x2; j++) {
				v[k].rosu = mat->mat_col[i][j].rosu;
				v[k].albastru = mat->mat_col[i][j].albastru;
				v[k].verde = mat->mat_col[i][j].verde;
				k++;
			}

		k = 0;
		for (int i = 0; i < mat->y2 - mat->y1; i++)
			for (int j = 0; j < mat->y2 - mat->y1; j++) {
				copie[i][j].rosu = v[k].rosu;
				copie[i][j].albastru = v[k].albastru;
				copie[i][j].verde = v[k].verde;
				k++;
			}

		k = 0;
		for (int i = 0; i < mat->y2 - mat->y1; i++)
			for (int j = 0; j < mat->y2 - mat->y1; j++) {
				v[k].rosu = copie[mat->y2 - mat->y1 - 1 - j][i].rosu;
				v[k].albastru = copie[mat->y2 - mat->y1 - 1 - j][i].albastru;
				v[k].verde = copie[mat->y2 - mat->y1 - 1 - j][i].verde;
				k++;
			}

		k = 0;
		for (int i = mat->y1; i < mat->y2; i++)
			for (int j = mat->x1; j < mat->x2; j++) {
				mat->mat_col[i][j].rosu = v[k].rosu;
				mat->mat_col[i][j].albastru = v[k].albastru;
				mat->mat_col[i][j].verde = v[k].verde;
				k++;
			}

		eliberare2(mat->y2 - mat->y1, copie);
		free(v);
	}
}

void rotatie(img *mat, char sir[])
{
	if (mat->incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	int numar = atoi(sir);

	if (mat->x1 != 0 || mat->y1 != 0 || mat->x2 != mat->m || mat->y2 != mat->n)
		if (mat->x2 - mat->x1 != mat->y2 - mat->y1) {
			printf("The selection must be square\n");
			return;
			}

	if (numar % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	if (numar == 90 || numar == -270) {
		if (mat->tip_color == 0)
			rot_90(mat);
		else
			rot_90_color(mat);
	}
	if (numar == -90 || numar == 270) {
		if (mat->tip_color == 0)
			for (int i = 1; i <= 3; i++)
				rot_90(mat);
		else
			for (int i = 1; i <= 3; i++)
				rot_90_color(mat);
	}
	if (numar == 180 || numar == -180) {
		if (mat->tip_color == 0)
			for (int i = 1; i <= 2; i++)
				rot_90(mat);
		else
			for (int i = 1; i <= 2; i++)
				rot_90_color(mat);
	}
	if (numar == 360 || numar == -360 || numar == 0) {
		if (mat->tip_color == 0)
			for (int i = 1; i <= 4; i++)
				rot_90(mat);
		else
			for (int i = 1; i <= 4; i++)
				rot_90_color(mat);
	}
	printf("Rotated %d\n", numar);
}

void edge(double **matrice)
{
	matrice[0][0] = -1;
	matrice[0][1] = -1;
	matrice[0][2] = -1;
	matrice[1][0] = -1;
	matrice[1][1] = 8;
	matrice[1][2] = -1;
	matrice[2][0] = -1;
	matrice[2][1] = -1;
	matrice[2][2] = -1;
}

void sharpen(double **matrice)
{
	matrice[0][0] = 0;
	matrice[0][1] = -1;
	matrice[0][2] = 0;
	matrice[1][0] = -1;
	matrice[1][1] = 5;
	matrice[1][2] = -1;
	matrice[2][0] = 0;
	matrice[2][1] = -1;
	matrice[2][2] = 0;
}

void blur(double **matrice)
{
	matrice[0][0] = 1.0 / 9;
	matrice[0][1] = 1.0 / 9;
	matrice[0][2] = 1.0 / 9;
	matrice[1][0] = 1.0 / 9;
	matrice[1][1] = 1.0 / 9;
	matrice[1][2] = 1.0 / 9;
	matrice[2][0] = 1.0 / 9;
	matrice[2][1] = 1.0 / 9;
	matrice[2][2] = 1.0 / 9;
}

void g_blur(double **matrice)
{
	matrice[0][0] = 1.0 / 16;
	matrice[0][1] = 1.0 / 8;
	matrice[0][2] = 1.0 / 16;
	matrice[1][0] = 1.0 / 8;
	matrice[1][1] = 1.0 / 4;
	matrice[1][2] = 1.0 / 8;
	matrice[2][0] = 1.0 / 16;
	matrice[2][1] = 1.0 / 8;
	matrice[2][2] = 1.0 / 16;
}

void apply(img *mat, char sir[])
{
	if (mat->incarcat == 0) {
		printf("No image loaded\n");
		return;
	}
	double **aux;
	aux = (double **)malloc(3 * sizeof(double *));
	for (int i = 0; i < 3; i++)
		aux[i] = (double *)malloc(3 * sizeof(double));

	char *p, c[256];
	p = strtok(sir, "\n ");
	p = strtok(NULL, "\n ");
	if (!p) {
		printf("Invalid command\n");
		for (int i = 0; i < 3; i++)
			free(aux[i]);
		free(aux);
		return;
	}
	strcpy(c, p);
	if (mat->tip_color == 0) {
		printf("Easy, Charlie Chaplin\n");
		for (int i = 0; i < 3; i++)
			free(aux[i]);
		free(aux);
		return;
	}
	if (strcmp(c, "EDGE") == 0) {
		edge(aux);
	} else if (strcmp(c, "SHARPEN") == 0) {
		sharpen(aux);
	} else if (strcmp(c, "BLUR") == 0) {
		blur(aux);
	} else if (strcmp(c, "GAUSSIAN_BLUR") == 0) {
		g_blur(aux);
	} else {
		printf("APPLY parameter invalid\n");
		for (int i = 0; i < 3; i++)
			free(aux[i]);
		free(aux);
		return;
	}
	img_color **copie;
	copie = alocare2(mat->n, mat->m);
	for (int i = 0; i < mat->n; i++)
		for (int j = 0; j < mat->m; j++) {
			copie[i][j].rosu = mat->mat_col[i][j].rosu;
			copie[i][j].albastru = mat->mat_col[i][j].albastru;
			copie[i][j].verde = mat->mat_col[i][j].verde;
		}

	for (int i = mat->y1; i < mat->y2; i++)
		for (int j = mat->x1; j < mat->x2; j++) {
			if (i > 0 && j > 0 && i < mat->n - 1 && j < mat->m - 1) {
				double s1 = 0, s2 = 0, s3 = 0;
				for (int p = i - 1; p <= i + 1; p++)
					for (int q = j - 1; q <= j + 1; q++) {
						double val = aux[p - i + 1][q - j + 1];
						s1 += mat->mat_col[p][q].rosu * val;
						s2 += mat->mat_col[p][q].albastru * val;
						s3 += mat->mat_col[p][q].verde * val;
					}
				copie[i][j].rosu = clamp(round(s1), 0, 255);
				copie[i][j].albastru = clamp(round(s2), 0, 255);
				copie[i][j].verde = clamp(round(s3), 0, 255);
			}
		}

	for (int i = mat->y1; i < mat->y2; i++)
		for (int j = mat->x1; j < mat->x2; j++) {
			mat->mat_col[i][j].rosu = copie[i][j].rosu;
			mat->mat_col[i][j].albastru = copie[i][j].albastru;
			mat->mat_col[i][j].verde = copie[i][j].verde;
		}
	eliberare2(mat->n, copie);
	for (int i = 0; i < 3; i++)
		free(aux[i]);
	free(aux);
	printf("APPLY %s done\n", c);
}

void save_text(img *mat, char sir[])
{
	FILE *out = fopen(sir, "wt");
	if (!out)
		return;

	if (mat->tip_color == 0)
		fprintf(out, "%s\n", "P2");
	else
		fprintf(out, "%s\n", "P3");

	fprintf(out, "%d %d\n", mat->m, mat->n);
	fprintf(out, "%d\n", mat->maxim);

	if (mat->tip_color == 0) {
		for (int i = 0; i < mat->n; i++, fprintf(out, "\n"))
			for (int j = 0; j < mat->m; j++)
				fprintf(out, "%d ", mat->matrice[i][j]);
	} else {
		for (int i = 0; i < mat->n; i++, fprintf(out, "\n"))
			for (int j = 0; j < mat->m; j++) {
				fprintf(out, "%d ", mat->mat_col[i][j].rosu);
				fprintf(out, "%d ", mat->mat_col[i][j].verde);
				fprintf(out, "%d ", mat->mat_col[i][j].albastru);
			}
	}

	fclose(out);
}

void salvare_binar(img *mat, char nume_fisier[])
{
	FILE *out = fopen(nume_fisier, "wb");
	if (!out)
		return;

	if (mat->tip_color == 0)
		fprintf(out, "%s\n", "P5");
	else
		fprintf(out, "%s\n", "P6");

	fprintf(out, "%d %d\n", mat->m, mat->n);
	fprintf(out, "%d\n", mat->maxim);

	if (mat->tip_color == 0) {
		unsigned char c;
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++) {
				c = (unsigned char)mat->matrice[i][j];
				fwrite(&c, sizeof(unsigned char), 1, out);
			}
	} else {
		unsigned char r, v, a;
		for (int i = 0; i < mat->n; i++)
			for (int j = 0; j < mat->m; j++) {
				r = (unsigned char)mat->mat_col[i][j].rosu;
				v = (unsigned char)mat->mat_col[i][j].verde;
				a = (unsigned char)mat->mat_col[i][j].albastru;
				fwrite(&r, sizeof(unsigned char), 1, out);
				fwrite(&v, sizeof(unsigned char), 1, out);
				fwrite(&a, sizeof(unsigned char), 1, out);
			}
	}

	fclose(out);
}

void save(img *mat, char sir[])
{
	if (mat->incarcat == 0) {
		printf("No image loaded\n");
		return;
	}

	int ascii = 1;
	char fisier[256], *p;
	p = strtok(sir, "\n ");
	p = strtok(NULL, "\n ");
	strcpy(fisier, p);
	p = strtok(NULL, " ");
	if (p)
		if (p[0] == 'a')
			ascii = 0;

	if (ascii == 0) {//text
		save_text(mat, fisier);
	} else if (ascii == 1) {//binar
		salvare_binar(mat, fisier);
	}

	printf("Saved %s\n", fisier);
}

int main(void)
{
	img mat;
	mat.incarcat = 0;
	char s[256], cs[256], c[256], *p, sc[256];
	int ok = 1;

	while (ok == 1) {
		fgets(s, 256, stdin);
		//strcpy(s + strlen(s) - 1, s + strlen(s));
		strcpy(sc, s);

		p = strtok(s, "\n ");
		strcpy(cs, p);
		p = strtok(NULL, "\n ");
		if (p)
			strcpy(c, p);

		if (strcmp(cs, "LOAD") == 0) {
			load(&mat, c);
		} else if (strcmp(cs, "SELECT") == 0) {
			selectare(&mat, sc);
		} else if (strcmp(cs, "EQUALIZE") == 0) {
			equalize(&mat);
		} else if (strcmp(cs, "EXIT") == 0) {
			if (mat.incarcat == 0)
				printf("No image loaded\n");
			else
				reatribuire(&mat);
			ok = 0;
		} else if (strcmp(cs, "CROP") == 0) {
			fcrop(&mat);
		} else if (strcmp(cs, "ROTATE") == 0) {
			rotatie(&mat, c);
		} else if (strcmp(cs, "APPLY") == 0) {
			apply(&mat, sc);
		} else if (strcmp(cs, "SAVE") == 0) {
			save(&mat, sc);
		} else if (strcmp(cs, "HISTOGRAM") == 0) {
			histo(&mat, sc);
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}

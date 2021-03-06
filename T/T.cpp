#include "stdafx.h"
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#define N 16
#define TIMESTEP 10000
void compute_force(double* force, double* xball, double* yball, long iball);
void compute_velocity(double* xball, double* yball, double* xvel, double* yvel, long iball);
void compute_position(double* xball, double* yball, double* xvel, double* yvel, long iball);
int main(int argc, char* argv[])
{
	long i,j,k,nper;
	int rank, size;
	double px[N*N], py[N*N], vx[N*N], vy[N*N];
	double start, end;
	FILE* fp;
	fp = fopen("proc.txt","w");
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	nper = N*N / size;
	if (rank == 0)start = clock();
	for (i = 0; i < N;i++) {
	    for (j = 0; j < N; j++) {
			px[i*N + j]= i * 0.01;
			py[i*N + j]= j * 0.01;
			vx[i*N + j] =0;
			vy[i*N + j] =0;
		}
	}
	//for (i = 0; i < N; i++) {
	//	for (j = 0; j < N; j++) {
	//		printf("(%f,%f) ", px[i*N + j], py[i*N + j]);
	//	}
	//	printf("\n");
	//}
	for (k = 0; k < TIMESTEP;k++) {
		for (i = 0; i < nper;i++) {
			compute_position(px,py,vx,vy,rank*nper+i);
			//fprintf(fp, "%f %f\n", px[i], py[i]);
		}
		MPI_Allgather(px + rank * nper, nper, MPI_DOUBLE, px, nper, MPI_DOUBLE, MPI_COMM_WORLD);
		MPI_Allgather(py + rank * nper, nper, MPI_DOUBLE, py, nper, MPI_DOUBLE, MPI_COMM_WORLD);
		//if (rank == 0) {
		//	for (i = 0; i < nper; i++) {
		//		fprintf(fp, "%f %f\n", px[i], py[i]);
		//	}
		//}
	}
	if (rank == 0) {
		end = clock();
		printf("time cost is %f", end - start);
	}
	//for (i = 0; i < N; i++) {
	//	for (j = 0; j < N; j++) {
	//		printf("(%f,%f) ", px[i*N + j], py[i*N + j]);
	//	}
	//	printf("\n");
	//}
	fclose(fp);
	MPI_Finalize();
    return 0;
}
void compute_force(double* force,double* xball, double* yball, long iball) {
	long i;
	double acc, dist, rdist;
	for (i = 0; i < N*N;i++) {
		if (i != iball) {
			rdist = sqrt((xball[i] - xball[iball])*(xball[i] - xball[iball]) + (yball[i] - yball[iball])*(yball[i] - yball[iball]));
			if (rdist < 0.02)
				dist = 0.02;
			else
				dist = rdist;
			acc = 6.67E-11*1E4*1E4 / (dist*dist);
			force[0] =force[0]+ acc * (xball[i] - xball[iball]) / rdist;
			force[1] = force[1] + acc * (yball[i] - yball[iball]) / rdist;
		}
	}
}
void compute_velocity(double* xball, double* yball, double* xvel, double* yvel, long iball) {
	double force[2] = {0,0};
	compute_force(force,xball,yball,iball);
	xvel[iball] =xvel[iball]+ force[0]/ (TIMESTEP*10000);
	yvel[iball] = yvel[iball] + force[1] /(TIMESTEP*10000);
	//printf("%f\n",xvel[iball]);
}
void compute_position(double* xball, double* yball, double* xvel, double* yvel, long iball) {
	compute_velocity(xball,yball,xvel,yvel,iball);
	xball[iball] = xball[iball] + xvel[iball]/ TIMESTEP;
	yball[iball] = yball[iball] + yvel[iball]/ TIMESTEP;
}
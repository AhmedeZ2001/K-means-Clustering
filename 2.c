#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <omp.h> 
#define ITERATIONS 2
struct Point {
    int x , y;
	int clusterid;
 };
 struct Cluster{
	 int x ,y,numPoints;
	 double sumX, sumY,mean;
 };
 double distance(struct Point p, struct Cluster u){
    double px = p.x;
    double py = p.y;
    double ux = u.x;
    double uy = u.y;
    return sqrt(pow(px - ux, 2) + pow(py - uy, 2));
 }
int main(int argc, char argv[])
{
    struct Point* objects = malloc(sizeof(struct Point) * 1000);
    struct Cluster* cluster = malloc(sizeof(struct Cluster) * 10);
    int threads_num;
    FILE *fp,*fc;
    int clustar_num;
    struct Cluster p1;
    struct Point p2;
    double pow1;
    double pow2;
    double res;
    fp=fopen("/shared/test.txt","r");
    fc=fopen("/shared/test2.txt","r");
    int id,cid;
    int i,j,m,c,n=1,l;
    double dist,min=1000;
    double a2[1000];
    double x[30][20];
    printf("Enter a number of cluster: ");
    scanf("%d", & clustar_num);
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            n = n + 1;
            rewind(fp);
                //printf("%d \n" ,n);
    for ( i = 0; i < n; i++) 
        fscanf(fp, "%d %d", &objects[i].x,&objects[i].y);
    for ( i = 0; i < clustar_num; i++) 
        fscanf(fc, "%d %d", &cluster[i].x,&cluster[i].y);
 
    for(l=0; l<ITERATIONS; l++){

    for(i=0 ;i<threads_num;i++){
		cluster[i].sumX=0;
		cluster[i].sumY=0;
		cluster[i].numPoints=0;
		min=1000;
     }
 
    #pragma omp parallel shared(j) num_threads(clustar_num)
    {	
        threads_num=omp_get_num_threads();  
        #pragma omp for schedule(static)
        for (j=0 ; j<clustar_num;j++)
        {
            p1.x=cluster[j].x;
            p1.y=cluster[j].y;
            for(i=0; i < n; i++)
            {
                p2.x=objects[i].x;
                p2.y=objects[i].y;
                res=distance(p2,p1);
                x[j][i]=res;
                printf("IN Thread %d distance between point %d (%d,%d) and cluster %d (%d,%d) is %lf \n", omp_get_thread_num() ,i,p2.x,p2.y,j,p1.x,p1.y,res);
            }
        }
    }
    for(j=0 ;j< n ;j++){
       for(i=0; i <clustar_num; i++){
	     if(min>x[i][j])
		{
			min=x[i][j];
			cid=i;
		}
	}
		min=1000;
		objects[j].clusterid=cid;
	}

    #pragma omp parallel shared(j) num_threads(clustar_num)
    { 
    #pragma omp for schedule(static)
	for(i=0 ;i<n;i++)
	{
		cluster[objects[i].clusterid].sumX += objects[i].x;
		cluster[objects[i].clusterid].sumY += objects[i].y;
		cluster[objects[i].clusterid].numPoints++;	
	}
	#pragma omp for schedule(static)
	for(i=0 ;i<clustar_num;i++)
	{
		printf("points in cluster %d",i);
		for(j=0 ;j< n ;j++)
		{
			if(objects[j].clusterid == i)
				printf("  (%d , %d)   " ,objects[j].x,objects[j].y );
		}
		printf("\n"); 
	}
	#pragma omp for schedule(static)
	for(i=0 ;i<clustar_num;i++)
	{
		if(cluster[i].numPoints>0)
		{
			cluster[i].x =(int) (cluster[i].sumX / cluster[i].numPoints);
			cluster[i].y = (int)(cluster[i].sumY / cluster[i].numPoints);
		}
			printf("new Cluster %d is (%d ,%d) , ",i,cluster[i].x ,cluster[i].y );
			printf("With Number of points %d \n" ,cluster[i].numPoints);
	}
  }
}
}
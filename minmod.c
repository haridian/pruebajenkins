#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 10000
#define MAXITER 10000
#define RESTART 5

/******************************************************************************/

int r[N];
int violaciones[N];

long ratio; 
clock_t time1; 
clock_t time2;
/******************************************************************************/

int ver_rest(int,int,int,int);
void greedy(int);
int ver_viol(int);
void swap(int *, int *);

/******************************************************************************/

void swap(int *x, int *y)
{
	int temp;

	temp = r[*x];
	r[*x] = r[*y];
	r[*y] = temp;

}

/******************************************************************************/

int ver_rest(int ri, int rj, int i, int j)
{
	int a, b;
	if(ri==rj)
		return 4;
	else
	{
		a=abs(ri-rj);
		b=abs(i-j);
		if (a==b)
			return 1;
		else
			return 0;
	}
}

/******************************************************************************/

void greedy(int n)
{
	int i, j,l,x,ver;
	int min, ind;
	int k[N];

	for(i=0;i<(n+1);i++)
	{
   	   k[i]=0;
	}
	 r[0]=(int)(((n-1+1.0)*rand())/(RAND_MAX+1.0));

	for(i=1;i<n;i++)
	{
		j=0;
		for(x=0;x<i;x++)
		{
		  k[x]=0;
		}

		do
		{
			r[i]=j;

			for(l=0;l<i;l++)
			{
				ver=ver_rest(r[i],r[l],i,l);
                if(ver==1)
                {
                   k[j]++;

                }    
                else if(ver==4)
                {
                   k[j]=k[j]+n;

                }    
			}
			j++;
		} while((k[j-1]!=0)&&(j<n));

		if(k[j-1]!=0)
		{
			min=999;
			for(j=0;j<n;j++)
			{
				if(k[j]<min)
				{
					min=k[j];
					ind=j;
				}
			}

			r[i]=ind;

		}
	}
}

/******************************************************************************/

int ver_viol(int n)
{

	int i, j, k, l=0;

	for(i=0;i<(n);i++)
	{
		violaciones[i]=0;
	}

	for(i=0;i<(n-1);i++)
	{
		for(j=(i+1);j<n;j++)
		{
			k=ver_rest(r[i],r[j],i,j);
			if(k!=0)
			{
				violaciones[i]++;
				violaciones[j]++;
				l++;
			}
		}
	}
	return(l);
}
/******************************************************************************/
int main(void)
{
	int i, j, n, x, rest;
	int ant;
	int indmax, vmax;
	int res=0;
	int iter=0;
	int optimo=0;
	long xx;
	int h, mi, s;

	ratio = 1000/CLK_TCK;
	time1 = clock();
	srand (time(NULL));

	printf("ingrese el valor de n? ");
	scanf("%d",&n);

	printf("\niniciando el proceso: ");
	do{
		iter=0;
		greedy(n);
		optimo=ver_viol(n);
		ant=-1;
		printf(".");
		while((iter<MAXITER)&&(optimo!=0))
		{
			i=0;
			j=-1;
			indmax=-1;
			vmax=-1;

			for(i=0;i<n;i++)
			{
				if((violaciones[i]!=0)&&(i!=ant))
				{
					if(violaciones[i]>vmax)
					{
						indmax=i;
						vmax=violaciones[i];
					}
				}
			}
			j=indmax;
			do{
				x=(int)(((n-1+1.0)*rand())/(RAND_MAX+1.0));
			}while(j==x);

			swap(&j,&x);
			rest=ver_viol(n);
			if(rest>optimo)
			{
				swap(&x,&j);
				rest=ver_viol(n);
			}
			else
			{
				optimo=ver_viol(n);
			}
         iter++;
         ant=j;
		}
		res++;
	}while((res<RESTART)&&(optimo!=0));

	time2 = clock();

	printf("\nVector final\n");
	for(i=0;i<n;i++)
	{
		printf("%d-",r[i]);
	}

	printf("\nVector violaciones para n=%d:\n",n);
	for(i=0;i<n;i++)
	{
		printf("%d-",violaciones[i]);
	}

	xx=ratio * (long)time2 - ratio * (long)time1;

	s=xx/1000;
	mi=s/60;
	h=mi/60;
	s=s%60;
	mi=mi%60;
	printf("\n\nTiempo de Ejecucion : %d:%d:%d", h,mi,s);
	return 0;
}


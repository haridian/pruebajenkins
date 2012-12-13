#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//#define ALFA 1
//#define BETA 5
//#define RHO 0.5
//#define Q 100
//#define T0 0.000001
//#define E 5
#define MAXITER 30
#define N 17

/*********************************************************************************************/
int dist[N][N];
int nodos;
double tau[N][N];
int tour[N][N];
int tmin[N];
unsigned long lmin;
int permitido[N];
double suma;
double p[N];
double RHO=0.5;
double Q=100;
double T0=0.000001;
double E=5;
int arco[N+1][N][N];
double delta_tau;
double delta_tau_e;
double alfa=1;
double beta=5;
int l[N];

/*********************************************************************************************/

void inivar(void);
double azar(void);
void ver_perm(int, int);

/*********************************************************************************************/
void inivar(void)
{
	FILE *dato;

	int i, j, dis;

	if ((dato = fopen("gr.tsp","r"))==NULL)
	{
		printf("\nNo se puede abrir el archivo gr.txt!\n");
		exit(1);
	}

	// Lectura de datos de Cantidad de Nodos
	fscanf(dato, "%d",&nodos);

	//Inicializacion de las matrices
	for (i=0; i<N; i++)
	{
		for (j=0; j<N; j++)
		{
			dist[i][j]=-1;
			tour[i][j]=-1;
			tau[i][j]=-1.0;
		}
		tmin[i]=-1;
      l[i]=-1;
	}


	//Lectura y creacion de matriz dist[i][j]
	for (i=0; i<nodos; i++)
	{
		for (j=0; j<i+1; j++)
		{
			fscanf(dato, "%d", &dis);
			dist[i][j]=dis;
			dist[j][i]=dis;
		}
	}

	//Inicializacion de la intensidad de feromonas en cada arco
	for (i=0; i<nodos; i++)
	{
		for (j=0; j<nodos; j++)
		{
			tau[i][j]=T0;
		}
	}

	//Ubicacion de una Hormiga en cada nodo
	for (i=0; i<nodos; i++)
	{
		tour[i][0]=i;
	}

	//Inicializacion del tour
	for (i=0; i<nodos; i++)
	{
		tmin[i]=i;
	}

	lmin=10000;

	//Finalizacion del archivo
	fclose(dato);
}

/*********************************************************************************************/
double azar(void)
{
	double val;
	val = (double)rand()/(RAND_MAX+1);
	return(val);
}

/*********************************************************************************************/
void ver_perm(int hormiga, int n)
{
	int i, j;
	for(j=0; j<nodos; j++)
	{
		permitido[j]=j;
	}
	for(i=0; i<n; i++)
	{
		if(tour[hormiga][i]>=0)
		{
			for(j=0; j<nodos; j++)
			{
				if(permitido[j]==tour[hormiga][i])
				{
					permitido[j]=-1;
				}
			}
		}
	}
}

/*********************************************************************************************/
int main(void)
{
	int i, j, k, n, t, m, x;
	double a, b;
	int hormigas;
	int c_actual;
	int c_siguiente;
	double r;

	srand (time(NULL));
	inivar();
	hormigas=nodos;
	for(t=0; t<MAXITER; t++)
	{
		//Inicializacion de la matriz tour
		for (i=0; i<N; i++)
		{
			for (j=1; j<N; j++)
			{
				tour[i][j]=-1;
			}
		}

		for(k=0; k<hormigas; k++)
		{
			for(n=1; n<nodos; n++)//repetir
			{
				c_actual=tour[k][n-1];
				//seleccionar la siguiente ciudad a visitar con
				//probabilidad Pijk:

				//verificar que ciudades quedan disponibles
				ver_perm(k,n);

				//calcular la probabilidad de cada ciudad
				j=0;
				suma=0;
				for(i=0; i<nodos; i++)
				{
					if(permitido[i]>=0)
					{
						a=pow(tau[c_actual][permitido[i]], alfa);
						b=pow(1.0/dist[c_actual][permitido[i]], beta);
						p[j]=a*b;
						//printf("\np[%d]=%E",i, p[j]);

						suma=suma+p[j];
						//printf("\nsuma=%E",suma);
						j++;
					}
				}

				for(i=0; i<j; i++)
				{
					p[i]=p[i]/suma;
				}

				/*printf("\n\np[i]=p[i]/suma");
				for(i=0; i<nodos; i++)
				{
					printf("\np[%d]=%E",i, p[i]);
				}
				*/
				//calcular la probabilidad acumulada de cada ciudad
				for(i=1; i<j; i++)
				{
					p[i]=p[i]+p[i-1];
				}
				/*printf("\n\np[i]=p[i]+p[i-1]");
				for(i=0; i<nodos; i++)
				{
					printf("\np[%d]=%E",i, p[i]);
				}*/
				//generar un numero aleatorio y comparar con las probabilidades
				r=azar();
				c_siguiente=-1;

				//caso 1: r<p[0]
				if(r<p[0])
				{
					i=0;
					do
					{
						if(permitido[i]>=0)
							c_siguiente=permitido[i];
						i++;
					}while((i<nodos)&&(c_siguiente<0));
				}

				//caso 2: p[0]<=r<p[max]
				for(i=1; i<j; i++)
				{
					if((r>=p[i-1])&&(r<p[i]))
					{
						m=0;
						x=0;
						do
						{
							if(permitido[m]>=0)
							{
								x++;
								if(x==i+1)
									c_siguiente=permitido[m];
							}
							m++;
						}while((m<nodos)&&(c_siguiente<0));
					}
				}

				//caso 3: p[max]<=r
				if(r>=p[j-1])
				{
					m=0;
					for(i=0; i<nodos; i++)
					{
						if(permitido[i]>=0)
						{
							m++;
							if(m==j-1)
								c_siguiente=permitido[i];
						}
					}
				}
				//agregar la ciudad seleccionada al tour de la hormiga k
				tour[k][n]=c_siguiente;
				//printf("\n\nc_siguiente=%d", c_siguiente);
			}//hasta hormiga k complete su tour

			//Calcular el largo Lk del tour de la Hormiga k
			l[k]=0;
			for(i=0; i<nodos-1; i++)
			{
				c_actual=tour[k][i];
				c_siguiente=tour[k][i+1];
				l[k]=l[k] + dist[c_actual][c_siguiente];
			}

			c_actual=tour[k][nodos-1];
			c_siguiente=tour[k][0];
			l[k]=l[k] + dist[c_actual][c_siguiente];
		}

		//Guardar el mejor tour tmin y lmin encontrado hasta ahora
		for(i=0; i<nodos; i++)
		{
			if(l[i]<=lmin)
			{
				lmin=l[i];
				for(j=0; j<nodos; j++)
				{
					tmin[j]=tour[i][j];
				}
			}
		}

		//inicializacion de la matriz arco
		for(k=0; k<hormigas+1; k++)
		{
			for(i=0; i<nodos; i++)
			{
				for(j=0; j<nodos; j++)
				{
					arco[k][i][j]=-1;
				}
			}
		}

		//Creacion de la matriz de arcos (i,j) para delta_tau:
		for(k=0; k<hormigas; k++)
		{
			for(j=0; j<nodos-1; j++)
			{
				arco[k][tour[k][j]][tour[k][j+1]]=1;
			}
			arco[k][tour[k][j]][tour[k][0]]=1;
		}

		//Creacion de la matriz de arcos (i,j) para delta_tau_e:
		for(j=0; j<nodos-1; j++)
		{
			arco[hormigas][tmin[j]][tmin[j+1]]=1;
		}
		arco[hormigas][tmin[j]][tmin[0]]=1;


		//Para cada arco (i,j) hacer:
		for(i=0; i<nodos; i++)
		{
			for(j=0; j<nodos; j++)
			{
				delta_tau=0.0;

				for(k=0; k<hormigas; k++)
				{
					if(arco[k][i][j]==1)
					{
						delta_tau=delta_tau+(Q/l[k]);
					}
				}
				delta_tau_e=0.0;
				if(arco[hormigas][i][j]==1)
				{
					delta_tau_e=Q/lmin;
				}

				//Modificar los niveles de feromona aplicando la regla
				tau[i][j]=(1.0-RHO)*tau[i][j]+delta_tau+E*delta_tau_e;
			}
		}
	}

  /*
	printf("Matriz de distancias:\n");
	for(i=0;i<nodos;i++)
	{
		for(j=0;j<nodos;j++)
		{
			printf("%d\t",dist[i][j]);
		}
		printf("\n");
	}

	printf("Tour de cada Hormiga:\n");
	for (i=0; i<nodos; i++)
	{
      	printf("%d\t",i);
       	for(j=0;j<nodos;j++)
		{
      		printf("%d\t",tour[i][j]);
		}		
   		printf("\n");
	}
	*/
	printf("\nTour mas corto:");
	//Inicializacion del tour
	for (i=0; i<nodos; i++)
	{
		printf("%d\t",tmin[i]);
	}
	printf("\nLargo del Tour mas corto: %lu",lmin);

	return 0;
}

#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#define MAX_SEARCH_ITERATIONS 1000

double get_final_temperature(double, double, double);

/* Implement bisection search */

double bisection_search(double a, double b, double tol, double target, double fx,  double fy) {
    printf("-----------Running Bisection Search --------\n");
    int iter=0;
    while(iter<MAX_SEARCH_ITERATIONS) {
        /* double mid = ...; Calculate next point to test*/
        double mid=(a+b)/2;
        double temp = get_final_temperature(mid, fx, fy);

        if(a>b) {
            printf("Invalid inputs\n");
            return -1;
        }

        /*Temperature within the tolerance of target*/
        if(abs(temp-target)<tol) {
            printf("Reached target %f with temp %f and tol %f in %d iterations\n", target, temp, tol, iter);
            return mid;
        } else if(temp > target) {
            /*do something (change either a or b)*/
            /*temp is greater than target */
            b=mid;

        } else {
            a=mid;
        }

            if(abs(b-a) < tol) {
                printf("Interval too small %d iterations\n", iter);
                return -1;
            }
            printf("Reached a temperature of %f with radiator set at %f\n", temp, mid);
            iter++;
        }
        printf("Search failed\n");
        return -1;
}

/*Implementing nsection search*/

double nsection_search(int n, double a, double b, double tol, double target, double fx, double fy) {
    printf("-----------Running Nsection Search --------\n");
    int iter=0;
    double space[n+1],mids[n+1],temps[n+1];
    int i;
    /*Allocate arrays of size n to store points to test (mids) and corresponding temperatures (temps)*/
    while(iter<MAX_SEARCH_ITERATIONS) {
        double interval_dist= (b-a)/(n-1);

        if(a>b) {
            printf("Invalid inputs\n");
            return -1;
        }
        /* Populate array mids with n equally spaced points between a and b
        Making equal intervals spaces from a and b */
        space[n]=b;
        for(i=0;i<n;i++)
        {
            space[i]=a+(i*interval_dist);
        }

        /*Getting midpoints of each spaces*/

        for(i=0;i<n+1;i++)
        {
            mids[i]=(space[i]+space[i+1])*0.5;
        }
        /*Getting current midpoint's final temperature to check with target temperature*/
        for(i=0; i<n+1 ; i++) {
            temps[i] = get_final_temperature(mids[i], fx, fy);
        }

        /*Checking the temperature at a position and determining a and b values*/
        for(i=0; i<=n; i++) {
            if(abs(temps[i] -target)<tol) {
            printf("Reached target %f with temp %f and tol %f in %d iterations\n", target, mids[i], tol, iter);
            return mids[i];
            } else if(temps[i+1]>target && temps[i] < target)
            {
                a=space[i];
                b=space[i+1];
            }
        }

        if(abs(b-a) < tol) {
            printf("Interval too small %d iterations\n", iter);
            return -1;
        }
        iter++;
    }
    printf("Search failed\n");
    return -1;
}


double nsection_search_mpi(int rank, int n, double a, double b, double tol, double
target, double fx, double fy) {
    print("--------Nsection Search MPI-----------");
    int iter=0;
    double result;

    //Allocate arrays of size n to store points to test (mids) and corresponding temperatures (temps)
    double mids[n+1],temps[n+1],midpoint,tempmid;

    if(a>b) {
        printf("Invalid inputs\n");
        return -1;
    }
    // Perhaps some local variables initialised here
    int found = 0;
    int i;
    double *ptr=NULL;


    while(iter<MAX_SEARCH_ITERATIONS && !found) {
        double interval_dist= (b-a)/(n-1);
        mids[0]=a;
        if(rank==0)
        {
            for(i=1;i<n;i++)
            {
                mids[i]=a+(i*interval_dist);
            }
            //Movement of the pointer
            ptr=mids;
        }
        //First, scattering the midpoints
        MPI_SCATTER(ptr,1,MPI_DOUBLE,&midpoint,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

        //Ranks getting their own temp
        tempmid= get_final_temperature(midpoint, fx, fy);


        //Then gathering up the data
        MPI_GATHER(&tempmid,1,MPI_DOUBLE,temps,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

        if(rank==0)
        {
            for(i=0; i<=n; i++) {
                if(abs(temps[i] -target)<tol) {
                    printf("Reached target %f with temp %f and tol %f in %d iterations\n", target, mids[i], tol, iter);
                    found=1;
                    MPI_BCAST(&found,1,MPI_INT,0,MP_COMM_WORLD);
                    return mids[i];
                } else if(temps[i]<target)
                {
                    printf("Temperature of %f is less than target. Setting a to %f \n",temps[i], mids[i]);
                    a=mids[i];
                } else
                {
                    printf("Temperature of %f is greater than target. Setting b to %f\n", temps[i], mids[i]);
                    b=mids[i];
                }
            }
            if(abs(b-a) < tol) {
                printf("Interval too small %d iterations\n", iter);
                return -1;
            }
            iter++;
        }

    }
    if (!found) {
        printf("Search failed\n");
        result = -1;
    }
    return result;
}

int main(int argc, char *argv[])
{
    int rank, size;
     //Do some MPI initialisation here
    MPI_Init(NULL,NULL);
    MPI_size(MPI_COMM_WORLD,&size);
    MPI_rank(MPI_COMM_WORLD,&rank);
    printf("Communication Size: %d, Rank: %d \n, size,rank);
   /*Start timing*/
    double a = atoi(argv[1]);
    double b = atoi(argv[2]);
    double target = atoi(argv[3]);
    /*Values as given in question */
    /*double a = 10, b=5000, target = 37;*/
    double n = size;
    double px=5.0, py=5.0, lx=10.0, ly=10.0;
    /* Tolerance value */
    double tol = 0.000000000000001;
    double fx=px/lx, fy=py/ly;
    /*double result=nsection_search(n, a, b, tol, target, fx, fy);*/

    /*double result = bisection_search(a, b, tol, target, fx, fy);*/
    Finally, move to an MPI implementation of the nsection search
    double start_time=MPI_WTime();
    double result = nsection_search_mpi(rank, n, a, b, tol, target, fx, fy);;
    double end_time=MPI_WTime();

    if(rank==0)
    {
       /*On a single rank, print the final result and the time taken */

        printf("Time taken to run the computation \n", end_time-start_time);
        printf("Result = ",result);
    }
    MPI_Finalize();
    return 1;
}
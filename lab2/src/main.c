#include<math.h>
#include <pthread.h>
#include <semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>

struct timeval stop_time, aastart;


clock_t start,end;
long gNumOfThreads;
long gNumOfTerms;
sem_t gSumSemaphore;
long double gThreadSumLeibniz=0.0;
long double gSerialSumLeibniz=0.0;
long double gThreadProductWallis=1.0;
long double gSerialProductWallis=1.0;
long int gTermsLeibniz;
long int gTermsWallis;

void* ThreadJobLeibniz(void* threadCount);
void* ThreadJobWallis(void* threadCount);
long double CalPiFromLeibnizFormulaMT(long int terms);
long double CalPiFromWallisFormulaMT(long int terms);
long double GetSumLeibniz(long int startTerm, long int endTerm);
long double GetProductWallis(long int startTerm, long int endTerm);
long double CalPiFromLeibnizFormula(long int terms);
long double CalPiFromWallisFormula(long int terms);
int Accuracy(long double dReal, long double dCal);
int AccuracyBits(long double dReal, long double dCal);
long double AccuracyPercent(long double dReal, long double dCal);
void GetArgs(int argc, char *argv[]);
void PrintUsage();


int main(int argc, char**argv)
{
	GetArgs(argc,argv);
	printf("Calcuating Pi values for %ld terms.\n", gNumOfTerms);

	double dLeibnizSerialTime, dLeibnizMultithreadTime,
			dWallisSerialTime, dWallisMultiThreadTime;
	long double dRealPI = M_PI;
	
	long double dLeibnizPI = CalPiFromLeibnizFormula(gNumOfTerms);
	dLeibnizSerialTime =  ((double) (end - start)) / CLOCKS_PER_SEC;

	long double dLeibnizPIMT = CalPiFromLeibnizFormulaMT(gNumOfTerms);
	dLeibnizMultithreadTime =  ((double) (end - start)) / CLOCKS_PER_SEC;

	long double dWallisPI = CalPiFromWallisFormula(gNumOfTerms);
	//dWallisSerialTime =  ((double) (end - start)) / CLOCKS_PER_SEC;
	
    fprintf(stderr, "SERIAL WALLIS %lu us\n", (stop_time.tv_sec - aastart.tv_sec) * 1000000 + stop_time.tv_usec - aastart.tv_usec);

	long double dWallisPIMT = CalPiFromWallisFormulaMT(gNumOfTerms);
	//dWallisMultiThreadTime =  ((double) (end - start)) / CLOCKS_PER_SEC;
    fprintf(stderr, "MT WALLIS %lu us\n", (stop_time.tv_sec - aastart.tv_sec) * 1000000 + stop_time.tv_usec - aastart.tv_usec);
	


	printf("Actual PI             = %.30Lf\n", dRealPI);
	printf("Leibniz PI            = %.30Lf\n", dLeibnizPI);
	printf("Leibniz PI\n"
		   "with Multi-threading  = %.30Lf\n", dLeibnizPIMT);
	printf("Wallis PI             = %.30Lf\n", dWallisPI);
	printf("Wallis PI\n"
		   "with Multi-threading  = %.30Lf\n", dWallisPIMT);
	

	printf("Accuracy (in digits) \n"
		"Leibniz Serial = %d decimal digits\n"
		"Leibniz MT     = %d decimal digits\n"
		"Wallis  Serial = %d decimal digits\n"
		"Wallis  MT     = %d decimal digits\n",
		 Accuracy(dRealPI,dLeibnizPI),
		 Accuracy(dRealPI,dLeibnizPIMT),
		 Accuracy(dRealPI,dWallisPI),
		 Accuracy(dRealPI,dWallisPIMT)
		 );

	printf("Accuracy (in bits) \n"
		"Leibniz Serial = %d decimal bits\n"
		"Leibniz MT     = %d decimal bits\n"
		"Wallis  Serial = %d decimal bits\n"
		"Wallis  MT     = %d decimal bits\n",
		 AccuracyBits(dRealPI,dLeibnizPI),
		 AccuracyBits(dRealPI,dLeibnizPIMT),
		 AccuracyBits(dRealPI,dWallisPI),
		 AccuracyBits(dRealPI,dWallisPIMT)
	);

	printf("Accuracy (absolute percentage) \n"
		"Leibniz Serial = %Lf%%\n"
		"Leibniz MT     = %Lf%%\n"
		"Wallis  Serial = %Lf%%\n"
		"Wallis  MT     = %Lf%%\n",
		 AccuracyPercent(dRealPI,dLeibnizPI),
		 AccuracyPercent(dRealPI,dLeibnizPIMT),
		 AccuracyPercent(dRealPI,dWallisPI),
		 AccuracyPercent(dRealPI,dWallisPIMT)
	);
	return 0;
}

void GetArgs(int argc, char **argv)
{
	if(argc == 3){
		gNumOfThreads = (int)strtol(argv[1], NULL, 10);
		gNumOfTerms   = strtol(argv[2], NULL, 10);
		if(gNumOfTerms%gNumOfThreads != 0){
			fprintf(stderr, "!!Number of terms not divisible by"
				" number of thread.!!\n");
			PrintUsage(argv);
		}
		if(gNumOfThreads < 1){
			fprintf(stderr, "!!Number of thread cannot be"
				" less than 1.!!\n");
			PrintUsage(argv);
		}
		sem_init(&gSumSemaphore, 0, 1);
		return;
	}
	PrintUsage(argv);
}

void PrintUsage(char**argv)
{
	fprintf(stderr, "Usage: %s <Number of Threads> <Number of Terms>\n", argv[0]);
	fprintf(stderr, "   Number of Threads: T\n"
					"   Number of Terms  : N\n"
					"   T should be >= 1. N should be evenly divisible by T.\n");
	exit(0);
}

int Accuracy(long double dReal, long double dCal)
{
	int ans = 0;
	long long x = dReal /10;
	long long y = dCal  /10;
	while(x==y){
		dReal*=10;
		dCal*=10;
		x=dReal/10;
		y=dCal/10;
		ans++;
	}
	if(ans<=0)return 0;
	return ans-1;
}

int AccuracyBits(long double dReal, long double dCal)
{
	long double diff = dReal-dCal;
    int e;
    long double fraction = frexp(diff, &e);
    return abs(e);
}

long double AccuracyPercent(long double dReal, long double dCal)
{
	long double diff = (fabsl(dReal-dCal))*100;
	long double ans = 100.0;
	ans = ans - (diff/dReal);
	return ans;
}

void* ThreadJobLeibniz(void* threadCount)
{
	long tCount = (long) threadCount;
	long int startTerm = (gTermsLeibniz/gNumOfThreads)*tCount;
	long int endTerm = (gTermsLeibniz/gNumOfThreads)*(tCount+1);
	long double tSum = GetSumLeibniz(startTerm,endTerm);
	sem_wait(&gSumSemaphore);
	gThreadSumLeibniz += tSum;
	sem_post(&gSumSemaphore);
	return NULL;
}

void* ThreadJobWallis(void* threadCount)
{
	long tCount = (long) threadCount;
	long int startTerm = (gTermsLeibniz/gNumOfThreads)*tCount;
	long int endTerm = (gTermsLeibniz/gNumOfThreads)*(tCount+1);
	long double tProduct = GetProductWallis(startTerm,endTerm);
	sem_wait(&gSumSemaphore);
	gThreadProductWallis = gThreadProductWallis*tProduct;
	sem_post(&gSumSemaphore);
	return NULL;
}


long double CalPiFromLeibnizFormulaMT(long int n)
{
	gTermsLeibniz = n;
	pthread_t* threads = (pthread_t*) malloc (gNumOfThreads*sizeof(pthread_t));
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	start = clock();
	for(long lThread=0; lThread<gNumOfThreads;lThread++){
		pthread_create(&threads[lThread],&attr,ThreadJobLeibniz,(void*)lThread);
	}
	for(long i=0 ;i<gNumOfThreads;i++){
		pthread_join(threads[i],NULL);
	}
	end = clock();
	return 4*gThreadSumLeibniz;
}

long double CalPiFromWallisFormulaMT(long int n)
{
	gTermsWallis = n;
	pthread_t* threads = (pthread_t*) malloc (gNumOfThreads*sizeof(pthread_t));
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	//start = clock();
	gettimeofday(&aastart, NULL);
	for(long lThread=0; lThread<gNumOfThreads;lThread++){
		pthread_create(&threads[lThread],&attr,ThreadJobWallis,(void*)lThread);
	}
	for(long i=0 ;i<gNumOfThreads;i++){
		pthread_join(threads[i],NULL);
	}
	gettimeofday(&stop_time, NULL);
	//end = clock();
	return 2*gThreadProductWallis;
}

long double GetSumLeibniz(long int start, long int end)
{
	// [start,end)
	long double sum=0.0;
	long double term;
	for(long double i=start;(long int)i<end;i++){
		term= pow(-1,i)/(2*i+1);
		sum+= term;
	}
	return sum;
}

long double GetProductWallis(long int start, long int end)
{
	// [start,end)
	long double product=1.0;
	long double term;
	if(start==0)start++;
	for(long double i=start;(long int)i<end;i++){
		term= (2*i)/(2*i-1);;
		product= product*term;
		term=(2*i)/(2*i+1);
		product=product*term;
	}
	return product;
}

//Leibniz Formula. 
long double CalPiFromLeibnizFormula(long int n)
{
	start = clock();
	if(n == -1){
		printf("Enter number of terms for Leibniz Formula: ");
 		scanf("%ld", &n);
	}
	long double pi;
	long double sum= GetSumLeibniz(0,n);
	pi = 4 * sum;
	end = clock();
	return pi;
}


//Wallis Method.
long double CalPiFromWallisFormula(long int n)
{
	//start = clock();
	gettimeofday(&aastart, NULL);
	if(n == -1){
		printf("Enter number of terms for Wallis Formula: ");
 		scanf("%ld", &n);
	}
	long double product = GetProductWallis(0,n);
	long double pi = 2 * product;
	//end = clock();
	gettimeofday(&stop_time, NULL);
	return pi;
}
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdbool.h>
#include <ctime>
#include <cmath>
#include <cfloat>
#include <sstream>
#include <iostream>
#include <pthread.h>
#include <complex>
#include "armadillo"
#include "rpoly.h"
#include <fcntl.h>
#include <vector>
#include <cerrno>
#include <algorithm>
#include <ifaddrs.h>


using namespace arma;
using namespace std;

//#define ClientNum 4  //There are four clients in Prony system

//#define Height_H 60
#define NUM 2   // number of machine in the system;
#define PoleNum 2*NUM //  number of poles of Laplace transfer equation;
#define ParaNum PoleNum
#define StateNum 4
#define Ts 0.2 // sampling time
#define MSS 200
#define DEFAULT_QUEUE_LEN 10
#define DEFAULT_MAX_BUFFER_LEN 4096
#define SERVER_MAX_BUF 1500
#define MAX_FILENAME_LEN 50

//define the parameter of Delay Model
//#define MU 5.3
#define SIGMA 0.078
#define LAMBDA 1.39
#define P 0.58
#define MAX_DELAY 100000000  // 100msec


/* declare global variable*/



//static volatile int Exit_Flag = 0;

int timer_sub(const struct timeval *start_time, const struct timeval *end_time, struct timeval *result);
double RandomGenerator(double mu, double sigma, double lambda, double p);
int set_delay(char ip[], int delay);
void *Server_handle(void * parmPtr);
void Mutex_initialization();
void Mutex_destroy();
char* GetHostIP();

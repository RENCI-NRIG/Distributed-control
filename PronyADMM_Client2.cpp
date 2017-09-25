/*
 ============================================================================
 Name        : PronyADMM_Server.cpp
 Author      : Jianhua Zhang
 Description : Initialize the Server to receive local estimated parameters from clients, average them, and then broadcast them back to each client.
 File        : ISO center of power system.  In this version, we only use TCP connection.
 Data        : Feb,14th,2014
 Update	     : April,7th,2015 Basic S-ADMM algorithm with time collection
 CommandLine : ./ADMMServer <TCP_port> <Num of PMUs>
 ============================================================================
 */
#include "Prony_common.h"
#include "pthread.h"
#include "stdlib.h"
#define ERROR 0.00001
#define imagError 0.005
#define SamNum 90

int main(int argc, char *argv[])
{
	//========================================= Initialization Phase ========================================//
    if (argc != 7) cout<<"The command format should be ./ADMMServer <TCP_port> <# of PMUs>."<<endl;
    int ClientNum,i,j;
    ClientNum = atoi(argv[1]);
	struct timeval Start, Result, t34_start, t34_end, SendingTime,t34_result, Tstart, Tend, Tresult;
    long int t23[ClientNum];

	// For collecting timing
	ofstream timing_file;
	char filename1[50];
	char tempbuf1[5];
	char tempbuf2[5];
	char tempbuf3[5];

	time_t now_time;
	time(&now_time);
	struct tm * tm_info;
	tm_info = localtime(&now_time);
	//itoa((now->tm_year+1900), tempbuf1, 10);
	strcpy(filename1, "Serverdata_file_");	
	snprintf(tempbuf1, 5, "%d", tm_info->tm_year+1900);	
	strcat(filename1, tempbuf1);
	strcat(filename1, "-");
	//itoa(now->tm_mon + 1, tempbuf2, 10);
	snprintf(tempbuf2, 5, "%d", tm_info->tm_mon + 1);	
	strcat(filename1, tempbuf2);
	strcat(filename1, "-");
	//itoa(now->tm_mday, tempbuf3, 10);
	snprintf(tempbuf3, 5, "%d", tm_info->tm_mday);	
	strcat(filename1, tempbuf3);
    strcat(filename1, "-");
    snprintf(tempbuf1, 5, "%d", tm_info->tm_hour);
    strcat(filename1, tempbuf1);
    strcat(filename1, ":");
    snprintf(tempbuf2, 5, "%d", tm_info->tm_min);
    strcat(filename1, tempbuf2);
    strcat(filename1, ":");
    snprintf(tempbuf3, 5, "%d", tm_info->tm_sec);
    strcat(filename1, tempbuf3);
    
    timing_file.open(filename1);
    for (i=0; i<ClientNum; i++){
        timing_file << fixed <<"t23_"<<i+1<<"	";
    }
    timing_file << fixed <<"t34	"<<"Ts-t34	"<<"Ts	";
    timing_file << fixed <<endl;

    	char* buffer = (char*)malloc(512*sizeof(char));


  	int sockfd = 0;
   	struct sockaddr_in serv_addr;       
    	memset(&serv_addr, '0', sizeof(serv_addr));   
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(atoi(argv[3])); 
   	if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0){
        	printf("\n inet_pton error occured\n");
        	return 1;
    	  } 
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)   {
            printf("\n Error : Could not create socket \n");
            return 1;
        } 
        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)   {
            perror("\n Error : Connect Failed \n");
            return 1;
        } else {
             printf("\n TCP Connection has been set up\n");
             }


    int sockfd2 = 0;
    	memset(&serv_addr, '0', sizeof(serv_addr));   
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(atoi(argv[5])); 
   	if(inet_pton(AF_INET, argv[4], &serv_addr.sin_addr)<=0){
        	printf("\n inet_pton error occured\n");
        	return 1;
    	  } 
	if((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)   {
            printf("\n Error : Could not create socket \n");
            return 1;
        } 
        if(connect(sockfd2, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)   {
            perror("\n Error : Connect Failed \n");
            return 1;
        } else {
             printf("\n TCP Connection has been set up\n");
             }




	int CountClient;
	unsigned Iteration = 0;
    pthread_t handler_thread[ClientNum];
	string list[ClientNum];
	char * thread_result = (char *)malloc(512);;
	void * exit_status;
	mat localpara(StateNum,1);
	double error[ParaNum];
	double sum, cal_error;
	char *pht;
	double origeigen_imag[3];
	mat new_state(StateNum,1);
	mat state(StateNum,1);
	double X[100][4];
	origeigen_imag[0] = 3.124981;
	origeigen_imag[1] = 5.56238;
	origeigen_imag[2] = 6.095;

		mat I(ParaNum, ParaNum);
	mat a(ParaNum,1);
	mat new_a(ParaNum,1);
	mat avgpara(ParaNum,1);
	mat new_avgpara(ParaNum,1);
	mat w(ParaNum,1);
	mat new_w(ParaNum,1);
	mat new_u(100,1);
mat A;
A<< 0 << 1 << 0 << 0 << endr

 << 0 << 0 << 1 << 0 << endr

 << 0 << 0 << 0 << 1 << endr

 << -11 << -13 << -7 << -5 << endr;
// mat K;
// K<< -0.690317123957517 << -0.106246553169458 << -0.264402702740854 << -0.231411470022186 << endr

 // << -0.688833331469347 << -0.639185742868010 << 0.336001900276136 << -0.222040305849917 << endr

 // << -0.39511273857506 << -0.384582377448378 << -0.160369625184352 << 0.489819382216583 << endr

 // << -7.07545482350762 << -8.29914505287652 << -4.43994094846219 << -3.18019648899664 << endr;
mat K;
K<< 1.39670676256686 << 1.06457368054630 << 0.279398699553787 << -0.0724950705518260 << endr

 << 0.973327190849847 << 1.86224023841152 << 0.675299304689265 << -0.0396929916821485 << endr

 << 0.162416818702149 << 0.528850579378536 << 0.983535620834140 << 0.0842224178894165 << endr

 << -0.159125624319031 << -0.154600784389643 << 0.0125375426152317 << 0.0797297011928824 << endr;
	int root_count;

	// Initialize avg vectors
	for (i=0; i<ParaNum; i++){
		avgpara[i] = 0;
		new_avgpara[i] = 0;
	    }
	for (i=0; i<StateNum; i++){
		state(i,0) = 0.5;
		new_state(i,0) = 0.5;
		localpara(i,0) =0;
	    }

	// Prepare sending message
	char Buffer[DEFAULT_MAX_BUFFER_LEN];
 	char* avgBuffer = (char*)malloc(512*sizeof(char));
   	int size, avgBufferLen;
	struct timeval tvalStart, currenttime;

	//Set up TCP socket of server side for collection local parameters for each client
	int Server_sockfd;
	struct sockaddr_in LocalAddr;

	int Client_sockfd[ClientNum];
	struct sockaddr_in Client_address;
	int Client_len = sizeof(Client_address);
	// collecting IP addresses of clients from TCP connection for latter UDP connection
	vector<sockaddr_in> client_list;   
	

	/* Initiate local TCP server socket */
	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	LocalAddr.sin_port = htons(atoi(argv[6]));

	/* Create, bind and listen the TCP_socket */
	Server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (bind(Server_sockfd, (struct sockaddr *)&LocalAddr, sizeof(LocalAddr))==-1) {
		perror("Error: Can not bind the TCP socket! \n Please wait a few seconds or change port number.\n");
		exit(EXIT_FAILURE);
	}
	listen(Server_sockfd, DEFAULT_QUEUE_LEN);

	// Calculate roots
	typedef complex<double> dcomp;
   	dcomp z[PoleNum];
   	dcomp root_est_c[PoleNum];

   	int Degree = PoleNum;
   	double op[MDP1], zeroi[MAXDEGREE], zeror[MAXDEGREE]; // Coefficient vectors
   	int index; // vector index
	Mutex_initialization();
	
	// ========================================= The below is the loop =========================================// 
   gettimeofday (&Tstart, NULL);     
   while(Iteration < SamNum){

	//========================Step1: Collect the parameters from all clients===================//
	//cout<<"Step1: start collecting localpara ..."<<endl;
	gettimeofday (&Start, NULL); 
	CountClient = 0;
	
	while(CountClient < ClientNum) {
                        
		gettimeofday (&currenttime, NULL);
		cout<<"Current time is "<<currenttime.tv_sec*1000000 + currenttime.tv_usec<<endl;

		// Accept connection		
		if(Iteration == 0){
			Client_sockfd[CountClient] = accept(Server_sockfd,(struct sockaddr *)&Client_address, (socklen_t*)&Client_len);
			cout<<"Client_sockfd is "<<Client_sockfd[CountClient]<<endl;		
		   }
							   
		// Handle connection
		// pthread_create(&handler_thread[CountClient], NULL, Server_handle, (void*) &Client_sockfd[CountClient]);
                CountClient++;             
	}
        
		for (i=0; i<4; i++){
		        if (i==0){
        			avgBufferLen = read(sockfd, avgBuffer, DEFAULT_MAX_BUFFER_LEN);

					pht = strtok(avgBuffer, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					new_avgpara(i,0) = strtod(pht, NULL);
					cout<<"new_avgpara 1 ("<<i<<") is "<<new_avgpara(i,0)<<endl;

					for (CountClient = 0; CountClient < 2; CountClient++){
					    size = sprintf(buffer, "Iteration: %d, ini_state: %f, \r\n Starttime: %ld \r\n\r\n", Iteration, new_avgpara(i,0), (SendingTime.tv_sec*1000000 + SendingTime.tv_usec));	
					    write(Client_sockfd[CountClient], buffer, size); 
					}
					write(sockfd2, buffer, size);     
    	// 			write(Client_sockfd[i], avgBuffer, size); 
					// printf( "Prony_Server Write Client 1:\n %s \n", avgBuffer);
        		}

        		if (i==1){
        			avgBufferLen = read(sockfd2, avgBuffer, DEFAULT_MAX_BUFFER_LEN);
					pht = strtok(avgBuffer, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					new_avgpara(i,0) = strtod(pht, NULL);
					cout<<"new_avgpara 2 ("<<i<<") is "<<new_avgpara(i,0)<<endl; 
        		}

        		if (i==2){
        			avgBufferLen = read(Client_sockfd[i-2], avgBuffer, DEFAULT_MAX_BUFFER_LEN);
					pht = strtok(avgBuffer, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					new_avgpara(i,0) = strtod(pht, NULL);
					cout<<"new_avgpara 3 ("<<i<<") is "<<new_avgpara(i,0)<<endl;        		}

        		if (i==3){
        			avgBufferLen = read(Client_sockfd[i-2], avgBuffer, DEFAULT_MAX_BUFFER_LEN);
					pht = strtok(avgBuffer, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					pht = strtok(NULL, " ");
					new_avgpara(i,0) = strtod(pht, NULL);
					cout<<"new_avgpara 4 ("<<i<<") is "<<new_avgpara(i,0)<<endl;  
        		}
        	}
	

	// printf( "Prony_Server Write:\n %s \n", avgBuffer);    

	// for (i=0; i<ClientNum; i++){
	// 	write(Client_sockfd[i], avgBuffer, size); 
	//     }

        // Wait for all threads finish and return the values
	// for (CountClient = 0; CountClient < ClientNum; CountClient++){
	// 	pthread_join(handler_thread[CountClient], &exit_status);
	// 	thread_result = (char *)exit_status;
	// 	list[CountClient] = thread_result;
	// 	//printf("I got paras %s from the thread %d. \n", list[CountClient].c_str(), CountClient);		
	//     }	
	cout << "All threads completed."<<endl ;

	new_u(Iteration,0)=-(K(1,1)*new_avgpara(0,0)+ K(1,0)*new_avgpara(1,0) + K(1,2)*new_avgpara(2,0) + K(1,3)*new_avgpara(3,0));
    cout<<"new_u ("<<Iteration<<") is "<<new_u(Iteration,0)<<endl;

    size = sprintf(buffer, "Iteration: %d, ini_state: %f, \r\n Starttime: %ld \r\n\r\n", Iteration, new_u[Iteration] , (SendingTime.tv_sec*1000000 + SendingTime.tv_usec));	
    write(sockfd, buffer, size); 

	gettimeofday (&t34_start, NULL); 


	
// 	for (i=0; i<ClientNum; i++){
// 		strcpy(Buffer, list[i].c_str());
// 		pht = strtok(Buffer, " ");
// 		pht = strtok(NULL, " ");
// 		Iteration = strtol(pht, NULL, 10); 
// 		pht = strtok(NULL, " ");
// //		printf("First word in para line is %s\n", pht);

// 			// pht = strtok(NULL, " ");
// 			 localpara(i,0)= strtod(pht, NULL);
// 			 cout<<"local para ["<<i<<"] "<<localpara[i]<<endl;
		    
//         // pht = strtok(NULL, " ");
//         // pht = strtok(NULL, " ");
//         // t23[i] = strtol(pht, NULL, 10);
// 	    }
	
	//========================== Step2: Average collected parameters =====================//
    new_state=A*new_state+localpara;
    cout<<"Iteration ["<<Iteration<<"] "<<endl;
    for (i=0; i<4; i++){
    cout<<"new_state ["<<i<<"] "<<new_state(i,0)<<endl;
	}
	// cout<<"Step2: Average collected parameters ..."<<endl;
	// cal_error = 0;
	// for (j=0; j<ParaNum; j++){
	// 	//1. update old avgpara
	// 	avgpara[j] = new_avgpara[j];
	// 	//2. calculate new_avgpara
	// 	sum = 0;
	// 	for (i=0; i<ClientNum; i++){
	// 		sum = sum + localpara[i][j];
	// 	    }	
	// 	new_avgpara[j] = sum/ClientNum;
	// 	cout<<"avgpara ["<<j<<"] is "<<new_avgpara[j]<<endl;
	// 	//3. Calculate the squared error
	// 	error[j] = (new_avgpara[j] - avgpara[j])*(new_avgpara[j] - avgpara[j]);
	// 	//4. max error
	// 	if (error[j] > cal_error) cal_error =  error[j];	
	//     }	
	// cout<<"Cal_error is "<<cal_error<<endl;

	// if (cal_error <= ERROR) {
	// 	//send out the finish package on TCP connection
	// 	size = sprintf(avgBuffer, "Finish! \r\n\r\n");	
 //        	printf( "Error Server Write:\n %s \n", avgBuffer);        
	// 	for (i=0; i<ClientNum; i++){
	// 		write(Client_sockfd[i], avgBuffer, size); 
	//     	    }
	// 	break;
	//    }


	// Check the roots of S-function
		//Compute the poles of the polynomial equation of Z function
   	
   	//Input the polynomial coefficients from the file and put them in the op vector
   	// op[0] = 1;
   	// for (index = 1; index < (Degree+1); index++){
    //     	op[index] = (-1)*new_avgpara[index-1];
    // 	    }

   	// rpoly_ak1(op, &Degree, zeror, zeroi);
   	// cout.precision(DBL_DIG);
/*
   	//cout << "The Z domain roots of Discentralized Prony Algorithm follow:\n";
  	//cout << "\n";
   	for (index = 0; index < Degree; index++){
        	//cout << zeror[index] << " + " << zeroi[index] << "i" << " \n";  
            }
   	cout << "\n";
*/
   	//cout << "Poles of S function: \n";

	//Compute the poles of the polynomial equation of S function
	// root_count = 0;
 // 	for (index = 0; index < Degree; index++){
 //        	z[index] = dcomp(zeror[index], zeroi[index]);
 //        	root_est_c[index] = log(z[index])/Ts; 
	// 	for (i=0; i<3; i++){
	// 		if (abs(abs(root_est_c[index].imag()) - origeigen_imag[i]) < imagError) root_count++;
	// 	   }
		
 //            }
	// cout<<"The root_count number is "<<root_count<<endl;

	// if (root_count == 6) {
	// 	//send out the finish package on TCP connection
	// 	size = sprintf(avgBuffer, "Finish! \r\n\r\n");
 //        	printf( "Count Server Write:\n %d \n", avgBuffer);        
	// 	for (i=0; i<ClientNum; i++){
	// 		write(Client_sockfd[i], avgBuffer, size); 
	//     	    }
	// 	break;
	//    }

	//======================== Step3: Broadcast the average parameters ==============//
        
	// cout<<"Step3: Broadcast avgpara ..."<<endl;  
	// // Record sending time
	 gettimeofday (&tvalStart, NULL);

	// //1: Format the export message
 //   	size = sprintf(avgBuffer, "Iteration: %d, new_avgpara: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, ƒ%f\r\n Starttime: %ld \r\n\r\n", Iteration, new_avgpara[0], new_avgpara[1], new_avgpara[2], new_avgpara[3], new_avgpara[4], new_avgpara[5], new_avgpara[6], new_avgpara[7], new_avgpara[8], new_avgpara[9], new_avgpara[10], new_avgpara[11], new_avgpara[12], new_avgpara[13], new_avgpara[14], new_avgpara[15], new_avgpara[16], new_avgpara[17], new_avgpara[18], new_avgpara[19], (tvalStart.tv_sec*1000000 + tvalStart.tv_usec));
	
	// //printf( "Prony_Server Write:\n %s \n", avgBuffer);    

	// for (i=0; i<ClientNum; i++){
	// 	write(Client_sockfd[i], avgBuffer, size); 
	//     }
	 gettimeofday (&t34_end, NULL); 
	 timer_sub(&t34_start, &t34_end, &t34_result);
	 timer_sub(&Start, &t34_end, &Result);
    // for (i=0; i<ClientNum; i++){
    //     timing_file << fixed <<t23[i]<<"    ";
    // }
	//timing_file << fixed <<t34_result.tv_sec*1000000+t34_result.tv_usec<<"	"<<Result.tv_sec*1000000+Result.tv_usec-t34_result.tv_sec*1000000-t34_result.tv_usec<<"	"<<Result.tv_sec*1000000+Result.tv_usec<<endl;
	for (i=0; i<4; i++){
        X[Iteration][i] =new_state(i,0);
    }
    Iteration=Iteration+1; 

	cout<<"========================= This is end of Iteration "<<Iteration<<" . ^_^ ========================= "<<endl;
    } // end big loop
		
	Mutex_destroy();
	close(Server_sockfd);
	cout<<"Prony algorithm ends happily.  ^_^"<<endl;
	for (i=0; i<100; i++){
        cout<<X[i]<<endl;
    }  
	gettimeofday (&Tend, NULL); 
	timer_sub(&Tstart, &Tend, &Tresult);
	timing_file << fixed <<"Total algorithm convergency time is "<<Tresult.tv_sec*1000000+Tresult.tv_usec<<endl;

	// ========================================= Print out the roots =========================================// 
	// for (index = 0; index < Degree; index++){
	// 	if ((abs(root_est_c[index].imag())>3 && abs(root_est_c[index].imag())<4) || (abs(root_est_c[index].imag())>5 && abs(root_est_c[index].imag())<7)){
	// 		cout<<root_est_c[index]<<endl;
	// 	   }
 //            }

	
	
	return 0;

}


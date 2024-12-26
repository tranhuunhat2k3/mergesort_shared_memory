#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>
#include <chrono> //To estimate time
void merge(int a[], int l1, int h1, int h2);
//Merge sort function
void mergeSort(int a[], int l, int h)
{
  if (l<h)
  {
    int mid = (l+h)/2;
    //Call recursive to sort left part and right part
    mergeSort(a,l,mid);
    mergeSort(a,mid+1,h);
    merge(a,l,mid,h);
  }
}
void parallelMergeSort(int a[], int l, int h)
{
    int mid = (l+h) /2;
    
    pid_t leftPid = fork();
    if (leftPid < 0)
    {
        //Lchild proc not created
        perror("Left Child Proc. not created\n");
        exit(1);
    }
    else if (leftPid == 0)
    {
        mergeSort(a,l, mid);
        exit(0);
    }
    pid_t rightPid = fork();
    if (rightPid < 0)
    {
        //Lchild proc not created
        perror("Left Child Proc. not created\n");
        exit(1);
    }
    else if (rightPid == 0)
    {
        mergeSort(a,mid +1, h);
        exit(0);
    }
      
      //Wait for child processes to finish
      waitpid(leftPid,NULL,0);
      waitpid(rightPid,NULL,0);
      
      //Merge the sorted subarrays
      merge(a,l,mid,h);
}

  //Method to merge sorted subarrays
void merge(int a[], int l1, int h1, int h2)
{
    //We can directly copy the sorted elements
    // int the final array, no need for a temporary
    // sorted array
    int count = h2-l1+1;
    int sorted[count];
    int i = l1, k= h1+1, m=0;
    while (i<=h1 && k <= h2)
    {
      if (a[i] < a[k])
        sorted[m++] = a[i++];
      else 
        sorted[m++] = a[k++];
    }
    while (i <= h1)
      sorted[m++] = a[i++];
    while (k <= h2)
      sorted[m++] = a[k++];
    
    for (i=0;i<count; i++,l1++)
      a[l1] = sorted[i];
  }
//To check if array is actually sorted or not
void isSorted(int arr[], int len)
{
  for (int i=1;i<len;i++)
  {
    if (arr[i] < arr[i-1])
    {
      std::cout<<"Sorting Not Done" <<std::endl;
      return;
    }
  }
  std::cout<<"Sorting Done Successfully"<<std::endl;
}
  
//To fill random values in arry for testing purpose
void fillData(int a[],int len)
{
    //Create random arrays
    int i;
    for (i=0;i<len;i++)
    {
      a[i] = rand();
    }
    return;
}

//Driver code
int main()
{
    int shmid;
    key_t key = ftok("sortfile",66);
    int *shm_array;
    
    int length;
    printf("Number of elements: ");
    scanf("%d",&length);
    printf("Array length: [%d]\n",length);
    //Caculate segment length
    size_t SHM_SIZE = sizeof(int) * length;
    
    //Create the segment.
    if ((shmid = shmget(key,SHM_SIZE,IPC_CREAT | 0666)) <0)
    {
        perror("shmget");
        _exit(1);
    }
    
    //Now we attach the segment to our data space.
    if ((shm_array = (int *)shmat(shmid,NULL,0)) == (int *)-1)
    {
      perror("shmat");
      _exit(1);
    }
    
    //Create a random array of given length
    srand(time(NULL));
    fillData(shm_array,length);
    
    //Create a copy array to test with two method
    int *copy_array = new int[length];
    std::copy(shm_array,shm_array+length,copy_array);
    
    //Estimate time single
    auto start_single = std::chrono::high_resolution_clock::now();
    mergeSort(copy_array,0,length-1);
    auto end_single = std::chrono::high_resolution_clock::now();
    
    isSorted(copy_array,length);
    
    //Print time sort single process
    auto duration_single = std::chrono::duration_cast<std::chrono::microseconds>(end_single-start_single);
    std::cout<<"Single sorting time: "<<duration_single.count()<<" microseconds"<<std::endl;
    
    //Estimate time parallel
    auto start_parallel = std::chrono::high_resolution_clock::now();
    parallelMergeSort(copy_array,0,length-1);
    auto end_parallel = std::chrono::high_resolution_clock::now();
    
    isSorted(copy_array,length);
    
    //Print time sort parallel
    auto duration_parallel = std::chrono::duration_cast<std::chrono::microseconds>(end_parallel-start_parallel);
    std::cout<<"Parallel sorting time: "<<duration_parallel.count()<<" microseconds"<<std::endl;
    
    delete[] copy_array;
    
    /*Detach from the shared memory now that we are done using it. */
    if (shmdt(shm_array) == -1)
    {
      perror("shmdt");
      _exit(1);
    }
    
    /* Delete the shared memory segment. */
    if (shmctl(shmid,IPC_RMID,NULL) == -1)
    {
      perror("shmctl");
      _exit(1);
    }
    return 0;
}

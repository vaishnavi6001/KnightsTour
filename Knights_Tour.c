/* Do not make any changes in the skeleton. Your submission will be invalidated if the skeleton is changed */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#define MAX_THREADS 100

// can be made local if required
int N;
int dx[]={2, 2, -2, -2, 1, 1, -1, -1}, dy[]={1, -1, 1, -1, 2, -2, 2, -2};

// Helper struct, can utilize any other custom structs if needed
typedef struct{
    int x,y;
} pair;

typedef struct {
    int x, y, c, ret;
    pair *path;
    int *trav;
}thread_args;

// Not necessary to use this function as long as same printing pattern is followed
void print_path(pair path[], int n){
    for(int i=0;i<n;i++){
        printf("%d,%d|",path[i].x, path[i].y);
    }
}

// void print_path1(pair path[], int n){
//     FILE *fp;
//     fp = fopen("output3.txt", "a");
//     //fprintf(fp, "[%d: %d] ", getppid(), getpid());
//     for(int i=0;i<n;i++){
//      fprintf(fp, "%d,%d|",path[i].x,path[i].y);
//  }
//     fprintf(fp, "\n");
//     fclose(fp);
// }

int getDegree(int *trav, int x, int y){
    int count = 0;
    for (int i = 0; i < 8; ++i) {
        int x1 = x + dx[i];
        int y1 = y + dy[i];
        if (x1 >= 0 && x1 < N && y1 >= 0 && y1 < N && trav[x1 + y1 * N] == 0)
            count++;
    }
    return count;
}

int tcount = 0;
//clock_t t1;
pthread_mutex_t lock;
int flag = 1;
pthread_t threads[MAX_THREADS];
int tret[MAX_THREADS];
thread_args *my_args;

void *path_complete(void *args){
    thread_args *params = (thread_args*) args;
    if(params->c >= N*N){                   //if counter is greater than N*N, we got a path
        pthread_mutex_lock(&lock);          // mutex for locking
        //printf("in print\n");
        if(flag){                           //flag for not letting any 2nd/later completed thread print the path
            flag = 0;
            print_path(params->path, N * N);
            // clock_t t2 = clock() - t1;
            // double time_taken = ((double)t2) / CLOCKS_PER_SEC;     // in seconds
            //printf("\n program took %f seconds to execute \n", time_taken);
            params->ret = 1;
        }
        pthread_mutex_unlock(&lock);
        exit(0);
        return NULL;
    }
    int x = params->x, y = params->y;
    int deg[8];
    int min_deg = 8;
    for (int i1 = 0; i1 < 8; i1++) {
        int x1 = x + dx[i1];
        int y1 = y + dy[i1];
        deg[i1] = 100;
        if (x1 >= 0 && x1 < N && y1 >= 0 && y1 < N && params->trav[x1 + y1 * N] == 0) {
            deg[i1] = getDegree(params->trav, x1, y1);
            if(deg[i1]<min_deg) {
                min_deg = deg[i1];
            }
        }
    }
   
    // int level_count = 0;
    // for(int i = 0; i < 8; i++) if(deg[i] == min_deg){
    //     int x1 = x + dx[i];
    //     int y1 = y + dy[i];
    //     if(x1>=0 && x1<N && y1>=0 && y1<N && params->trav[x1+N*y1]==0) {
    //         level_count++;
    //     }
    // }
   
    // int i = -1, sum = 100;
    // int j;
    // if(level_count > 1)
    // for(int m=0; m<8; m++){
    //     if(deg[m]==min_deg) {
    //         int s1 = 0, f = 0;
    //         for(int p=0; p<8; p++){
    //             if(x+dx[m]+dx[p]>=0 && x+dx[m]+dx[p]<N && y+dy[m]+dy[p]>=0 && y+dy[m]+dy[p]<N && params->trav[x+dx[m]+dx[p]+N*(y+dy[m]+dy[p])]==0)
    //                 {
    //                     s1 += getDegree(params->trav, x+dx[m]+dx[p], y+dy[m]+dy[p]);
    //                     f = 1;
    //                 }
    //         }
    //         if( s1 < sum && f == 1){
    //             sum = s1;
    //             i = m;
    //         }
    //     }
    // }

    //int iii = i;
    for(int i = 0; i < 8; i++) if(deg[i] == min_deg){
        //if (iii != -1)  i = iii;
        int x1 = x + dx[i];
        int y1 = y + dy[i];
        int do_backtrack = 0;
        //printf("test3\n");
        if(x1>=0 && x1<N && y1>=0 && y1<N && params->trav[x1+N*y1]==0) {
            //printf("%dt", params->trav[x1+N*y1]);
            params->x = x1;
            params->y = y1;
            params->trav[x1+N*y1] = 1;
            params->path[params->c].x = x1;
            params->path[params->c].y = y1;
            params->c++;
            do_backtrack = 1;
            //printf("xx\n");
            if(tcount < N){
                printf("making new thread");
                my_args[tcount].x = x1;
                my_args[tcount].y = y1;
                my_args[tcount].c = params->c;
                for(int a = 0; a < N*N; a++){
                    my_args[tcount].trav[a] = params->trav[a];
                    my_args[tcount].path[a].x = params->path[a].x;
                    my_args[tcount].path[a].y = params->path[a].y;
                }
                tcount++;
                if(pthread_create(&threads[tcount-1], NULL, path_complete, &my_args[tcount-1])){
                    printf("thread error\n");
                }
            }
            else{
                path_complete(params);
            }
            
        }
        if(do_backtrack){
            params->trav[x1+N*y1] = 0;
            params->x = x;
            params->y = y;
            params->c--;
            //break;
        }
        // if (iii != -1)
        //     break;
    }
    params->ret = 0;
    return NULL;

}

int helper(int StartX, int StartY)
{
    //Find all sorroundings.
    int ret = 0;
    int trav1[N*N];
    for(int i=0; i<N*N; i++){
        trav1[i] = 0;
    }
    trav1[StartX + N*StartY] = 1;
    int deg[8];
    int min_deg = 8;
    for (int i1 = 0; i1 < 8; i1++) {
        int x1 = StartX + dx[i1];
        int y1 = StartY + dy[i1];
        deg[i1] = 100;
        if (x1 >= 0 && x1 < N && y1 >= 0 && y1 < N) {
            deg[i1] = getDegree(trav1, x1, y1);
            if(deg[i1]<min_deg) {
                min_deg = deg[i1];
            }
        }
    }
   
    for (int i = 0; i < 8; i++) if(deg[i] == min_deg ) {
        int x1 = StartX + dx[i];
        int y1 = StartY + dy[i];
        if (x1 >= 0 && x1 < N && y1 >= 0 && y1 < N) {
            trav1[x1 + y1*N] = 1;
            int deg1[8];
            int min_deg1 = 8;
            for (int t1 = 0; t1 < 8; t1++) {
                int x12 = x1 + dx[t1];
                int y12 = y1 + dy[t1];
                deg1[t1] = 100;
                if (x12 >= 0 && x12 < N && y12 >= 0 && y12 < N && x12 != StartX && y12 != StartY) {
                    deg1[t1] = getDegree(trav1, x12, y12);
                    if(deg1[t1]<min_deg1) {
                        min_deg1 = deg1[t1];
                    }
                }
            }

            for(int j=0; j<8; j++) if(deg1[j] == min_deg1){
                tret[i+j*8] = 0;
                int x2 = x1 + dx[j];
                int y2 = y1 + dy[j];
                if(x2 >= 0 && x2 < N && y2 >= 0 && y2 < N && x2 != StartX && y2 != StartY) {
                    for (int l = 0; l < N; l++) {
                        for (int m = 0; m < N; m++) {
                            my_args[i+j*8].trav[l + N*m] = 0;
                        }
                    }
                    my_args[i+j*8].path[0].x = StartX;
                    my_args[i+j*8].path[0].y = StartY;
                    my_args[i+j*8].trav[StartX + StartY * N] = 1;

                    my_args[i+j*8].path[1].x = x1;
                    my_args[i+j*8].path[1].y = y1;
                    my_args[i+j*8].trav[x1 + y1 * N] = 1;

                    my_args[i+j*8].path[2].x = x2;
                    my_args[i+j*8].path[2].y = y2;
                    my_args[i+j*8].trav[x2 + y2 * N] = 1;
                    my_args[i+j*8].c = 3;
                    my_args[i+j*8].x = x2;
                    my_args[i+j*8].y = y2;
                    if(pthread_create(&threads[tcount++], NULL, path_complete, &my_args[i+j*8])){
                        printf("thread error\n");
                    }
                }
            }
            trav1[x1 + y1*N] = 0;
        }
    }
    printf("Thread count: %d\n", tcount);
    for (int j = 0; j < tcount; j++){
        pthread_join(threads[j], NULL);
    }
    for(int j=0; j<MAX_THREADS; j++){
        free(my_args[j].path);
        my_args[j].path = NULL;
        free(my_args[j].trav);
        my_args[j].trav = NULL;
    }
    for (int j = 0; j < MAX_THREADS; j++)
        ret = ret | my_args[j].ret;

    return ret;
}

int main(int argc, char *argv[]) {
    // clock_t t;
    // t = clock();
   
    if (argc != 4) {
        printf("Usage: ./Knight.out grid_size StartX StartY");
        exit(-1);
    }
   
    N = atoi(argv[1]);
    int StartX=atoi(argv[2]);
    int StartY=atoi(argv[3]);

    /* Do your thing here */
    my_args = (thread_args *)malloc(sizeof(thread_args)*MAX_THREADS);
    for (int i = 0; i < MAX_THREADS; i++)
        tret[i] = 0;

    for(int i=0; i<MAX_THREADS; i++){
        my_args[i].ret = 0;
        my_args[i].trav = (int *)malloc(sizeof(int)*(N*N));
        my_args[i].path = (pair *)malloc(sizeof(pair)*(N*N));
    }

    int ret = helper(StartX, StartY);
   
    if (ret == 0) {
        printf("No Possible Tour");
    }
    
    // t = clock() - t;
    // double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
 
    // printf("program took %f seconds to execute \n", time_taken);
    return 0;
}
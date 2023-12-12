// đây là bài tập phát sinh từ bài tập 12, làm phần nhỏ rồi gộp lại
// Bài tập như sau, nhập dữ liệu từ bàn phím (họ tên, ngày sinh, quê quán) rồi ghi vào file dulieu.txt
// Đọc dữ liệu ghi vào ra màn hình
//giờ phải làm cho nó quay lại thread1
/**********************************/
/**********************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inputBackToThread1 = PTHREAD_COND_INITIALIZER;

char buffer[100];
//biến bên dưới được tạo để nhằm mục đích lấy điều kiện cho vòng lặp
int inputReady = 1; 


//Thread1: Nhập dữ liệu từ bàn phím
static void *handle_th1(void *args)
{
    /*ban đầu khi inputBackToThread1 = 1, sẽ bỏ qua vòng lặp -> thực hiện câu lệnh tiếp theo rồi qua thread2
    khi thread3 chạy xong giá trị sẽ là inputBackToThread1 = 2, thực hiện câu lệnh trong vòng lặp -> kích thích*/ 
    while(inputReady != 1){ // nhận kích thích từ thread3, sau khi thread3 chạy xong, inputReady sẽ khác 1, thực hiện vòng lặp
    pthread_cond_wait(&inputBackToThread1 , &lock1);                /*hiện tại khi quay lại đang bị block tại đây*/
    }
    
    pthread_mutex_lock(&lock1); 

    //nhập dữ liệu rồi lưu vào mảng buffer
    printf("bien o thread1:%d\n", inputReady);
    printf("Nhap Ho ten, ngay sinh, que quan ! \nVD: Nguyen Quang Dat, sinh ngay 21, Quang Tri\n");
    if (scanf("%99[^\n]", buffer) != 1) {
    printf("Error reading input\n");
    }

    inputReady = 1; //chạy xong thread1 thì inputReady = 1 -> kết thúc vòng lặp trên
    sleep(2);
    pthread_mutex_unlock(&lock1);
    pthread_exit(NULL);
}

//Thread2: Ghi thông tin vào file dulieu.txt
static void *handle_th2(void *args)
{
    pthread_mutex_lock(&lock2);

    //Mở tệp để đọc t 
    int fd;
    fd = open("dulieu.txt", O_RDWR | O_CREAT , 0666);
    if (fd == -1) {
        perror("Error opening file\n");
        // return 1;
    } else{
        printf("Open successful\n");
    }
    printf("bien o thread2 %d\n", inputReady);
    
    //bây giờ ghi dữ liệu vào tệp
    ssize_t result = write(fd , buffer , 50);
    if (result == -1) {
        perror("Error writing to file\n");
        close(fd);
    } else {
        printf("Write successful, %zd bytes written\n", result);
    }
    close (fd);
    sleep(5);
    pthread_mutex_unlock(&lock2);
    pthread_exit(NULL);
    printf("bien o thread2 %d\n", inputReady);
}

//Đọc dữ liệu vừa ghi được và in ra màn hình, sau đó thông báo cho Thread 1 tiếp tục nhập
static void *handle_th3(void *args)
{
    pthread_mutex_lock(&lock3);
    printf("Thông tin vừa nhập:\n%s\n", buffer);
    printf("bien o thread3: %d\n", inputReady);
    pthread_cond_signal(&inputBackToThread1);
    inputReady = 2;
    pthread_mutex_unlock(&lock3);
    pthread_exit(NULL);
}

int main (void){
    
    int rel;
    pthread_t thread_id1, thread_id2, thread_id3;
    while(1){
    printf ("Chuong trinh cua Dat\n");
    
    // while(1)
    // {
        if(rel = pthread_create(&thread_id1 , NULL, &handle_th1, NULL)){
        printf("pthread_create() error number = %d\n", rel);
        return -1;
        }
        pthread_join(thread_id1, NULL);
    
        if(rel = pthread_create(&thread_id2 , NULL, &handle_th2, NULL)){
        printf("pthread_create() error number = %d\n", rel);
        return -1;
        }
        pthread_join(thread_id2, NULL);
    
        if(rel = pthread_create(&thread_id3 , NULL, &handle_th3, NULL)){
        printf("pthread_create() error number = %d\n", rel);
        return -1;
        }    
        pthread_join(thread_id3, NULL);
    // }
    }
    
    return 0;
}
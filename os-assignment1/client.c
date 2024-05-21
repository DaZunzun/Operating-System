#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#define BUFFER_SIZE 20  // 缓冲区大小为20个槽
#define PRODUCER_COUNT 3  // 生产者线程数

int buffer[BUFFER_SIZE];  // 共享缓冲区
int fill_ptr = 0;  // 缓冲区的填充指针
int use_ptr = 0;  // 缓冲区的使用指针
int count = 0;  // 缓冲区中的当前元素数量

// 线程同步使用的互斥锁和条件变量
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

// 向缓冲区中放入一个元素
void put(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % BUFFER_SIZE;
    count++;
}

// 从缓冲区中取出一个元素
int get() {
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % BUFFER_SIZE;
    count--;
    return tmp;
}

// 生成符合指数分布的随机延迟
double rand_exp(double lambda) {
    double u = rand() / (RAND_MAX + 1.0);
    return -log(1 - u) / lambda;
}

// 生产者线程函数
void *producer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        int value = rand() % 100;
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE)  // 等待缓冲区有空间
            pthread_cond_wait(&empty, &mutex);
        put(value);
        printf("Producer %d produced %d\n", id, value);
        pthread_cond_signal(&fill);  // 通知可能等待的管道线程
        pthread_mutex_unlock(&mutex);
        usleep((useconds_t)(rand_exp(1.0) * 1000000));  // 按指数分布随机延迟
    }
    return NULL;
}

// 管道线程函数
void *pipe_thread(void *arg) {
    char *fifo = "/tmp/myfifo";  // 管道文件路径
    mkfifo(fifo, 0666);  // 创建命名管道
    int fd = open(fifo, O_WRONLY);  // 打开管道文件进行写操作
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0)  // 等待缓冲区有数据
            pthread_cond_wait(&fill, &mutex);
        int data = get();
        write(fd, &data, sizeof(data));  // 写数据到管道
        pthread_cond_signal(&empty);  // 通知可能等待的生产者线程
        pthread_mutex_unlock(&mutex);
    }
    close(fd);
    return NULL;
}

// 主函数
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <lambda_c>\n", argv[0]);
        return 1;
    }

    double lambda_c = atof(argv[1]);  // 从命令行读取λc值

    pthread_t producers[PRODUCER_COUNT], p_thread;
    int ids[PRODUCER_COUNT];
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        ids[i] = i;
        double delay = -log((1.0 - (double)rand() / RAND_MAX)) / lambda_c;
        int sleep_time = (int)(delay * 1000000);  // 转换延迟为微秒
        usleep(sleep_time);  // 休眠以模拟生产速率
        pthread_create(&producers[i], NULL, producer, &ids[i]);
    }
    pthread_create(&p_thread, NULL, pipe_thread, NULL);

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(p_thread, NULL);

    return 0;
}


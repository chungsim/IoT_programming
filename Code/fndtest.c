#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define FND_DEVICE "/dev/fnd"

int main() {
    unsigned char test_data[4] = { 0xC0, 0x98, 0xD8, 0xB0 }; // �׽�Ʈ ������ (1234)

    int fndFd = open(FND_DEVICE, O_RDWR);
    if (fndFd < 0) {
        perror("FND ����̽� ���� ����");
        printf("errno: %d\n", errno);
        return -1;
    }
    else {
        printf("FND ����̽� ���� ����, ���� ��ũ����: %d\n", fndFd);
    }

    ssize_t written = write(fndFd, test_data, sizeof(test_data));
    if (written < 0) {
        perror("FND ������ ���� ����");
        printf("errno: %d\n", errno);
    }
    else {
        printf("FND ������ ���� ����, %zd ����Ʈ\n", written);
    }

    int close_result = close(fndFd);
    if (close_result < 0) {
        perror("FND ����̽� �ݱ� ����");
        printf("errno: %d\n", errno);
    }
    else {
        printf("FND ����̽� �ݱ� ����\n");
    }

    return 0;
}

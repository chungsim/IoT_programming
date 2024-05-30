#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// ����̽� ��� ����
#define fnd "/dev/fnd"
#define tact "/dev/tactsw"
#define led "/dev/led"
#define clcd "/dev/clcd"

// �Լ� �����
void init_devices();
void close_devices();
void test_devices();
int read_tact_switch();
void print_fnd(int number);

// ���� ����
int dipsw;
int leds;
int dot_mtx;
int tactsw;
int clcds;
int fnds;

// ����̽� �ʱ�ȭ �Լ�
void init_devices() {
    if ((leds = open(led, O_RDWR)) < 0 ||
        (tactsw = open(tact, O_RDWR)) < 0 ||
        (clcds = open(clcd, O_RDWR)) < 0 ||
        (fnds = open(fnd, O_RDWR)) < 0) {
        printf("����̽� ���� ����\n");
        exit(0);
    }
    printf("��� ����̽��� ���������� ���Ƚ��ϴ�.\n");
}

void close_devices() {
    close(leds);
    close(tactsw);
    close(clcds);
    close(fnds);
    printf("��� ����̽��� ���������� �������ϴ�.\n");
}

// Tact ����ġ �б� �Լ�
int read_tact_switch() {
    unsigned char b;
    read(tactsw, &b, sizeof(b));
    return b;
}

// FND ��� �Լ�
void print_fnd(int number) {
    unsigned char fnd_data[4];
    unsigned char fnd_codes[10] = { 0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xD8, 0x80, 0x90 };

    fnd_data[0] = fnd_codes[number / 1000 % 10];
    fnd_data[1] = fnd_codes[number / 100 % 10];
    fnd_data[2] = fnd_codes[number / 10 % 10];
    fnd_data[3] = fnd_codes[number % 10];

    write(fnds, fnd_data, sizeof(fnd_data));
}

// ����̽� �׽�Ʈ �Լ�
void test_devices() {
    // ������ �׽�Ʈ�� ���� CLCD�� �޽��� ���
    write(clcds, "Tact ����ġ�� ��������", 30);

    // Tact ����ġ ���¸� �о� FND�� ǥ��
    int tact_value;
    while (1) {
        tact_value = read_tact_switch();
        if (tact_value > 0 && tact_value <= 9) {
            print_fnd(tact_value);
            break;
        }
    }
}

int main() {
    init_devices();

    // �׽�Ʈ �Լ� ȣ��
    test_devices();

    // ��� �ð� ���� (5��)
    sleep(5);

    close_devices();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define fnd "/dev/fnd"
#define dot "/dev/dot"
#define tact "/dev/tactsw"
#define led "/dev/led"
#define clcd "/dev/clcd"

// ���� ���
#define MAX_ROUNDS 2
#define MAX_TRIES 10
#define INIT_SCORE 1000

// ��ġ ���� ��ũ����
int fnd_fd, dot_fd, tact_fd, led_fd, clcd_fd;
int player_scores[2] = { INIT_SCORE, INIT_SCORE };

// �Լ� ����
void initialize_devices();
void close_devices();
void initialize_game();
void show_message(const char* msg);
void clear_clcd();
void show_score(int player);
void show_dot_matrix(const unsigned char* data);
int read_tact_switch();
void blink_led(int led_num);
int get_number_input();
void play_round(int round_num, int num_digits);
int check_guess(const char* guess, const char* answer);
void get_secret_number(char* secret, int num_digits, int player);

// ��Ʈ ��Ʈ���� ����
unsigned char BASEBALL_ICON[] = { 0x7E, 0x81, 0xA5, 0xDB, 0x81, 0xA5, 0x99, 0x7E };
unsigned char STRIKE_ICON[] = { 0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x3C };
unsigned char OUT_ICON[] = { 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C };
unsigned char HOME_RUN_ICON[] = { 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42 };
unsigned char BALL_ICON[] = { 0x1E, 0x22, 0x22, 0x1E, 0x22, 0x22, 0x22, 0x1E };

int main() {
    initialize_devices();
    initialize_game();

    for (int round = 1; round <= MAX_ROUNDS; ++round) {
        play_round(round, round + 2); // 1����: 3�ڸ� ����, 2����: 4�ڸ� ����
    }

    close_devices();
    return 0;
}

void initialize_devices() {
    fnd_fd = open(fnd, O_RDWR);
    dot_fd = open(dot, O_RDWR);
    tact_fd = open(tact, O_RDWR);
    led_fd = open(led, O_RDWR);
    clcd_fd = open(clcd, O_RDWR);

    if (fnd_fd < 0 || dot_fd < 0 || tact_fd < 0 || led_fd < 0 || clcd_fd < 0) {
        printf("��ġ�� �� �� �����ϴ�\n");
        exit(1);
    }

    clear_clcd();
    show_message("���� �߱� ���ӿ� ���� ���� ȯ���մϴ�!");
}

void close_devices() {
    close(fnd_fd);
    close(dot_fd);
    close(tact_fd);
    close(led_fd);
    close(clcd_fd);
}

void initialize_game() {
    // ��� ��ġ�� �ʱ�ȭ�Ͽ� �⺻ ���·� ����
    // CLCD, ��Ʈ ��Ʈ����, FND�� �ʱ�ȭ�ϰ� ��� LED�� ��
    clear_clcd();
    show_message("�ʱ�ȭ ��...");
    show_dot_matrix(BASEBALL_ICON);
    usleep(1000000);
}

void show_message(const char* msg) {
    clear_clcd();
    write(clcd_fd, msg, strlen(msg));
}

void clear_clcd() {
    char clear_msg[] = "                "; // 16���� �� ĭ
    write(clcd_fd, clear_msg, sizeof(clear_msg) - 1);
}

void show_score(int player) {
    char score_msg[32];
    sprintf(score_msg, "P%d ����: %d", player + 1, player_scores[player]);
    show_message(score_msg);
}

void show_dot_matrix(const unsigned char* data) {
    write(dot_fd, data, 8);
}

int read_tact_switch() {
    unsigned char b;
    read(tact_fd, &b, sizeof(b));
    return b;
}

void blink_led(int led_num) {
    unsigned char data = (1 << led_num);
    write(led_fd, &data, sizeof(data));
    usleep(500000);
    data = 0;
    write(led_fd, &data, sizeof(data));
}

int get_number_input() {
    int number = 0;
    while (1) {
        int key = read_tact_switch();
        if (key >= 1 && key <= 9) {
            number = number * 10 + key;
            if (number >= 1000) break; // �ִ� 4�ڸ� ���� �Է�
        }
        else if (key == 12) {
            break; // �Է� �Ϸ�
        }
    }
    return number;
}

void get_secret_number(char* secret, int num_digits, int player) {
    show_message("��� ��ȣ�� �Է��ϼ���:");
    int secret_num = get_number_input();
    snprintf(secret, num_digits + 1, "%0*d", num_digits, secret_num);
    printf("Player %d ��� ��ȣ: %s\n", player + 1, secret); // ����� ���
}

void play_round(int round_num, int num_digits) {
    char secrets[2][5]; // �� �÷��̾��� ��� ��ȣ�� ����

    // �� �÷��̾ ��� ��ȣ ����
    for (int player = 0; player < 2; ++player) {
        get_secret_number(secrets[player], num_digits, player);
    }

    for (int try = 1; try <= MAX_TRIES; ++try) {
        for (int player = 0; player < 2; ++player) {
            show_message("���ڸ� �Է��ϼ���:");
            int player_guess = get_number_input();
            char guess[5];
            snprintf(guess, sizeof(guess), "%0*d", num_digits, player_guess);

            int strikes, balls;
            int result = check_guess(guess, secrets[1 - player]);

            if (result == num_digits * 10) {
                show_message("Ȩ��!");
                blink_led(3); // Ȩ�� �� �Ķ��� LED
                player_scores[player] += 100;
                break;
            }
            else {
                // ��Ʈ����ũ�� �� ���
                strikes = result / 10;
                balls = result % 10;
                if (strikes > 0) {
                    show_message("��Ʈ����ũ!");
                    show_dot_matrix(STRIKE_ICON);
                    blink_led(1); // ��Ʈ����ũ �� �ʷϻ� LED
                }
                else if (balls > 0) {
                    show_message("��!");
                    show_dot_matrix(BALL_ICON);
                    blink_led(2); // �� �� ����� LED
                }
                else {
                    show_message("�ƿ�!");
                    show_dot_matrix(OUT_ICON);
                    blink_led(0); // �ƿ� �� ������ LED
                }
                player_scores[player] -= 10;
            }

            show_score(player);
            usleep(2000000); // ���� �õ� �� ���
        }
    }
}

int check_guess(const char* guess, const char* answer) {
    int strikes = 0, balls = 0;
    for (int i = 0; i < strlen(guess); ++i) {
        if (guess[i] == answer[i]) {
            strikes++;
        }
        else if (strchr(answer, guess[i])) {
            balls++;
        }
    }
    return strikes * 10 + balls;
}

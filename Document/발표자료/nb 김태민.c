#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

// ����̽� ��� ����
#define fnd "/dev/fnd"
#define dot "/dev/dot"
#define tact "/dev/tactsw"
#define led "/dev/led"
#define dip "/dev/dipsw"
#define clcd "/dev/clcd"

// �Լ� �����
void print_clcd(const char* message);
void writeToDotDevice(unsigned char* data, int time);
int tactsw_get_with_timer(int t_second);
int dipsw_get_with_timer(int t_second);
void led_on(int strikes, int balls, int outs, int homerun);
void init_devices();
void game_rule(int round);
void start_game();
void input_number(char* number, int digits);
void check_guess(const char* guess, const char* secret, int length, int* strikes, int* balls);
void display_score(int player1_score, int player2_score);
void print_game_start();
void print_round_start(int round);
void print_input_number();
void print_result(int strikes, int balls, int outs, int homerun);
void print_final_score(int player1_score, int player2_score);
void print_game_over();
int intro();
int is_valid_number(const char* number, int length);

// ���� ����
int dipsw;
int leds;
int dot_mtx;
int tactsw;
int clcds;
int fnds;
unsigned char fnd_data[4];

// Dot Matrix ����
unsigned char patterns[4][8] = {
    {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x3C}, // STRIKE
    {0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C}, // OUT
    {0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42}, // HOME RUN
    {0x1E, 0x22, 0x22, 0x1E, 0x22, 0x22, 0x22, 0x1E}  // BALL
};

// Character LCD �Լ�
void print_clcd(const char* message) {
    clcds = open(clcd, O_RDWR);
    if (clcds < 0) {
        printf("Character LCD ���� ����.\n");
        exit(0);
    }
    write(clcds, message, strlen(message));
    close(clcds);
}

void print_game_start() {
    print_clcd("Game Start!");
    usleep(2000000);  // 2�� ���
}

void print_round_start(int round) {
    char buffer[32];
    sprintf(buffer, "Round %d Start!", round);
    print_clcd(buffer);
    usleep(2000000);  // 2�� ���
}

void print_result(int strikes, int balls, int outs, int homerun) {
    char buffer[32];
    sprintf(buffer, "S:%d B:%d O:%d H:%d", strikes, balls, outs, homerun);
    print_clcd(buffer);
    usleep(2000000);  // 2�� ���
}

void print_final_score(int player1_score, int player2_score) {
    char buffer[32];
    sprintf(buffer, "P1: %d P2: %d", player1_score, player2_score);
    print_clcd(buffer);
    usleep(2000000);  // 2�� ���
}

void print_game_over() {
    print_clcd("Game Over");
    usleep(2000000);  // 2�� ���
}

// Dot Matrix �Լ�
void writeToDotDevice(unsigned char* data, int time) {
    dot_mtx = open(dot, O_RDWR);
    if (dot_mtx < 0) {
        printf("Dot ����̽� ���� ����\n");
        exit(0);
    }
    write(dot_mtx, data, 8);
    usleep(time);
    close(dot_mtx);
}

// TACT Switch �Լ�
int tactsw_get_with_timer(int t_second) {
    int tactswFd, selected_tact = 0;
    unsigned char b = 0;

    tactswFd = open(tact, O_RDONLY);
    if (tactswFd < 0) {
        perror("TACT ����ġ ����̽� ���� ����");
        return -1;
    }

    while (t_second > 0) {
        usleep(100000); // 100ms ���
        read(tactswFd, &b, sizeof(b));
        if (b) {
            selected_tact = b;
            close(tactswFd);
            return selected_tact;
        }
        t_second -= 0.1;
    }

    close(tactswFd);
    return 0; // Ÿ�Ӿƿ�
}

// DIP Switch �Լ�
int dipsw_get_with_timer(int t_second) {
    int dipswFd, selected_dip = 0;
    unsigned char d = 0;

    dipswFd = open(dip, O_RDONLY);
    if (dipswFd < 0) {
        perror("DIP ����ġ ����̽� ���� ����");
        return -1;
    }

    while (t_second > 0) {
        usleep(100000); // 100ms ���
        read(dipswFd, &d, sizeof(d));
        if (d) {
            selected_dip = d;
            close(dipswFd);
            return selected_dip;
        }
        t_second -= 0.1;
    }

    close(dipswFd);
    return 0; // Ÿ�Ӿƿ�
}

// LED ���� �Լ�
void led_on(int strikes, int balls, int outs, int homerun) {
    unsigned char led_data = 0;
    if (strikes > 0) led_data |= 0x22; // Green LEDs
    if (balls > 0) led_data |= 0x44;   // Yellow LEDs
    if (outs > 0) led_data |= 0x11;    // Red LEDs
    if (homerun > 0) led_data |= 0x88; // Blue LEDs
    leds = open(led, O_RDWR);
    if (leds < 0) {
        printf("LED ���� ����.\n");
        exit(0);
    }
    write(leds, &led_data, sizeof(unsigned char));
    close(leds);
}

// ����̽� �ʱ�ȭ �Լ�
void init_devices() {
    dipsw = open(dip, O_RDWR);
    leds = open(led, O_RDWR);
    dot_mtx = open(dot, O_RDWR);
    tactsw = open(tact, O_RDWR);
    clcds = open(clcd, O_RDWR);
    fnds = open(fnd, O_RDWR);
    if (dipsw < 0 || leds < 0 || dot_mtx < 0 || tactsw < 0 || clcds < 0 || fnds < 0) {
        printf("����̽� ���� ����\n");
        exit(0);
    }
    close(dipsw);
    close(leds);
    close(dot_mtx);
    close(tactsw);
    close(clcds);
    close(fnds);
}

// ���� ��Ģ ��� �Լ�
void game_rule(int round) {
    if (round == 1) {
        print_clcd("Round 1, 3 digits");
    }
    else if (round == 2) {
        print_clcd("Round 2, 4 digits");
    }
    usleep(3000000);
}

// ���� ��ȿ�� �˻� �Լ�
int is_valid_number(const char* number, int length) {
    if (strlen(number) != length) return 0; // ������ ���̰� �ƴ�
    for (int i = 0; i < length; i++) {
        for (int j = i + 1; j < length; j++) {
            if (number[i] == number[j]) {
                return 0; // �ߺ��� ���� �߰�
            }
        }
    }
    return 1; // ��ȿ�� �Է�
}

// ����� ���� �˻� �� ��Ʈ����ũ, �� ���
void check_guess(const char* guess, const char* secret, int length, int* strikes, int* balls) {
    *strikes = 0;
    *balls = 0;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            if (guess[i] == secret[j]) {
                if (i == j)
                    (*strikes)++;
                else
                    (*balls)++;
            }
        }
    }
}

// FND ��� �Լ�
void PrintFnd(const char* number) {
    unsigned char fnd_codes[10] = { 0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xD8, 0x80, 0x98 };

    // FND �����͸� �ʱ�ȭ�մϴ�.
    for (int i = 0; i < 4; i++) {
        if (i < strlen(number)) {
            fnd_data[i] = fnd_codes[number[i] - '0'];
        }
        else {
            fnd_data[i] = 0xFF;
        }
    }

    int fndFd = open(fnd, O_RDWR);
    if (fndFd < 0) {
        perror("FND ����̽� ���� ����");
        return;
    }
    write(fndFd, fnd_data, sizeof(fnd_data));
    close(fndFd);
}

void ResetFnd() {
    // FND �����͸� �ʱ�ȭ�մϴ�.
    fnd_data[0] = 0xFF;
    fnd_data[1] = 0xFF;
    fnd_data[2] = 0xFF;
    fnd_data[3] = 0xFF;

    int fndFd = open(fnd, O_RDWR);
    if (fndFd < 0) {
        perror("FND ����̽� ���� ����");
        return;
    }
    write(fndFd, fnd_data, sizeof(fnd_data));
    close(fndFd);
}

// ���� ǥ�� �Լ�
void display_score(int player1_score, int player2_score) {
    char score[32];
    sprintf(score, "P1: %d, P2: %d", player1_score, player2_score);
    print_clcd(score);
    usleep(5000000); // 5�� ���
}

// ���� �Է� �Լ�
void input_number(char* number, int digits) {
    int tactsw_value;
    int idx = 0;

    while (idx < digits) {
        tactsw_value = tactsw_get_with_timer(100);
        if (tactsw_value >= 1 && tactsw_value <= 9) {
            number[idx++] = '0' + tactsw_value;
            PrintFnd(number); // �Է� ���� ���ڸ� FND�� ǥ��
        }
    }
    number[digits] = '\0';
}

// ���� ���� �Լ�
void start_game() {
    char secret_number1[5]; // �÷��̾� 1�� ��� ���� ���� (�ִ� 4�ڸ�)
    char secret_number2[5]; // �÷��̾� 2�� ��� ���� ����
    char guess[5];          // ���� ���� (�ִ� 4�ڸ�)
    int strikes, balls;
    int turn = 1;
    int score1 = 1000, score2 = 1000; // ���� ���� ����
    int rounds = 2;                   // �� ���� ��
    int digits[2] = { 3, 4 };         // �� ������ �ڸ���

    for (int round = 0; round < rounds; round++) {
        int current_digits = digits[round];
        game_rule(round + 1);

        // �÷��̾� 1�� 2�� ��� ���� ����
        for (int player = 1; player <= 2; player++) {
            while (1) {
                print_clcd(player == 1 ? "P1 Set Number:" : "P2 Set Number:");
                input_number(player == 1 ? secret_number1 : secret_number2, current_digits);
                if (!is_valid_number(player == 1 ? secret_number1 : secret_number2, current_digits)) {
                    print_clcd("Invalid Number!");
                    usleep(2000000); // 2�� ���
                }
                else {
                    break; // ��ȿ�� �Է�
                }
            }
        }

        // ���� ����
        int home_run1 = 0, home_run2 = 0;
        while (home_run1 == 0 || home_run2 == 0) {
            if ((turn == 1 && home_run1) || (turn == 2 && home_run2)) {
                turn = turn == 1 ? 2 : 1; // Ȩ���� ģ �÷��̾�� �ǳʶٱ�
                continue;
            }

            while (1) {
                print_clcd(turn == 1 ? "P1 Guess:" : "P2 Guess:");
                input_number(guess, current_digits);
                if (!is_valid_number(guess, current_digits)) {
                    print_clcd("Invalid Guess!");
                    usleep(2000000); // 2�� ���
                }
                else {
                    break; // ��ȿ�� �Է�
                }
            }

            if (turn == 1) {
                check_guess(guess, secret_number2, current_digits, &strikes, &balls);
                if (strikes == current_digits) home_run1 = 1;
                score1 -= (10 * (current_digits - strikes) + balls); // ���� ����
            }
            else {
                check_guess(guess, secret_number1, current_digits, &strikes, &balls);
                if (strikes == current_digits) home_run2 = 1;
                score2 -= (10 * (current_digits - strikes) + balls);
            }

            int outs = current_digits - (strikes + balls); // �ƿ� ���
            print_result(strikes, balls, outs, 0); // Ȩ���� ������ ǥ��
            led_on(strikes, balls, outs, (strikes == current_digits));

            if (strikes == current_digits) {
                writeToDotDevice(patterns[2], 2000000); // Ȩ�� ���� ǥ��
            }
            else {
                if (strikes > 0) {
                    writeToDotDevice(patterns[0], 2000000); // ��Ʈ����ũ ���� ǥ��
                }
                if (balls > 0) {
                    writeToDotDevice(patterns[3], 2000000); // �� ���� ǥ��
                }
                if (outs > 0) {
                    writeToDotDevice(patterns[1], 2000000); // �ƿ� ���� ǥ��
                }
            }

            // �� ����
            turn = turn == 1 ? 2 : 1;
        }

        display_score(score1, score2);
    }

    print_final_score(score1, score2);
    char final_scores[5];
    sprintf(final_scores, "%04d", score1);
    PrintFnd(final_scores);
}

// ���� �Ұ� �Լ�
int intro() {
    int dip_value = 0;

    print_clcd("DIP UP to Start");
    dip_value = dipsw_get_with_timer(100);  // 10�ʷ� ����
    return dip_value;
}

// ���� �Լ�
int main() {
    init_devices();
    if (intro() != 0) {
        print_game_start();
        start_game();
        print_game_over();
    }
    else {
        print_clcd("Game Not Started");
        usleep(2000000);
    }
    return 0;
}

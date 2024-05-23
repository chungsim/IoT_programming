// Header File
#include<stdio.h>          		// ����� ���� 
#include<stdlib.h>         		// ���ڿ� ��ȯ, �޸� ���� 
#include<unistd.h>       		// POSIX �ü�� API�� ���� �׼��� ���� 
#include<fcntl.h>			// Ÿ�ٽý��� ����� ��ġ ���� 
#include<sys/types.h>    		// �ý��ۿ��� ����ϴ� �ڷ��� ���� 
#include<sys/ioctl.h>    		// �ϵ������ ����� ���� ���� 
#include<sys/stat.h>     		// ������ ���¿� ���� ���� 
#include <string.h>       		// ���ڿ� ó�� 
#include <time.h>         		// �ð� ���� 


// Target System
#define fnd "/dev/fnd"			// 7-Segment FND 
#define dot "/dev/dot"			// Dot Matrix
#define tact "/dev/tactsw"    		// Tact Switch
#define led "/dev/led"			// LED 
#define dip "/dev/dipsw"		// Dip Switch
#define clcd "/dev/clcd"		// Character LCD


// �Լ� �����

// ������Լ�

// ������ �Է¹޾� Character LCD �� ����ϴ� �Լ� 
void print(char P[]);

// ����� ī��� ��� �ð��� �Է� �޾� ��½ð���ŭ Dot Matrix LED �� ������ִ� �Լ�
void writeToDotDevice(int card, int time);

// ���� �ð��� �Է� �޾� �Էµ� ���� �ð� ���� Dip Switch �� �Է°��� �а� ���ÿ� 1 �� �������� 7-Segment LED �� ���� ���ѽð��� ǥ�����ִ� �Լ� 
int dipsw_get_with_timer(int t_second);

// ���� �ð��� �Է� �޾� �Էµ� ���� �ð� ���� Tact Switch �� �Է°��� �а� ���� ���� �����̳� ��Ʈ �׸��� ��Ģ ��� ���� ó���� ���ִ� �Լ�
int tactsw_get_with_timer(int t_second);

// ���尡 ������ ȣ��Ǹ�, �÷��̾��� �¸� �� ��ŭ Chip LED �� �������ִ� �Լ� 
void led_on(int user_score);


// ���� ���� �� �Լ� 

// ��Ʈ�� �޽����� ���� ������ְ� Dip Switch �Է°��� �̿��� �������� �ʾ��� ��� ���ӽ��ۿ��θ� �ٽ� �� �� ���� ������ �������� ���θ� �����ϴ� �Լ�
int intro_key();

// ��Ʈ�� �޽����� �Է¹޾� Character LCD �� ����ϰ� Dip Switch�� �Է��� ������ �Է°��� ��ȯ���ִ� �Լ� 
int intro(char p[]);

// ī�带 ���� �˰����� ����Ǿ� �ִ� �Լ�
void shuffle_card(int start, int* cards);

// ���� �Լ����� ȣ��Ǹ�, ���� ���� �� ��ǻ�Ϳ� �÷��̾��� ī�带 ���� ���� shuffle_card �Լ��� ȣ�����ִ� �Լ�
void prepare(int* cards1, int* cards2);

// ������ ��Ģ�� Character LCD �� ������ִ� �Լ�
void game_rule();



// ���� ���� �Լ�

// ���� �Լ�
int main();

//���� ��ü ������ �������ִ� �Լ���, ��ǻ�Ϳ� �÷��̾��� ī��迭�� �Է� ���� 
void start(int* cards1, int* cards2);

//������ ����ϴ� �Լ� 
int betting_start(int com_card, int round, int* cards2);

//�迭 ���� �Լ���, ī�� �迭�� ������ �κ��� ������ ��ġ�� �Է¹޾� �������� ���� 
void ascending(int arr[], int cnt);

//��Ʈ ��� �Լ�, �÷��̾��� �Է� ���� user_answer �� �÷��̾��� ī��迭, ���� ���带 �Է¹޾� user_answer �� �´� ��Ʈ�� Dot Matrix LED ��  ��� 
void hint(int user_answer, int* user_card, int round);

//��ǻ�Ϳ� �÷��̾��� ī�� �� �Լ� 
//�� ���庰�� ��ǻ�Ϳ� �÷��̾��� ī�带 �Է� �ް� �񱳸� �� �� ���� ��ȯ.
int compare_card(int com_card, int user_card);

//���� ����� Ȯ���ϴ� �Լ�
//�÷��̾ �Է��� ����� compare_card �Լ����� ������ ���� ���� �Է��� ����� compare_card �Լ����� ������ ���� ������ 1 �ٸ��� 0 ��ȯ 
int win_lose(int user_answer, int correct_answer);


// ���� ����
// �������ġ
int dipsw;
int leds;
int dot_mtx;
int tactsw;
int clcds;
int fnds;

// ��������� �� �� ��� �� �ϱ� ���� �� ī��Ʈ ����
int rule_count = 1;

// ��Ʈ Ƚ�� �������� ����
int hint_count[2];

// ChipLED 16���� ��
char led_array[] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00 };

//ī�� ���� ���� ���� ����
#define CARD_NUM 13
int usercards[CARD_NUM] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
int comcards[CARD_NUM] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

// fnd led ���� ���� ���� ����
unsigned char fnd_num[4] = { 0, };

//0,1,2,3,4,5,6,7,8,9,turn off
unsigned char Time_Table[11] = { ~0x3f, ~0x06, ~0x5b, ~0x4f, ~0x66, ~0x6d, ~0x7d, ~0x07, ~0x7f, ~0x67, ~0x00 };

//dot matrix�� ǥ���� Ʈ���� ī��
unsigned char deck[13][8] = {
    // Number 1 (A)
    {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42},
    // Number 2
    {0x1C, 0x22, 0x02, 0x02, 0x1C, 0x20, 0x20, 0x3E},
    // Number 3
    {0x1C, 0x22, 0x02, 0x1C, 0x02, 0x02, 0x22, 0x1C},
    // Number 4
    {0x04, 0x0C, 0x14, 0x24, 0x44, 0x3E, 0x04, 0x04},
    // Number 5
    {0x1E, 0x20, 0x20, 0x1C, 0x02, 0x02, 0x22, 0x1C},
    // Number 6
    {0x3C, 0x40, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C},
    // Number 7
    {0x3E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
    // Number 8
    {0x1C, 0x22, 0x22, 0x1C, 0x22, 0x22, 0x22, 0x1C},
    // Number 9
    {0x1C, 0x22, 0x22, 0x22, 0x1E, 0x02, 0x02, 0x02},
    // Number 10
    {0x84, 0x8A, 0x91, 0x91, 0x91, 0x91, 0x8A, 0x84},
    // Number 11 (J)
    {0x1C, 0x08, 0x08, 0x08, 0x08, 0x48, 0x48, 0x30},
    // Number 12 (Q)
    {0x38, 0x44, 0x82, 0x82, 0x82, 0x8A, 0x44, 0x3A},
    // Number 13 (K)
    {0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x44}
};

// ����� �Լ�
void print(char P[]) {
    clcds = open(clcd, O_RDWR);
    if (clcds < 0) { printf("Can't open Character LCD.\n"); exit(0); }
    write(clcds, P, strlen(P));
    close(clcds);
}

void writeToDotDevice(int card, int time) {
    int dot_mtx = open(dot, O_RDWR);
    if (dot_mtx < 0) {
        printf("Cannot open dot device\n");
        exit(0);
    }
    write(dot_mtx, &deck[card - 1], sizeof(deck[card - 1]));
    usleep(time);
    close(dot_mtx);
}

int dipsw_get_with_timer(int t_second)
{
    int selected_dip = 0;
    unsigned char d = 0;
    int dipsw;

    //dip switch ���� �ð��� 0�� ������ ��� �Է°� ���� 
    if (t_second <= 0) {
        return 0;
    }

    if ((dipsw = open(dip, O_RDWR)) < 0) {        // ����ó��    
        printf("dip open error");
        return 0;
    }
    if ((fnds = open(fnd, O_RDWR)) < 0) {
        printf("fnd open error");
        return 0;
    }

    int i, j;

    //i=10~0�ʱ��� 
    for (i = t_second; i > -1;i--) {
        for (j = 100; j > 0;j--) {
            usleep(10000); //0.01 �� ���� 
            read(dipsw, &d, sizeof(d));
            //�Է°��� 1~128 ������ �� 
            if (1 <= d && d <= 128) {
                selected_dip = d;
                //close �� fnd �ʱ�ȭ 
                int turnOff = Time_Table[10];
                fnd_num[0] = turnOff;
                fnd_num[1] = turnOff;
                fnd_num[2] = turnOff;
                fnd_num[3] = turnOff;

                write(fnds, &fnd_num, sizeof(fnd_num));
                close(dipsw);
                close(fnds);
                return selected_dip;
            }
        }
        //1�� ���� = 0.01��*100�� 
        int s = i / 10;
        int ss = i % 10;
        fnd_num[0] = Time_Table[0];
        fnd_num[1] = Time_Table[0];
        fnd_num[2] = Time_Table[s];
        fnd_num[3] = Time_Table[ss];
        write(fnds, &fnd_num, sizeof(fnd_num));
    }
    //close �� fnd �ʱ�ȭ 
    int turnOff = Time_Table[10];
    fnd_num[0] = turnOff;
    fnd_num[1] = turnOff;
    fnd_num[2] = turnOff;
    fnd_num[3] = turnOff;

    write(fnds, &fnd_num, sizeof(fnd_num));
    close(dipsw);
    close(fnds);
    return 0; //���ѽð� ��   
}

int tactsw_get_with_timer(int t_second) {
    int selected_tact = 0;
    unsigned char b = 0;
    int tactsw;

    //tact switch ���� �ð��� 0�� ������ ��� �Է°� ���� 
    if (t_second <= 0) {
        return 0;
    }

    if ((tactsw = open(tact, O_RDWR)) < 0) {        	// ����ó��    
        printf("tact open error");
        return 0;
    }

    if ((fnds = open(fnd, O_RDWR)) < 0) {                     // ����ó�� 
        printf("fnd open error");
        return 0;
    }

    int i, j;
    //i=10~0�ʱ���
    for (i = t_second; i > -1;i--) {
        for (j = 100; j > 0;j--) {
            usleep(10000); //0.01 �� ���� 
            read(tactsw, &b, sizeof(b));
            //�Է°��� 1~12 ������ ���            
            if (1 <= b && b <= 12) {
                switch (b) {
                case 1:  selected_tact = 1; break;
                case 2:  selected_tact = 2; break;
                case 3:  selected_tact = 3; break;
                case 4:  selected_tact = 4; break;
                case 5:  selected_tact = 5; break;
                case 6:  selected_tact = 6; break;
                case 12: {
                    //12������ �� ������ 1~6�� ������ ��� 
                    if (selected_tact == 1 || selected_tact == 2 || selected_tact == 3 || selected_tact == 4 || selected_tact == 5 || selected_tact == 6) {
                        //printf("tactswitch �Է°�: %d\n", selected_tact);
                        int turnOff = Time_Table[10];
                        fnd_num[0] = turnOff;
                        fnd_num[1] = turnOff;
                        fnd_num[2] = turnOff;
                        fnd_num[3] = turnOff;
                        write(fnds, &fnd_num, sizeof(fnd_num));
                        close(tactsw);
                        close(fnds);
                        return selected_tact;
                    }
                    //12�� �������� ������ 1~6�� ������ �ʾ��� ��� 
                    else {
                        printf("press 12 after  press 1 ~ 5");
                    }
                }

                       //6~11���� 
                default: {
                    printf("press other key"); break;
                }
                }
            }
        }
        //1�� ���� = 0.01��*100�� 
        int s = i / 10;
        int ss = i % 10;
        fnd_num[0] = Time_Table[0];
        fnd_num[1] = Time_Table[0];
        fnd_num[2] = Time_Table[s];
        fnd_num[3] = Time_Table[ss];
        write(fnds, &fnd_num, sizeof(fnd_num));
    }
    //close �� fnd �ʱ�ȭ 
    int turnOff = Time_Table[10];
    fnd_num[0] = turnOff;
    fnd_num[1] = turnOff;
    fnd_num[2] = turnOff;
    fnd_num[3] = turnOff;
    write(fnds, &fnd_num, sizeof(fnd_num));

    close(tactsw);
    close(fnds);
    return 0; //���ѽð� ��   
}

void led_on(int user_score) {
    unsigned char data;

    // chip led �ҷ�����
    leds = open(led, O_RDWR);
    if (leds < 0) {
        printf("Can't open LED.\n");
        exit(0);
    }

    data = led_array[user_score];

    // 5�ʵ��� ���
    write(leds, &data, sizeof(unsigned char));  usleep(5000000);

    close(leds);
}


// ���� ���� �� �Լ�
int intro_key() {
    int dip_value = 0;

    char first_msg[] = " PRESS ANY KEY!  USE DIP SWITCH ";
    char second_msg[] = " PRESS ANY KEY!  NO INPUT: QUIT ";

    //���ӽ��ۿ��� ����(ù��°  �޽�����) 
    dip_value = intro(first_msg);

    //dip switch �Է� ������ �Է°� ��ȯ 
    if (dip_value != 0) return dip_value;

    //dip switch �Է� ������ ���ӽ��ۿ��� ����(�ι�° �޽�����) 
    dip_value = intro(second_msg);

    return dip_value;
}

int intro(char P[]) {

    //clcd�� ��Ʈ�� �޽��� ��� 
    print(P);

    // dip switch 10�� ���� �Է��߳� �� �߳�
    int dip_value = 0;
    dip_value = dipsw_get_with_timer(10);
    printf("dip value: %d\n", dip_value);

    return dip_value;
}

void shuffle_card(int start, int* cards) {
    srand(time(NULL));
    int temp;
    int rn;
    int i;
    for (i = start; i < CARD_NUM; i++) {
        rn = rand() % CARD_NUM;
        while (rn == i) {
            rn = rand() % CARD_NUM;
        }
        temp = cards[i];
        cards[i] = cards[rn];
        cards[rn] = temp;
    }
}

void prepare(int* cards1, int* cards2) {
    shuffle_card(0, cards1);
    shuffle_card(0, cards2);
    shuffle_card(0, cards2); // �ٸ��� ���̱� ���� �� �� �� ����
}

void game_rule() {
    print("  INDIAN POKER     GAME  RULE   ");  usleep(1500000);
    print("     ON THE       TACT  SWITCH  ");  usleep(1500000);
    print("1ST, 2ND, 3RD IS BETTING BUTTON ");  usleep(1500000);
    print("   1ST BUTTON     PLAYER = COM  ");  usleep(1500000);
    print("   2ND BUTTON     PLAYER < COM  ");  usleep(1500000);
    print("   3RD BUTTON     PLAYER > COM  ");  usleep(1500000);
    print("  4TH, 5TH  IS    HINT  BUTTON  ");  usleep(1500000);
    print("  THE  HINT IS    GIVEN  TWICE  ");  usleep(1500000);
    print("   4TH BUTTON   SHOW UNUSED CARD");  usleep(1500000);
    print("   5TH BUTTON    SHOW USED CARD ");  usleep(1500000);
    print("  12TH  BUTTON       CHOOSE     ");  usleep(1500000);
}



// ���� ���� �Լ�
void start(int* cards1, int* cards2) {
    int ROUND = 13;
    int com_score = 0;
    int user_score = 0;
    char round_clcd[32];
    char score_clcd[32];

    print("      GAME           START!     ");  usleep(1500000);

    // ù ���� ��� Game Rule ���
    if (rule_count >= 1) {
        // Game Rule ����
        game_rule();

        rule_count = rule_count - 1;
    }

    // Round �ݺ�
    int i;
    for (i = 0; i < ROUND; i++) {
        // �ش� ���� ī�� ����
        int com_card = cards1[i];
        int user_card = cards2[i];

        // teraterm���� �ش� ���� ī�� Ȯ��
        printf("com_card: %d\n", com_card);
        printf("user_card: %d\n", user_card);

        // CLCD�� ���� ���
        sprintf(round_clcd, "    ROUND  %d         START!     ", i + 1);
        print(round_clcd);  usleep(2000000);

        // betting_start �Լ� ȣ���� user_answer�� ���� �� ����
        int user_answer = betting_start(com_card, i, cards2);         // ���� �� ����

        print("  BETTING DONE  CHECK  YOUR CARD");

        // ����� ī�� ����(3��)
        writeToDotDevice(user_card, 3000000);

        // ī�� �� ��� ����
        int correct_answer = compare_card(com_card, user_card);

        // ���� ��� Ȯ��
        if (win_lose(user_answer, correct_answer)) {
            user_score++;
            print("     PLAYER           WIN!      "); usleep(2000000);
        }
        else {
            com_score++;
            print("     PLAYER           LOSE      "); usleep(2000000);
        }

        // ���ھ� ������ ���ÿ� CHIP LED Ű��(5�ʷ� �����Ǿ� ����)
        sprintf(score_clcd, "PLAYER SCORE = %d COM  SCORE = %d ", user_score, com_score);
        print(score_clcd);
        led_on(user_score);

        if (user_score >= 7) {
            print("   GAME CLEAR      PLAYER WIN   ");   usleep(2000000);
            break;
        }
        if (com_score >= 7) {
            print("   GAME  OVER     PLAYER  LOSE  ");   usleep(2000000);
            break;
        }
    }
}

int betting_start(int com_card, int round, int* cards2) {
    // COM ī�� Ȯ�� ���� 
    print("   CHECK  THE       COM CARD    ");

    // COM ī�� ��� 
    writeToDotDevice(com_card, 3000000);

    while (1) {
        print(" PLEASE BETTING  USE TACTSWITCH ");

        // tactswitch ���� �Է�, fnd 10�� ����
        int user_answer = tactsw_get_with_timer(10);

        //������ �������� ��� 
        int user_bet = user_answer == 0 || user_answer == 1 || user_answer == 2 || user_answer == 3;

        //������ ��Ʈ�� ��û���� ��� 
        int user_hint = user_answer == 4 || user_answer == 5;

        int user_rule = user_answer == 6;

        //���ð� �Է½� ���ð� ��ȯ 
        if (user_bet) {
            int bet_answer = user_answer;
            return bet_answer;
        }

        //��Ʈ ��û�� ���� ������ 
        else if (user_hint) {
            //��û�� ��Ʈ 4�� �ܿ� ��Ʈ �������� �� 
            if (user_answer == 4 && hint_count[0] >= 1) {
                //��Ʈ �Լ� ȣ��
                hint(4, cards2, round);
                hint_count[0]--;
            }

            //��û�� ��Ʈ 5�� �ܿ� ��Ʈ �������� �� 
            else if (user_answer == 5 && hint_count[1] >= 1) {
                //��Ʈ �Լ� ȣ�� 
                hint(5, cards2, round);
                hint_count[1]--;
            }

            //�ܿ� ��Ʈ �������� ���� �� 
            else {
                print(" HINT COUNT = 0  CAN'T USE HINT ");   usleep(2000000);
            }
        }

        else if (user_rule) {
            game_rule();
        }
    }
}

void ascending(int arr[], int cnt) {
    int i, j, tmp = 0;
    for (i = 0; i < cnt; i++) {
        for (j = i; j < cnt; j++) {
            if (arr[i] > arr[j]) {
                tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
}

void hint(int user_answer, int* user_card, int round) {
    int hint_result[13] = {};

    // 4�� ��� �ش� ���� ī����� �Ⱦ� ī����� �� ���
    if (user_answer == 4) {
        int j = round;
        print(" DISPLAY UNUSED   PLAYER  CARD  ");

        int cnt = 0;

        // �ش� ���� ī����� ī�� �迭 ũ�⸸ŭ���� ����
        for (round; round < 13; round++) {
            hint_result[cnt] = user_card[round];
            cnt = cnt + 1;
        }

        // ��Ʈ ����Ǿ� �ִ� �迭 �������� ����
        ascending(hint_result, 13 - j);

        int k;
        for (k = 0; k < 13 - j; k++) {
            writeToDotDevice(hint_result[k], 1500000);
        }
    }

    // 5�� ��� ���ݱ��� ����� ī�带 ���
    else if (user_answer == 5) {
        int j;
        print("  DISPLAY USED    PLAYER  CARD  ");
        for (j = 0; j < round; j++) {
            int card = user_card[j];
            writeToDotDevice(card, 1500000);
        }
    }
}

int compare_card(int com_card, int user_card) {
    if (com_card == user_card) {
        return 1;
    }
    else if (com_card > user_card) {
        return 2;
    }
    else {
        return 3;
    }
}

int win_lose(int user_answer, int correct_answer) {
    if (user_answer == correct_answer) {
        return 1;
    }
    else {
        return 0;
    }
}

int main() {
    while (1) {
        if (intro_key() != 0) {
            hint_count[0] = 1;
            hint_count[1] = 1;
            prepare(usercards, comcards);
            start(usercards, comcards);
            print("    CONTINUE         GAME ?     ");   usleep(2000000);

            if (intro(" DIP SWITCH  ON NEW GAME STARTS!") == 0) {
                print("      GAME            END.      ");   usleep(2000000);
                return 0;
            }
        }
        else {
            print("      GAME            END.      "); return 0;
        }
    }
}
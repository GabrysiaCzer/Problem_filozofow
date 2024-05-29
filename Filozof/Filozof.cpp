
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define NUMBER_OF_PHILOSOPHERS 10

int x = 10;
int left_fork, right_fork;
int thinkingT, eatingT, hungryT = 0;

//Deklaracja statusu filozofow (Myslenie, Jedzenie i Glod)
enum states {
    THINKING, EATING, HUNGRY
};

enum states state[NUMBER_OF_PHILOSOPHERS];
pthread_mutex_t forks[NUMBER_OF_PHILOSOPHERS];

void sleep_time(unsigned milliseconds) {
    usleep(milliseconds * 1000);
}

void* philosopher(void* num) {
    int id = *(int*)num;
    int left = id; // id ma kopie  wartosci do kturej kierowal num
    int right = (id + 1) % x;

    while (true) {

        //Myslenie
        state[id] = THINKING;
        thinkingT = (rand() % (300 - 150 + 150) + 2000);
        sleep_time(thinkingT);

        //Glod
        state[id] = HUNGRY;
        hungryT = ((rand() % (300 - 150 + 150)) + 2000);
        sleep_time(hungryT);

        //Podniesienie widelcow
        pthread_mutex_lock(&forks[left]);
        pthread_mutex_lock(&forks[right]);
        left_fork = left;
        right_fork = right;

        //Jedzenie
        state[id] = EATING;
        eatingT = ((rand() % (300 - 150 + 150)) + 2000);
        sleep_time(eatingT);

        //Odlozenie widelcow
        pthread_mutex_unlock(&forks[left]);
        pthread_mutex_unlock(&forks[right]);
    }
}

int main(int argc, char* argv[]) {

    //pobranie ilosci filozofow
    x = atoi(argv[1]);
    if (x < 5 || x>10) {
        printf("Invalid Philosopher Number (Number of philosophes must be a integer betweem 5 to 10) ");
        exit(1);
    }



    initscr();
    cbreak();
    noecho();
    use_default_colors();
    start_color();

    //Inicjalizowanie widelcow
    for (int i = 0; i < x; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Tworzenie okienka wizualizacji
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    WINDOW* philosophers_win = newwin(yMax / 2, xMax / 2, yMax / 4, xMax / 4);

    init_pair(1, COLOR_GREEN, -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_CYAN, -1);

    // Tworzenie Filozofow
    pthread_t philosophers[x];
    int ids[x];
    for (int i = 0; i < x; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    //Wydrukowanie stanu filozofow
    while (true) {
        werase(philosophers_win);
        box(philosophers_win, 0, 0);

        for (int i = 0; i < x; i++) {
            wattron(philosophers_win, A_BOLD);
            mvwprintw(philosophers_win, 0, 2, "Dining philosophers");
            mvwprintw(philosophers_win, 2, 2, "Philosopher:");
            mvwprintw(philosophers_win, 2, yMax - 14, "State:");
            mvwprintw(philosophers_win, 2, yMax - 2, "Duration:");
            mvwprintw(philosophers_win, 2, yMax + 12, "ForkID:");
            wattroff(philosophers_win, A_BOLD);

            if (state[i] == THINKING) {
                mvwprintw(philosophers_win, i + 4, 2, "Philosopher %d", i + 1);
                mvwprintw(philosophers_win, i + 4, yMax - 2, "%d ms", thinkingT);
                wattron(philosophers_win, COLOR_PAIR(3));
                mvwprintw(philosophers_win, i + 4, yMax - 14, "Thinking");
                wattroff(philosophers_win, COLOR_PAIR(3));
            }
            else if (state[i] == EATING) {
                mvwprintw(philosophers_win, i + 4, 2, "Philosopher %d", i + 1);
                mvwprintw(philosophers_win, i + 4, yMax - 2, "%d ms", eatingT);
                mvwprintw(philosophers_win, i + 4, yMax + 12, "%d", left_fork);
                mvwprintw(philosophers_win, i + 4, yMax + 13, "%d", right_fork);
                wattron(philosophers_win, COLOR_PAIR(1));
                mvwprintw(philosophers_win, i + 4, yMax - 14, "Eating");
                wattroff(philosophers_win, COLOR_PAIR(1));
            }
            else if (state[i] == HUNGRY) {
                mvwprintw(philosophers_win, i + 4, 2, "Philosopher %d", i + 1);
                mvwprintw(philosophers_win, i + 4, yMax - 2, "%d ms", hungryT);
                wattron(philosophers_win, COLOR_PAIR(2));
                mvwprintw(philosophers_win, i + 4, yMax - 14, "Hungry");
                wattroff(philosophers_win, COLOR_PAIR(2));
            }
        }
        thinkingT = thinkingT - 330;
        eatingT = eatingT - 330;
        hungryT = hungryT - 330;
        wrefresh(philosophers_win);
        sleep_time(330);
    }

    endwin();
    return 0;
}


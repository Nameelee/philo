#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>

struct s_data;

typedef struct s_philo //개별 필로소퍼
{
	int				id;
	pthread_t		thread;
    int				left_fork;
	int				right_fork;
	struct s_data	*data; // 공유 데이터에 대한 포인터
    long			last_meal_time;
	int				meals_eaten;

}	t_philo;

typedef struct s_data //여러 스레드가 공유하는 데이터
{
	int             num_of_philos;
	long            time_to_die;
	long            time_to_eat;
	long            time_to_sleep;
	int             num_of_eats;
    int				fin_simulation; // 시뮬레이션 종료 플래그
	pthread_mutex_t	finish_mutex;        // 종료 플래그를 보호할 뮤텍스
	pthread_mutex_t	print_mutex;         // 상태 출력을 보호할 뮤텍스
	pthread_mutex_t	meal_mutex;          // 식사 관련 데이터 보호 뮤텍스

    long			start_time; // 시뮬레이션 시작 시간

    t_philo         *philos;
	pthread_mutex_t *forks;
	// 여러 스레드가 공유해야 할 데이터를 추가할 수 있습니다.
} t_data;

int	ft_init_data(t_data *data);
int	ft_atoi(const char *str);
long	ft_get_time(void);
int	ft_simul_start(t_data *data);
void	ft_print_status(t_philo *philo, char *status, int is_dead);
void	ft_usleep(long time_in_ms);
void	ft_cleanup(t_data *data);

#endif
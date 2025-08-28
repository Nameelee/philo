#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

struct s_data;

typedef struct s_philo
{
	int				id;
	pthread_t		thread;
	int				left_fork;
	int				right_fork;
	long			last_meal_time;
	int				meals_eaten;
	struct s_data	*data;
	pthread_mutex_t	meal_lock; // 철학자 개인의 식사 데이터 잠금
}	t_philo;

typedef struct s_data
{
	int				num_of_philos;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				num_of_eats;
	int				fin_simulation;
	long			start_time;
	t_philo			*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	write_lock; // 출력을 위한 잠금
	pthread_mutex_t	stop_lock;  // 시뮬레이션 종료 플래그를 위한 잠금
	pthread_mutex_t start_simulation;//Julian: simulation start mutex
	int				is_sim_start;//Julian: simulation start variable = 0
}	t_data;

// Function Prototypes
int		ft_init_data(t_data *data, int argc, char **argv);
int		ft_simul_start(t_data *data);
long	ft_get_time(void);
void	ft_usleep(long time_in_ms, t_data *data);
void	ft_print_status(t_philo *philo, char *status);
void	ft_cleanup(t_data *data);
int		ft_atoi(const char *str);
int	is_finished(t_data *data);

#endif
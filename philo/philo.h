#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>

struct s_data;

typedef struct s_philo
{
	int				id;
	pthread_t		thread;
	int				left_fork;
	int				right_fork;
	struct s_data	*data;
	long			last_meal_time;
	int				meals_eaten;
}	t_philo;

typedef struct s_data
{
	int				num_of_philos;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				num_of_eats;
	int				fin_simulation;
	pthread_mutex_t	write_mutex;
	pthread_mutex_t	meal_mutex;
	long			start_time;
	t_philo			*philos;
	pthread_mutex_t	*forks;
}	t_data;

// --- 함수 프로토타입 ---
int		ft_init_data(t_data *data);
int		ft_atoi(const char *str);
long	ft_get_time(void);
int		ft_simul_start(t_data *data);
//void	ft_print_status(t_philo *philo, char *status);
void	ft_print_status(t_philo *philo, char *status, int is_dead);
void	ft_usleep(long time_in_ms, t_data *data);
void	ft_cleanup(t_data *data);
void	*ft_philos_routine(void *arg); // 철학자 루틴 함수

#endif
// philo/init.c

#include "philo.h"

int	ft_init_data(t_data *data, int argc, char **argv)
{
	int	i;

	data->num_of_philos = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	data->num_of_eats = (argc == 6) ? ft_atoi(argv[5]) : -1;
	data->philos = malloc(sizeof(t_philo) * data->num_of_philos);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_of_philos);
	if (!data->philos || !data->forks)
		return (1);
	pthread_mutex_init(&data->write_lock, NULL);
	pthread_mutex_init(&data->stop_lock, NULL);
	// --- 추가된 부분 ---
	pthread_mutex_init(&data->start_simulation, NULL);
	data->is_sim_start = 0; // 시뮬레이션 시작 플래그 0으로 초기화
	// -------------------
	data->fin_simulation = 0;
	i = -1;
	while (++i < data->num_of_philos)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		pthread_mutex_init(&data->philos[i].meal_lock, NULL);
		data->philos[i].id = i + 1;
		data->philos[i].data = data;
		data->philos[i].meals_eaten = 0;
		data->philos[i].left_fork = i;
		data->philos[i].right_fork = (i + 1) % data->num_of_philos;
	}
	return (0);
}
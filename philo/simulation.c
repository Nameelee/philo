#include "philo.h"

// is_finished, monitor, ft_eat 함수는 이전과 동일합니다.
// ... (이전 코드 생략) ...

static int	is_finished(t_data *data)
{
	int	ret;

	pthread_mutex_lock(&data->write_mutex);
	ret = data->fin_simulation;
	pthread_mutex_unlock(&data->write_mutex);
	return (ret);
}

static void	*monitor(void *arg)
{
	t_data	*data;
	int		i;
	int		all_ate;

	data = (t_data *)arg;
	while (!is_finished(data))
	{
		all_ate = 1;
		i = -1;
		while (++i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->meal_mutex);
			if (ft_get_time() - data->philos[i].last_meal_time >= data->time_to_die)
			{
				ft_print_status(&data->philos[i], "died", 1);
				pthread_mutex_unlock(&data->meal_mutex);
				goto end_monitor;
			}
			if (data->num_of_eats != -1 && data->philos[i].meals_eaten < data->num_of_eats)
				all_ate = 0;
			pthread_mutex_unlock(&data->meal_mutex);
		}
		if (data->num_of_eats != -1 && all_ate)
		{
			pthread_mutex_lock(&data->write_mutex);
			data->fin_simulation = 1;
			pthread_mutex_unlock(&data->write_mutex);
			break ;
		}
		usleep(200);
	}
end_monitor:
	return (NULL);
}

static void	ft_eat(t_philo *philo)
{
	int first_fork;
	int second_fork;

	if (philo->left_fork < philo->right_fork)
	{
		first_fork = philo->left_fork;
		second_fork = philo->right_fork;
	}
	else
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	pthread_mutex_lock(&philo->data->forks[first_fork]);
	ft_print_status(philo, "has taken a fork", 0);
	pthread_mutex_lock(&philo->data->forks[second_fork]);
	ft_print_status(philo, "has taken a fork", 0);

	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal_time = ft_get_time();
	ft_print_status(philo, "is eating", 0);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->meal_mutex);

	ft_usleep(philo->data->time_to_eat, philo->data);

	pthread_mutex_unlock(&philo->data->forks[second_fork]);
	pthread_mutex_unlock(&philo->data->forks[first_fork]);
}


void	*ft_philos_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;

	// 👇 [수정] 철학자가 1명일 때의 예외 처리 로직 추가
	if (philo->data->num_of_philos == 1)
	{
		// 유일한 포크를 집고 죽을 때까지 기다립니다.
		pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
		ft_print_status(philo, "has taken a fork", 0);
		ft_usleep(philo->data->time_to_die, philo->data);
		// 포크를 내려놓지만, 어차피 직후에 monitor에 의해 'died'가 출력되고 종료됩니다.
		pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
		return (NULL);
	}

	if (philo->id % 2 == 0)
		ft_usleep(philo->data->time_to_eat / 2, philo->data);
	while (!is_finished(philo->data))
	{
		ft_eat(philo);
		if (is_finished(philo->data))
			break ;
		ft_print_status(philo, "is sleeping", 0);
		ft_usleep(philo->data->time_to_sleep, philo->data);
		if (is_finished(philo->data))
			break ;
		ft_print_status(philo, "is thinking", 0);
		usleep(200);
	}
	return (NULL);
}

// ft_simul_start, ft_usleep 함수는 이전과 동일합니다.
// ... (이전 코드 생략) ...

int	ft_simul_start(t_data *data)
{
	int			i;
	pthread_t	monitor_thread;

	data->start_time = ft_get_time();
	i = -1;
	while (++i < data->num_of_philos)
		data->philos[i].last_meal_time = data->start_time;
	if (pthread_create(&monitor_thread, NULL, &monitor, data) != 0)
		return (1);
	i = -1;
	while (++i < data->num_of_philos)
		if (pthread_create(&data->philos[i].thread, NULL, \
			&ft_philos_routine, &data->philos[i]) != 0)
			return (1);
	i = -1;
	while (++i < data->num_of_philos)
		if (pthread_join(data->philos[i].thread, NULL) != 0)
			return (1);
	if (pthread_join(monitor_thread, NULL) != 0)
		return (1);
	ft_cleanup(data);
	return (0);
}

void	ft_usleep(long time_in_ms, t_data *data)
{
	long	start_time;

	start_time = ft_get_time();
	while ((ft_get_time() - start_time) < time_in_ms)
	{
		if (is_finished(data))
			break ;
		usleep(200);
	}
}
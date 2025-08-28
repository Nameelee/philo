#include "philo.h"

int	is_finished(t_data *data)
{
	int	ret;

	pthread_mutex_lock(&data->stop_lock);
	ret = data->fin_simulation;
	pthread_mutex_unlock(&data->stop_lock);
	return (ret);
}

static void	ft_eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
	ft_print_status(philo, "has taken a fork");
	pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
	ft_print_status(philo, "has taken a fork");
	
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = ft_get_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
	
	ft_print_status(philo, "is eating");
	ft_usleep(philo->data->time_to_eat, philo->data);
	
	pthread_mutex_unlock(&philo->data->forks[philo->right_fork]);
	pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
}

void	*ft_philos_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	// mutex lock (simulation start)
	// while (simulation start variable == 0)
	// continue;
	// mutex unlock (sumiulation start)
	if (philo->id % 2 == 0)
		ft_usleep(1, philo->data);
	while (!is_finished(philo->data))
	{
		ft_eat(philo);
		if (is_finished(philo->data))
			break ;
		ft_print_status(philo, "is sleeping");
		ft_usleep(philo->data->time_to_sleep, philo->data);
		if (is_finished(philo->data))
			break ;
		ft_print_status(philo, "is thinking");
	}
	return (NULL);
}

static void	*monitor(void *arg)
{
	t_data	*data;
	int		i;
	int		all_ate;

	data = (t_data *)arg;
	while (!is_finished(data))
	{
		i = -1;
		all_ate = 1;
		while (++i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->philos[i].meal_lock);
			if (ft_get_time() - data->philos[i].last_meal_time > data->time_to_die)
			{
				ft_print_status(&data->philos[i], "died");
				pthread_mutex_lock(&data->stop_lock);
				data->fin_simulation = 1;
				pthread_mutex_unlock(&data->stop_lock);
				pthread_mutex_unlock(&data->philos[i].meal_lock);
				return (NULL);
			}
			if (data->num_of_eats != -1 && data->philos[i].meals_eaten < data->num_of_eats)
				all_ate = 0;
			pthread_mutex_unlock(&data->philos[i].meal_lock);
		}
		if (data->num_of_eats != -1 && all_ate)
		{
			pthread_mutex_lock(&data->stop_lock);
			data->fin_simulation = 1;
			pthread_mutex_unlock(&data->stop_lock);
		}
	}
	return (NULL);
}

int	ft_simul_start(t_data *data)
{
	int			i;
	pthread_t	monitor_thread;

	data->start_time = ft_get_time();
	i = -1;
	while (++i < data->num_of_philos)
		data->philos[i].last_meal_time = data->start_time;

		// --- 철학자가 1명일 때의 예외 처리 ---
	if (data->num_of_philos == 1)
	{
		ft_print_status(&data->philos[0], "has taken a fork");
		ft_usleep(data->time_to_die, data);
		ft_print_status(&data->philos[0], "died");
		ft_cleanup(data); // 뒷정리 후 바로 종료
		return (0);
	}
	if (pthread_create(&monitor_thread, NULL, &monitor, data) != 0)
		return (1);
	i = -1;
	while (++i < data->num_of_philos)
		if (pthread_create(&data->philos[i].thread, NULL,
				&ft_philos_routine, &data->philos[i]) != 0)
			return (1);
	// set start time
	// lock mutex (simulation start) // simulation start variable = 1;
	// unlock mutex (simulation start)
	i = -1;
	while (++i < data->num_of_philos)
		if (pthread_join(data->philos[i].thread, NULL) != 0)
			return (1);
	if (pthread_join(monitor_thread, NULL) != 0)
		return (1);
	ft_cleanup(data);
	return (0);
}
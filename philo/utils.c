#include "philo.h"

// ft_atoi 함수는 그대로 둡니다.
int	ft_atoi(const char *str)
{
	long	res = 0;
	int		sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		res = res * 10 + (*str - '0');
		str++;
	}
	return (sign * res);
}

long	ft_get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_usleep(long time_in_ms, t_data *data)
{
	long	start_time;

	start_time = ft_get_time();
	while ((ft_get_time() - start_time) < time_in_ms)
	{
		if (is_finished(data))
			break ;
		usleep(500);
	}
}

void	ft_print_status(t_philo *philo, char *status)
{
	long	timestamp;
	
	pthread_mutex_lock(&philo->data->write_lock);
	if (!is_finished(philo->data))
	{
		timestamp = ft_get_time() - philo->data->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, status);
	}
	pthread_mutex_unlock(&philo->data->write_lock);
}

void	ft_cleanup(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_destroy(&data->forks[i]);
		pthread_mutex_destroy(&data->philos[i].meal_lock);
		i++;
	}
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->stop_lock);
	free(data->philos);
	free(data->forks);
}
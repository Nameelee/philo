#include "philo.h"

static int	ft_isspace(int c)
{
	if (c == '\n' || c == '\t' || c == '\v' || c == '\f'
		|| c == '\r' || c == 32)
		return (1);
	return (0);
}

static int	ft_isdigit(int c)
{
	if ((c >= 48 && c <= 57))
		return (1);
	return (0);
}

int	ft_atoi(const char *str)
{
	size_t	i;
	size_t	is_neg;
	int		result;

	i = 0;
	is_neg = 0;
	result = 0;
	while (ft_isspace(str[i]))
		i++;
	if ((str[i] == '-' || str[i] == '+') && ft_isdigit(str[i + 1]))
	{
		if (str[i] == '-')
			is_neg = 1;
		i++;
	}
	while (ft_isdigit(str[i]))
	{
		result = result * 10 + str[i] - 48;
		i++;
	}
	if (is_neg)
		return (-result);
	return (result);
}

// 현재 시간을 밀리초(ms) 단위로 반환하는 유틸리티 함수
long	ft_get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);//사용 가능한 함수 
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

// 스레드-세이프한 상태 출력 함수
void	ft_print_status(t_philo *philo, char *status, int is_dead)
{
	long	timestamp;

	pthread_mutex_lock(&philo->data->print_mutex);
	
	pthread_mutex_lock(&philo->data->finish_mutex);
	if (!philo->data->fin_simulation || is_dead)
	{
		timestamp = ft_get_time() - philo->data->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, status);
	}
	pthread_mutex_unlock(&philo->data->finish_mutex);
	pthread_mutex_unlock(&philo->data->print_mutex);
}

// 자원 해제 함수
void	ft_cleanup(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->meal_mutex);
	pthread_mutex_destroy(&data->finish_mutex);
	free(data->philos);
	free(data->forks);
}

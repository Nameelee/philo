#include "philo.h"

// ... (ft_atoi 등 다른 유틸리티 함수는 변경 없습니다)
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

long	ft_get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_print_status(t_philo *philo, char *status, int is_dead)
{
	long	timestamp;

	pthread_mutex_lock(&philo->data->write_mutex);
	// 시뮬레이션이 아직 끝나지 않았을 때만 메시지를 출력하고 상태를 변경
	if (!philo->data->fin_simulation)
	{
		if (status && philo) // 모든 철학자가 식사를 마쳤을 때(philo=NULL)는 출력 안 함
		{
			if (is_dead && status && status[0] == 'd')
			{
				// 먼저 종료 플래그를 세워 이후 로그 interleave 방지
				philo->data->fin_simulation = 1;
				timestamp = (philo->last_meal_time + philo->data->time_to_die)
					- philo->data->start_time;
				printf("%ld %d %s\n", timestamp, philo->id, status);
			}
			else
			{
				// 종료된 후에는 어떤 상태도 출력하지 않음
				if (philo->data->fin_simulation)
				{
					pthread_mutex_unlock(&philo->data->write_mutex);
					return ;
				}
				timestamp = ft_get_time() - philo->data->start_time;
				printf("%ld %d %s\n", timestamp, philo->id, status);
			}
		}
		// 죽음이 선고되거나, 모든 철학자가 식사를 마쳤을 때 시뮬레이션을 종료
		if (is_dead && !(status && status[0] == 'd'))
			philo->data->fin_simulation = 1;
	}
	pthread_mutex_unlock(&philo->data->write_mutex);
}

void	ft_cleanup(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&data->write_mutex);
	pthread_mutex_destroy(&data->meal_mutex);
	free(data->philos);
	free(data->forks);
}
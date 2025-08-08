#include "philo.h"

static void	ft_eat(t_philo *philo)
{
	// 데드락 방지를 위해 ID가 짝수인 철학자와 홀수인 철학자의 포크 잡는 순서를 다르게 함
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
		ft_print_status(philo, "has taken a fork", 0);
		pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
		ft_print_status(philo, "has taken a fork", 0);
	}
	else
	{
		pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
		ft_print_status(philo, "has taken a fork", 0);
		pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
		ft_print_status(philo, "has taken a fork", 0);
	}
	
	ft_print_status(philo, "is eating", 0);
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal_time = ft_get_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->meal_mutex);
	
	ft_usleep(philo->data->time_to_eat);
	
	pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
	pthread_mutex_unlock(&philo->data->forks[philo->right_fork]);
}

void	*ft_philos_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;

	// 짝수 ID 철학자는 약간의 딜레이를 주어 포크 경쟁을 줄임
	if (philo->id % 2 == 0)
		ft_usleep(1);

    while (1)
	{
		pthread_mutex_lock(&philo->data->finish_mutex);
		if (philo->data->fin_simulation)
		{
			pthread_mutex_unlock(&philo->data->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->data->finish_mutex);

		ft_eat(philo);
		ft_print_status(philo, "is sleeping", 0);
		ft_usleep(philo->data->time_to_sleep);
		ft_print_status(philo, "is thinking", 0);
	}
	return (NULL);
}

// --- 죽음 및 종료 조건 감시 ---
static void	monitor_philosophers(t_data *data)
{
	int	i;
	int all_have_eaten;

	while (1)
	{
		all_have_eaten = 1;
		i = -1;
		while (++i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->meal_mutex);
			if (ft_get_time() - data->philos[i].last_meal_time > data->time_to_die)
			{
				ft_print_status(&data->philos[i], "died", 1);
				pthread_mutex_lock(&data->finish_mutex);
				data->fin_simulation = 1;
				pthread_mutex_unlock(&data->finish_mutex);
				pthread_mutex_unlock(&data->meal_mutex);
				return ;
			}
			if (data->num_of_eats != -1 && data->philos[i].meals_eaten < data->num_of_eats)
				all_have_eaten = 0;
			pthread_mutex_unlock(&data->meal_mutex);
		}
		if (data->num_of_eats != -1 && all_have_eaten)
		{
			pthread_mutex_lock(&data->finish_mutex);
			data->fin_simulation = 1;
			pthread_mutex_unlock(&data->finish_mutex);
			return ;
		}
	}
}

// 시뮬레이션을 시작하는 함수
int	ft_simul_start(t_data *data)
{
	int	i;

	// 현재 시간을 시뮬레이션 시작 시간으로 기록
	data->start_time = ft_get_time();

    // 스레드를 만들기 전에 모든 철학자의 마지막 식사 시간을 미리 초기화합니다.
	i = 0;
	while (i < data->num_of_philos)
	{
		data->philos[i].last_meal_time = data->start_time;
		i++;
	}

	// 1. 철학자 스레드 생성
	i = 0;
	while (i < data->num_of_philos)
	{
		// pthread_create를 호출하여 각 철학자 스레드를 시작시킵니다.
		// philosopher_routine 함수를 실행하도록 하고,
		// 해당 철학자의 정보(philo)를 인자로 넘겨줍니다.
		if (pthread_create(&data->philos[i].thread, NULL,
				&ft_philos_routine, &data->philos[i]) != 0)
			return (printf("Error: Failed to create a thread\n"), 1);
		i++;
	}

    monitor_philosophers(data); // 메인 스레드는 감시자 역할을 함

	// 2. 모든 스레드가 종료될 때까지 대기
	i = 0;
	while (i < data->num_of_philos)
	{
		if (pthread_join(data->philos[i].thread, NULL) != 0)
			return (printf("Error: Failed to join a thread\n"), 1);
		i++;
	}
    ft_cleanup(data);
	return (0);
}

void	ft_usleep(long time_in_ms)
{
	long	start_time;

	start_time = ft_get_time();
	while ((ft_get_time() - start_time) < time_in_ms)
		usleep(500); // CPU 부하를 줄이기 위해 짧은 대기 반복
}
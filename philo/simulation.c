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
	if (philo->data->num_of_philos == 1) // 철학자가 1명일 때 포크 하나만 들고 죽도록
	{
		ft_usleep(philo->data->time_to_die, philo->data);
		pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
		return ;
	}
	pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
	ft_print_status(philo, "has taken a fork");

	// --- 식사 상태 보호 강화 ---
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = ft_get_time();
	ft_print_status(philo, "is eating");
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
	
	ft_usleep(philo->data->time_to_eat, philo->data);

	pthread_mutex_unlock(&philo->data->forks[philo->right_fork]);
	pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
}

void	*ft_philos_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	// 홀수/짝수 전략으로 초기 교착 상태 방지
	if (philo->id % 2 == 0)
		ft_usleep(philo->data->time_to_eat / 2, philo->data);
		
	while (!is_finished(philo->data))
	{
		ft_eat(philo);
		if (philo->data->num_of_eats != -1 && philo->meals_eaten >= philo->data->num_of_eats)
			break;
		ft_print_status(philo, "is sleeping");
		ft_usleep(philo->data->time_to_sleep, philo->data);
		ft_print_status(philo, "is thinking");
	}
	return (NULL);
}

// --- 모니터 로직 대폭 수정 ---
static void	*monitor(void *arg)
{
	t_data	*data;
	int		i;
	int		all_ate_count;

	data = (t_data *)arg;
	while (1)
	{
		all_ate_count = 0;
		i = -1;
		while (++i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->philos[i].meal_lock);
			// 1. 죽음 확인
			if (ft_get_time() - data->philos[i].last_meal_time > data->time_to_die)
			{
				ft_print_status(&data->philos[i], "died");
				pthread_mutex_lock(&data->stop_lock);
				data->fin_simulation = 1;
				pthread_mutex_unlock(&data->stop_lock);
				pthread_mutex_unlock(&data->philos[i].meal_lock);
				return (NULL);
			}
			// 2. 식사 횟수 확인
			if (data->num_of_eats != -1 && data->philos[i].meals_eaten >= data->num_of_eats)
				all_ate_count++;
			pthread_mutex_unlock(&data->philos[i].meal_lock);
		}
		// 3. 모든 철학자가 다 먹었는지 확인
		if (data->num_of_eats != -1 && all_ate_count == data->num_of_philos)
		{
			pthread_mutex_lock(&data->stop_lock);
			data->fin_simulation = 1;
			pthread_mutex_unlock(&data->stop_lock);
			return (NULL);
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
	{
		data->philos[i].last_meal_time = data->start_time;
		if (pthread_create(&data->philos[i].thread, NULL,
				&ft_philos_routine, &data->philos[i]) != 0)
			return (1);
	}
	// 모니터 스레드를 철학자들보다 약간 늦게 시작하여 초기 경쟁 방지
	if (pthread_create(&monitor_thread, NULL, &monitor, data) != 0)
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
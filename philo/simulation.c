#include "philo.h"

static void	take_forks(t_philo *philo)
{
	// 데드락 방지를 위해 짝수/홀수 철학자의 포크 잡는 순서를 다르게 합니다.
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
}

// 포크를 내려놓는 함수
static void	ft_drop_forks(t_philo *philo)
{
	pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
	pthread_mutex_unlock(&philo->data->forks[philo->right_fork]);
}

// 식사하는 행동 전체를 묶은 함수
static void	ft_eat(t_philo *philo)
{
	take_forks(philo);

	// 식사 관련 정보를 업데이트합니다. (뮤텍스로 보호)
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal_time = ft_get_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->meal_mutex);

	ft_print_status(philo, "is eating", 0);
	ft_usleep(philo->data->time_to_eat, philo->data);//이게 잠자는 함수 같지만 아래 입력 인자를 보면
	//식사하는데 걸리는 시간을 입력했다. 즉 밥을 먹는 시간이다. 
	
	ft_drop_forks(philo);
}

static void ft_philo_sleep(t_philo *philo)
{
	ft_print_status(philo, "is sleeping", 0);
	ft_usleep(philo->data->time_to_sleep, philo->data);
}

static void ft_philo_think(t_philo *philo)//철학자에게 생각하는 시간을 설정이 안 되어있음
{
	ft_print_status(philo, "is thinking", 0);
	ft_usleep(1, philo->data); // 1ms의 짧은 딜레이 추가
}

void	*ft_philos_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;

	if (philo->data->num_of_philos == 1)//필로소퍼가 딱 1명일 때
	{
		pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
		ft_print_status(philo, "has taken a fork", 0);
		// 어차피 죽을 운명이므로, 죽을 때까지 기다려줍니다.
		ft_usleep(philo->data->time_to_die, philo->data);
		pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
		return (NULL);
	}
	if (philo->id % 2 == 0)//짝수 철학자는 1밀리초만큼 쉬게 해서 데드락을 방지
		ft_usleep(1, philo->data);

	while (1)
	{
		// 1. 시뮬레이션이 종료되었는지 먼저 확인합니다.
		pthread_mutex_lock(&philo->data->finish_mutex);
		if (philo->data->fin_simulation)
		{
			pthread_mutex_unlock(&philo->data->finish_mutex);
			break ; // 루프 탈출
		}
		pthread_mutex_unlock(&philo->data->finish_mutex);

		ft_eat(philo);
		ft_philo_sleep(philo);
		ft_philo_think(philo);
	}
	return (NULL);
}

/*
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
*/

// --- 죽음 및 종료 조건 감시 ---
/*static void	monitor_philosophers(t_data *data)//감시인
{
	int	i;
	int all_have_eaten;

	while (1)
	{
		all_have_eaten = 1;
		i = -1;
		while (++i < data->num_of_philos)//모든 철학자(스레드)를 한 명씩 순서대로 감시
		//체크하는 내용은 1) 굶고 있는 철학자가 있는가? 2) 식사 목표량이 채워졌는가? 
		{
			pthread_mutex_lock(&data->meal_mutex);//마지막 식사 시간을 안전하게 읽기 위해 잠금
			if (ft_get_time() - data->philos[i].last_meal_time > data->time_to_die)
			//현재 시간과 마지막 식사 시간의 차이가 생존 시간보다 큰지 체크
			{
				pthread_mutex_unlock(&data->meal_mutex);
				ft_print_status(&data->philos[i], "died", 1);
				pthread_mutex_lock(&data->finish_mutex);
				data->fin_simulation = 1;
				pthread_mutex_unlock(&data->finish_mutex);
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
		usleep(1000);//프로세스를 잠깐 쉬게 하는 것. 허용된 함수. 
	}
}
*/

static void	monitor_philosophers(t_data *data)
{
	int		i;
	int		all_are_full;

	while (1)
	{
		all_are_full = 1;
		i = -1;
		while (++i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->meal_mutex);
			if (ft_get_time() - data->philos[i].last_meal_time > data->time_to_die)
			{
				pthread_mutex_unlock(&data->meal_mutex);
				ft_print_status(&data->philos[i], "died", 1);
				return ;
			}
			if (data->num_of_eats != -1 && data->philos[i].meals_eaten < data->num_of_eats)
				all_are_full = 0;
			pthread_mutex_unlock(&data->meal_mutex);
		}
		if (data->num_of_eats != -1 && all_are_full)
		{
			pthread_mutex_lock(&data->finish_mutex);
			data->fin_simulation = 1;
			pthread_mutex_unlock(&data->finish_mutex);
			return ;
		}
		usleep(1000);
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
		if (pthread_join(data->philos[i].thread, NULL) != 0)//자식 스레드들이 종료될 때까지 기다리는 것
			return (printf("Error: Failed to join a thread\n"), 1);
		i++;
	}
    ft_cleanup(data);
	return (0);
}

/*
void	ft_usleep(long time_in_ms)
{
	long	start_time;

	start_time = ft_get_time();
	while ((ft_get_time() - start_time) < time_in_ms)
		usleep(100); // CPU 부하를 줄이기 위해 짧은 대기 반복
}
*/

void	ft_usleep(long time_in_ms, t_data *data)
{
	long	start_time;

	start_time = ft_get_time();
	while ((ft_get_time() - start_time) < time_in_ms)
	{
		pthread_mutex_lock(&data->finish_mutex);
		if (data->fin_simulation)
		{
			pthread_mutex_unlock(&data->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&data->finish_mutex);
		usleep(200); // 너무 짧으면 CPU 부하 ↑, 적당히 200us 정도
	}
}
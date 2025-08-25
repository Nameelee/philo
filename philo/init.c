# include "philo.h"

static void	ft_clean_on_error(t_data *data, int idx_fork_last)
{
	int i;

	i = 0;
	while (i < idx_fork_last)
	{
		pthread_mutex_destroy(&data->forks[i]);//허용 함수다.
		i++;
	}
    //아래 뮤텍스들은 1개씩 밖에 없기 때문에 
    //pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->write_mutex);
	pthread_mutex_destroy(&data->meal_mutex);
	//pthread_mutex_destroy(&data->finish_mutex);

	if (data->philos)
		free(data->philos);
	if (data->forks)
		free(data->forks);
	printf("Error: Initialization failed. Cleaning up.\n");
}

int	ft_init_data(t_data *data)
{
	int	i;

    i = 0;
	// 1. 철학자 스레드 배열 메모리 할당
    data->philos = malloc(sizeof(t_philo) * data->num_of_philos);
	if (!data->philos)
	{
		printf("Error: Malloc for philos failed\n");
		return (1);
	}
	// 2. 포크 뮤텍스 배열 메모리 할당
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_of_philos);
	if (!data->forks)
	{
		free(data->philos); // philos 메모리만 해제
        printf("Error: Malloc for forks failed\n");
		return (1);
	}
    //3. 공유 뮤텍스 초기화
	pthread_mutex_init(&data->write_mutex, NULL);
    //pthread_mutex_init(&data->print_mutex, NULL);
	pthread_mutex_init(&data->meal_mutex, NULL);
	//pthread_mutex_init(&data->finish_mutex, NULL);
	data->fin_simulation= 0;
	// 4. 각 포크에 대한 뮤텍스 초기화
	while (i < data->num_of_philos)
	{
        data->philos[i].id = i + 1;
		data->philos[i].data = data;
        data->philos[i].meals_eaten = 0;
		// 각 철학자가 사용할 왼쪽, 오른쪽 포크의 인덱스 설정
		data->philos[i].left_fork = i;
		data->philos[i].right_fork = (i + 1) % data->num_of_philos;
		//왜 (i + 1) 에다가 철학자의 숫자로 나눠서 그 나머지를 취하는 거냐면...
		//철학자가 5명일 경우 인덱스 4번(제일 끝) 철학자의 포크 번호는 0번이어야 하기 때문에
		//아항. 여기까지 보면 다들 고정된 포크를 사용하는구나. 
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)//허용된 함수
		{
			// 만약 초기화에 실패하면, 지금까지 생성된 뮤텍스들을 파괴해야 합니다.
			// (이 부분은 프로젝트의 에러 처리 요구사항에 따라 구현)
			ft_clean_on_error(data, i);
			return (1);
		}
		i++;
	}
	printf("Initialization complete. %d philosophers and %d forks are ready.\n",
		data->num_of_philos, data->num_of_philos);
	return (0);
}

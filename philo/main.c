#include "philo.h"

int main(int argc, char **argv)
{
	t_data data;

	if (argc < 5 || argc > 6)
    {
        printf("Error: Not enough arguments");
		return (1);
    }
	data.num_of_philos = ft_atoi(argv[1]);
	data.time_to_die = ft_atoi(argv[2]);
	data.time_to_eat = ft_atoi(argv[3]);
	data.time_to_sleep = ft_atoi(argv[4]);
    if (argc == 6)
		data.num_of_eats = ft_atoi(argv[5]);
	else
		data.num_of_eats = -1;
    if (ft_init_data(&data) != 0)
		return (1);

	ft_simul_start(&data);

	// 5. (마지막 단계) 자원 해제 함수 호출
	// cleanup(&data);
	return (0);
}
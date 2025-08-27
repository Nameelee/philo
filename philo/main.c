#include "philo.h"

// 입력값이 숫자인지 확인하는 간단한 함수
static int	is_digit_str(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

// 인자의 유효성을 검사하는 함수
static int	validate_args(int argc, char **argv)
{
	if (argc < 5 || argc > 6)
	{
		printf("Error: Wrong number of arguments.\n");
		printf("Usage: ./philo num_philosophers t_die t_eat t_sleep [num_eats]\n");
		return (1);
	}
	if (!is_digit_str(argv[1]) || !is_digit_str(argv[2]) || \
		!is_digit_str(argv[3]) || !is_digit_str(argv[4]) || \
		(argc == 6 && !is_digit_str(argv[5])))
	{
		printf("Error: All arguments must be positive integers.\n");
		return (1);
	}
	if (ft_atoi(argv[1]) <= 0)
	{
		printf("Error: There must be at least one philosopher.\n");
		return (1);
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (validate_args(argc, argv) != 0)
		return (1);
	if (ft_init_data(&data, argc, argv) != 0) // <- 에러가 발생했던 부분
		return (1);
	if (ft_simul_start(&data) != 0)
	{
		// 시뮬레이션 시작에서 에러 발생 시 정리
		ft_cleanup(&data);
		return (1);
	}
	return (0);
}
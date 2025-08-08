NAME		=	philo
BONUS_NAME	=	philo_bonus

CC			=	cc

# -pthread 플래그는 pthread 라이브러리를 링크하기 위해 필수적입니다.
CFLAGS		=	-Wall -Wextra -Werror -pthread
SRCS		=	$(wildcard *.c)
BONUS_SRCS	=	$(wildcard *_bonus.c)

# 필수 파트 소스 파일 목록에서 보너스 파일들을 제외합니다.
# 이렇게 하면 'make' 실행 시 보너스 관련 코드가 포함되지 않습니다.
SRCS		:=	$(filter-out $(BONUS_SRCS), $(SRCS))

# --- Object Files ---
# 소스 파일(.c) 목록을 기반으로 오브젝트 파일(.o) 목록을 생성합니다.
# 이 방식은 변경된 파일만 재컴파일하여 빌드 시간을 단축합니다.
OBJS		=	$(SRCS:.c=.o)
BONUS_OBJS	=	$(BONUS_SRCS:.c=.o)

# --- Commands ---
# 파일을 삭제할 때 사용할 명령어입니다.
RM			=	rm -f

# --- Rules ---

# 기본 규칙: 'make'만 입력하면 필수 파트를 빌드합니다.
all:		$(NAME)

# 필수 파트 실행 파일을 생성하는 규칙입니다.
# 오브젝트 파일들을 링크하여 최종 실행 파일을 만듭니다.
$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

# 보너스 파트를 빌드하는 규칙입니다. 'make bonus'로 실행할 수 있습니다.
bonus:		$(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
			$(CC) $(CFLAGS) -o $(BONUS_NAME) $(BONUS_OBJS)

# .c 소스 파일을 .o 오브젝트 파일로 컴파일하는 규칙입니다.
# $< 는 현재 규칙의 첫 번째 종속 파일(소스 파일)을 의미하고,
# $@ 는 현재 규칙의 타겟(오브젝트 파일)을 의미합니다.
%.o: %.c
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			$(RM) $(OBJS) $(BONUS_OBJS)

fclean:		clean
			$(RM) $(NAME) $(BONUS_NAME)

re:			fclean all

# 가짜 타겟(phony target)을 지정합니다.
# 이 규칙들은 실제 파일 이름이 아니므로, 파일 시스템에 해당 이름의 파일이 있어도
# 규칙이 정상적으로 실행되도록 보장합니다.
.PHONY:		all clean fclean re bonus

CWD			= 	$(shell pwd)
TARGET_NAME	=	dbl_sorter
INPUT_NAME	=	input_data
OUT_NAME	=	out_data




$(TARGET_NAME): main.c
	gcc -O2 -Wall -Werror -Wextra  main.c -o $(TARGET_NAME)

run: $(TARGET_NAME)
	date; \
	LOCALE=C $(CWD)/$(TARGET_NAME) $(INPUT_NAME) $(OUT_NAME); \
	date

clean:
	rm -f $(TARGET_NAME)	

gen_src:
	od -vAn -w8 -N375000000 -tfD < /dev/urandom | sed s/' '//g > $(INPUT_NAME)

gen_src_short:
	od -vAn -w8 -N130000000 -tfD < /dev/urandom | sed s/' '//g > $(INPUT_NAME)

gen_src_short2:
	od -vAn -w8 -N4000 -tfD < /dev/urandom | sed s/' '//g > $(INPUT_NAME)

gen_src_short3:
	od -vAn -w8 -N400 -tfD < /dev/urandom | sed s/' '//g > $(INPUT_NAME)

gen_src_short4:
	od -vAn -w8 -N80 -tfD < /dev/urandom | sed s/' '//g > $(INPUT_NAME)

gen_src_short5:
	od -vAn -w8 -N2000000 -tfD < /dev/urandom | sed s/' '//g > $(INPUT_NAME)

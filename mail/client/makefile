CC = gcc

# put your object files here
OBJS2 = mail_client.o 


# The executabel filename
EXEC2 = mail_client


C_COMP_FLAG = -std=c99   -DNDEBUG 
 

#C_COMP_FLAG = -std=c99 -Wall -Wextra \
#-Werror -pedantic-errors -DNDEBUG

$(EXEC2): $(OBJS2)
		$(CC) $(OBJS2) -o $@
mail_client.o:   
		$(CC) $(C_COMP_FLAG)  -c $*.c
# a rule for building a simple c souorce file

clean:
		rm -f $(OBJS2)  $(EXEC2)

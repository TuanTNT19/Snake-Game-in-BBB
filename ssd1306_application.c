#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <netdb.h>
#include <errno.h>

#define SSD1306_DEV_FILE    "/dev/my_device_ssd"
#define MAX_CHAR_PER_LINE   21

int c = 5;
int l = 5;

int ret;
int fd;
char user_cmd[50];

void set_cursor_to_LCD(int x, int y)
{
    char str[20];
    sprintf(str, "cursor %d %d", x, y);
    int w = write(fd, str, strlen(str));   
    if ( w == -1)
    {
        printf("Can not set cursor to LCD\n");
    }
}

void clearLCD()
{
    write(fd, "clear", 5);
}


int main()
{
    /* Try open the LCD dev file */
    fd = open(SSD1306_DEV_FILE, O_WRONLY);

    if (fd == -1)
    {
        printf("Open /dev/my_ssd1306 failed. Please check the dev folder\n");
        exit(EXIT_FAILURE);
    }

    
    system("clear");  
    //write(fd, "clear", 5);

    while(1){
    //    write(fd, "clear", 5);
        printf("Enter your data: ");
        fgets(user_cmd, 50, stdin);
        set_cursor_to_LCD(l, c);
        ret = write(fd, user_cmd, strlen(user_cmd));
        if (ret == -1)
        {
            printf("Can not write\n");
        }
    }
   

    return 0;

}
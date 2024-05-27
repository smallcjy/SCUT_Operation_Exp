#include<stdio.h>
#include<unistd.h>
int
main()
{
int p1,p2,i;
int *fp;
fp = fopen("to_be_locked.txt"
            ,"w+");
if (fp==NULL)
  {
    printf("Fail to create file");
    exit(-1);
   }
            while((p1=fork( ))== -1);       /*创建子进程p1*/
if (p1==0)
{
lockf(*fp,1,0);              /*加锁*/
for(i=0;i<10;i++)
  fprintf(fp,"daughter %d\n",i);
  lockf(*fp,0,0);              /*解锁*/
}
else
{
            while((p2=fork( ))==-1);  /*创建子进程p2*/
if (p2==0)
{
lockf(*fp,1,0);        /*加锁*/
for(i=0;i<10;i++)
fprintf(fp,"son %d\n",i);
lockf(*fp,0,0);        /*解锁*/
}
else
    {
      wait(NULL);
      lockf(*fp,1,0);         /*加锁*/
      for(i=0;i<10;i++)
      fprintf(fp,"parent %d\n",i);
      lockf(*fp,0,0);         /*解锁*/
   }
} 
fclose(fp);
}
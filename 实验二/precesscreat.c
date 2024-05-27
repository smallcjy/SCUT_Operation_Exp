 #include<stdio.h>
 int
    main()
    { int  p1;
      while((p1=fork())== -1);
      if(p1==0)
         printf("This is a child process.");		/*在子进程中*/
      else								/*在父进程中*/
         { 
			printf("This is a parent process.");	
         }
      return 0;
     }

# Funciones utilizadas en el proyecto PIPEX


## Función access.

prototipo: int access(const char *pathname, int mode);

Descripción: Comprueba los derechos de acceso de los archivos, verifica que el proceso de llamada tenga acceso a pathname.
modo: el modo especifica la verificacion de accesibilidad a realizar y es un valor F_OK o R_OK, W_OK, X_OK.
usando esos 4 puede dar un modo compuesto por OR bit a bit y puede pasar el modo que desea verificar como parametro.
Ej: si se desea verificar si es posible leer y ejecutar usa access(filepath, R_OX | X_OK)

Devuelve: 0 en caso de exito y -1 en caso de error y se establece errno.

Ej:
~~~
#include <unistd.h>
#include <stdio.h>

int	main(void)
{
    printf("**** F_OK ***\n");
    printf("F_OK not existing file : %d\n", access("not_access_test.txt", F_OK));
    printf("F_OK existing file : %d\n", access("access_test.txt", F_OK));

    printf("**** R_OK, W_OK, X_OK ***\n");
    printf("test file which user doesn't have any right\n");
    printf("R_OK %d\n", access("access_test.txt", R_OK));
    printf("W_OK %d\n", access("access_test.txt", W_OK));
    printf("X_OK %d\n", access("access_test.txt", X_OK));
}
~~~

## Funcion execve.

Prototipo: int execve(const char filename, char const argv[],char *const envp[]);

Descripción: Ejecuta  el  programa  indicado por filename.  filename debe ser bien un binario ejecutable, bien un guión shell (shell script) comenzando con una línea de  la  forma  "#! intérprete  [arg]".  En el segundo caso, el intérprete debe ser un nombre de camino válido para un ejecutable que no sea él mismo un guión y que será ejecutado como intérprete [arg] filename.
argv: Es un array de cadenas de argumentos pasados al nuevo programa.  
envp: Es un array de cadenas, convencionalmente de la forma clave=valor, que se pasan  como  entorno  al  nuevo programa. 
Tanto argv como envp deben terminar en un puntero nulo. El vector de argumentos y el entorno pueden ser accedidos por la función "main" del programa  invocado  cuando  se define como int main(int argc, char *argv[], char *envp[]).

Devuelve: La función execve no  regresa nada  en  caso  de  éxito,  y el código, datos, bss y la pila del proceso invocador se reescriben  con  los  correspondientes  del  programa  cargado.  El  programa invocado hereda el PID del proceso invocador y cualquier descriptor de fichero abierto que no se halla configurado para "cerrar en ejecución" (close on exec). Las señales pendientes del  proceso  invocador  se limpian. Cualquier señal capturada por el proceso invocador es devuelta a su comportamiento por defecto.  La señal SIGCHLD (cuando está puesta a SIG_IGN) puede o no puede ser reiniciada a SIG_DFL.
En caso de error el valor devuelto es -1, y a la variable errno se le asigna un valor apropiado.
~~~
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
    
int main(void){
    
    char *argumentos[] = {"/bin/ls", "-l", NULL}; 
    
    execv("/bin/ls", argumentos);
    
    printf("Termino!!!");
    
    return EXIT_SUCCESS;
}
~~~

## Función fork.

Prototipo: pid_t fork(void);

Descripción: Fork  crea  un proceso hijo que difiere de su proceso padre sólo en su PID y PPID, y en el hecho de que el uso de recursos esté asignado a 0.  Los candados de fichero (file locks) y las señales pendientes no se heredan.
En linux, fork está implementado usando páginas de copia-en-escritura (copy-on-write), así que la única penalización en que incurre fork es en el tiempo y  memoria  requeridos  para duplicar las tablas de páginas del padre, y para crear una única estructura de tarea (task structure) para el hijo.

Devuelve: En caso de éxito, el PID del proceso hijo se devuelve en el padre y 0 en el hijo. En caso de falla, se devuelve -1 en el padre, no se crea ningún proceso hijo y errno se establece de manera adecuada.

~~~
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

    pid_t c_pid = fork();
    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (c_pid == 0) {
        printf("printed from child process - %d\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        printf("printed from parent process - %d\n", getpid());
        wait(NULL);
    }

    exit(EXIT_SUCCESS);
}
~~~

## Función waitpdi.

Prototipo: 	pid_t wait(int *status); 
		pid_t waitpid(pid_t pid, int *status, int options); 
		int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

Descripción: En los sistemas basados en Unix, existe una noción de proceso que es simplemente una instancia en ejecución de un programa. El proceso puede crear otros procesos usando la llamada al sistema fork y ejecutar la porción dada del código. Tenga en cuenta que, para este tema, las llamadas al sistema son servicios del sistema operativo que se proporcionan al usuario como funciones de estilo C. Generalmente, en muchos escenarios, se requiere un programa para monitorear los procesos que creó llamados hijos. La familia de funciones wait es la que proporciona esta funcionalidad, y waitpid es una de ellas.
La llamada del sistema wait tiene múltiples limitaciones y, para cubrir funciones más avanzadas, es necesario utilizar la función waitpid. Es decir, si un proceso crea varios hijos y el padre necesita supervisar a un hijo específico, solo waitpid puede hacer esto. En el siguiente ejemplo, implementamos una función llamada spawnChild que crea un nuevo proceso hijo y ejecuta un programa diferente. En aras de una buena demostración del ejemplo, estamos ejecutando un programa top (disponible en casi todos los sistemas basados ​​en Unix) que se está ejecutando hasta que el usuario lo termina. Una vez que la función regresa al proceso padre, almacenamos un ID de proceso hijo y lo pasamos a la función waitpid para monitorear el estado.

El waitpid toma tres argumentos, el primero de los cuales es el número de identificación del proceso (pid). PID puede tener múltiples valores prediseñados con diferentes efectos, pero en este caso, solo vamos a mencionar -1 y >0. El valor -1 se puede pasar para monitorear cualquier proceso hijo que cambie su estado primero, que se usa para implementar la funcionalidad wait. >0 implica que el valor debe ser el ID de proceso real que se devolvió desde la función fork, que a su vez se usa para monitorear solo un proceso hijo específico. El segundo argumento es de tipo puntero int y debemos declarar una variable entera para pasar su dirección a la función. waitpid, por otro lado, almacenará la información del estado del niño en la variable int dada, que luego se puede decodificar usando las macros predefinidas. El último argumento es de tipo int, y se usa para especificar ciertos eventos de proceso hijo a monitorear además de los predeterminados.

Devuelve: En caso de éxito, devuelve el ID de proceso del niño cuyo estado ha cambiado; si se especificó WNOHANG y existen uno o más elementos secundarios especificados por pid, pero aún no han cambiado de estado, se devuelve 0. En caso de error, se devuelve -1.

~~~
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t spawnChild(const char* program, char** arg_list)
{
    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (ch_pid > 0) {
        printf("spawn child with pid - %d\n", ch_pid);
        return ch_pid;
    } else {
        execvp(program, arg_list);
        perror("execve");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    const char *args[] = { "top", NULL, NULL };

    pid_t child;
    int wstatus;

    child = spawnChild("top", args);

    if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
~~~

## Función pipe.

Prototipo: int pipe(int pipefd[2]);

Descripción: La pipe es una de las variantes de las primitivas de comunicación entre procesos (IPC) en los sistemas basados en UNIX. Proporciona un canal de comunicación unidireccional, es decir, un flujo de bytes entre dos procesos, y los datos se mueven secuencialmente en una dirección. La llamada al sistema pipe se utiliza para crear una pipe y adquirir descriptores de archivo para sus extremos de lectura y escritura. Tenga en cuenta que podemos operar en descriptores de pipe con las funciones habituales de E/S read y write. La llamada al sistema pipe toma un array int de dos elementos y la llamada exitosa devuelve dos descriptores de archivo que denotan el primer final de lectura y el segundo de escritura. Tenga en cuenta que los datos escritos en la pipe se almacenan en búfer en el kernel hasta que el lector recupera los bytes dados.
En el siguiente ejemplo, demostramos el uso básico de pipe para comunicarse entre los procesos padre e hijo. Al principio, creamos una pipe y almacenamos sus descriptores en el array pipe_fd. A continuación, llamamos fork en una expresión de declaración switch e incluimos el bloque de código del proceso hijo bajo el caso 0. El caso default, por otro lado, será ejecutado por el proceso padre.

Observe que el padre escribe la cadena que se ha recuperado del argumento de la línea de comando y luego espera a que el hijo termine. Mientras tanto, el proceso hijo lee de la pipe e imprime los bytes leídos en la consola. Tanto el hijo como el padre cierran un extremo de la pipe porque el proceso hijo hereda los descriptores de archivo del padre en fork. Finalmente, después de que se leen los bytes enviados, el hijo cierra el extremo leído de la pipe, terminando con una llamada de exit.

Devuelve: En caso de éxito, se devuelve cero. En caso de error, se devuelve -1 y errno se establece correctamente.

~~~
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main(){

     int fd[2];
     char buffer[30];
     pid_t pid;

     //#include<unistd.h>
     // int pipe(int fd[2]);
     // fd[0] contiene el descriptor para lectura

     pipe(fd); // Se crea el PIPE
     pid = fork();

     switch(pid){
     
          case -1: // Error
                printf("No se ha podido crear un hijo \n");
                exit(-1);
                break;
          case 0: // Hijo
                close(fd[0]); // Cierra el descriptor que no va a usar. El de lectura
                printf("El hijo escribe en el PIPE... \n");
                write(fd[1], "Hola papi", 10);
                break;
          default: // Padre
                close(fd[1]); // Cierra el descriptor de escritura
                wait(NULL); // Espera a que finalice el hijo
                printf("El padre lee el PIPE \n");
                read(fd[0], buffer, 10);
                printf("\t Mensaje leido: %s \n", buffer);
     }

~~~

## Función dup2.

Prototipo: int dup2(int fildes, int fildes2);

Descripción: Los archivos son normalmente manipulados después de haber sido abiertos usando la llamada al sistema open. En caso de éxito, open devuelve un nuevo descriptor de archivo asociado con el archivo recién abierto. En los sistemas basados en Unix, el sistema operativo mantiene una lista de archivos abiertos para cada programa en ejecución, llamada tabla de archivos. Cada entrada se representa con un entero de tipo int. Estos enteros se llaman descriptores de archivo en estos sistemas, y muchas llamadas al sistema toman los valores de los descriptores de archivo como parámetros.
Cada programa en ejecución tiene tres descriptores de archivo abiertos por defecto cuando se crea el proceso, a menos que se decida cerrarlos explícitamente. La función dup2 crea una copia del descriptor de fichero dado y le asigna un nuevo entero. dup2 toma como primer parámetro un descriptor de fichero antiguo a clonar y el segundo parámetro es el entero de un nuevo descriptor de fichero. Como resultado, ambos descriptores de archivo apuntan al mismo archivo y pueden usarse indistintamente. Tenga en cuenta que si el usuario especifica un entero utilizado actualmente por el archivo abierto como segundo parámetro, éste se cerrará y se reutilizará como el descriptor de archivo clonado.

Devuelve: Una vez completado con éxito, se devolverá un número entero no negativo, a saber, el descriptor del archivo; de lo contrario, se devolverá -1 y se establecerá errno para indicar el error.

Ej:
~~~
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

extern int counter;

int main(void) {
    int fd = open("tmp.txt", O_WRONLY | O_APPEND);

    printf("tmp.txt fd = %d\n", fd);
    dup2(fd, 121);
    dprintf(121, "This string will be printed in tmp.txt file\n");

    exit(EXIT_SUCCESS);
}
~~~
Resultado: tmp.txt fd = 3


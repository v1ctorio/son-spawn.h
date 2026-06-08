In linux, spawning a new process is an expensive operation. Since the linux kernel has no 
syscall for spawning processes, only `fork` which duplicates the current process. Including all it's memory.
and a `exec` syscall which replaces the current process with a provided executable.

What programs end up doing is duplicating the current process and instantly dropping it via an exec syscall
(for example, see posix_spawn implementation).

This is specially notable when large processes using large amounts of CPU and RAM spawn short-lived processes
(for example, calling `git --version`). And (according to what someone said on HN, idrk. this project is to experiment with this), 
the overhead can get so big to the point of salting the program for a few seconds.


So the idea of this project, is using an intermediary children process, created at the birth of the program 
as a proxy to be duplicated when the main process wants to spawn a process.

I'm sure the IPC and intermediary process will add some overhead over two programs
communicating directly. But, maybe, this penalty can outdo the fork overhead in large enough programs.

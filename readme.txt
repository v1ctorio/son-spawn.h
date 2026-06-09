In linux, spawning a new process is an expensive operation. This is because the linux kernel has no 
syscall for spawning processes. It only has a `fork` syscall which duplicates the current process, including all it's memory,
and an `exec` syscall which replaces the current process with a provided executable.

What programs end up doing is duplicating the current process and instantly dropping it via ˋexecˋ
(for example, see ˋposix_spawnˋ implementation).

This is specially notable when large processes using large amounts of CPU and RAM spawn short-lived processes
(for example, calling `git --version`). And (according to what someone said on HN, idrk. this project's goal is to experiment with this), 
the overhead can get so big to the point of halting the program for a few seconds.


So the idea of this project, is using an intermediary children process, created at the birth of the program,
as a proxy to be duplicated when the main process wants to spawn a process.

I'm sure the IPC and intermediary process will add some overhead over two programs
communicating directly. But, maybe, this penalty can outdo the fork copy overhead in large enough processes.

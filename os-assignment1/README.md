<center>

## 操作系统任务1
中山大学 陈佳俊

</center>

### 0 任务
#### 0.0 任务1 Client-Server问题
	需要创建客户Client和服务器Server两个进程，它们通过管道进行通信
	Client进程派生3个生产者线程，一个管道线程，共享一个20个slots的缓冲区。每个生产者线程随机产生一个数据，打印出来自己的id（进程、线程）以及该数据，通过管道发送线程发送给Server进程
	Server进程派生一个管道线程，3个消费者线程，共享一个20个slots的缓冲区。管道线程把数据接收后，分发给消费者线程处理
	生产者生成数据的间隔和消费者消费数据的间隔，按照负指数分布来控制，各有一个控制参数λc, λs
	运行的时候，开两个窗口，一个./client λc，另一个./server λs，要求测试不同的参数组合，打印结果

#### 0.1 任务2 MIT xv6课程实验
	提示：在vmware下安装ubuntu20，可以较为顺利完成xv6安装和编译和运行（Ctrl-a x可退出）如果阅读源代码，建议用source insight较为方便。
	1.阅读Lab: Xv6 and Unix utilities，完成其中的sleep，pingpong 2个任务。
	2.阅读Lab: system calls的内容，完成Using GDB, System call tracing, Sysinfo3个任务。
	3.结合book-riscv-rev3第1、2章，阅读xv6内核代码(kernel目录下)的进程和调度相关文件，围绕proc.h/proc.c，swtch.S，理解进程的基本数据结构，组织方式，以及调度方法。修改proc.c中procdump函数，打印各进程的扩展信息，包括大小（多少字节）、内核栈地址、关键寄存器内容等，通过^p可以查看进程列表。


### 1 Client-Server问题
#### 1.1 设计共享数据结构和同步机制

思路：我们需要两个程序 client 和 server，它们通过管道进行通信。它们还将使用多线程，所以需要考虑线程间的同步。据此，我们的客户端和服务器都将使用大小为 20 slots 的缓冲区；并且使用互斥锁 (mutex) 和条件变量 (cond) 来同步对缓冲区的访问。

#### 1.2 定义所需的函数和线程

客户端 client.c：
- 3个producer线程：生成数据，并打印进程、线程与数据的信息。
- 1个管道线程：读取生产者的数据并通过管道发送到服务器。

服务器 server.c：
- 3个consumer线程：从缓冲区取出数据进行处理。
- 1个管道线程：从管道读取数据并存储到缓冲区。

#### 1.3 实现 Client 端

客户端负责生成数据并通过管道发送到服务端。它包括多个生产者线程，每个线程生成数据并存入缓冲区，以及一个管道线程，负责将缓冲区的数据发送到服务端。

#### 1.4 实现 Server 端

服务端接收客户端发送的数据，处理并显示。它包括一个管道线程用于接收数据，并将数据放入缓冲区；三个消费者线程从缓冲区取数据并进行处理。全局变量与同步机制包含用于存储数据、跟踪数据位置和线程同步的变量和机制。

#### 1.5 编写 Makefile

包含make all, make client, make server, make clean等指令选项。


#### 1.6 用不同的 λc 和 λs 值编译、测试程序
在两个不同的终端窗口中运行：`./client 0.5`和`./server 0.5`可以测试程序，替换 0.5 为其他参数来测试不同的 λc 和 λs 值。

##### 1.6.1 分组测试
λc和λs分别为`(0.5,0.5)`、`(0.7,0.3)`、`(0.3,0.7)`。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/task1-1.png)  
λc=0.5, λs=0.5

![λc和λs](D:/aaa大三下/操作系统/big-homework/task1-3.png)  
λc=0.7, λs=0.3

![λc和λs](D:/aaa大三下/操作系统/big-homework/task1-2.png)  
λc=0.3, λs=0.7


</center>

##### 1.6.2 压力测试
在极端的producer速率（很高/很低的 λc）和consumer速率（很低/很高的 λs）下运行程序，观察系统是否能稳定运行或会崩溃。

λc和λs分别为`(0.99,0.01)`、`(0.01,0.99)`。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/task1-5.png)  
λc=0.99, λs=0.01

![λc和λs](D:/aaa大三下/操作系统/big-homework/task1-4.png)  
λc=0.01, λs=0.99

</center>

经测试，系统在两个程序不同的运行速率下仍能保持正常工作，本次编写的Client-Server程序性能良好。

### 2 MIT xv6课程实验
#### 2.1 Lab: Xv6 and Unix utilities
##### 2.1.1 sleep

###### 实验步骤

- 引入头文件`kernel/types.h`和`user/user.h`以访问基本类型定义和系统调用。

- 编写主函数 `main(int argc, char* argv[])`，便于后续处理命令行输入，检查参数正确性。

- 使用`write()`系统输出错误信息，确保参数数量正确。

- 将用户输入的字符串转换为整数`atoi()`，表示休眠时间。

- 执行休眠并退出：`sleep()`指定时钟周期休眠。`exit(0)`正常结束程序。

- 最后还要记得更新 Makefile，在`UPROGS`项中最后一行添加`$U/_sleep\`。


###### 实验结果

- 在xv6系统中输入命令，输入正确信息和错误信息时系统会有不同的表现。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-1-1.png)  
正确信息：睡眠

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-1-2.png)  
错误信息：纠正用法

</center>

- 退出xv6，运行单元测试。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-1-3.png)  
单元测试全部通过

</center>

##### 2.1.2 pingpong

###### 实验步骤

- 和上一小节一样，引入头文件`kernel/types.h`和`user/user.h`。

- 创建管道
- - 定义文件描述符数组 `ptoc_fd` 和 `ctop_fd`，分别用于父到子和子到父的通信。
- - 使用 `pipe()` 系统调用创建这两个管道。`pipefd[0]` 为管道读端，`pipefd[1]` 为管道写端。


- 使用 `fork()` 创建子进程。在父进程中，`fork()` 返回子进程的PID；在子进程中，`fork()` 返回0。父子进程通过 `if` 语句分别执行不同的代码块。

- 父进程操作
- - 通过 `write()` 向 `ptoc_fd[1]` 写入字符串 "ping"。
- - 使用 `wait()` 等待子进程执行完毕。
- - 通过 `read()` 从 `ctop_fd[0]` 读取子进程发送的数据。
- - 打印接收到的信息和父进程ID。

- 子进程操作
- - 通过 `read()` 从 `ptoc_fd[0]` 接收父进程发送的数据。
- - 打印接收到的信息和子进程ID。
- - 通过 `write()` 向 `ctop_fd[1]` 发送字符串 "pong"。


- 更新 Makefile，在`UPROGS`项中最后一行添加`$U/_pingpong\`。


###### 实验结果

- 在xv6系统中输入`pingpong`命令。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-1-4.png)  
父子进程之间成功通信

</center>


#### 2.2 Lab: system calls
##### 2.2.1 Using GDB

###### 学习指令

- 在终端中输入 `make qemu-gdb` ，运行qemu并开启调试功能。然后在GDB中输入以下指令

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-1.png)  
`b syscall` 将在函数 `syscall` 处设置断点； `c` 将会运行到此断点时等待调试指令

</center>

- `layout src` 将会开启一个窗口展示调试时的源代码；`backtrace` 将会打印堆栈回溯。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-2.png)  
展示窗口

</center>

###### 回答问题


> 1) Looking at the backtrace output, which function called `syscall`?  

通过backtrace返回的堆栈回溯过程可以得知，是函数usertrap()调用了syscall。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-3.png)  
项目指示1


![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-4.png)  
运行几次单步执行，执行到`struct proc *p = myproc();`这个语句


![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-5.png)  
在此时输出当前进程*p的值，输出如下，可以看到它是一个proc结构体。


</center>


> 2) What is the value of `p->trapframe->a7` and what does that value represent? (Hint: look `user/initcode.S`, the first user program xv6 starts.)  

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-6.png)  
the value of `p->trapframe->a7`

</center>

据此我们得知了a7的值为7，从下面的user/initcode.S代码中，我们能知道a7保存了将要执行的系统调用号，有可能是Sys_exec和SYS_exit。再通过查阅代码kernel/syscall.h，我们知道了a7==7时，这个系统调用为SYS_exec。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-7.png)  
initcode.S代码

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-8.png)  
syscall.h代码

</center>

> 3) What was the previous mode that the CPU was in?

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-9.png)  
查看sstatus寄存器，通过p/x以16进制打印

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-10.png)  
通过二进制打印比较方便观察

</center>

观察数据，借助官方文档，可以得到SSP位为0，则问题3答案为User mode。


> 4) Write down the assembly instruction the kernel is panicing at. Which register corresponds to the varialable `num`?

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-11.png)  
通过手册提供的方法修改代码，并寻找panic信息。

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-12.png)  
通过spec提供的地址，搜索kernel/kernel.asm，找到了正是我们修改的那行代码报错，对应寄存器是a3

</center>

所以说问题4答案：内核panicing发生在a3寄存器。

> 5) Why does the kernel crash? Hint: look at figure 3-3 in the text; is address 0 mapped in the kernel address space? Is that confirmed by the value in `scause` above? (See description of `scause` in RISC-V privileged instructions)

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-13.png)  
再次打开gdb调试，将断点设置在发生panic的地方。

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-14.png)  
查看scase寄存器（代指内核panic的原因），确认panic原因。

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-2-15.png)  
查阅文档，发现13指的是Load page fault，即之前修改的代码：从内存地址0中加载数据给寄存器a3，产生错误。

</center>


所以本题答案就是内核加载了一个地址0处的内存数据（这个数据根本没有被使用）而崩溃。没有被confirmed，由scause中报了异常13可以知道。

> 6) What is the name of the binary that was running when the kernel paniced? What is its process id (pid)?

根据下列打印的输出信息，可以知道这个二进制的名字叫initcode，pid = 1。
<pre>
(gdb) p p->name  
$1 = "initcode\000\000\000\000\000\000\000"  
gdb) p *p  
$3 = {lock = {locked = 0, name = 0x80008178 "proc", cpu = 0x0}, state = RUNNING, chan = 0x0, killed = 0, xstate = 0,  
  pid = 1, parent = 0x0, kstack = 274877894656, sz = 4096, pagetable = 0x87f73000, trapframe = 0x87f74000, context = {  
    ra = 2147488870, sp = 274877898368, s0 = 274877898416, s1 = 2147519792, s2 = 2147518720, s3 = 1, s4 = 0, s5 = 3,  
    s6 = 2147588560, s7 = 8, s8 = 2147588856, s9 = 4, s10 = 1, s11 = 0}, ofile = {0x0 <repeats 16 times>},  
  cwd = 0x80016e40 <itable+24>, name = "initcode\000\000\000\000\000\000\000"}  
(gdb)
</pre>


##### 2.2.2 System call tracing

###### 实验思路

在准备进行xv6相关实验之前，需要仔细阅读xv6文档的第2章以及第4章的4.3节和4.4节，这些部分分别介绍了xv6系统的组织结构和系统调用的过程及其参数。

###### 实验步骤


<p>在 xv6 操作系统中添加一个新的系统调用 <code>trace</code> 需要几个步骤。首先，在 <code>kernel/syscall.h</code> 中定义系统调用号：</p>
<pre><code>#define SYS_trace  22</code></pre>

<p>接着，在 <code>user/user.h</code> 中声明可由用户态调用的 <code>trace</code> 函数：</p>
<pre><code>int trace(int);</code></pre>

<p>为使 <code>trace</code> 系统调用在用户态可用，需要在 <code>user/usys.pl</code> 中添加对应条目，使得该调用在 <code>usys.S</code> 中自动生成对应的用户态接口：</p>
<pre><code>entry("trace");</code></pre>
<p>系统调用的实现依赖于内核的 <code>syscall</code> 函数（定义于 <code>kernel/syscall.c</code>），在该函数中根据系统调用号调用对应的处理函数：</p>
<pre><code>void syscall(void) {
  ...
  if(num &gt; 0 &amp;&amp; num &lt; NELEM(syscalls) &amp;&amp; syscalls[num]) {
    p-&gt;trapframe-&gt;a0 = syscalls[num]();
  }
  ...
}</code></pre>


<p>我们将 <code>trace</code> 添加到内核的 <code>syscalls[]</code> 函数指针数组，并在 <code>kernel/sysproc.c</code> 中实现 <code>sys_trace</code> 函数：</p>
<pre><code>static uint64 (*syscalls[])(void) = {
  ...
  [SYS_trace]   sys_trace,
};

uint64
sys_trace(void) {
    int mask;
    if(argint(0, &amp;mask) &lt; 0)
        return -1;
    myproc()-&gt;mask = mask;
    return 0;
}</code></pre>

<p>另外，我们还需要在在文件开头给内核态的系统调用 trace 加上声明， `kernel/syscall.c` 加上：<code>extern uint64 sys_trace(void);
</code></p>

<p>因为 `proc` 结构体(见 kernel/proc.h )里的 `name` 是整个线程的名字，不是函数调用的函数名称，所以我们不能用 `p->name` ，而要自己定义一个数组，直接在 kernel/syscall.c 中定义。</p>
<pre><code>static char* syscalls_name[] = {
[SYS_fork]    "syscall fork",
[SYS_exit]    "syscall exit",
...
[SYS_close]   "syscall close",
[SYS_trace]   "syscall trace",
};</code></pre>

<p>在系统调用发生时，如需输出调用情况，将根据进程的掩码判断是否打印相关信息：</p>
<pre><code>uint64 a0 = syscalls[num]();
    if ((p->mask >> num) & 0b1) {
      printf("%d: %s -> %d\n", p->pid, syscalls_name[num], a0);
    }
    p->trapframe->a0 = a0;</code></pre>


<p>为确保子进程能够继承父进程的系统调用跟踪设置，在 <code>fork()</code> 函数中添加代码，使子进程复制父进程的 <code>mask</code>。此 <code>mask</code> 是一个整数，指定了需要跟踪的系统调用。修改后的 <code>fork()</code> 函数如下：</p>
<pre><code>int
fork(void)
{
  ...
  pid = np->pid;
  np->state = RUNNABLE;

  // 子进程复制父进程的 mask
  np->mask = p->mask;
  ...
}</code></pre>

<p>为编译和链接新添加的 <code>trace</code> 系统调用的用户程序 <code>_trace</code>，需要在 <code>Makefile</code> 的 <code>UPROGS</code> 部分进行更新。更新后的 <code>Makefile</code> 中的 <code>UPROGS</code> 部分应如下所示：</p>
<pre>
<code>
UPROGS=\
  ...
  $U/_trace\
</code>
</pre>
<p>以上修改确保系统支持通过新的 <code>trace</code>。 </p>


###### 实验结果

编译并运行 xv6 进行测试。

<center>
<body>
        <img src="D:/aaa大三下/操作系统/big-homework/2-3-1.png" alt="Image 1">
        <img src="D:/aaa大三下/操作系统/big-homework/2-3-1-2.png" alt="Image 2">
        <img src="D:/aaa大三下/操作系统/big-homework/2-3-1-3.png" alt="Image 3">
</body>

运行结果符合实验手册中的结果

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-3-2.png)  
测试均通过

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-3-3.png)  
退出xv6，进行trace单元测试，均通过

</center>

##### 2.2.3 Sysinfo


###### 实验思想

在 xv6 操作系统中添加`sysinfo`这一系统调用。它的主要功能是收集当前运行系统的信息。它通过一个参数工作，即一个指向 struct sysinfo 的指针（该结构定义在 `kernel/sysinfo.h` 文件中）。当系统调用被执行时，内核会填充这个结构体的两个字段：`freemem`字段，它代表系统的剩余内存量，以及 `nproc` 字段记录当前状态不是`UNUSED`的进程数量。此外，还有一个测试程序 sysinfotest 用于验证这个系统调用的正确性。如果测试程序输出 "sysinfotest: OK"，则表示系统调用实现正确。


###### 实验步骤


<p><code>sysinfo</code>系统调用用于收集 xv6 操作系统的关键运行信息。</p>

<li><strong>系统调用定义：</strong>
        <p>和上个实验类似，在 <code>kernel/syscall.h</code> 中定义系统调用的序号，为 <code>sysinfo</code> 分配唯一标识符：</p>
        <pre><code>#define SYS_sysinfo  23</code></pre>
    </li>
    <li><strong>用户态接口设置：</strong>
        <p>在 <code>user/usys.pl</code> 文件中添加入口点，自动生成用户态系统调用接口 <code>usys.S</code>：</p>
        <pre><code>entry("sysinfo");</code></pre>
    </li>
    <li><strong>声明系统调用和数据结构：</strong>
        <p>在 <code>user/user.h</code> 中声明 <code>sysinfo</code> 函数和 <code>struct sysinfo</code>：</p>
        <pre><code>struct sysinfo; 
int sysinfo(struct sysinfo *);</code></pre>
    </li>
    <li><strong>实现系统调用：</strong>
        <p>在 <code>kernel/syscall.c</code> 中添加对 <code>sysinfo</code> 函数的处理：</p>
        <ul>
            <li>函数声明：<code>extern uint64 sys_sysinfo(void);</code></li>
            <li>函数指针数组映射：<code>[SYS_sysinfo] sys_sysinfo,</code></li>
            <li>添加系统调用名称：<code>"sysinfo"</code> 到 <code>syscall_names</code> 数组。</li>
        </ul>
    </li>

<li><strong>辅助功能实现：</strong>
        <p>分别在 <code>kernel/kalloc.c</code> 和 <code>kernel/proc.c</code> 中添加函数，用于计算未使用的内存和统计活跃进程数量。</p>
    </li>	
<p>在<code>kernel/kalloc.c</code>中添加函数<code>free_mem</code>，获取空闲内存数量。</p>
<pre><code>uint64
free_mem(void)
{
  struct run *r;
  // counting the number of free page
  uint64 num = 0;
  acquire(&kmem.lock);
  // r points to freelist
  r = kmem.freelist;
  while (r)
  {
    num++;
    r = r->next;
  }
  release(&kmem.lock);
  // page multiplicated 4096-byte page
  return num * PGSIZE;
}
</code></pre>

<p>在<code>kernel/proc.c</code>中添加函数<code>nproc</code>，获取空闲进程数量。</p>
<pre><code>nproc(void)
{
  struct proc *p;
  // counting the number of processes
  uint64 num = 0;
  // traverse all processes
  for (p = proc; p < &proc[NPROC]; p++)
  {
    acquire(&p->lock);
    // if the state is not UNUSED
    if (p->state != UNUSED)
    {num++;}
    release(&p->lock);
  }
  return num;
}
</code></pre>

<p>最后在 kernel/defs.h 中添加上述两个新增函数的声明即可。</p>
<pre><code>uint64          free_mem(void);
uint64          nproc(void);</code></pre>

<li><strong>数据从内核到用户空间的复制：</strong>
        <p>使用 <code>copyout</code> 函数将填充的 <code>struct sysinfo</code> 数据从内核空间复制到用户空间。在 <code>kernel/sysproc.c</code> 文件中添加 <code>sys_sysinfo</code> 函数。</p>
    </li>

<pre><code>uint64
sys_sysinfo(void)
{
  uint64 addr;
  struct sysinfo info;
  struct proc *p = myproc();
  
  if (argaddr(0, &addr) < 0)
	  return -1;
  info.freemem = free_mem();
  info.nproc = nproc();

  if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  
  return 0;
}
</code></pre>


<p>最后我们在 <code>user</code> 目录下添加一个 <code>sysinfo.c</code> 用户程序。并更新Makefile。</p>
<pre><code>int
main(int argc, char *argv[])
{
    // param error
    if (argc != 1)
    {
        fprintf(2, "Usage: %s need not param\n", argv[0]);
        exit(1);
    }
    struct sysinfo info;
    sysinfo(&info);
    // print the sysinfo
    printf("free space: %d\nused process: %d\n", info.freemem, info.nproc);
    exit(0);
}</code></pre>

<pre><code>// Makefile
$U/_sysinfotest\
$U/_sysinfo\</code></pre>


<p>以上步骤确保 <code>sysinfo</code> 系统调用能够正确实现，从而收集并返回 xv6 操作系统的关键运行信息。</p>






###### 实验结果

编译并运行 xv6 进行测试。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-3-4.png)  
运行结果符合实验手册中的结果


![λc和λs](D:/aaa大三下/操作系统/big-homework/2-3-5.png)  
退出xv6，进行sysinfo单元测试，均通过

</center>


#### 2.3 打印各进程的扩展信息


###### 实验思路与做法

访问 proc 结构体中的各个字段，并相应地修改 procdump 函数。通过遍历每一个进程，访问各项扩展信息并直接用`printf`输出，即可打印。

###### 实验结果

修改了`proc.c`的`procdump`函数后重新编译系统，进入系统，在xv6系统终端摁下`^p`(Ctrl+P)，即可看到进程列表，以及各进程的扩展信息。包括了每个进程的状态、进程名、进程内存大小(`sz`)、内核栈地址(`kstack`)、返回地址寄存器(`ra`)和栈指针寄存器(`sp`)。

<center>

![λc和λs](D:/aaa大三下/操作系统/big-homework/2-3-6.png)  
运行结果（包含两个进程）


</center>

通过这样的信息，系统管理员可以了解到哪些进程是活动的，它们各自占用多少资源，以及它们在系统中的角色，便于优化。







#Coursework
----------
<br>
##Compile:

<br>

The project is built with **cmake**. If build with other tools, cxx flag *-std=c++11* and thread support are required.
```
cmake .
make
```

<br>

##Run:

<br>

```
./main <seed[-1]> <given[30]> <nproc[1]>

```

The game board is randomly generated from *&#60;seed&#62;*. if *&#60;seed&#62;* is -1, the seed will generate from system time. 
Default value is -1.

The game then shrinks to the *&#60;given&#62;* number, hides the rest. Default value is 30.

The solving part will use *&#60;nproc&#62;* threads. 
System **nproc** is recommended. Default value is 1.


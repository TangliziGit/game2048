# game2048
A implementation of game 2048, used Armadillo which is C++ library for linear algebra &amp; scientific computing.

# 主要思路
1. 考虑到棋盘大小较小（限定MaxBoardSize=10, 可修改），对棋盘进行矩阵旋转，减少代码量。使用Armadillo进行矩阵操作。
2. 利用termios做到非阻塞输入，实际效果类似windows下conio.h中的kbhit()和getch()组合。

# 实现过程要点
1. static const 可在类中类外初始化，static 只能在类外初始化， const 只能在构造函数舒适化列表里初始化
2. 用enum{...} 来设定常数
3. gdb调试SegmentFault，编译时加上-g参数，gdb里run到SegmentFault，用print ... 检查变量

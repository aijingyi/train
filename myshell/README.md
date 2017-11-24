使用方法：
在目录下输入make，编译成myshell可执行程序。
然后./myshell进入命令行，即可执行shell名。
例如ls，cd /opt,time,/usr/bin/cal -y等

完成的功能有：
step0
建立了Makefile文件，使用make命令即可编译成myshell程序。

step1
执行外部命令，如/bin/ls或/usr/bin/cal -y

setp3
执行内部命令，也就是自己定义的命令
1. exit 退出命令
2. cd 切换目录，cd为切换到默认目录或者cd /opt 切换到指定目录。
3. time 执行时间命令
4. ls 列出目录ls ,ls /opt

step4
支持顺序执行；
支持与执行&&
支持或执行||

step6
支持文件输出重定向
ls > a.txt 重新写
ls >> a.txt 追加写

step9
后台执行函数
ls; /bin/ps & time
time将会后台执行

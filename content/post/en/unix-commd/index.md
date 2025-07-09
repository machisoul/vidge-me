---
author: "Vidge Wong"
title: "Common unix-like commands"
date: "2023-07-14"
description: "Some commonly used unix-like commands."
tags: ["unix", "linux"]
categories: ["Tools"]
#aliases: ["migrate-from-jekyl"]
image: img/cover/neofetch.jpg
ShowToc: true
TocOpen: true
---

## Overview
Git is a widely used version control system for tracking changes in source code during software development. It allows multiple developers to collaborate on a project without interfering with each other's work.

## Commands

### tcpdump
```shell
sudo tcpdump -i any src port 13400 or dst port 13400
```

### mount
```shell
mount -o remount,rw <path>
```

### ps (process status)
```shell
ps aux | grep <-i> <xxx>

# check related precess information and reserver first line.
ps -eo pid,ppid,user,cmd | awk 'NR==1 || /<key_word>/' | less -S
# sz: viture memory usage, unit: page/4k, 
ps -eo pid,ppid,user,cmd,pcpu,sz,rss | awk 'NR==1 || /<key_word>/' | less -S
```
### vmstat
```shell
#check virtual memory usage
vmstat -Sm 1
```

### top
1. Type Shift + P: Sort by CPU load in descending order 
2. Type Shift + M: Sort by memory usage in descending order
3. Type c: Show complete command.
4. Type E: Switching unit.

### Network
```shell
lsof -i :<port> # check port's process

# -t: TCP, -u: UDP, -l: only listening, -n: don't parse servername, 
# -p: show port process information
ss -tulnp | grep <"process name"> 

ss -tulnp | grep <":port">

# show first line title
ss -an | awk 'NR==1 || /:13400/'
```

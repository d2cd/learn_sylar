#!/bin/bash
if [ -f "../bin/test" ]; then
   # 文件存在，删除文件
   rm "../bin/test"
   echo "文件 '../bin/test' 已被删除。"
else
   # 文件不存在，输出消息
   # 相对地址是相对于脚本文件所在的目录
   echo "文件 '../bin/test' 不存在，无需删除。"
fi
cmake --build .
if [ $? -eq 0 ]; then
	echo "build succeeded."
	# ../bin/test
   # ../bin/test_config
   ../bin/test_thread
else
	echo "build failed."
	exit 1
fi

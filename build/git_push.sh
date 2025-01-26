#!/bin/bash

echo "请输入 commit 信息:"
read commit_msg

git add *
git commit -m "$commit_msg"
git push origin master

if [ $? -eq 0 ]; then
	echo "git上传成功"
fi

# C++学生信息管理系统
## 项目介绍
基于C++实现学生信息管理，分为数组内存版、MySQL持久化存储版。
功能：添加、查询、修改、删除学生信息，解决控制台中文乱码。

## 运行环境
1. Visual Studio 2022 x64
2. MySQL 9.7（数据库版本需要启动MySQL97服务）

## 使用步骤
1. 数组版：直接编译运行main.cpp
2. MySQL版：
   - 管理员CMD启动服务：net start MySQL97
   - 登录MySQL创建student_management库与student表
   - VS配置MySQL include与lib依赖，复制libmysql.dll到exe目录

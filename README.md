# 学生成绩管理系统

基于 C++ 实现学生信息管理后端 + Flask Web 前端，支持 MySQL 持久化存储。

- **C++ 后端** (`students/`)：控制台程序，添加/查询/修改/删除学生与成绩
- **Flask 前端** (`frontend/`)：Web 界面管理学生、成绩、学科权重，支持 Excel 导入

---

## Linux 环境（推荐，WSL / Ubuntu / Debian）

### 1. 安装依赖

```bash
sudo apt update
sudo apt install -y g++ make python3 python3-pip mysql-server libmysqlclient-dev
```

### 2. 启动并配置 MySQL

```bash
sudo service mysql start
sudo mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '123456'; FLUSH PRIVILEGES;"
```

### 3. 一键启动（自动编译 C++ + 启动 MySQL + 启动 Flask）

```bash
chmod +x start.sh
./start.sh
```

> 环境变量：`DB_USER`（默认 root）、`DB_PASSWORD`（默认 123456）、`DB_NAME`（默认 student_sys）

### 4. 手动方式（分步执行）

```bash
# 初始化数据库
mysql -u root -p123456 < sql/student_sys.sql

# 编译 C++ 后端
cd students
make
export DB_PASSWORD=123456
./student_ss      # 启动 C++ 后端（控制台）

# 另一个终端启动 Flask 前端
cd ../frontend
python3 app.py    # 或回到根目录: python3 run.py
```

访问 http://127.0.0.1:5000

---

## Windows 环境

### 使用步骤
1. 数组版：直接编译运行 `students/main.cpp`
2. MySQL 版：
   - 用 Visual Studio 2022 打开 `students.slnx` 编译
   - 配置 MySQL include 与 lib 依赖（`libmysql.lib`）
   - 运行根目录 `run.py` 启动 Flask

### Flask Web 端
1. 安装依赖：`pip install -r requirements.txt`
2. 配置密码：复制 `.env.example` 为 `.env`，填入 `DB_PASSWORD=你的MySQL密码`
3. 初始化数据库：执行 `sql/student_sys.sql`
4. 启动：`python run.py`
5. 访问：http://127.0.0.1:5000

---

## 数据库结构 (`sql/student_sys.sql`)

- `student(sid, sname)` — 学生表
- `score(sid, subject, usual_score, end_score, final_score)` — 成绩表
- `subject_weight(subject_name, weight_ratio)` — 学科权重表

## 技术栈
- C++20 + MySQL C API
- Python 3 + Flask + pymysql + openpyxl
- Bootstrap Icons 前端样式

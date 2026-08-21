#!/usr/bin/env bash
# 学生成绩管理系统 - Linux 一键启动脚本（仅 Web 前端）
# 用法: ./start.sh
# 功能: 创建虚拟环境 → 安装依赖 → 启动MySQL → 初始化数据库 → 启动Flask前端

set -e

# 颜色
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; NC='\033[0m'
info() { echo -e "${GREEN}[INFO]${NC} $1"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
err()  { echo -e "${RED}[ERROR]${NC} $1"; }

# 项目路径（脚本所在目录）
BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FRONTEND_DIR="$BASE_DIR/frontend"

DB_USER="${DB_USER:-root}"
DB_PASSWORD="${DB_PASSWORD:-123456}"
DB_NAME="student_sys"

# 检查 Python
if ! command -v python3 >/dev/null 2>&1; then
    err "未安装 python3，请先执行: sudo apt install -y python3 python3-venv python3-pip"
    exit 1
fi

# 1. 创建虚拟环境并安装 Python 依赖
VENV_DIR="$BASE_DIR/.venv"
if [ ! -d "$VENV_DIR" ]; then
    info "创建 Python 虚拟环境..."
    python3 -m venv "$VENV_DIR"
fi
source "$VENV_DIR/bin/activate"

info "检查并安装 Python 依赖..."
pip install -q -r "$BASE_DIR/requirements.txt" 2>/dev/null || pip install -q flask pymysql openpyxl python-dotenv

# 2. 检查并启动 MySQL
info "检查 MySQL..."
if ! command -v mysql >/dev/null 2>&1; then
    err "未安装 MySQL，请先执行: sudo apt install -y mysql-server"
    exit 1
fi

if ! mysqladmin ping --silent 2>/dev/null; then
    info "启动 MySQL 服务..."
    sudo service mysql start || true
    sleep 2
fi

# 3. 初始化数据库（若不存在）
if ! mysql -u"$DB_USER" -p"$DB_PASSWORD" -e "USE $DB_NAME" >/dev/null 2>&1; then
    info "初始化数据库 $DB_NAME ..."
    mysql -u"$DB_USER" -p"$DB_PASSWORD" < "$BASE_DIR/sql/student_sys.sql" 2>/dev/null \
        || mysql -u"$DB_USER" -e "ALTER USER '$DB_USER'@'localhost' IDENTIFIED WITH mysql_native_password BY '$DB_PASSWORD'; FLUSH PRIVILEGES;" 2>/dev/null \
        || sudo mysql -u"$DB_USER" -e "ALTER USER '$DB_USER'@'localhost' IDENTIFIED WITH mysql_native_password BY '$DB_PASSWORD'; FLUSH PRIVILEGES;"
    mysql -u"$DB_USER" -p"$DB_PASSWORD" < "$BASE_DIR/sql/student_sys.sql" 2>/dev/null || true
fi

# 4. 启动 Flask 前端
export DB_PASSWORD
export PYTHONPATH="$FRONTEND_DIR:$PYTHONPATH"

info "========================================"
info " 学生成绩管理系统 - Web 前端启动"
info " 访问地址: http://127.0.0.1:5000"
info " 按 Ctrl+C 退出"
info "========================================"

cd "$FRONTEND_DIR"
python3 -c "
from app import app
app.run(host='0.0.0.0', port=5000, debug=False)
"

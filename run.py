import sys
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
FRONTEND_DIR = os.path.join(BASE_DIR, 'frontend')


def start_frontend():
    """启动 Flask 前端（Web 界面）"""
    print("[前端] 正在启动 Flask Web 服务...")
    os.chdir(FRONTEND_DIR)
    sys.path.insert(0, FRONTEND_DIR)
    from app import app
    return app


if __name__ == '__main__':
    print("=" * 50)
    print("  学生成绩管理系统 - Web 前端")
    print("=" * 50)

    app = start_frontend()
    print("[前端] Flask 已就绪")
    print("=" * 50)
    print("  访问地址: http://127.0.0.1:5000")
    print("  按 Ctrl+C 退出")
    print("=" * 50)

    app.run(host='0.0.0.0', port=5000, debug=False)

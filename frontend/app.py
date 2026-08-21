import sys
import subprocess

# 自动检测并安装缺失的依赖库
def check_and_install():
    needed = ["flask", "pymysql", "openpyxl", "python-dotenv"]
    for pkg in needed:
        try:
            __import__(pkg)
        except ImportError:
            print(f"正在自动安装 {pkg} ...")
            subprocess.run([sys.executable, "-m", "pip", "install", pkg], check=True)
            print(f"{pkg} 安装成功。")

# 执行检测
check_and_install()

import os
from dotenv import load_dotenv
load_dotenv()

import pymysql
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

_db_password = os.getenv('DB_PASSWORD')
if not _db_password:
    print("[警告] 未在 .env 文件中设置 DB_PASSWORD，请创建 .env 并写入 DB_PASSWORD=你的密码")
    _db_password = ''

DB_CONFIG = {
    'host': '127.0.0.1',
    'port': 3306,
    'user': 'root',
    'password': _db_password,
    'database': 'student_sys',
    'charset': 'utf8mb4',
}

def get_conn():
    return pymysql.connect(**DB_CONFIG)


def validate_score(value):
    """校验成绩分数，必须在 0~100 之间"""
    try:
        v = int(value)
    except (TypeError, ValueError):
        return None, "分数必须是整数"
    if not (0 <= v <= 100):
        return None, "分数必须在 0~100 之间"
    return v, None


def validate_weight(value):
    """校验权重，必须在 0~1 之间（开区间，0和1无意义）"""
    try:
        w = float(value)
    except (TypeError, ValueError):
        return None, "权重必须是数字"
    if not (0 < w < 1):
        return None, "权重必须在 0~1 之间（不含0和1）"
    return w, None

# ===== 页面路由 =====

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/students')
def students():
    return render_template('students.html')

@app.route('/scores')
def scores():
    return render_template('scores.html')

@app.route('/weights')
def weights():
    return render_template('weights.html')

# ===== 统计 =====

@app.route('/api/stats')
def api_stats():
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('SELECT COUNT(*) FROM student')
            stu_count = cur.fetchone()[0]
            cur.execute('SELECT COUNT(*) FROM score')
            score_count = cur.fetchone()[0]
            cur.execute('SELECT COUNT(*) FROM subject_weight')
            weight_count = cur.fetchone()[0]
            cur.execute('SELECT COUNT(DISTINCT subject) FROM score')
            subj_count = cur.fetchone()[0]
            cur.execute('''
                SELECT sname, sc.subject, sc.final_score
                FROM score sc JOIN student s ON s.sid = sc.sid
                ORDER BY sc.final_score DESC LIMIT 5
            ''')
            top5 = [{'name': r[0], 'subject': r[1], 'score': r[2]} for r in cur.fetchall()]
        return jsonify({'student_count': stu_count, 'score_count': score_count,
                        'weight_count': weight_count, 'subject_count': subj_count, 'top5': top5})
    finally:
        conn.close()

# ===== 学生 CRUD =====

@app.route('/api/students')
def api_students():
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('''
                SELECT s.sid, s.sname, COUNT(sc.sid),
                       COALESCE(ROUND(AVG(sc.final_score), 1), 0)
                FROM student s LEFT JOIN score sc ON sc.sid = s.sid
                GROUP BY s.sid, s.sname ORDER BY s.sid
            ''')
            rows = cur.fetchall()
        return jsonify([{'sid': r[0], 'sname': r[1], 'course_count': r[2], 'avg_score': r[3]} for r in rows])
    finally:
        conn.close()

@app.route('/api/students/<sid>')
def api_student_detail(sid):
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('SELECT sid, sname FROM student WHERE sid=%s', (sid,))
            stu = cur.fetchone()
            if not stu:
                return jsonify({'error': '\u5b66\u751f\u4e0d\u5b58\u5728'}), 404
            cur.execute('SELECT subject, usual_score, end_score, final_score FROM score WHERE sid=%s ORDER BY subject', (sid,))
            scores = [{'subject': r[0], 'usual': r[1], 'end': r[2], 'final': r[3]} for r in cur.fetchall()]
        return jsonify({'sid': stu[0], 'sname': stu[1], 'scores': scores})
    finally:
        conn.close()

@app.route('/api/students', methods=['POST'])
def api_add_student():
    data = request.get_json()
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('INSERT INTO student(sid, sname) VALUES(%s, %s)', (data['sid'], data['sname']))
            conn.commit()
        return jsonify({'success': True})
    except pymysql.err.IntegrityError:
        return jsonify({'error': '\u5b66\u53f7\u5df2\u5b58\u5728'}), 409
    except Exception as e:
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

@app.route('/api/students/<sid>', methods=['PUT'])
def api_update_student(sid):
    '''修改学生姓名'''
    data = request.get_json()
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('UPDATE student SET sname=%s WHERE sid=%s', (data['sname'], sid))
            conn.commit()
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

@app.route('/api/students/<sid>', methods=['DELETE'])
def api_delete_student(sid):
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('DELETE FROM score WHERE sid=%s', (sid,))
            cur.execute('DELETE FROM student WHERE sid=%s', (sid,))
            conn.commit()
        return jsonify({'success': True})
    except Exception as e:
        conn.rollback()
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

# ===== 成绩 CRUD =====

@app.route('/api/scores')
def api_scores():
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('''
                SELECT sc.sid, s.sname, sc.subject, sc.usual_score, sc.end_score, sc.final_score
                FROM score sc JOIN student s ON s.sid = sc.sid
                ORDER BY sc.sid, sc.subject
            ''')
            rows = cur.fetchall()
        return jsonify([{'sid': r[0], 'sname': r[1], 'subject': r[2],
                         'usual': r[3], 'end': r[4], 'final': r[5]} for r in rows])
    finally:
        conn.close()

@app.route('/api/scores', methods=['POST'])
def api_add_score():
    data = request.get_json()
    sid = data['sid']
    subject = data['subject']
    usual, uerr = validate_score(data['usual'])
    end, eerr = validate_score(data['end'])
    if uerr or eerr:
        return jsonify({'error': uerr or eerr}), 400
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('SELECT weight_ratio FROM subject_weight WHERE subject_name=%s', (subject,))
            row = cur.fetchone()
            w = float(row[0]) if row else 0.5
            final = int(w * usual + (1.0 - w) * end + 0.5)
            cur.execute(
                'INSERT INTO score(sid, subject, usual_score, end_score, final_score) VALUES(%s, %s, %s, %s, %s)',
                (sid, subject, usual, end, final))
            conn.commit()
        return jsonify({'success': True, 'final_score': final, 'weight': w})
    except pymysql.err.IntegrityError:
        return jsonify({'error': '\u8be5\u5b66\u751f\u6b64\u79d1\u76ee\u6210\u7ee9\u5df2\u5b58\u5728'}), 409
    except Exception as e:
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

@app.route('/api/scores/update', methods=['POST'])
def api_update_score():
    '''修改成绩'''
    data = request.get_json()
    sid = data['sid']
    subject = data['subject']
    usual, uerr = validate_score(data['usual'])
    end, eerr = validate_score(data['end'])
    if uerr or eerr:
        return jsonify({'error': uerr or eerr}), 400
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('SELECT weight_ratio FROM subject_weight WHERE subject_name=%s', (subject,))
            row = cur.fetchone()
            w = float(row[0]) if row else 0.5
            final = int(w * usual + (1.0 - w) * end + 0.5)
            cur.execute(
                'UPDATE score SET usual_score=%s, end_score=%s, final_score=%s WHERE sid=%s AND subject=%s',
                (usual, end, final, sid, subject))
            conn.commit()
        return jsonify({'success': True, 'final_score': final, 'weight': w})
    except Exception as e:
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

@app.route('/api/scores/delete', methods=['POST'])
def api_delete_score():
    data = request.get_json()
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('DELETE FROM score WHERE sid=%s AND subject=%s', (data['sid'], data['subject']))
            conn.commit()
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

# ===== 权重 CRUD =====

@app.route('/api/weights')
def api_weights():
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('''
                SELECT w.subject_name, w.weight_ratio, COUNT(sc.sid)
                FROM subject_weight w LEFT JOIN score sc ON sc.subject = w.subject_name
                GROUP BY w.subject_name, w.weight_ratio ORDER BY w.subject_name
            ''')
            rows = cur.fetchall()
        return jsonify([{'subject': r[0], 'weight': float(r[1]), 'student_count': r[2]} for r in rows])
    finally:
        conn.close()

@app.route('/api/weights', methods=['POST'])
def api_add_weight():
    data = request.get_json()
    subject = (data.get('subject') or '').strip()
    if not subject:
        return jsonify({'error': '\u79d1\u76ee\u4e0d\u80fd\u4e3a\u7a7a'}), 400
    w, werr = validate_weight(data.get('weight'))
    if werr:
        return jsonify({'error': werr}), 400
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('REPLACE INTO subject_weight(subject_name, weight_ratio) VALUES(%s, %s)',
                        (subject, w))
            conn.commit()
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

@app.route('/api/weights/<subject>', methods=['DELETE'])
def api_delete_weight(subject):
    conn = get_conn()
    try:
        with conn.cursor() as cur:
            cur.execute('DELETE FROM subject_weight WHERE subject_name=%s', (subject,))
            conn.commit()
        return jsonify({'success': True})
    except Exception as e:
        return jsonify({'error': str(e)}), 400
    finally:
        conn.close()

# ===== Excel导入 =====

@app.route('/api/import/excel', methods=['POST'])
def api_import_excel():
    if 'file' not in request.files:
        return jsonify({'error': '\u8bf7\u9009\u62e9\u6587\u4ef6'}), 400
    f = request.files['file']
    if f.filename == '':
        return jsonify({'error': '\u6587\u4ef6\u540d\u4e3a\u7a7a'}), 400
    if not f.filename.endswith(('.xlsx', '.xls')):
        return jsonify({'error': '\u4ec5\u652f\u6301.xlsx\u6216.xls\u6587\u4ef6'}), 400
    if f.content_length and f.content_length > 10 * 1024 * 1024:
        return jsonify({'error': '\u6587\u4ef6\u8fc7\u5927\uff08\u6700\u592710MB\uff09'}), 400

    import openpyxl
    import tempfile, os

    tmp = tempfile.NamedTemporaryFile(delete=False, suffix='.xlsx')
    f.save(tmp.name)
    tmp.close()

    result = {'students_added': 0, 'scores_added': 0, 'errors': []}

    try:
        wb = openpyxl.load_workbook(tmp.name)
        ws = wb.active
        rows = list(ws.iter_rows(values_only=True))
        if not rows:
            return jsonify({'error': 'Excel\u6587\u4ef6\u4e3a\u7a7a'}), 400

        headers = [str(h).strip() if h else '' for h in rows[0]]
        data_rows = rows[1:]
        if len(data_rows) > 10000:
            return jsonify({'error': '\u5bfc\u5165\u884c\u6570\u8fc7\u591a\uff08\u6700\u591a10000\u884c\uff0c\u5b9e\u9645 %d \u884c\uff09' % len(data_rows)}), 400

        has_sid = '\u5b66\u53f7' in headers
        has_name = '\u59d3\u540d' in headers
        has_subject = '\u79d1\u76ee' in headers
        has_usual = any(k in headers for k in ['\u5e73\u65f6', '\u5e73\u65f6\u5206', '\u5e73\u65f6\u6210\u7ee9'])
        has_end = any(k in headers for k in ['\u671f\u672b', '\u671f\u672b\u5206', '\u671f\u672b\u6210\u7ee9'])

        col_sid = headers.index('\u5b66\u53f7') if has_sid else -1
        col_name = headers.index('\u59d3\u540d') if has_name else -1
        col_subject = headers.index('\u79d1\u76ee') if has_subject else -1
        col_usual = next((headers.index(k) for k in ['\u5e73\u65f6', '\u5e73\u65f6\u5206', '\u5e73\u65f6\u6210\u7ee9'] if k in headers), -1)
        col_end = next((headers.index(k) for k in ['\u671f\u672b', '\u671f\u672b\u5206', '\u671f\u672b\u6210\u7ee9'] if k in headers), -1)

        def add_error(msg):
            if len(result['errors']) < 50:
                result['errors'].append(msg)

        conn = get_conn()
        try:
            with conn.cursor() as cur:
                for row_idx, row in enumerate(data_rows, 2):
                    vals = [str(c).strip() if c is not None else '' for c in row]

                    if has_sid and has_name and vals[col_sid] and vals[col_name]:
                        sid = vals[col_sid]
                        sname = vals[col_name]
                        try:
                            cur.execute('INSERT IGNORE INTO student(sid, sname) VALUES(%s, %s)', (sid, sname))
                            if cur.rowcount > 0:
                                result['students_added'] += 1
                                conn.commit()
                        except Exception as ex:
                            add_error('\u7b2c%d\u884c\u5b66\u751f\u5bfc\u5165\u5931\u8d25: %s' % (row_idx, str(ex)))

                    if has_sid and has_subject and vals[col_sid] and vals[col_subject]:
                        sid = vals[col_sid]
                        subject = vals[col_subject]
                        usual, uerr = validate_score(vals[col_usual]) if col_usual >= 0 and vals[col_usual] else (0, None)
                        end, eerr = validate_score(vals[col_end]) if col_end >= 0 and vals[col_end] else (0, None)
                        if uerr or eerr:
                            add_error('\u7b2c%d\u884c\u6210\u7ee9\u8bfb\u53d6\u5931\u8d25: %s' % (row_idx, uerr or eerr))
                            continue

                        cur.execute('SELECT weight_ratio FROM subject_weight WHERE subject_name=%s', (subject,))
                        row_w = cur.fetchone()
                        w = float(row_w[0]) if row_w else 0.5
                        final = int(w * usual + (1.0 - w) * end + 0.5)

                        try:
                            cur.execute(
                                'REPLACE INTO score(sid, subject, usual_score, end_score, final_score) VALUES(%s, %s, %s, %s, %s)',
                                (sid, subject, usual, end, final))
                            if cur.rowcount > 0:
                                result['scores_added'] += 1
                            conn.commit()
                        except Exception as ex:
                            add_error('\u7b2c%d\u884c\u6210\u7ee9\u5bfc\u5165\u5931\u8d25: %s' % (row_idx, str(ex)))
        finally:
            conn.close()
    finally:
        os.unlink(tmp.name)

    return jsonify(result)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=os.getenv('FLASK_DEBUG', 'false').lower() == 'true')

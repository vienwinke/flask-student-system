-- 学生成绩管理系统 数据库初始化脚本
-- 使用方法: mysql -u root -p < sql/student_sys.sql

CREATE DATABASE IF NOT EXISTS student_sys DEFAULT CHARSET utf8mb4;
USE student_sys;

-- 学生表
CREATE TABLE IF NOT EXISTS student (
    sid VARCHAR(20) PRIMARY KEY,
    sname VARCHAR(50) NOT NULL
);

-- 成绩表
CREATE TABLE IF NOT EXISTS score (
    sid VARCHAR(20) NOT NULL,
    subject VARCHAR(50) NOT NULL,
    usual_score INT DEFAULT 0,
    end_score INT DEFAULT 0,
    final_score INT DEFAULT 0,
    PRIMARY KEY(sid, subject)
);

-- 学科权重表
CREATE TABLE IF NOT EXISTS subject_weight (
    subject_name VARCHAR(50) PRIMARY KEY,
    weight_ratio DOUBLE DEFAULT 0.5
);

-- 测试数据
INSERT INTO student(sid, sname) VALUES('2023001', '张三');
INSERT INTO student(sid, sname) VALUES('2023002', '李四');
INSERT INTO student(sid, sname) VALUES('2023003', '王五');

INSERT INTO subject_weight(subject_name, weight_ratio) VALUES('数学', 0.4);
INSERT INTO subject_weight(subject_name, weight_ratio) VALUES('英语', 0.3);
INSERT INTO subject_weight(subject_name, weight_ratio) VALUES('语文', 0.5);

INSERT INTO score(sid, subject, usual_score, end_score, final_score) VALUES('2023001', '数学', 80, 90, 86);
INSERT INTO score(sid, subject, usual_score, end_score, final_score) VALUES('2023001', '英语', 70, 85, 79);
INSERT INTO score(sid, subject, usual_score, end_score, final_score) VALUES('2023002', '数学', 90, 80, 84);
INSERT INTO score(sid, subject, usual_score, end_score, final_score) VALUES('2023002', '语文', 85, 88, 87);
INSERT INTO score(sid, subject, usual_score, end_score, final_score) VALUES('2023003', '英语', 75, 82, 80);

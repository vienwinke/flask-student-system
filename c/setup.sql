CREATE DATABASE IF NOT EXISTS student_sys DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE student_sys;

CREATE TABLE IF NOT EXISTS student (
    sid INT PRIMARY KEY,
    sname VARCHAR(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS score (
    sid INT NOT NULL,
    subject VARCHAR(50) NOT NULL,
    usual_score INT DEFAULT 0,
    end_score INT DEFAULT 0,
    final_score INT DEFAULT 0,
    PRIMARY KEY (sid, subject)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

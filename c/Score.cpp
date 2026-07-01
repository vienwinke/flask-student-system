#include "Score.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

void Score::setAll(const std::string& id, const std::string& sub, int usual, int end, int final)
{
    sid = id; subject = sub; usual_score = usual; end_score = end; final_score = final;
}

void Score::showScore()
{
    std::cout << "学号:" << sid << " 科目:" << subject
        << " 平时:" << usual_score << " 期末:" << end_score
        << " 总评:" << final_score << std::endl;
}

std::string Score::getSid() { return sid; }
std::string Score::getSubject() { return subject; }
int Score::getUsual() { return usual_score; }
int Score::getEnd() { return end_score; }
int Score::getFinal() { return final_score; }
void Score::setSid(const std::string& id) { sid = id; }
void Score::setSubject(const std::string& sub) { subject = sub; }
void Score::setUsual(int s) { usual_score = s; }
void Score::setEnd(int s) { end_score = s; }
void Score::setFinal(int s) { final_score = s; }

void AddScore(Score sc)
{
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char esc_sid[256], esc_sub[256];
    mysql_real_escape_string(conn, esc_sid, sc.getSid().c_str(), (unsigned long)sc.getSid().length());
    mysql_real_escape_string(conn, esc_sub, sc.getSubject().c_str(), (unsigned long)sc.getSubject().length());
    char q[512];
    snprintf(q, sizeof(q), "INSERT INTO score(sid,subject,usual_score,end_score,final_score) VALUES('%s','%s',%d,%d,%d)",
        esc_sid, esc_sub, sc.getUsual(), sc.getEnd(), sc.getFinal());
    if (mysql_query(conn, q) != 0) std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
}

Score QueryScore(const std::string& sid, const std::string& sub)
{
    Score res;
    MYSQL* conn = getDBConn();
    if (!conn) return res;
    char esc_sid[256], esc_sub[256];
    mysql_real_escape_string(conn, esc_sid, sid.c_str(), (unsigned long)sid.length());
    mysql_real_escape_string(conn, esc_sub, sub.c_str(), (unsigned long)sub.length());
    char q[512];
    snprintf(q, sizeof(q), "SELECT sid,subject,usual_score,end_score,final_score FROM score WHERE sid='%s' AND subject='%s'", esc_sid, esc_sub);
    if (mysql_query(conn, q) == 0) {
        MYSQL_RES* result = mysql_store_result(conn);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) res.setAll(row[0]?row[0]:"", row[1]?row[1]:"", row[2]?atoi(row[2]):0, row[3]?atoi(row[3]):0, row[4]?atoi(row[4]):0);
            mysql_free_result(result);
        }
    }
    return res;
}

void UpdateFinalScore(const std::string& sid, const std::string& sub, int newFinal)
{
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char esc_sid[256], esc_sub[256];
    mysql_real_escape_string(conn, esc_sid, sid.c_str(), (unsigned long)sid.length());
    mysql_real_escape_string(conn, esc_sub, sub.c_str(), (unsigned long)sub.length());
    char q[512];
    snprintf(q, sizeof(q), "UPDATE score SET final_score=%d WHERE sid='%s' AND subject='%s'", newFinal, esc_sid, esc_sub);
    if (mysql_query(conn, q) != 0) std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
}

void DeleteScore(const std::string& sid, const std::string& sub)
{
    MYSQL* conn = getDBConn();
    if (!conn) return;
    char esc_sid[256], esc_sub[256];
    mysql_real_escape_string(conn, esc_sid, sid.c_str(), (unsigned long)sid.length());
    mysql_real_escape_string(conn, esc_sub, sub.c_str(), (unsigned long)sub.length());
    char q[512];
    snprintf(q, sizeof(q), "DELETE FROM score WHERE sid='%s' AND subject='%s'", esc_sid, esc_sub);
    if (mysql_query(conn, q) != 0) std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
}
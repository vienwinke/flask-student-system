#pragma once

#include <string>
#include <mysql.h>

class Score
{
private:
    std::string sid;
    std::string subject;
    int usual_score;
    int end_score;
    int final_score;
public:
    void setAll(std::string id, std::string sub, int usual, int end, int final);
    void showScore();
    std::string getSid();
    std::string getSubject();
    int getUsual();
    int getEnd();
    int getFinal();
    void setSid(std::string id);
    void setSubject(std::string sub);
    void setUsual(int s);
    void setEnd(int s);
    void setFinal(int s);
};

extern MYSQL* getDBConn();
void AddScore(Score sc);
Score QueryScore(std::string sid, std::string sub);
void UpdateFinalScore(std::string sid, std::string sub, int newFinal);
void DeleteScore(std::string sid, std::string sub);
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
using namespace std;

struct Question {
    string question;
    int answer;
    map<int, string> answers;
};

//populate map with questions from file
map<int, Question> loadQuestionsFromFile(const string& filename) {
    map<int, Question> questions;
    ifstream file(filename);
    string line;
    int currentId = 0;
    Question currentQuestion;

    while (getline(file, line)) {
        if (line.rfind("QID:", 0) == 0) {
            if (currentId != 0) {
                questions[currentId] = currentQuestion;
                currentQuestion = Question();
            }
            currentId = stoi(line.substr(5));
        } else if (line.rfind("Question:", 0) == 0) {
            currentQuestion.question = line.substr(10);
        } else if (line.rfind("Answer:", 0) == 0) {
            currentQuestion.answer = stoi(line.substr(8));
        } else if (line.rfind("Options:", 0) == 0) {
            while (getline(file, line) && !line.empty()) {
                istringstream optionStream(line);
                int optionId;
                string optionText;
                optionStream >> optionId;
                getline(optionStream, optionText, ':');
                currentQuestion.answers[optionId] = optionText;
            }
        }
    }
    if (currentId != 0) {
        questions[currentId] = currentQuestion;
    }
    return questions;
}

int main() {
    map<int, Question> questions = loadQuestionsFromFile("questions.txt");

    //print ques
    for (const auto& [id, question] : questions) {
        cout << "QID: " << id << "\nQuestion: " << question.question
                  << "\nAnswer: " << question.answer << "\nOptions:\n";
        for (const auto& [optionId, answerText] : question.answers) {
            cout << "  " << optionId << ": " << answerText << "\n";
        }
        cout << endl;
    }

    return 0;
}

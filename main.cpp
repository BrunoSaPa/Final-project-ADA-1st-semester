#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <list>
#include <random>
#include <iterator>
using namespace std;

struct Question {
    int id;
    string question;
    int answer;
    map<int, string> answers;
};


Question getRandomQuestion(const list<Question>& questions) {
    if (questions.empty()) {
        throw runtime_error("No questions available to select.");
    }

    //gen random num
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, questions.size() - 1);

    int randomIndex = dist(gen);

    //go to random num index and return question
    auto it = questions.begin();
    advance(it, randomIndex);

    return *it;
}

void removeQuestion(list<Question>& questions, int idToRemove) {
    for (auto it = questions.begin(); it != questions.end(); ++it) {
        if (it->id == idToRemove) {
            questions.erase(it);
            cout << "Question with ID " << idToRemove << " removed successfully.\n";
            return;
        }
    }
    cout << "Question with ID " << idToRemove << " not found.\n";
}

//populate map with questions from file
list<Question> loadQuestionsFromFile(const string& filename) {
    list<Question> questions;
    ifstream file(filename);
    string line;
    int currentId = 0;
    Question currentQuestion;

    while (getline(file, line)) {
        if (line.rfind("QID:", 0) == 0) {
            if (currentId != 0) {
                questions.push_back(currentQuestion);
                currentQuestion = Question(); 
            }
            currentId = stoi(line.substr(5));
            currentQuestion.id = currentId;
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
                optionStream.ignore(1, ':');
                getline(optionStream, optionText);
                currentQuestion.answers[optionId] = optionText;
            }
        }
    }
    if (currentId != 0) {
        questions.push_back(currentQuestion);
    }
    return questions;
}

void printQuestions(list<Question> questions){
    //print ques
    for (const auto& question : questions) {
        cout << "QID: " << question.id << "\nQuestion: " << question.question
             << "\nAnswer: " << question.answer << "\nOptions:\n";
        for (const auto& [optionId, answerText] : question.answers) {
            cout << "  " << optionId << ": " << answerText << "\n";
        }
        cout << endl;
    }
}

int main() {
    list<Question> questions = loadQuestionsFromFile("questions.txt");

    printQuestions(questions);

    /*int idToRemove;
    cout << "Enter the ID of the question to remove: ";
    cin >> idToRemove;*/

    //demo of the program selecting a random question and then eliminating it
    Question randomQuestion = getRandomQuestion(questions);


    cout << "Question randomly selected that will be eliminated: " << randomQuestion.id << "\n";

    removeQuestion(questions, randomQuestion.id);

    printQuestions(questions);

    return 0;
}

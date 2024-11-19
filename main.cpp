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
                optionStream.ignore(1, ':');
                getline(optionStream, optionText);
                currentQuestion.answers[optionId] = optionText;
            }
        }
    }
    if (currentId != 0) {
        questions[currentId] = currentQuestion;
    }
    return questions;
}

void updateConsole(string prices[14], int levelPrice, Question question, char State, int &answer){
    string windowDivisor (90,'_');

    //Start printing in console
    for (int layer = 19; layer>=0; layer--){
        
        //Print the divisions
        if (layer==19 || layer==4 || layer == 0){
            
            cout << "\n"<< windowDivisor;
        
        }
        else if (layer>4 && layer <19){
            //Restart of variables
            string firstSection (79,' ');
            string secondSection (8,' ');

            // Preparing the update for the second section before printing
            for (int position = 2 ;  position < 6; position++){

                secondSection[position] = prices[layer-5][position-2];
                
                //If level price is the same as the price print the indicator
                if ((layer-4)==levelPrice){

                    secondSection[0]='>', secondSection[1]='>', secondSection[6]='<', secondSection[7]='<';
                }
            }

            //Preparing the update for the first section before printing
            
            //The line that contains the question
            if (layer==18){
                int position=0;

                for (char character : question.question){
                    firstSection[position]=character;
                    position++;
                }
            
            //The lines that contains the possible answers
            }else if (layer == 16 || layer == 14 || layer == 12 || layer == 10){

                int position=2;

                for(char character: question.answers[(layer-8)/2]){
                    
                    firstSection[0]= (layer-8)/2 + 48, firstSection[1]='-';
                    firstSection[position]=character;
                    position++;
                
                }

            }

            //Print the corresponding line of the first part of the window
            cout <<"\n|" << firstSection <<"|" << secondSection<<"|"; 

        }else
        {
            
            string thirdSection (88,' ');
            string outputInConsole = "";

            if(layer==3){
                //Change the output in console depend of the state of the game.
                switch (State){
                    case 'Q':
                        outputInConsole = "Seleccione la respuesta correcta: ";
                        break;
                    case 'C':
                        outputInConsole = "¿Deseas continuar con la siguiente preguna?";
                        break;
                    case 'A':
                        outputInConsole = "Si no estas en la ronda 5 o 10 no obtendras premio, ¿Deseas continuar?";
                        break;
                    case 'W':
                        outputInConsole = "Felicidades, tu respuesta ha sido correcta.";
                        break;
                    case 'L':
                        outputInConsole = "Has perdido, el juego se ha terminado.";
                        break;

                }
            }
            //For the state that only requires a yes or no.
            if (layer == 2 && (State == 'C' || State == 'A')){
                outputInConsole= "1. Si     2.No";
            }
            //If the state doesnt require an input it wont be execute
            if (layer == 1 && !(State == 'W' || State == 'L')){
                outputInConsole = string(47,' ');

                cout << "\n|";
                cin >> answer;
            }

            int position =0;

            for (char character : outputInConsole ){
                thirdSection[position] = character;
                position++;
            }

            cout << "\n|" << thirdSection<<"|";
        
        }
        
    }
    
    system("pause");
    //Clean console.
    system("cls");

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

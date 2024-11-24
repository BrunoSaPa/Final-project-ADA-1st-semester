//libraries for the general functions of the game
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <list>
#include <random>
#include <iterator>
//libraries for the timer
#include <thread>
#include <atomic>
#include <chrono>

using namespace std;

struct Question {
    int id;
    string question;
    int answer;
    map<int, string> answers;
};

struct Player {
    bool aditionalTime;
    bool halfAnswers;
    bool changeQuestion;
    bool bonusUsed;
};

//Variables shared between threads
atomic<bool> timeExpired(false);
atomic<bool> alreadyAnswered(false);
atomic<bool> adittionalTime(false);

void timerFunction(int durationSeconds) {
    timeExpired = false;
    int iteration;

    for (iteration = 1; iteration <durationSeconds; iteration++){
        this_thread::sleep_for(chrono::seconds(1));
        //Finish the loop so we dont wait the whole time to continue with the program
        if (alreadyAnswered){
            break;
        }
        //In case adittional time bonus is used duplicate the time
        if (adittionalTime){
            adittionalTime=false;
            durationSeconds*=2;
        }
        //Save the cursor position before update the timer and then restore the cursor position
        cout<<"\033[s" << "\033[24;70H" << "Time "<< iteration << "/" << durationSeconds << "\033[u";
    }

    if (iteration>=durationSeconds){
        timeExpired = true;
    }
    else{
        timeExpired = false;
    }
}

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
            return;
        }
    }
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

//update console with the respective state of the game
void updateConsole(string prices[14], int levelPrice, Question question, char State, Player player){

    //Clean other things that where on console.
    system("cls");

    string windowDivisor (90,'_');
    
    //Start printing in console

    //Header
    cout << "\n" << windowDivisor;

    for(int layer = 22; layer >=20; layer--){
        string header (88,' ');

        if (layer==22){
            int position = 27;

            for (char character: "¿Quien quiere ser millonario?"){
            header[position]= character;
            position++;
            }

            header = "\033[1;5;36m" + header + "\033[0;0m";

            cout << "\n|" << header << " |";
        }
        if (layer==20){
            string header (16, ' ');
            cout << "\n|" << header << ((player.halfAnswers)?"\033[35;1m":"\033[2m") << "A.Half answers    " <<"\033[0m"<<((player.aditionalTime)?"\033[35;1m":"\033[2m") << "B.Additional time   "<<"\033[0m" << ((player.changeQuestion)?"\033[35;1m":"\033[2m") << "C.Change question" << header << "\033[0m |";
        }
        if (layer==21){

            cout << "\n|" << header << "|";
        }
    }

    //Interactive part of the console
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
            //Give a delay to give a chance to read the question and the answers
            if (State=='Q' && !player.bonusUsed){
                this_thread::sleep_for(chrono::seconds(1));
            }

            //Print the corresponding line of the first part of the window
            cout <<"\n|" << firstSection <<"|"<<(((layer-4)==levelPrice)?"\033[1;5;36m":"\033[1;34m" ) << secondSection<<"\033[0m"<<"|"; 

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
                        outputInConsole = "¿Deseas continuar con la siguiente pregunta?";
                        break;
                    case 'A':
                        outputInConsole = "Solo podras asegurar el premio de la ronda 5 o 10, ¿Deseas continuar?";
                        break;
                    case 'W':
                        outputInConsole = "Felicidades, tu respuesta ha sido correcta.";
                        break;
                    case 'L':
                        outputInConsole = "Has perdido, el juego se ha terminado.";
                        break;
                    case 'I':
                        outputInConsole = "El juego va a comenzar, ¿Deseas iniciar el juego?";

                }
            }
            //For the state that only requires a yes or no.
            if (layer == 2 && (State == 'C' || State == 'A' || State == 'I')){
                outputInConsole= "1. Si     2.No";
            }
            

            int position =0;

            for (char character : outputInConsole ){
                thirdSection[position] = character;
                position++;
            }

            cout << "\n|" << thirdSection<<"|";
        
        }
        
    }

    cout << "\033[24;18H";        
    

}

//change message of the first section of console
Question changeMessage(char state, int levelPrice, string prices[14]){
    Question message;

    switch (state){
        case 'C':
            message.question = "Puedes descansar por ahora. Muy buen trabajo.";
            message.answers.insert({1,""});
            break;
        case 'A':
            message.question = "Estas apunto de abandonar este juego.";
            message.answers.insert({1,""});
            break;
        case 'W':
            message.question = "Correcto, puedes seguir avanzando.";
            message.answers.insert({1, "El premio que llevas es de " + (prices[levelPrice-1])+" y a cabas de pasar la ronda " + char((levelPrice+48))});
            break;
        case 'L':
            message.question = "Lo siento tu respuesta ha sido incorrecta, ya no puedes continuar.";
            message.answers.insert({1, "Debido a que te equivocaste no te llevas ningun premio."});
            break;
        case 'I':
            message.question = "Bienvenido al juego.";
            message.answers.insert({1,""});
            break;
    }

    message.answers.insert({2,""});
    message.answers.insert({3,""});
    message.answers.insert({4,""});
    return message;
}

int main (){
    //Variables of the player
    Player player; 
    player.aditionalTime = true;    //B
    player.halfAnswers = true;      //A
    player.changeQuestion = true;   //C
    player.bonusUsed = false;
    
    //Setting up the variables of the game
    string prices[14] ={"0200","0400","0600","0800","1000","1300","1600","1900","2100","2400","2800","3200","3600","4000"};
    int levelPrice=1;
    char answer = 'I';
    int reward = 0;
    
    //load questions
    list<Question> listQuestions = loadQuestionsFromFile("./questions.txt");

    //game starts
    do{

        updateConsole(prices, levelPrice,changeMessage('I',levelPrice, prices),'I', player);
        cin >> answer;

    } while (!((answer==49)||(answer==50)));
    
    if(answer == 49){
        
        while (1==1)
        {
            chooseQuestion:
            Question randomQuestion = getRandomQuestion(listQuestions);
            alreadyAnswered=false;
            thread timerThread(timerFunction, 30);
            
            reloadConsole:
            updateConsole(prices,levelPrice,randomQuestion,'Q', player);
            player.bonusUsed=false;
            
            returnToQuestion:
            // restart of cursor and clean of the other answer
            cout << "\033[24;18H" << "\033[K" <<"\033[24;90H|"<< "\033[24;18H";  
            
            do{     
                
                //the timer should be put here
                cin >> answer;
            
            } while (!((answer==49)||(answer==50)||(answer==51)||(answer==52)||(answer==65)||(answer==66)||(answer==67)));
            
            // use bonuses 
            switch (answer)
            {
                case 'A': 
                    if(player.halfAnswers){
                        int count = 0;

                        while(count < 2){
                            random_device rd;
                            mt19937 gen(rd());
                            uniform_int_distribution<int> dist(1,4);

                            int randomIndex = dist(gen);

                            if((randomIndex!=randomQuestion.answer)&&(randomIndex<5)){
                                randomQuestion.answers[randomIndex] = " ";
                                count++;
                            }

                        }

                        player.halfAnswers=false;
                        player.bonusUsed = true;
                        goto reloadConsole;
                    }
                    else{
                        goto returnToQuestion;
                    }
                    
                    break;
                    
                case 'B':
                    if(player.aditionalTime){
                        
                        adittionalTime = true;
                        player.aditionalTime=false;
                        player.bonusUsed=true;
                        goto reloadConsole;
                    }
                    else{
                        goto returnToQuestion;
                    }
                    break;
                case 'C':
                    if(player.changeQuestion){
                        cout<< "\033[23;18HChanging question...\033[24;18H";
                        removeQuestion(listQuestions, randomQuestion.id);
                        player.changeQuestion=false;
                        alreadyAnswered = true;

                        //wait for thread to finish
                        if (timerThread.joinable()) {
                            cout << "\033[24;18H"<<"Esperando a que se acabe el tiempo...";
                            timerThread.join();
                        } 

                        goto chooseQuestion;
                    }
                    else{
                        goto returnToQuestion;
                    }

                    break;
            }
            //Quiting the bonuses from a false positive to finish the timer
            alreadyAnswered = true;
            
            //wait for thread to finish
            if (timerThread.joinable()) {
                cout << "\033[24;18H"<<"Esperando a que se acabe el tiempo...";
                timerThread.join();
            } 
            
            //verify if the answer is correct
            if ((answer-48) == randomQuestion.answer){
                //verify if the player already won
                if(levelPrice==14){
                    reward = 4000;
                    break;
                }
                
                //let know the player the answer is correct
                updateConsole(prices,levelPrice,changeMessage('W',levelPrice,prices),'W',player);
                system("pause");

                //show the player if he wants to continue          
                continueDecision:  
                do{
                    updateConsole(prices,levelPrice,changeMessage('C',levelPrice,prices),'C',player);
                    // restart of cursor and clean of the other answer
                    cout << "\033[24;18H" << "\033[K" <<"\033[24;90H|"<< "\033[24;18H";  
                    //wait for the response of the player
                    cin >> answer;
                } while (!((answer==49)||(answer==50)));

                if (answer==50){
                    do{
                            updateConsole(prices,levelPrice,changeMessage('A',levelPrice,prices),'A',player);
                            // restart of cursor and clean of the other answer
                            cout << "\033[24;18H" << "\033[K" <<"\033[24;90H|"<< "\033[24;18H";  
                            //wait for the response of the player
                            cin >> answer;
                    } while (!((answer==49)||(answer==50)));

                    if (answer==49){
                        reward = (((levelPrice/5)>0)?1000:0);
                        reward = (((levelPrice/10)>0)?2400:reward);
                        break;
                    }
                    else{
                        goto continueDecision;
                    }

                }
       
            }
            else{
                updateConsole(prices,levelPrice,changeMessage('L',levelPrice,prices),'L',player);
                system("pause");
                break;
            }
            
            //update the game variables
                removeQuestion(listQuestions,randomQuestion.id);
                levelPrice++;
        }
        
    }

    system("cls");
    cout << "Gracias por jugar.\n\n\n\t"<< "El premio que te has llevado ha sido de "<< reward <<".\n\nEl juego ya termino\n";
    system("pause");
}
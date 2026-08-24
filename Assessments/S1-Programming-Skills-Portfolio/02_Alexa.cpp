#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// Stores the setup and punchline of each joke.
struct Joke
{
    string setup;
    string punchline;
};

// Loads the jokes from the text file.
vector<Joke> loadJokes()
{
    vector<Joke> jokes;
    ifstream file("randomJokes.txt");
    string line;

    if (!file)
    {
        cout << "Error opening randomJokes.txt\n";
        return jokes;
    }

    while (getline(file, line))
    {
        int questionMark = line.find('?');

        if (questionMark != string::npos)
        {
            Joke newJoke;

            newJoke.setup = line.substr(0, questionMark + 1);
            newJoke.punchline = line.substr(questionMark + 1);

            jokes.push_back(newJoke);
        }
    }

    file.close();
    return jokes;
}

// Selects and displays a random joke.
void tellJoke(vector<Joke> jokes)
{
    int randomNumber = rand() % jokes.size();

    cout << "\nAlexa: " << jokes[randomNumber].setup << endl;
    cout << "Press Enter for the punchline...";
    cin.get();

    cout << "Alexa: " << jokes[randomNumber].punchline << "\n\n";
}

int main()
{
    vector<Joke> jokes = loadJokes();
    string command;

    if (jokes.empty())
    {
        cout << "No jokes were loaded.\n";
        return 1;
    }

    srand(time(0));

    cout << "=== Alexa Joke Generator ===\n";
    cout << "Type: Alexa, tell me a joke\n";
    cout << "Type: quit to exit\n\n";

    while (command != "quit")
    {
        cout << "You: ";
        getline(cin, command);

        if (command == "Alexa, tell me a joke")
        {
            tellJoke(jokes);
        }
        else if (command != "quit")
        {
            cout << "Alexa: Please use the correct phrase.\n\n";
        }
    }

    cout << "Alexa: Goodbye!\n";

    return 0;
}

#include <iostream>
#include <iomanip>
#include <limits>
#include <cctype>

using namespace std;

// Values used for the CO2 calculation.
const double FUEL_CONSUMPTION = 9.66;
const double PETROL_EMISSIONS = 2.31;
const double DIESEL_EMISSIONS = 2.68;

// Gets and checks the journey distance.
double getDistance()
{
    double distance;

    while (true)
    {
        cout << "Enter journey distance in miles: ";

        if (cin >> distance && distance > 0)
        {
            return distance;
        }

        cout << "Please enter a valid number greater than zero.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Gets and checks the fuel type.
char getFuelType()
{
    char fuel;

    while (true)
    {
        cout << "Enter fuel type (P for petrol or D for diesel): ";
        cin >> fuel;

        fuel = toupper(static_cast<unsigned char>(fuel));

        if (fuel == 'P' || fuel == 'D')
        {
            return fuel;
        }

        cout << "Please enter P or D.\n";
    }
}

// Calculates the total CO2 emissions.
double calculateEmissions(double distance, char fuel)
{
    double litresUsed = distance / 100 * FUEL_CONSUMPTION;

    if (fuel == 'D')
    {
        return litresUsed * DIESEL_EMISSIONS;
    }

    return litresUsed * PETROL_EMISSIONS;
}

int main()
{
    cout << "=== CO2 Journey Calculator ===\n\n";

    double distance = getDistance();
    char fuel = getFuelType();
    double emissions = calculateEmissions(distance, fuel);

    cout << fixed << setprecision(2);

    cout << "\nJourney Summary\n";
    cout << "Distance: " << distance << " miles\n";
    cout << "Fuel: " << (fuel == 'P' ? "Petrol" : "Diesel") << "\n";
    cout << "Estimated CO2 emissions: " << emissions << " kg\n";

    return 0;
}
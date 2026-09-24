# GlobeLens 3.2 - Responsive Country Intelligence

GlobeLens is a C++ desktop application developed using openFrameworks. It retrieves country information from the REST Countries API and presents the data through an interactive graphical interface.

The application was developed for the CodeLab II Data Driven Application assessment.

## Features

- Search for countries by name
- Display population, capital, area, currency and population density
- Display languages, calling codes, time zones and neighbouring countries
- Interactive world map
- Country highlighting
- Map zoom, pan and reset
- Compare two countries
- Save favourite countries
- Persistent favourites using JSON
- Population and density insights
- Recent search history
- Country-name aliases such as UK, USA and UAE
- Error handling for invalid searches and network problems
- Responsive interface

## Project Structure

The application uses separate C++ classes to organise different responsibilities:

- `CountryAPI` - handles API requests and JSON responses
- `CountryData` - stores and formats country information
- `FavoritesManager` - saves and loads favourite countries
- `WorldMap` - manages GeoJSON map rendering and interaction
- `ofApp` - controls the user interface and application flow

## Technologies

- C++
- openFrameworks
- REST Countries API
- JSON
- GeoJSON
- Visual Studio

## API Configuration

The API configuration file is intentionally excluded from this repository to protect the API key.

Create:

`bin/data/config/globelens.json`

The configuration should contain the API credentials required by the application.

Do not commit API keys to the repository.

## Running the Application

1. Install openFrameworks.
2. Open `emptyExample.sln` in Visual Studio.
3. Add the required API configuration locally.
4. Build the project.
5. Run the application.

## Main Application Areas

### Explore

Search for a country and view its key statistics, detailed information and location on the interactive map.

### Compare

Select two countries and compare population, land area and population density.

### Favorites

Save countries for quick access. Saved countries are stored locally.

### Insights

View population and population-density charts for selected countries.

## Error Handling

GlobeLens provides feedback when:

- the search field is empty
- a country cannot be found
- the API cannot be reached
- an invalid country name is entered

The previous valid country remains displayed when a search fails.

## Data Sources

Country information is retrieved from the REST Countries API.

Map boundaries are rendered from GeoJSON geographic data.

## Author

Caleb Oppong

Bath Spa University London  
CodeLab II - Data Driven Application
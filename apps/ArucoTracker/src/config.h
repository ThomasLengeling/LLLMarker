#pragma once

/*
Config class for the initial state of the application
*/

class SystemVars{
public:

    static SystemVars& getInstance()
    {
        static SystemVars    instance;
        // Instantiated on first use.
        return instance;
    }

    //add here config parameters
    int width;
    int height;

private:

    //default values
    SystemVars() {
        width  = 1920;
        height =  1080;
    }

    SystemVars(SystemVars const&) = delete;
    void operator=(SystemVars const&) = delete;

};

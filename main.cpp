/*
Cabanayan, Ivan Perry
Rodriguez, Adrian Enrico
Santos, John Lester
ENGG 151.01
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <typeinfo>
using namespace std;

class signalClass
{
    public:
        int startIndex;
        int dur;
        string fileName;
        double* arr;

        signalClass ()
        {
            startIndex = 0;
            dur = 1;
            arr = new double[dur];
            arr[0] = 0;
        }
};

void computeDFT (double * sigArr, int sigDur, double stepSize,
    double sampFreq, double startFreq, double endFreq, int nSteps,
	double ** realPart, double ** imagPart, double ** magnitude, double ** phase);

void fileToArray (string* fileName, double** arr, int* index, int* dur);

void arrayToFile (ofstream& outputFile, double startFreq, int nSteps, double stepSize,
                  double** realPart, double** imagPart, double** magnitude, double** phase);

int main (int argc, char* argv[])
{

    cout.unsetf(ios::floatfield);

    cout << setprecision(8);

    signalClass sig;
    double sampRate;
    double startFreq;
    double endFreq;
    int nSteps;
    string logFileName;

    double* realPart;
    double* imagPart;
    double* magnitude;
    double* phase;

    if (argc == 7)
    {
        sig.fileName = argv[1];
        fileToArray(&sig.fileName, &sig.arr, &sig.startIndex, &sig.dur);

        sampRate = atof(argv[2]);
        startFreq = atof(argv[3]);
        endFreq = atof(argv[4]);
        nSteps = atoi(argv[5]);
        logFileName = argv[6];
    }

    else if (argc == 6)
    {
        sig.fileName = argv[1];
        fileToArray(&sig.fileName, &sig.arr, &sig.startIndex, &sig.dur);\

        sampRate = atof(argv[2]);
        startFreq = atof(argv[3]);
        endFreq = atof(argv[4]);
        nSteps = atoi(argv[5]);
        logFileName = "dftlog.txt";
    }

    else if (argc == 1)
    {
        cout << "Enter the signal file name: ";
        getline(cin, sig.fileName);

        fileToArray(&sig.fileName, &sig.arr, &sig.startIndex, &sig.dur);

        cout << "Enter the sampling rate: ";
        cin >> sampRate;

        cout << "Enter the starting frequency (Hz): ";
        cin >> startFreq;

        cout << "Enter the end frequency (Hz): ";
        cin >> endFreq;

        cout << "Enter the number of steps: ";
        cin >> nSteps;

        cout << "Enter the output filename: ";
        getline(cin >> ws, logFileName);
    }

    double stepSize = (endFreq - startFreq)/nSteps;

    cout << "\nImported Signal from " + sig.fileName + ", Duration: " << sig.dur << "\n";

    computeDFT (sig.arr, sig.dur, stepSize, sampRate,
                startFreq, endFreq, nSteps, &realPart,
                &imagPart, &magnitude, &phase);

    ofstream outputFile (logFileName, ios::binary | ios::app);

    arrayToFile (outputFile, startFreq, nSteps, stepSize, &realPart, &imagPart, &magnitude, &phase);

    outputFile.close();

    cout << "\nDFT Report sent to " + logFileName + "\n";

    if (nSteps < 10)
    {
        cout << "frequency (Hz) \t real part \t imaginary part" << endl;

        for (int i = 0; i <= nSteps; i++)
        {
            cout << startFreq + i * stepSize << "\t" << realPart[i] << "\t" << imagPart[i] << endl;
        }

        cout << endl << "frequency (Hz) \t magnitude \t phase (degrees)" << endl;

        for (int i = 0; i <= nSteps; i++)
        {
            cout << startFreq + i * stepSize << "\t" << magnitude[i] << "\t" << phase[i] << endl;
        }

        cout << endl;
    }

    return 0;
}

void computeDFT (double* sigArr, int sigDur, double stepSize,
    double sampRate, double startFreq, double endFreq, int nSteps,
	double** realPart, double** imagPart, double** magnitude, double** phase)
	{
	    *realPart = (double*)calloc(nSteps+1, sizeof(double));
	    *imagPart = (double*)calloc(nSteps+1, sizeof(double));
	    *magnitude = (double*)calloc(nSteps+1, sizeof(double));
	    *phase = (double*)calloc(nSteps+1, sizeof(double));

	    for (int i = 0; i <= nSteps; i++)
        {
            double digFreq = 2 * (M_PI) * (startFreq + i * stepSize)/sampRate;

            for (int j = 0; j < sigDur; j++)
            {
                (*realPart)[i] += sigArr[j] * cos(digFreq * j);
                (*imagPart)[i] += -sigArr[j] * sin(digFreq * j);

                if (abs((*realPart)[i]) < pow(10, -8))
                {
                    (*realPart)[i] = 0;
                }

                if (abs((*imagPart)[i]) < pow(10, -8))
                {
                    (*imagPart)[i] = 0;
                }
            }

            (*magnitude)[i] = sqrt( pow((*realPart)[i], 2) + pow((*imagPart)[i], 2));
            (*phase)[i] = atan2 ( (*imagPart)[i], (*realPart)[i] ) * 180/M_PI;
        }
	}

void fileToArray (string* fileName, double** arr, int* index, int* dur)
{
    string line;
    stringstream s;
    double number;
    double firstNum;
    vector<double> signalVector;

    ifstream signalFile(*fileName);

    if (signalFile.is_open())
    {
        getline(signalFile, line);
        s.str(line);

        s >> firstNum;

        if (s.fail())
        {
            cout << "Unable to extract a valid signal from " << *fileName << ". Enter a new file: ";
            string newFile;
            double* newArr;
            int newIndex;
            int newDur;
            getline(cin, newFile);
            *fileName = newFile;
            fileToArray(&newFile, &newArr, &newIndex, &newDur);
            *arr = newArr;
            *index = newIndex;
            *dur = newDur;

            return;
        }

        s >> number;

        if (s.fail())
        {
            *index = 0;
            number = firstNum;
        }
        else
        {
            *index = 0;
        }

        signalVector.push_back(number);

        while (getline(signalFile, line))
        {
            s.clear();
            s.str(line);
            s >> number;

            if (s.fail())
            {
                s.clear();
                break;
            }
            signalVector.push_back(number);
        }
    }
    else
    {
        cout << "File cannot be opened. Enter a new file: ";
        string newFile;
        double* newArr;
        int newIndex;
        int newDur;
        getline(cin, newFile);
        *fileName = newFile;
        fileToArray(&newFile, &newArr, &newIndex, &newDur);
        *arr = newArr;
        *index = newIndex;
        *dur = newDur;

        return;
    }

    signalFile.close();

    *dur = signalVector.size();

    *arr = (double*)malloc(*dur * sizeof(double));

    for (int i = 0; i < *dur; i++)
    {
        (*arr)[i] = signalVector[i];
    }
}

void arrayToFile (ofstream& outputFile, double startFreq, int nSteps, double stepSize,
                  double** realPart, double** imagPart, double** magnitude, double** phase)
{
    if(outputFile.is_open())
    {
        cout << "File is open. Writing values to file... \n";
    }

    outputFile << "frequency (Hz) \t real part \t imaginary part" << endl;

    for (int i = 0; i <= nSteps; i++)
    {
        outputFile << startFreq + i * stepSize << "\t" << (*realPart)[i] << "\t" << (*imagPart)[i] << endl;
    }

    outputFile << endl << "frequency (Hz) \t magnitude \t phase (degrees)" << endl;

    for (int i = 0; i <= nSteps; i++)
    {
        outputFile << startFreq + i * stepSize << "\t" << (*magnitude)[i] << "\t" << (*phase)[i] << endl;
    }

    outputFile << endl;
}

/*
Tasks
- provide feedback on result from extracting signal from file, 1 line containing duration and origin filename
- create log file if no file yet, append to file if file exists
- precision of console (default 6)
*/
